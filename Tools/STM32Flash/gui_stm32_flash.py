#!/usr/bin/env python3
r"""
gui_stm32_flash.py
--------------------
GUI Tkinter cho cong cu flash firmware va doc thanh ghi STM32 qua ST-Link.

Tai su dung cac ham da co trong find_stlink_com.py:
- flash_hex() / find_cube_programmer_cli()  -> flash qua STM32CubeProgrammer CLI
- read_core_registers()                     -> doc thanh ghi core qua pyocd
File nay PHAI dat CUNG THU MUC voi find_stlink_com.py.

Chuc nang tren GUI:
- O nhap + nut "Chon file..." de tro toi file .hex hoac .elf da build.
- Bang thong tin chip / ST-Link (doc tu STM32CubeProgrammer khi ket noi).
- Nut "Flash" + thanh progress bar hien thi % qua trinh nap firmware.
- Terminal/log hien thi toan bo output cua qua trinh flash / doc thanh ghi.
- Nut "Doc thanh ghi" de doc trang thai thanh ghi core (R0-R12, SP, LR,
  PC, xPSR...) cua chip SAU KHI code da chay, hien ket qua ngay tren terminal.
- Khi khoi dong, app tu mo THEM 1 cua so cmd/terminal rieng, tail (theo doi
  thoi gian thuc) file log "stm32_flash_tool.log" cung thu muc - chay song
  song voi GUI de ban xem/copy toan bo log ngay ca khi khong thao tac tren GUI.

Ve loi "Unable to get core ID" / "No STM32 target found":
- Xay ra khi firmware dang chay tren chip da vao che do tiet kiem dien
  (Stop/Standby lam tat clock SWD) hoac cau hinh lai chan PA13/PA14
  (SWDIO/SWCLK) thanh GPIO thuong, khien ST-Link khong "bat" duoc core theo
  kieu ket noi thong thuong.
- App nay da mac dinh dung che do ket noi "under reset" (mode=UR) - giu chip
  trong trang thai reset trong luc ST-Link bat SWD - day la cach ST khuyen
  dung de xu ly truong hop tren.

Yeu cau cai dat:
    pip install pyserial pyocd
    (va da cai STM32CubeProgrammer cho chuc nang Flash / xem thong tin chip)

Chay:
    python gui_stm32_flash.py
"""

import contextlib
import datetime
import io
import os
import queue
import re
import subprocess
import sys
import threading
import tkinter as tk
from tkinter import ttk, filedialog, scrolledtext

# Tai su dung logic tu file dong lenh (bat buoc dat cung thu muc)
import find_stlink_com as core


PERCENT_RE = re.compile(r"(\d{1,3})\s*%")


class QueueWriter(io.TextIOBase):
    """Gia lam sys.stdout: moi lan print() se day text vao queue de GUI doc va hien thi."""

    def __init__(self, q):
        self.q = q

    def write(self, s):
        if s:
            self.q.put(("log", s))
        return len(s)

    def flush(self):
        pass


