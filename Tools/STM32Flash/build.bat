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
    python -m pip install pyinstaller
    if errorlevel 1 (
        echo [!] Cai pyinstaller that bai. Kiem tra lai pip/venv.
        pause
        exit /b 1
    )
)

echo [*] Dang don dep ban build cu (neu co)...
REM Dong tien trinh exe cu neu con dang chay, tranh loi overwrite file bi khoa.
for /f "skip=1 tokens=2" %%p in ('wmic process where "name='STM32Flashing.exe'" get processid 2^>nul') do (
    echo [*] Dung tien trinh STM32Flashing.exe cu (PID=%%p)
    taskkill /PID %%p /F >nul 2>&1
)

rmdir /s /q build 2>nul
rmdir /s /q dist 2>nul
if exist STM32Flashing.spec del /q STM32Flashing.spec

REM Bat buoc dung mode console de terminal log nam trong cung process voi app.
REM Neu dung --windowed/thiet lap console=False, exe se mo them terminal khac va
REM co the tat ngay khi app khoi dong hoac dong app khong giong nhu mong doi.
echo [*] Dang build STM32Flashing.exe ...
REM Bat buoc collect data cua pyocd vi pyocd can file sequences.lark khi khoi
REM dong va doc thanh ghi. Neu khong collect, exe se bi loi "No such file or directory: ...\pyocd\debug\sequences\sequences.lark".
pyinstaller --noconfirm --clean --onefile --console --collect-data pyocd --name STM32Flashing gui_stm32_flash.py

if errorlevel 1 (
    echo [!] Build that bai. Xem log ben tren de biet chi tiet.
    pause
    exit /b 1
)

echo.
echo [OK] Build xong! File exe nam tai: dist\STM32Flashing.exe
pause