# build-run.ps1
# Navigate to the build folder
Set-Location -Path "C:\PixelPong\PixelPong-build"

# Build the project
cmake --build . --config Debug

# Run the executable and wait
Start-Process ".\src\Debug\PixelPong.exe" -WorkingDirectory ".\src\Debug" -Wait