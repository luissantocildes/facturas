/****************************************************************************
 * Fichero que contiene las definiciones constantes del programa, tanto para*
 * el m¢dulo factura como para el m¢dulo control                            *
 ****************************************************************************
 * Fecha £ltima modificaci¢n : 18/11/96                                     *
 * (c)1996 Luis Fernando Santocildes Romero                                 *
 ****************************************************************************/

// Constantes para salida
#define PANTALLA 1
#define IMPRESORA 2

// Atributos de las funciones de dibujar l¡nea, imprimir cadenas y ventanas
#define SIN_MARCO 0
#define SUBRAYA 0x0100
#define HACE_CRUCES 0x0200
#define L_DOBLES 0x0400
#define L_SIMPLES 0x0000
#define L_CURVA 0x0800
#define L_RECTA 0x0000

// Definiciones de errores
#define OK 0
#define LISTA_VACIA 0
#define MEMORIA_AGOTADA 5
#define BLOQUE_MEMORIA_ERRONEO 6
#define CAMPO_INEXISTENTE 10
#define LISTA_CAMPOS_VACIA 15
#define FICHA_INEXISTENTE 65535
#define LISTA_FICHAS_VACIA 25
#define LISTA_NO_VACIA 30
#define ULTIMA_FICHA 35
#define PRIMER_FICHA 36
#define IMPOSIBLE_ABRIR_FICHERO 40
#define NOMBRE_FICHERO_INVALIDO 41
#define TIPO_DE_FICHERO_INVALIDO 42
#define TIPO_CAMPO_INVALIDO 50
#define FICHA_SIN_RELLENAR 60
#define CAMBIO 70
#define APARTADO_INVALIDO 80
#define APARTADO_YA_DEFINIDO 85
#define FIN_DE_LISTA 65535
#define SIN_ORDENAR 65535
#define ERROR 65535

// Definici¢n de los tipos de datos de los campos de la ficha
#define CADENA 0x01
#define ENTERO 0x02
#define REAL 0x03
#define MONETARIO 0x04
#define FECHA 0x05
#define HORA 0x06
#define TITULO 0x07
#define NO_SELEC 0x08
#define SUBRAYADO 0x10
#define COPIA_DATOS 0x40
#define OCULTO 0x80
#define CALCULA_TOTAL 0x100
#define ESP_EXTRA 0x200
#define TRAMA (OCULTO|COPIA_DATOS|CALCULA_TOTAL|ESP_EXTRA)

// Definici¢n de los criterios de ordenaci¢n de las fichas
#define ASCENDENTE 1
#define DESCENDENTE 2

/* definiciones para teclas */
// C¢digos ASCII normales
#define ESC 27
#define ENTER 13
#define LF 10      //<------ Este c¢digo se devuelve al pulsar CTRL-ENTER
#define TAB 9
#define BS 8       // Back-Space (tecla de borrar)
// C¢digos extendidos
#define T_control 0
#define INS 82
#define S_TAB 15   //<------ SHIFT + TAB
#define CURS_ARR 72
#define CURS_ABJ 80
#define CURS_IZQ 75
#define CURS_DER 77
#define INICIO 71
#define FIN 79
#define REPAG 73
#define AVPAG 81
#define SUPR 83
#define CTRL_REPAG 132
#define CTRL_AVPAG 118
#define F1 59
#define F2 60
#define F3 61
#define F4 62
#define F5 63
#define F6 64
#define F7 65
#define F8 66
#define F9 67
#define F10 68

// Definiciones booleanas comunes
#define SI TRUE
#define NO FALSE
#define NO_HAY_MEMORIA FALSE
#define CORRECTO TRUE

#define FIN_LINEA 13           /* definiciones para formato de lineas */
#define NUEVA_LINEA 10
#define FIN_TEXTO 0

#define COLOR 0x1700                   /* definiciones para pantalla */
#define COLUMNAS_PANTALLA 80
#define MAX_X 80
#define MAX_Y 25
#define PRIMERA_COLUMNA 5
#define LINEA_SUPERIOR 1

//Clave para reconocer que los ficheros de datos pertenecen al programa de
// facturaci¢n y no a otros
#define CLAVE_FICH "FACTURAS"

// Nuevo nombre para el complemento a 1
#define NEG ~

// Constantes para las facturas
#define LARGO_CAMPO_FACT 50

// Constantes que indican el valor devuelto en busqueda_binaria
#define INDICE 0
#define NRO_FICHA 1

// Constantes para tama¤o de memoria, pasan de Kb a bytes
#define _1K 1024
#define _2K 2048
#define _4K 4096
#define _8K 8192
#define _16K 16384
#define _32K 32768
#define _64K 65536