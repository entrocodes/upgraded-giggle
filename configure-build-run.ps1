# build-run.ps1

Set-Location -Path "C:\PixelPong\PixelPong-build"

Write-Host "Configuring project..."
cmake -DBUILD_SHARED_LIBS=OFF ../PixelPong
if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configure failed"
    exit $LASTEXITCODE
}

Write-Host "Building project..."
cmake --build . --config Debug
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed"
    exit $LASTEXITCODE
}

Write-Host "Build succeeded. Launching PixelPong..."
Start-Process ".\src\Debug\PixelPong.exe" `
    -WorkingDirectory ".\src\Debug" `
    -Wait
