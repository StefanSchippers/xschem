xcopy binary_template xschem-%1-win64 /E/H/C/I
cd xschem-%1-win64
copy "C:\Program Files\Xschem\bin\Xschem.exe" bin
copy "D:\Projects\XSchem\LICENSE" licenses
mkdir doc
cd doc
xcopy "C:\Program Files\Xschem\doc" . /E/H/C/I
cd ..
mkdir share
cd share
xcopy "C:\Program Files\Xschem\share" . /E/H/C/I
cd ..
mkdir xschem_library
cd xschem_library
xcopy "C:\Program Files\Xschem\xschem_library" . /E/H/C/I
cd ..
cd ..
tar.exe -a -c -f xschem-%1-win64.zip xschem-%1-win64
rmdir /s /q xschem-%1-win64
copy ..\XSchemWix\bin\Release\XSchem.msi .
rename XSchem.msi XSchem-%1-64bit.msi
