#!/usr/bin/env python3
r"""
find_stlink_com.py
-------------------
1) Do tim cong COM ao (Virtual COM Port) cua ST-Link tren may tinh.
2) Flash file .hex da build vao chip STM32 qua ST-Link (giao tiep SWD).
3) Doc thanh ghi core (R0-R12, SP, LR, PC, xPSR...) va doc thanh ghi/
   vung nho ngoai vi theo dia chi (memory-mapped register).

Luu y quan trong:
- Phan (1) dung "pyserial", giao tiep qua cong COM ao (UART) - dung de
  xem log debug (printf qua UART) tu STM32.
- Phan (2) dung STM32CubeProgrammer CLI (STM32_Programmer_CLI.exe, cong cu
  chinh hang cua ST) de flash - KHONG dung pyocd, vi pack index CMSIS cua
  ST tren pyocd hien dang bi loi/thieu tren nhieu may (loi
  "No matching devices" du da chay pack update).
- Phan (3) dung "pyocd" voi target generic "cortex_m" de doc thanh ghi/
  bo nho qua SWD - khong can CMSIS-Pack rieng cho tung chip.

Thong tin chip mac dinh: STM32F411CEU6 (Cortex-M4F)
- Flash: 512KB, bat dau tai 0x08000000
- SRAM : 128KB, bat dau tai 0x20000000

Yeu cau cai dat:
    pip install pyserial      # cho chuc nang do cong COM (1)
    pip install pyocd         # cho chuc nang doc thanh ghi/bo nho (3)
    # Cai STM32CubeProgrammer (tu trang chu ST, mien phi) cho chuc nang flash (2)
    # Neu da cai STM32CubeIDE thi thuong da co san STM32CubeProgrammer di kem.

    # Tren Linux can quyen truy cap USB cho ST-Link (chay 1 lan):
    #   xem huong dan udev rules cua pyocd (khong nen chay bang sudo lau dai)

Vi du chay:
    python find_stlink_com.py                              # do cong COM
    python find_stlink_com.py --watch                       # theo doi cam/rut COM
    python find_stlink_com.py --flash duong/dan/app.hex      # nap file hex vao chip
    python find_stlink_com.py --flash app.hex --cli-path "C:\...\STM32_Programmer_CLI.exe"
    python find_stlink_com.py --read-regs                    # doc thanh ghi core
    python find_stlink_com.py --read-mem 0x08000000           # doc 1 thanh ghi/bo nho
    python find_stlink_com.py --read-mem 0x40023800 --count 8 # doc 8 word lien tiep
"""

import argparse
import os
import shutil
import subprocess
import sys
import time

try:
    import serial.tools.list_ports as list_ports
except ImportError:
    list_ports = None

# VID/PID cua ST-Link (STMicroelectronics)
ST_VID = 0x0483
ST_KNOWN_PIDS = {
    0x5740: "ST-Link Virtual COM Port (VCP)",
    0x374B: "ST-Link/V2-1",
    0x3752: "ST-Link/V2-1 (Nucleo)",
    0x3753: "ST-Link/V3",
    0x374E: "ST-Link/V3 (khac)",
}


DEFAULT_TARGET = "cortex_m"      # target generic cua pyocd - dung cho doc thanh ghi/bo nho,
                                  # KHONG can CMSIS-Pack (pack cua ST hay bi loi "No matching devices")

# Co the doc target thanh cong bang target generic, nhung nhieu bo STM32 bi loi
# khi ket noi mac dinh (do firmware da vao low-power / SWD bi chan phai). Theo
# kinh nghiem, can thu them fallback den ten chip cu the neu target generic fail.
TARGET_CANDIDATES = [
    "cortex_m",
    "stm32f411ce",
    "stm32f411re",
    "stm32f407vg",
    "stm32f429zi",
]

# Ket noi "under reset": giu chip trong reset trong luc ST-Link bat SWD, giup
# tranh loi "Unable to get core ID" khi firmware dang chay tren chip da:
#   - vao che do tiet kiem dien (Stop/Standby) lam tat clock cho SWD, hoac
#   - cau hinh lai chan PA13/PA14 (SWDIO/SWCLK) thanh GPIO thuong.
# Day la cach ket noi duoc ST khuyen dung khi khong chac firmware hien tai
# co "than thien" voi debug hay khong.
CONNECT_ARGS = ["-c", "port=SWD", "mode=UR", "reset=HWrst"]

