:: Open from the Visual Studio Developer Console, so msbuild.exe is on PATH.
:: You also need to manually add ILMerge to PATH.
::
:: Use only for final builds.
@echo off

cd cmalcor
del build
del build_temp
mkdir build_temp
premake5 vs2019 && cd build && msbuild CmAlcor.sln /p:configuration=Release /p:platform=Win32 && cd ..
if %errorlevel% neq 0 exit /b %errorlevel%
move bin build_temp/alcor
del build
premake5 vs2019 --mizar && cd build && msbuild CmAlcor.sln /p:configuration=Release /p:platform=Win32 && cd ..
if %errorlevel% neq 0 exit /b %errorlevel%
move bin build_temp/mizar
del build
cd ..

cd cmalcor-gui
msbuild CmAlcorGUI.sln /t:Build /p:Configuration=Release /p:TargetFramework=v4.0
if %errorlevel% neq 0 exit /b %errorlevel%
cd ..

mkdir bin
mkdir bin\mizar_patch

set ILMERGE_PATH=%USERPROFILE%\.nuget\packages\ilmerge\3.0.41\tools\net452
copy /y /d "cmalcor\build_temp\alcor\cmalcor.exe" "bin\"
copy /y /d "cmalcor\build_temp\alcor\cmalcor.dll" "bin\"
copy /y /d "cmalcor\build_temp\mizar\cmalcor.exe" "bin\mizar_patch\"
copy /y /d "cmalcor\build_temp\mizar\cmalcor.dll" "bin\mizar_patch\"
%ILMERGE_PATH%\ilmerge.exe /targetplatform:v4 /out:bin/CmAlcorGUI.exe "cmalcor-gui/bin/Release/CmAlcorGUI.exe" "cmalcor-gui/bin/Release/Cyotek.Windows.Forms.ColorPicker.dll"
copy /y /d "cmalcor-gui\bin\Release\DarkUI.dll" "bin\"
copy /y /d "cmalcor-gui\bin\Release\CmAlcorGUI.exe.config" "bin\"
del bin\CmAlcorGUI.pdb

echo Better viewed on GitHub: https://github.com/thelink2012/cmalcor/blob/master/README.md > bin/readme.txt
echo. >> bin/readme.txt
type README.md >> bin/readme.txt

echo Those files allows the application to work with the CM Storm Mizar. >bin/mizar_patch/readme.txt
echo. >>bin/mizar_patch/readme.txt
echo Copy and replace the files in the directory CmAlcorGUI.exe is in. >>bin/mizar_patch/readme.txt
echo. >>bin/mizar_patch/readme.txt
echo I don't own a Mizar and did very small tests on someone that owns it, so I can't guarantee that this works great. >>bin/mizar_patch/readme.txt
echo. >>bin/mizar_patch/readme.txt
echo If it breaks something, see the 'Troubleshooting ^> CoolerMaster Warranty' section of the main readme as it explains how to erase the memory. >>bin/mizar_patch/readme.txt
echo. >>bin/mizar_patch/readme.txt

echo =================== https://github.com/thelink2012/cmalcor =================== > bin/license.txt
echo. >> bin/license.txt
type LICENSE >> bin/license.txt
echo. >> bin/license.txt
echo. >> bin/license.txt
echo =================== https://github.com/docopt/docopt.cpp =================== >> bin/license.txt
echo. >> bin/license.txt
type cmalcor\deps\docopt\LICENSE-MIT >> bin/license.txt
echo. >> bin/license.txt
echo. >> bin/license.txt
echo ========= https://github.com/cyotek/Cyotek.Windows.Forms.ColorPicker ========= >> bin/license.txt
echo. >> bin/license.txt
type cmalcor-gui\Dependencies\Cyotek.Windows.Forms.ColorPicker\license.txt >> bin/license.txt
echo. >> bin/license.txt
echo. >> bin/license.txt
echo =================== https://github.com/RobinPerris/DarkUI =================== >> bin/license.txt
echo. >> bin/license.txt
type cmalcor-gui\Dependencies\DarkUI\LICENSE >> bin/license.txt
echo. >> bin/license.txt
