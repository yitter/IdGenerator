@echo off

set result=[OK]
set tag=[OK]
set msg="auto commit"

echo --------------------------------------------------------

if not "%1" == "" (
	SET msg=%1
)

git add -A
git commit -am %msg%


git push
if "%errorlevel%"=="1" goto ERR

goto END

:ERR
	set result=[error]
	set tag=[error]
	
:END


echo %tag% result: %result%
echo ========================================================

if "%tag%"=="×" (
	SET __ERROR__=true
	@pause
)

:QUIT
