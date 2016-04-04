:: Open from the Visual Studio Developer Console, so msbuild.exe is on PATH.
:: You also need to manually add ILMerge to PATH.
::
:: Use only for final builds.
@echo off

cd cmalcor
premake5 vs2015 && msbuild CmAlcor.sln /p:configuration=Release /p:platform=Win32
if %errorlevel% neq 0 exit /b %errorlevel%
cd ..

cd cmalcor-gui
msbuild CmAlcorGUI.sln /t:Build /p:Configuration=Release /p:TargetFramework=v4.0
if %errorlevel% neq 0 exit /b %errorlevel%
cd ..

mkdir bin

copy /y /d "cmalcor\bin\cmalcor.exe" "bin\"
copy /y /d "cmalcor\bin\cmalcor.dll" "bin\"
ilmerge /targetplatform:v4 /out:bin/CmAlcorGUI.exe "cmalcor-gui/bin/Release/CmAlcorGUI.exe" "cmalcor-gui/bin/Release/Cyotek.Windows.Forms.ColorPicker.dll"
copy /y /d "cmalcor-gui\bin\Release\DarkUI.dll" "bin\"
copy /y /d "cmalcor-gui\bin\Release\CmAlcorGUI.exe.config" "bin\"
del bin\CmAlcorGUI.pdb

echo Better viewed on GitHub: https://github.com/thelink2012/cmalcor/blob/master/README.md > bin/readme.txt
echo. >> bin/readme.txt
type README.md >> bin/readme.txt

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
