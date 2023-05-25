@echo off

if %1. EQU . (
	echo %~0 [reset or create] FILE 
	goto end
) 
if %1% equ create ( 
	for /F "usebackq delims=" %%A in (`dir /b *.c`) do gawk -f create_alloc_ids_windows.awk %1 %%A
	for /F "usebackq delims=" %%A in (`dir /b *.h`) do gawk -f create_alloc_ids_windows.awk %1 %%A
	for /F "usebackq delims=" %%A in (`dir /b *.y`) do gawk -f create_alloc_ids_windows.awk %1 %%A
	for /F "usebackq delims=" %%A in (`dir /b *.l`) do gawk -f create_alloc_ids_windows.awk %1 %%A
	goto end
) 
if %1% equ reset (
	for /F "usebackq delims=" %%A in (`dir /b *.c`) do gawk -f create_alloc_ids_windows.awk %1 %%A
	for /F "usebackq delims=" %%A in (`dir /b *.h`) do gawk -f create_alloc_ids_windows.awk %1 %%A
	for /F "usebackq delims=" %%A in (`dir /b *.y`) do gawk -f create_alloc_ids_windows.awk %1 %%A
	for /F "usebackq delims=" %%A in (`dir /b *.l`) do gawk -f create_alloc_ids_windows.awk %1 %%A
	goto end
) 
echo %~0 [reset or create] FILE

:end