class FlashGUI(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("STM32 ST-Link Flash Tool")
        self.geometry("860x620")
        self.minsize(720, 520)

        self.msg_queue = queue.Queue()
        self.busy = False

        self.log_file_path = self._make_log_path()
        self.log_file = open(self.log_file_path, "a", encoding="utf-8", buffering=1)
        self._log_to_file(f"===== Phien lam viec bat dau: {datetime.datetime.now()} =====\n")

        # Khi build exe voi console=True, terminal log se ton tai trong chung
        # process cua app va dong dung luc app dong. Khong duoc bat dau mot
        # child terminal rieng ngoai; voi windowed build, log chi duoc ghi vao file.
        self._console_mode = hasattr(sys.stdout, "fileno") and sys.stdout is not None

        self._build_widgets()
        self.after(50, self._poll_queue)
        self.protocol("WM_DELETE_WINDOW", self._on_close)

    @staticmethod
    def _make_log_path():
        # Khi da dong goi bang PyInstaller (--onefile), __file__ tro toi thu muc
        # giai nen tam (bi xoa sau khi thoat), nen phai dung thu muc chua file .exe
        # thuc te (sys.executable) thay vi __file__.
        if getattr(sys, "frozen", False):
            base_dir = os.path.dirname(sys.executable)
        else:
            base_dir = os.path.dirname(os.path.abspath(__file__))
        return os.path.join(base_dir, "stm32_flash_tool.log")

    def _log_to_file(self, text):
        try:
            self.log_file.write(text)
        except Exception:
            pass

    def _open_external_console(self, log_path):
        """Khong tao them terminal rieng nua.

        Cac phien ban exe/packaged se khong can mo mot console phu. Log duoc_
        luu vao file va, neu app duoc chay trong console, se hien san trong
        console cua app chinh. Dieu nay dam bao terminal nay ton tai cung voi
        qua trinh app va tat khi app dong.
        """
        return None

    def _on_close(self):
        self._log_to_file(f"===== Phien lam viec ket thuc: {datetime.datetime.now()} =====\n")
        try:
            self.log_file.close()
        except Exception:
            pass
        self.destroy()

    # ---------------------------------------------------------------
    # Xay dung giao dien
    # ---------------------------------------------------------------
    def _build_widgets(self):
        pad = {"padx": 8, "pady": 6}

        # --- Chon file firmware ---
        file_frame = ttk.LabelFrame(self, text="File firmware (.hex / .elf)")
        file_frame.pack(fill="x", **pad)

        self.file_var = tk.StringVar()
        ttk.Entry(file_frame, textvariable=self.file_var).pack(
            side="left", fill="x", expand=True, padx=(8, 4), pady=8
        )
        ttk.Button(file_frame, text="Chon file...", command=self._browse_file).pack(
            side="left", padx=(0, 8), pady=8
        )

        # --- Thong tin chip / ST-Link ---
        info_frame = ttk.LabelFrame(self, text="Thong tin chip / ST-Link")
        info_frame.pack(fill="x", **pad)

        self.info_labels = {}
        info_fields = [
            ("device_name", "Device name"),
            ("device_id", "Device ID"),
            ("nvm_size", "NVM size"),
            ("stlink_sn", "ST-Link SN"),
            ("stlink_fw", "ST-Link FW"),
            ("voltage", "Voltage"),
        ]
        for i, (key, label) in enumerate(info_fields):
            r, c = divmod(i, 3)
            cell = ttk.Frame(info_frame)
            cell.grid(row=r, column=c, sticky="w", padx=8, pady=4)
            ttk.Label(cell, text=f"{label}:", font=("Segoe UI", 9, "bold")).pack(side="left")
            val = ttk.Label(cell, text="--")
            val.pack(side="left", padx=(4, 0))
            self.info_labels[key] = val

        ttk.Button(
            info_frame, text="Lam moi thong tin chip", command=self._refresh_chip_info
        ).grid(row=2, column=0, columnspan=3, sticky="w", padx=8, pady=(0, 8))

        # --- Nut hanh dong ---
        action_frame = ttk.Frame(self)
        action_frame.pack(fill="x", **pad)

        self.flash_btn = ttk.Button(action_frame, text="Flash", command=self._start_flash)
        self.flash_btn.pack(side="left", padx=(0, 8))

        self.readreg_btn = ttk.Button(
            action_frame, text="Doc thanh ghi", command=self._start_read_regs
        )
        self.readreg_btn.pack(side="left", padx=(0, 8))

        ttk.Button(action_frame, text="Xoa log", command=self._clear_log).pack(side="left")

        # --- Progress bar ---
        self.progress = ttk.Progressbar(self, orient="horizontal", mode="determinate", maximum=100)
        self.progress.pack(fill="x", **pad)

        self.status_var = tk.StringVar(value="San sang.")
        ttk.Label(self, textvariable=self.status_var).pack(anchor="w", padx=8)

        # --- Terminal / log ---
        term_frame = ttk.LabelFrame(self, text="Terminal / Log")
        term_frame.pack(fill="both", expand=True, **pad)

        self.log_widget = scrolledtext.ScrolledText(
            term_frame,
            bg="#101418",
            fg="#d6e2ea",
            insertbackground="#d6e2ea",
            font=("Consolas", 10),
            wrap="word",
            state="disabled",
        )
        self.log_widget.pack(fill="both", expand=True, padx=4, pady=4)

    # ---------------------------------------------------------------
    # Ham tien ich UI
    # ---------------------------------------------------------------
    def _browse_file(self):
        path = filedialog.askopenfilename(
            title="Chon file firmware",
            filetypes=[
                ("Firmware files", "*.hex *.elf"),
                ("Hex file", "*.hex"),
                ("ELF file", "*.elf"),
                ("Tat ca", "*.*"),
            ],
        )
        if path:
            self.file_var.set(path)

    def _append_log(self, text):
        self.log_widget.configure(state="normal")
        self.log_widget.insert("end", text)
        self.log_widget.see("end")
        self.log_widget.configure(state="disabled")

    def _clear_log(self):
        self.log_widget.configure(state="normal")
        self.log_widget.delete("1.0", "end")
        self.log_widget.configure(state="disabled")

    def _set_busy(self, busy):
        self.busy = busy
        state = "disabled" if busy else "normal"
        self.flash_btn.configure(state=state)
        self.readreg_btn.configure(state=state)

    def _poll_queue(self):
        """Chay tren main thread cua Tkinter: doc queue va cap nhat UI an toan."""
        try:
            while True:
                kind, payload = self.msg_queue.get_nowait()
                if kind == "log":
                    self._append_log(payload)
                    self._log_to_file(payload)
                elif kind == "progress":
                    self.progress["value"] = payload
                elif kind == "status":
                    self.status_var.set(payload)
                elif kind == "info":
                    for key, val in payload.items():
                        if key in self.info_labels:
                            self.info_labels[key].configure(text=val)
                elif kind == "done":
                    self._set_busy(False)
        except queue.Empty:
            pass
        self.after(50, self._poll_queue)

    # ---------------------------------------------------------------
    # Flash firmware
    # ---------------------------------------------------------------
    def _start_flash(self):
        if self.busy:
            return
        path = self.file_var.get().strip()
        if not path:
            self._append_log("Chua chon file firmware (.hex hoac .elf).\n")
            return
        if not os.path.isfile(path):
            self._append_log(f"Khong tim thay file: {path}\n")
            return

        self._set_busy(True)
        self.progress["value"] = 0
        self.status_var.set("Dang flash...")
        threading.Thread(target=self._flash_worker, args=(path,), daemon=True).start()

    def _flash_worker(self, path):
        exe = core.find_cube_programmer_cli()
        if not exe:
            self.msg_queue.put((
                "log",
                "Khong tim thay STM32_Programmer_CLI.exe. Kiem tra da cai "
                "STM32CubeProgrammer chua.\n",
            ))
            self.msg_queue.put(("status", "Loi: khong tim thay CLI"))
            self.msg_queue.put(("done", None))
            return

        cmd = [exe] + core.CONNECT_ARGS + ["-w", path, "-v", "-rst"]
        self.msg_queue.put(("log", "Dang chay: " + " ".join(cmd) + "\n"))

        try:
            proc = subprocess.Popen(
                cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, bufsize=1
            )
        except Exception as e:
            self.msg_queue.put(("log", f"Loi khi chay STM32_Programmer_CLI: {e}\n"))
            self.msg_queue.put(("status", "Loi khi flash"))
            self.msg_queue.put(("done", None))
            return

        # Doc tung ky tu de bat duoc ca cac dong duoc STM32CubeProgrammer
        # cap nhat bang \r (thanh % tien trinh), khong chi \n.
        buf = ""
        while True:
            ch = proc.stdout.read(1)
            if ch == "" and proc.poll() is not None:
                break
            if ch in ("\r", "\n"):
                if buf.strip():
                    self.msg_queue.put(("log", buf + "\n"))
                    m = PERCENT_RE.search(buf)
                    if m:
                        self.msg_queue.put(("progress", int(m.group(1))))
                buf = ""
            elif ch:
                buf += ch
        if buf.strip():
            self.msg_queue.put(("log", buf + "\n"))

        rc = proc.wait()
        if rc == 0:
            self.msg_queue.put(("progress", 100))
            self.msg_queue.put(("status", "Flash thanh cong."))
            self.msg_queue.put(("log", "=> Flash thanh cong.\n"))
        else:
            self.msg_queue.put(("status", "Flash that bai."))
            self.msg_queue.put(("log", "=> Flash that bai.\n"))

        self.msg_queue.put(("done", None))

    # ---------------------------------------------------------------
    # Doc thong tin chip / ST-Link (khong ghi/xoa gi ca, chi ket noi de doc)
    # ---------------------------------------------------------------
    def _refresh_chip_info(self):
        if self.busy:
            return
        self._set_busy(True)
        self.status_var.set("Dang doc thong tin chip...")
        threading.Thread(target=self._chip_info_worker, daemon=True).start()

    def _chip_info_worker(self):
        exe = core.find_cube_programmer_cli()
        if not exe:
            self.msg_queue.put(("log", "Khong tim thay STM32_Programmer_CLI.exe.\n"))
            self.msg_queue.put(("done", None))
            return

        cmd = [exe] + core.CONNECT_ARGS
        self.msg_queue.put(("log", "Dang chay: " + " ".join(cmd) + "\n"))
        try:
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
        except Exception as e:
            self.msg_queue.put(("log", f"Loi: {e}\n"))
            self.msg_queue.put(("status", "Loi khi doc thong tin chip."))
            self.msg_queue.put(("done", None))
            return

        output = (result.stdout or "") + (result.stderr or "")
        self.msg_queue.put(("log", output + "\n"))

        info = {}
        for line in output.splitlines():
            if ":" not in line:
                continue
            key, _, val = line.partition(":")
            key = key.strip().lower()
            val = val.strip()
            if "device name" in key:
                info["device_name"] = val
            elif "device id" in key:
                info["device_id"] = val
            elif "nvm size" in key:
                info["nvm_size"] = val
            elif "st-link sn" in key:
                info["stlink_sn"] = val
            elif "st-link fw" in key:
                info["stlink_fw"] = val
            elif key == "voltage":
                info["voltage"] = val

        if info:
            self.msg_queue.put(("info", info))
            self.msg_queue.put(("status", "Da cap nhat thong tin chip."))
        else:
            self.msg_queue.put((
                "status",
                "Khong doc duoc thong tin chip (kiem tra ST-Link da cam chua).",
            ))

        self.msg_queue.put(("done", None))

    # ---------------------------------------------------------------
    # Doc thanh ghi core (qua pyocd) - hien ket qua ngay tren terminal
    # ---------------------------------------------------------------
    def _start_read_regs(self):
        if self.busy:
            return
        self._set_busy(True)
        self.status_var.set("Dang doc thanh ghi...")
        threading.Thread(target=self._read_regs_worker, daemon=True).start()

    def _read_regs_worker(self):
        writer = QueueWriter(self.msg_queue)
        try:
            with contextlib.redirect_stdout(writer):
                core.read_core_registers(target_type=core.DEFAULT_TARGET)
        except BaseException as e:
            # Bat ca SystemExit (vi cac ham trong core co the sys.exit khi
            # thieu thu vien pyocd), de UI khong bi "ket" o trang thai busy.
            self.msg_queue.put(("log", f"Loi khi doc thanh ghi: {e}\n"))
            self.msg_queue.put(("status", "Loi khi doc thanh ghi."))
            self.msg_queue.put(("done", None))
            return

        self.msg_queue.put(("status", "Da doc xong thanh ghi."))
        self.msg_queue.put(("done", None))


if __name__ == "__main__":
    try:
        app = FlashGUI()
        app.mainloop()
    except Exception:
        # Khi da dong goi --windowed, khong co console de xem traceback, nen ghi
        # loi crash ra file de con debug duoc.
        import traceback

        if getattr(sys, "frozen", False):
            base_dir = os.path.dirname(sys.executable)
        else:
            base_dir = os.path.dirname(os.path.abspath(__file__))
        crash_path = os.path.join(base_dir, "crash.log")
        try:
            with open(crash_path, "a", encoding="utf-8") as f:
                f.write("\n" + "=" * 60 + "\n")
                f.write(str(datetime.datetime.now()) + "\n")
                traceback.print_exc(file=f)
        except Exception:
            pass
        raise