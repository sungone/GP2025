@echo off

set FILE_NAME=gp2025.sql

set DB_USER=serverdev
set DB_PASSWORD=pass123!
set DB_NAME=gp2025
set BACKUP_DIR=%~dp0Backups\

set FULL_PATH=%BACKUP_DIR%%FILE_NAME%

echo [INFO] Restoring database "%DB_NAME%" from "%FILE_NAME%"

mysql --user=%DB_USER% --password=%DB_PASSWORD% %DB_NAME% < "%BACKUP_DIR%%FILE_NAME%"

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Restore failed. Check table definitions or constraints.
    exit /b 1
)

echo [OK] Restore completed successfully.
pause