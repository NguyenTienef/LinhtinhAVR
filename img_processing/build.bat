@echo off
echo === Cleaning build folder ===
if exist build rmdir /s /q build
mkdir build
cd build

echo === Running CMake ===
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..

echo === Building ===
cmake --build . -j4

echo === Copying DLLs ===
cd ..
copy "C:\Users\Admin\Desktop\Cfile\img_processing\opencv\build\install\x64\mingw\bin\*.dll" ".\build\"
copy "C:\msys64\ucrt64\bin\libstdc++-6.dll" ".\build\"
copy "C:\msys64\ucrt64\bin\libgcc_s_seh-1.dll" ".\build\"
copy "C:\msys64\ucrt64\bin\libwinpthread-1.dll" ".\build\"

echo === Done ===