CUBE_PROGRAMMER_CANDIDATES = [
    r"C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe",
    r"C:\Program Files (x86)\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe",
]

# Thanh ghi core cua Cortex-M4 (STM32F411 dung loi Cortex-M4F)
CORE_REG_NAMES = [
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
    "r8", "r9", "r10", "r11", "r12",
    "sp", "lr", "pc", "xpsr",
    "msp", "psp", "primask", "control",
]


def _require_pyserial():
    if list_ports is None:
        print("Thieu thu vien pyserial. Cai bang lenh:\n    pip install pyserial")
        sys.exit(1)


def _require_pyocd():
    try:
        from pyocd.core.helpers import ConnectHelper  # noqa: F401
    except ImportError:
        print("Thieu thu vien pyocd. Cai bang lenh:\n    pip install pyocd")
        sys.exit(1)


def _open_session(target_type):
    """Mo ket noi SWD toi ST-Link va tra ve session pyocd (hoac None neu khong thay probe)."""
    from pyocd.core.helpers import ConnectHelper
    from pyocd.core.exceptions import TargetSupportError

    candidate_names = []
    if isinstance(target_type, str):
        candidate_names = [target_type]
        if target_type == DEFAULT_TARGET:
            candidate_names.extend(TARGET_CANDIDATES[1:])
    elif target_type is None:
        candidate_names = list(TARGET_CANDIDATES)

    last_error = None
    for name in candidate_names:
        try:
            session = ConnectHelper.session_with_chosen_probe(
                target_override=name,
                options={"connect_mode": "under-reset", "frequency": 1000000},
            )
            if session is not None:
                return session
        except TypeError:
            # phien ban pyocd cu hoac API khac: thu phuong thuc khong co options
            try:
                session = ConnectHelper.session_with_chosen_probe(target_override=name)
                if session is not None:
                    return session
            except Exception as exc:
                last_error = exc
        except TargetSupportError as exc:
            last_error = exc
            continue
        except Exception as exc:
            last_error = exc
            continue

    if last_error is not None:
        msg = str(last_error)
        if getattr(sys, "frozen", False) and "sequences.lark" in msg:
            print("Loi bundle PyInstaller: pyocd khong tim duoc file du lieu sequences.lark.")
            print("Can rebuild exe voi: pyinstaller --collect-data pyocd --onefile --console ...")
        print(f"Khong ket noi duoc ST-Link qua pyocd. Loi cuoi cung: {last_error}")
    print("Kiem tra day cap, driver ST-Link, va dam bao khong co chuong trinh")
    print("khac (STM32CubeIDE, STM32CubeProgrammer, OpenOCD...) dang chiem ST-Link.")
    return None


def find_cube_programmer_cli():
    """Tim duong dan toi STM32_Programmer_CLI.exe (cong cu chinh hang cua ST)."""
    exe = shutil.which("STM32_Programmer_CLI")
    if exe:
        return exe
    for candidate in CUBE_PROGRAMMER_CANDIDATES:
        if os.path.isfile(candidate):
            return candidate
    return None


