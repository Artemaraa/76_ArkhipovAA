@echo off
set RES=results.txt
set PATH=C:\QT\6.9.2\mingw_64\bin;%PATH%
echo Results > %RES%

REM ===== Обычные тесты (вход -> выход) =====
for %%N in (01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67) do (
echo. >> %RES%
echo ===== TEST %%N ===== >> %RES%
type test%%N.txt >> %RES% 2>^&1
echo --- output: --- >> %RES%
GraphDependencies.exe test%%N.txt out%%N.txt >> %RES% 2>^&1
if exist out%%N.txt type out%%N.txt >> %RES% 2>^&1
)

REM ===== Специальные запуски (аварийные ветки) =====
echo. >> %RES%
echo ===== SPECIAL: no args (argc^<3) ===== >> %RES%
GraphDependencies.exe >> %RES% 2>^&1

echo. >> %RES%
echo ===== SPECIAL: input file not found ===== >> %RES%
GraphDependencies.exe nofile.txt out.dot >> %RES% 2>^&1

echo. >> %RES%
echo ===== SPECIAL: output create fail ===== >> %RES%
GraphDependencies.exe test01.txt Z:\nonexistent\out.dot >> %RES% 2>^&1

echo Done
pause