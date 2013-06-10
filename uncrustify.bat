@echo off

for %%d in (src) do (
	for %%e in (c cpp h cl) do (
		call:uncrustify "%%d/*.%%e"
	)
)

exit /b 0

:uncrustify
uncrustify.exe -q -l cpp -c uncrustify.cfg --no-backup "%~1"
goto:eof
