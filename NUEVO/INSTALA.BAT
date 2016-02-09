@echo off
echo Instalando Facturas...
if exist %2\factura goto hay_direc
md %2:\factura
REM se verifica que exista el directorio, si existe se pasa directo a la
REM instalaci¢n, sino se crea.

:hay_direc
cpu.com
if errorlevel 8 goto no_instala
if errorlevel 1 goto sin_copro
REM Verifica el tipo de ordenador. Si es un 386 o superior con coprocesador
REM se instala el programa para coprocesador.

copy %1\prog_1\*.* %2\factura
ren %2\factura\f_math.exe %2\factura\factura.exe
goto fin

:sin_copro
copy %1\prog_2\*.* %2\factura
ren %2\factura\f_emu.exe %2\factura\factura.exe
goto fin
REM Si no hay copro se instala la versi¢n con emulador.

:no_instala
echo Lo siento, el programa Facturas necesita un ordenador 386 o superior para
echo funcionar.
goto fin2
REM Si no hay un 386 como m¡nimo, no se instala el programa.

:fin
echo El programa se ha instalado en %2\factura

:fin2