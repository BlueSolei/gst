pushd %~dp0
cd build\bin
if "%1"=="tests" (gstTest.exe) else (gst.exe)
popd
