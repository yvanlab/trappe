@echo off
echo Starting ESP Trappe Interface Simulator...
echo.
echo The web interface will be available at: http://localhost:8080
echo.
echo Press Ctrl+C to stop the server
echo.
powershell -ExecutionPolicy Bypass -File simulate_server.ps1
pause