@echo off
REM ==========================================================================
REM build.bat - Dong goi gui_stm32_flash.py + find_stlink_com.py thanh 1 file
REM             STM32Flashing.exe duy nhat bang PyInstaller.
REM
REM Cach dung:
REM   1) Dat file nay CUNG THU MUC voi gui_stm32_flash.py va find_stlink_com.py
REM   2) Mo cmd, activate venv da co pyserial + pyocd (venv da dung khi test)
REM   3) Chay: build.bat
REM   4) File ket qua nam tai: dist\STM32Flashing.exe
REM ==========================================================================

setlocal

where pyinstaller >nul 2>nul
if errorlevel 1 (
    echo [*] Chua co pyinstaller, dang cai dat...
    pip install pyinstaller
    if errorlevel 1 (
        echo [!] Cai pyinstaller that bai. Kiem tra lai pip/venv.
        pause
        exit /b 1
    )
)

echo [*] Dang don dep ban build cu (neu co)...
rmdir /s /q build 2>nul
rmdir /s /q dist 2>nul
del /q STM32Flashing.spec 2>nul

echo [*] Dang build STM32Flashing.exe ...
pyinstaller --noconfirm --onefile --windowed --name STM32Flashing gui_stm32_flash.py

if errorlevel 1 (
    echo [!] Build that bai. Xem log ben tren de biet chi tiet.
    pause
    exit /b 1
)

echo.
echo [OK] Build xong! File exe nam tai: dist\STM32Flashing.exe
echo      (file log stm32_flash_tool.log va crash.log se duoc tao CUNG THU MUC voi .exe khi chay)
pause