@echo off
setlocal

python -m pip install --upgrade pip
python -m pip install -r requirements.txt pyinstaller
python -m PyInstaller --noconfirm --clean --onefile --windowed ^
  --name ArduinoFlashTool app.py

if errorlevel 1 (
  echo Build failed.
  exit /b 1
)

echo.
echo Build completed: dist\ArduinoFlashTool.exe

