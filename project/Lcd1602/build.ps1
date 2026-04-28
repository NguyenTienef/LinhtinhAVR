# Xóa build cũ nếu có
if (Test-Path build) {
    Remove-Item build -Recurse -Force
}

# Tạo build
cmake -S . -B build -G "MinGW Makefiles"

# Build
cmake --build build