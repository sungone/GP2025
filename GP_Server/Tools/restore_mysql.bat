@echo off

set FILE_NAME=gp2025.sql

set DB_USER=serverdev
set DB_PASSWORD=pass123!
set DB_NAME=gp2025
set BACKUP_DIR=%~dp0Backups\

set FULL_PATH=%BACKUP_DIR%%FILE_NAME%

mysql -u %DB_USER% -p%DB_PASSWORD% %DB_NAME% < "%FULL_PATH%"

echo Completed: %FULL_PATH%
