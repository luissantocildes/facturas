@echo off
echo Instalando Facturas...
if "%1" == "" goto error1
if "%2" == "" goto error1
if exist %2\factura\nul. goto instala
md %2\factura >nul

:instala
cpu.com
if errorlevel 8 goto no_instala
if errorlevel 1 goto sin_copro

echo Instalando la versi¢n para ordenadores con coprocesador matem tico
copy %1\prog_1\*.* %2\factura > nul
ren %2\factura\f_math.exe %2\factura\factura.exe > nul
goto correcto

:sin_copro
echo Instalando la versi¢n para ordenadores sin coprocesador matem tico
copy %1\prog_2\*.* %2\factura > nul
ren %2\factura\f_emu.exe %2\factura\factura.exe > nul
goto correcto

:no_instala
echo Lo siento, el programa Facturas necesita un ordenador 386 o superior para
echo funcionar.
goto fin2

:error1
echo Para instalar el programa escriba INSTALAR unidad_origen: unidad_destino:
echo Por ejemplo:  INSTALAR A: C:
goto fin2

:correcto
readme.com \docs\manual.txt
cls
echo El programa se ha instalado en %2\factura
if NOT "%2" == "C:" del %2\factura\factura.cfg>nul

:fin2
