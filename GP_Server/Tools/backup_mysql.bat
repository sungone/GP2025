@echo off

set DB_USER=serverdev
set DB_PASSWORD=pass123!
set DB_NAME=gp2025
set BACKUP_DIR=%~dp0Backups\
set FILE_NAME=%DB_NAME%.sql

if not exist "%BACKUP_DIR%" (
    mkdir "%BACKUP_DIR%"
)

echo [INFO] Starting MySQL backup...

mysqldump ^
 --user=%DB_USER% ^
 --password=%DB_PASSWORD% ^
 --databases %DB_NAME% ^
 --single-transaction ^
 --skip-lock-tables ^
 --no-tablespaces ^
 > "%BACKUP_DIR%%FILE_NAME%"

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Backup failed. Please check MySQL credentials or access rights.
    exit /b 1
)

echo [OK] Backup completed: %FILE_NAME%
pause
