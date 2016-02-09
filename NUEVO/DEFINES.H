/****************************************************************************
 * Fichero que contiene las definiciones constantes del programa, tanto para*
 * el m¢dulo factura como para el m¢dulo control                            *
 ****************************************************************************
 * Fecha creaci¢n : 5/4/96                                                  *
 * Fecha £ltima modificaci¢n : 25/4/97                                      *
 * (c)1996 Luis Fernando Santocildes Romero                                 *
 ****************************************************************************/

// Constantes para salida
#define PANTALLA 1
#define IMPRESORA 2

// Atributos de las funciones de dibujar l¡nea, imprimir cadenas y ventanas
#define SIN_MARCO 0
#define M_GRUESO 1
#define M_FINO 2
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
#define CADENA 0x01            // 0000000000000001
#define ENTERO 0x02            // 0000000000000010
#define REAL 0x03              // 0000000000000011
#define MONETARIO 0x04         // 0000000000000100
#define FECHA 0x05             // 0000000000000101
#define HORA 0x06              // 0000000000000110
#define TITULO 0x07            // 0000000000000111
#define NO_SELEC 0x08          // 0000000000001000
#define SUBRAYADO 0x10         // 0000000000010000
#define ALINEA_DER 0x20        // 0000000000100000
#define COPIA_DATOS 0x40       // 0000000001000000
#define OCULTO 0x80            // 0000000010000000
#define CALCULA_TOTAL 0x100    // 0000000100000000
#define ESP_EXTRA 0x200        // 0000001000000000
#define TRAMA 0x3C0            // 0000001101001000

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

                                /* definiciones para pantalla */
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

// Constantes de men£s
#define _CADENA 0x0001     // 0000000000000001
#define _SEPARADOR 0x0002  // 0000000000000010
#define _MARCADO 0x0004    // 0000000000000100
#define _ACTIVADO 0x0008   // 0000000000001000
#define _GRIS 0x0010       // 0000000000010000
#define _MPOPUP 0x0020     // 0000000000100000
#define _PRINCIPAL 0x0040  // 0000000001000000
#define _OCULTO 0x0080     // 0000000010000000

#define FIN_MENU 65535

// Valores devueltos por algunas funciones de men£
#define _ESCOGE 0
#define _CANCELA -1
#define _IZQUIERDA -2
#define _DERECHA -3
#define _SIN_SUBMENU -4
#define _SIN_OPCIONES -5
#define _NO_HAY_MENU -6

#define MAXFILENAME MAXFILE + MAXEXT
#define MAX_BUF_TEXT 8191
/********************************** Macros ********************************/