def flash_hex(hex_path, cli_path=None):
    """Nap file .hex da build vao chip STM32 qua ST-Link, dung STM32CubeProgrammer CLI.

    Ly do khong dung pyocd de flash: pyocd can CMSIS-Pack rieng cho tung chip de biet
    thuat toan ghi flash, nhung pack index cua ST tren pyocd hien dang bi loi/thieu o
    nhieu may (loi "No matching devices" du da chay pack update). STM32CubeProgrammer CLI
    la cong cu chinh hang cua ST, khong phu thuoc van de nay va thuong da co san neu ban
    dang dung STM32CubeIDE.
    """
    if not os.path.isfile(hex_path):
        print(f"Khong tim thay file hex: {hex_path}")
        return False

    exe = cli_path or find_cube_programmer_cli()
    if not exe:
        print("Khong tim thay STM32_Programmer_CLI.exe.")
        print("Cai STM32CubeProgrammer (mien phi, tai tu trang chu ST) neu chua co,")
        print("hoac neu da cai o vi tri khac, chi ro bang tham so --cli-path, VD:")
        print(r'    --cli-path "C:\Duong\dan\STM32_Programmer_CLI.exe"')
        return False

    cmd = [exe] + CONNECT_ARGS + ["-w", hex_path, "-v", "-rst"]
    print("Dang chay:", " ".join(f'"{c}"' if " " in c else c for c in cmd))
    try:
        result = subprocess.run(cmd, capture_output=True, text=True)
    except Exception as e:
        print(f"Loi khi goi STM32_Programmer_CLI: {e}")
        return False

    print(result.stdout)
    if result.returncode != 0:
        print(result.stderr)
        print("=> Flash that bai.")
        return False

    print("=> Flash thanh cong, chip da duoc reset va chay firmware moi.")
    return True


def read_core_registers(target_type=DEFAULT_TARGET):
    """Halt CPU va doc cac thanh ghi core (R0-R12, SP, LR, PC, xPSR, ...)."""
    _require_pyocd()

    session = _open_session(target_type)
    if session is None:
        return

    with session:
        target = session.target
        print("Dang halt CPU de doc thanh ghi (chuong trinh se tam dung)...")
        try:
            target.halt()
        except Exception as exc:
            print(f"Khong halt duoc CPU: {exc}")
            print("Co the do chip dang o che do low-power hoac SWD khong truy cap duoc.")
            return

        print("\nGia tri thanh ghi core:\n")
        for name in CORE_REG_NAMES:
            try:
                val = target.read_core_register(name)
                print(f"  {name.upper():8s} = 0x{val:08X}")
            except Exception as e:
                print(f"  {name.upper():8s} = (khong doc duoc: {e})")

        print("\nDang resume CPU (chay tiep chuong trinh)...")
        try:
            target.resume()
        except Exception as e:
            print(f"Khong resume duoc CPU: {e}")

    print("=> Hoan tat doc thanh ghi.")


def read_memory(address, count=1, target_type=DEFAULT_TARGET):
    """Doc 'count' word 32-bit lien tiep bat dau tu 'address' (dung cho thanh ghi ngoai vi
    nhu RCC, GPIO, ... hoac bat ky vung nho nao trong khong gian dia chi cua chip)."""
    _require_pyocd()

    session = _open_session(target_type)
    if session is None:
        return

    with session:
        target = session.target
        print(f"Dang doc {count} word tai dia chi 0x{address:08X}...\n")
        try:
            if count <= 1:
                val = target.read32(address)
                print(f"  [0x{address:08X}] = 0x{val:08X}")
            else:
                values = target.read_memory_block32(address, count)
                for i, val in enumerate(values):
                    addr = address + i * 4
                    print(f"  [0x{addr:08X}] = 0x{val:08X}")
        except Exception as e:
            print(f"Loi khi doc bo nho: {e}")
            print("Kiem tra lai dia chi co hop le voi STM32F411CEU6 khong.")

    print("\n=> Hoan tat doc bo nho.")


