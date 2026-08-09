from __future__ import annotations

import json
import os
import queue
import re
import subprocess
import tempfile
import threading
import tkinter as tk
from pathlib import Path
from tkinter import filedialog, messagebox, ttk

from serial.tools import list_ports


APP_NAME = "Arduino Flash Tool"
CONFIG_PATH = Path(os.getenv("APPDATA", Path.home())) / "ArduinoFlashTool" / "settings.json"

BOARD_PROFILES = {
    "Arduino Uno (ATmega328P)": {
        "mcu": "atmega328p", "programmer": "arduino", "baud": "115200"
    },
    "Arduino Nano (ATmega328P, new bootloader)": {
        "mcu": "atmega328p", "programmer": "arduino", "baud": "115200"
    },
    "Arduino Nano (ATmega328P, old bootloader)": {
        "mcu": "atmega328p", "programmer": "arduino", "baud": "57600"
    },
}


class FlashApp(tk.Tk):
    def __init__(self) -> None:
        super().__init__()
        self.title(f"{APP_NAME} - Python MVP")
        self.geometry("850x680")
        self.minsize(720, 580)

        self.events: queue.Queue[tuple[str, object]] = queue.Queue()
        self.process: subprocess.Popen[str] | None = None
        self.cancel_requested = False

        self.board_var = tk.StringVar(value=next(iter(BOARD_PROFILES)))
        self.port_var = tk.StringVar()
        self.firmware_var = tk.StringVar()
        self.avrdude_var = tk.StringVar()
        self.config_var = tk.StringVar()
        self.objcopy_var = tk.StringVar()
        self.verify_var = tk.BooleanVar(value=True)
        self.status_var = tk.StringVar(value="Ready")

        self._build_ui()
        self._load_settings()
        self.refresh_ports()
        self.after(100, self._poll_events)
        self.protocol("WM_DELETE_WINDOW", self._on_close)

    def _build_ui(self) -> None:
        root = ttk.Frame(self, padding=12)
        root.pack(fill=tk.BOTH, expand=True)
        root.columnconfigure(0, weight=1)
        root.rowconfigure(4, weight=1)

        target = ttk.LabelFrame(root, text="Target", padding=10)
        target.grid(row=0, column=0, sticky="ew", pady=(0, 8))
        target.columnconfigure(1, weight=1)

        ttk.Label(target, text="Board:").grid(row=0, column=0, sticky="w", padx=(0, 8))
        self.board_combo = ttk.Combobox(
            target, textvariable=self.board_var, values=list(BOARD_PROFILES), state="readonly"
        )
        self.board_combo.grid(row=0, column=1, columnspan=2, sticky="ew")

        ttk.Label(target, text="Serial port:").grid(row=1, column=0, sticky="w", padx=(0, 8), pady=(8, 0))
        self.port_combo = ttk.Combobox(target, textvariable=self.port_var, state="readonly")
        self.port_combo.grid(row=1, column=1, sticky="ew", pady=(8, 0))
        self.refresh_button = ttk.Button(target, text="Refresh", command=self.refresh_ports)
        self.refresh_button.grid(row=1, column=2, padx=(8, 0), pady=(8, 0))

        firmware = ttk.LabelFrame(root, text="Firmware", padding=10)
        firmware.grid(row=1, column=0, sticky="ew", pady=(0, 8))
        firmware.columnconfigure(1, weight=1)
        ttk.Label(firmware, text="Application:").grid(row=0, column=0, sticky="w", padx=(0, 8))
        self.firmware_entry = ttk.Entry(firmware, textvariable=self.firmware_var)
        self.firmware_entry.grid(row=0, column=1, sticky="ew")
        ttk.Button(firmware, text="Browse...", command=self.browse_firmware).grid(row=0, column=2, padx=(8, 0))
        self.verify_check = ttk.Checkbutton(firmware, text="Verify flash after writing", variable=self.verify_var)
        self.verify_check.grid(row=1, column=1, sticky="w", pady=(8, 0))

        tools = ttk.LabelFrame(root, text="External tools", padding=10)
        tools.grid(row=2, column=0, sticky="ew", pady=(0, 8))
        tools.columnconfigure(1, weight=1)
        self._add_path_row(tools, 0, "avrdude.exe:", self.avrdude_var, self.browse_avrdude)
        self._add_path_row(tools, 1, "avrdude.conf:", self.config_var, self.browse_config)
        self._add_path_row(tools, 2, "avr-objcopy.exe:", self.objcopy_var, self.browse_objcopy)

        actions = ttk.Frame(root)
        actions.grid(row=3, column=0, sticky="ew", pady=(0, 8))
        actions.columnconfigure(0, weight=1)
        ttk.Label(actions, textvariable=self.status_var).grid(row=0, column=0, sticky="w")
        self.flash_button = ttk.Button(actions, text="Flash application", command=self.start_flash)
        self.flash_button.grid(row=0, column=1, padx=(8, 0))
        self.cancel_button = ttk.Button(actions, text="Cancel", command=self.cancel, state=tk.DISABLED)
        self.cancel_button.grid(row=0, column=2, padx=(8, 0))

        log_frame = ttk.LabelFrame(root, text="Log", padding=8)
        log_frame.grid(row=4, column=0, sticky="nsew")
        log_frame.columnconfigure(0, weight=1)
        log_frame.rowconfigure(1, weight=1)
        self.progress = ttk.Progressbar(log_frame, mode="determinate", maximum=100)
        self.progress.grid(row=0, column=0, sticky="ew", pady=(0, 8))
        self.log = tk.Text(log_frame, wrap="word", height=15, state=tk.DISABLED)
        scrollbar = ttk.Scrollbar(log_frame, orient="vertical", command=self.log.yview)
        self.log.configure(yscrollcommand=scrollbar.set)
        self.log.grid(row=1, column=0, sticky="nsew")
        scrollbar.grid(row=1, column=1, sticky="ns")

    @staticmethod
    def _add_path_row(parent: ttk.LabelFrame, row: int, label: str,
                      variable: tk.StringVar, command) -> None:
        ttk.Label(parent, text=label).grid(row=row, column=0, sticky="w", padx=(0, 8), pady=3)
        ttk.Entry(parent, textvariable=variable).grid(row=row, column=1, sticky="ew", pady=3)
        ttk.Button(parent, text="Browse...", command=command).grid(row=row, column=2, padx=(8, 0), pady=3)

    def refresh_ports(self) -> None:
        current_device = self._selected_device()
        values = []
        for port in sorted(list_ports.comports(), key=lambda item: item.device):
            description = f" - {port.description}" if port.description else ""
            values.append(f"{port.device}{description}")
        self.port_combo["values"] = values
        if current_device:
            for item in values:
                if item.split(" - ", 1)[0] == current_device:
                    self.port_var.set(item)
                    return
        self.port_var.set(values[0] if values else "")

    def browse_firmware(self) -> None:
        path = filedialog.askopenfilename(
            title="Select firmware",
            filetypes=[("Firmware", "*.hex *.elf"), ("Intel HEX", "*.hex"), ("ELF", "*.elf")],
        )
        if path:
            self.firmware_var.set(path)

    def browse_avrdude(self) -> None:
        path = filedialog.askopenfilename(title="Select avrdude.exe", filetypes=[("Executable", "*.exe")])
        if path:
            self.avrdude_var.set(path)

    def browse_config(self) -> None:
        path = filedialog.askopenfilename(title="Select avrdude.conf", filetypes=[("Config", "*.conf"), ("All", "*")])
        if path:
            self.config_var.set(path)

    def browse_objcopy(self) -> None:
        path = filedialog.askopenfilename(title="Select avr-objcopy.exe", filetypes=[("Executable", "*.exe")])
        if path:
            self.objcopy_var.set(path)

    def start_flash(self) -> None:
        error = self._validate()
        if error:
            messagebox.showwarning("Cannot flash", error)
            return
        self._save_settings()
        self._clear_log()
        self.cancel_requested = False
        self._set_busy(True)
        self.progress.configure(mode="indeterminate")
        self.progress.start(10)
        job = {
            "firmware": self.firmware_var.get(),
            "avrdude": self.avrdude_var.get(),
            "config": self.config_var.get().strip(),
            "objcopy": self.objcopy_var.get(),
            "board": self.board_var.get(),
            "port": self._selected_device(),
            "verify": self.verify_var.get(),
        }
        threading.Thread(target=self._flash_worker, args=(job,), daemon=True).start()

    def _flash_worker(self, job: dict[str, object]) -> None:
        try:
            firmware = Path(str(job["firmware"]))
            flash_file = firmware
            if firmware.suffix.lower() == ".elf":
                flash_file = Path(tempfile.gettempdir()) / "ArduinoFlashTool_firmware.hex"
                self.events.put(("status", "Converting ELF..."))
                self.events.put(("log", "Converting ELF to Intel HEX..."))
                command = [str(job["objcopy"]), "-O", "ihex", "-R", ".eeprom", str(firmware), str(flash_file)]
                if self._run_process(command) != 0:
                    raise RuntimeError("ELF conversion failed.")

            board_name = str(job["board"])
            profile = BOARD_PROFILES[board_name]
            command = [str(job["avrdude"])]
            config = str(job["config"])
            if config:
                command += ["-C", config]
            command += [
                "-v", "-p", profile["mcu"], "-c", profile["programmer"],
                "-P", str(job["port"]), "-b", profile["baud"], "-D",
            ]
            if not bool(job["verify"]):
                command.append("-V")
            command += ["-U", f"flash:w:{flash_file}:i"]

            self.events.put(("status", "Flashing application..."))
            self.events.put(("log", f"Board: {board_name}"))
            self.events.put(("log", f"Port: {job['port']}, baud: {profile['baud']}"))
            result = self._run_process(command)
            if self.cancel_requested:
                self.events.put(("cancelled", None))
            elif result == 0:
                self.events.put(("success", None))
            else:
                self.events.put(("error", f"avrdude failed with exit code {result}."))
        except Exception as exc:
            self.events.put(("error", str(exc)))

    def _run_process(self, command: list[str]) -> int:
        creation_flags = subprocess.CREATE_NO_WINDOW if os.name == "nt" else 0
        self.process = subprocess.Popen(
            command,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            encoding="utf-8",
            errors="replace",
            creationflags=creation_flags,
        )
        assert self.process.stdout is not None
        for line in self.process.stdout:
            self.events.put(("log", line.rstrip()))
            matches = re.findall(r"(\d{1,3})%", line)
            if matches:
                self.events.put(("progress", min(100, int(matches[-1]))))
        return self.process.wait()

    def cancel(self) -> None:
        self.cancel_requested = True
        process = self.process
        if process and process.poll() is None:
            process.terminate()
        self._append_log("Cancellation requested...")

    def _poll_events(self) -> None:
        try:
            while True:
                event, value = self.events.get_nowait()
                if event == "log":
                    self._append_log(str(value))
                elif event == "status":
                    self.status_var.set(str(value))
                elif event == "progress":
                    self.progress.stop()
                    self.progress.configure(mode="determinate")
                    self.progress["value"] = int(value)
                elif event == "success":
                    self._finish("Completed", 100)
                    self._append_log("Flash completed successfully.")
                    messagebox.showinfo(APP_NAME, "Application flashed successfully.")
                elif event == "cancelled":
                    self._finish("Cancelled", 0)
                elif event == "error":
                    self._finish("Failed", 0)
                    self._append_log(f"ERROR: {value}")
                    messagebox.showerror(APP_NAME, str(value))
        except queue.Empty:
            pass
        self.after(100, self._poll_events)

    def _finish(self, status: str, progress: int) -> None:
        self.process = None
        self.status_var.set(status)
        self.progress.stop()
        self.progress.configure(mode="determinate")
        self.progress["value"] = progress
        self._set_busy(False)

    def _set_busy(self, busy: bool) -> None:
        normal = tk.DISABLED if busy else tk.NORMAL
        readonly = tk.DISABLED if busy else "readonly"
        self.board_combo.configure(state=readonly)
        self.port_combo.configure(state=readonly)
        self.refresh_button.configure(state=normal)
        self.firmware_entry.configure(state=normal)
        self.flash_button.configure(state=normal)
        self.verify_check.configure(state=normal)
        self.cancel_button.configure(state=tk.NORMAL if busy else tk.DISABLED)

    def _validate(self) -> str | None:
        firmware = Path(self.firmware_var.get())
        if not self._selected_device():
            return "Select a valid COM port."
        if not firmware.is_file():
            return "Firmware file does not exist."
        if firmware.suffix.lower() not in {".hex", ".elf"}:
            return "Only .hex and .elf firmware are supported."
        if not Path(self.avrdude_var.get()).is_file():
            return "avrdude.exe was not found."
        if self.config_var.get().strip() and not Path(self.config_var.get()).is_file():
            return "avrdude.conf was not found."
        if firmware.suffix.lower() == ".elf" and not Path(self.objcopy_var.get()).is_file():
            return "avr-objcopy.exe is required for ELF firmware."
        return None

    def _selected_device(self) -> str:
        return self.port_var.get().split(" - ", 1)[0].strip()

    def _append_log(self, text: str) -> None:
        if not text:
            return
        self.log.configure(state=tk.NORMAL)
        self.log.insert(tk.END, text + "\n")
        self.log.see(tk.END)
        self.log.configure(state=tk.DISABLED)

    def _clear_log(self) -> None:
        self.log.configure(state=tk.NORMAL)
        self.log.delete("1.0", tk.END)
        self.log.configure(state=tk.DISABLED)

    def _load_settings(self) -> None:
        try:
            data = json.loads(CONFIG_PATH.read_text(encoding="utf-8"))
        except (OSError, json.JSONDecodeError):
            return
        if data.get("board") in BOARD_PROFILES:
            self.board_var.set(data["board"])
        self.avrdude_var.set(data.get("avrdude", ""))
        self.config_var.set(data.get("config", ""))
        self.objcopy_var.set(data.get("objcopy", ""))
        self.verify_var.set(bool(data.get("verify", True)))

    def _save_settings(self) -> None:
        data = {
            "board": self.board_var.get(),
            "avrdude": self.avrdude_var.get(),
            "config": self.config_var.get(),
            "objcopy": self.objcopy_var.get(),
            "verify": self.verify_var.get(),
        }
        CONFIG_PATH.parent.mkdir(parents=True, exist_ok=True)
        CONFIG_PATH.write_text(json.dumps(data, indent=2), encoding="utf-8")

    def _on_close(self) -> None:
        if self.process and self.process.poll() is None:
            if not messagebox.askyesno(APP_NAME, "A flash operation is running. Cancel it and exit?"):
                return
            self.cancel()
        self.destroy()


if __name__ == "__main__":
    FlashApp().mainloop()
