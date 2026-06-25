@echo off
set PATH=C:\QT\6.9.3\msvc2022_64\bin;%PATH%
set OCC="C:\Program Files\OpenCppCoverage\OpenCppCoverage.exe"
set EXE="C:\cov2\GraphDependencies\debug\GraphDependencies.exe"
set SRC="C:\cov2\GraphDependencies"
set COVDIR=covdata

if not exist %COVDIR% mkdir %COVDIR%
del /q %COVDIR%\*.cov 2>nul

for %%N in (01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68) do (
    if exist test%%N.txt (
        echo === TEST %%N ===
        %OCC% --sources %SRC% --export_type=binary:%COVDIR%\cov%%N.cov --quiet -- %EXE% test%%N.txt out%%N.txt
    )
)

echo === SPECIAL: no args ===
%OCC% --sources %SRC% --export_type=binary:%COVDIR%\cov_noargs.cov --quiet -- %EXE%
echo === SPECIAL: input not found ===
%OCC% --sources %SRC% --export_type=binary:%COVDIR%\cov_nofile.cov --quiet -- %EXE% nofile.txt out.dot
echo === SPECIAL: output fail ===
%OCC% --sources %SRC% --export_type=binary:%COVDIR%\cov_outfail.cov --quiet -- %EXE% test01.txt Z:\nonexistent\out.dot

echo === Merging into HTML ===
setlocal enabledelayedexpansion
set INPUTS=
for %%f in (%COVDIR%\*.cov) do set INPUTS=!INPUTS! --input_coverage=%%f
%OCC% !INPUTS! --sources %SRC% --export_type=html:MergedReport
endlocal

echo.
echo Done. Otchet: MergedReport\index.html
pause