def scan_ports(verbose=True):
    """Quet toan bo cong serial dang co, tra ve list cac cong cua ST-Link."""
    _require_pyserial()
    ports = list(list_ports.comports())
    stlink_ports = []

    for p in ports:
        is_st_vid = (p.vid == ST_VID)
        desc = (p.description or "").lower()
        hwid = (p.hwid or "").lower()
        matched_by_text = ("stlink" in desc or "st-link" in desc
                            or "stmicroelectronics" in desc
                            or "stlink" in hwid or "st-link" in hwid)

        if is_st_vid or matched_by_text:
            pid_name = ST_KNOWN_PIDS.get(p.pid, "Khong xac dinh PID (van co the la STM32)")
            stlink_ports.append({
                "device": p.device,
                "description": p.description,
                "vid": p.vid,
                "pid": p.pid,
                "hwid": p.hwid,
                "serial_number": p.serial_number,
                "pid_name": pid_name,
            })

    if verbose:
        print(f"Tim thay {len(ports)} cong serial tren he thong.")
        if not stlink_ports:
            print("=> Khong tim thay cong COM nao thuoc ST-Link.")
            print("   Kiem tra: da cam ST-Link chua, driver ST-Link VCP da cai chua,")
            print("   day cap co van de khong, hoac Windows Device Manager / dmesg (Linux).")
        else:
            print(f"=> Tim thay {len(stlink_ports)} cong nghi la ST-Link:\n")
            for i, sp in enumerate(stlink_ports, 1):
                print(f"[{i}] {sp['device']}")
                print(f"    Mo ta      : {sp['description']}")
                print(f"    VID:PID    : {sp['vid']:04X}:{sp['pid']:04X}" if sp['vid'] and sp['pid'] else "    VID:PID    : (khong doc duoc)")
                print(f"    Loai       : {sp['pid_name']}")
                print(f"    Serial No. : {sp['serial_number']}")
                print(f"    HWID       : {sp['hwid']}")
                print()

    return stlink_ports


def watch_mode():
    """Lien tuc quet, in ra khi co thay doi (cam them / rut cong)."""
    _require_pyserial()
    print("Che do theo doi (Ctrl+C de thoat). Dang quet moi 2 giay...\n")
    last_devices = set()
    try:
        while True:
            found = scan_ports(verbose=False)
            current_devices = {f["device"] for f in found}

            added = current_devices - last_devices
            removed = last_devices - current_devices

            for d in added:
                info = next(f for f in found if f["device"] == d)
                print(f"[+] Vua cam: {d}  ({info['description']})")
            for d in removed:
                print(f"[-] Vua rut: {d}")

            last_devices = current_devices
            time.sleep(2)
    except KeyboardInterrupt:
        print("\nDa dung theo doi.")


def _auto_int(value):
    """Cho phep nhap dia chi dang hex: 0x40023800 hoac so thuong."""
    return int(value, 0)


def main():
    parser = argparse.ArgumentParser(
        description="Do cong COM ao / flash hex / doc thanh ghi cho STM32 qua ST-Link"
    )
    parser.add_argument("--watch", action="store_true",
                         help="Tu dong quet lien tuc cong COM, bao khi cam/rut thiet bi")
    parser.add_argument("--flash", metavar="FILE.hex",
                         help="Duong dan file .hex da build, se duoc nap vao chip qua STM32CubeProgrammer CLI")
    parser.add_argument("--cli-path", metavar="PATH",
                         help="Duong dan toi STM32_Programmer_CLI.exe (neu khong tu tim thay)")
    parser.add_argument("--read-regs", action="store_true",
                         help="Halt CPU va doc cac thanh ghi core (R0-R12, SP, LR, PC, xPSR...) qua pyocd")
    parser.add_argument("--read-mem", metavar="ADDRESS", type=_auto_int,
                         help="Doc thanh ghi/vung nho tai dia chi chi dinh qua pyocd, VD: 0x40023800")
    parser.add_argument("--count", type=int, default=1,
                         help="So luong word 32-bit can doc lien tiep, dung voi --read-mem (mac dinh 1)")
    parser.add_argument("--target", default=DEFAULT_TARGET,
                         help=f"Target type cua pyocd cho doc thanh ghi/bo nho (mac dinh: {DEFAULT_TARGET}, "
                              f"khong can CMSIS-Pack; co the doi thanh 'stm32f411ce' neu da cai pack thanh cong)")
    args = parser.parse_args()

    # Cac hanh dong lien quan ST-Link/SWD (flash, doc thanh ghi/bo nho)
    if args.flash:
        flash_hex(args.flash, cli_path=args.cli_path)
        return
    if args.read_regs:
        read_core_registers(target_type=args.target)
        return
    if args.read_mem is not None:
        read_memory(args.read_mem, count=args.count, target_type=args.target)
        return

    # Mac dinh: do cong COM ao (VCP)
    if args.watch:
        watch_mode()
    else:
        scan_ports(verbose=True)


if __name__ == "__main__":
    main()