/*************************************************************************
 * Fichero : CONTROL.CPP                                                 *
 * Fichero que contiene las funciones necesarias para la impresion de    *
 * las lineas de texto, y para el control de la pantalla (scroles,       *
 * inicializar y restaurar la pantalla, etc). Tambien contiene todos     *
 * los ficheros necesarios, y las definiciones                           *
 *************************************************************************
 * Fecha £ltima modificaci¢n : 28/5/97                                   *
 * (c)1996,97 Luis Fernando Santocildes Romero                           *
 *************************************************************************
 * V1.5 : 31/7/96                                                        *
 *        M¢dulo para la V1.0 de FACTURA                                 *
 *        - Mejora de las funciones de ventanas.                         *
 *        - Funciones para un sencillo control de men£s, barra de estado *
 *          y selecci¢n de opciones, con la posibilidad de recalcar      *
 *          teclas.                                                      *
 *        - Reloj de tiempo real, con la fecha, en la esquina inferior   *
 *          derecha del monitor.                                         *
 *        - Funci¢n para un sonido de error.                             *
 *        - Varias cadenas con mensajes comunes al sistema de men£s.     *
 *        - Funciones para guardar y leer la configuraci¢n.              *
 *        - Funciones para imprimir cadenas dentro de una ventana cuyo   *
 *          origen no est‚ en el (0,0).                                  *
 *        - Lectura de cadenas con capacidad de edici¢n.                 *
 *        - Paso de papel en impresora.                                  *
 *        - Funciones para poder imprimir en cualquier puerto, incluyendo*
 *          ficheros.                                                    *
 *        - Lectura de fechas en el formato correcto                     *
 *        - Detecci¢n y manejo de las funciones del estandar de video    *
 *          VESA.                                                        *
 *        - Dibujo de l¡neas con recorte en texto.                       *
 *                                                                       *
 * V1.6 : 2/1/97                                                         *
 *        - Se mejora el control de los colores en las ventanas          *
 *        - Se redefine el sistema de men£s, pudi‚ndose usar men£s de    *
 *          persiana.                                                    *
 *        - Se pasa el sistema de men£s a MENUS.CPP                      *
 *        - Se a¤ade una funci¢n que activa un modo de 132 columnas.     *
 * V1.7 : 28/7/97                                                        *
 *        - Se mejora la funci¢n que activa el modo de 132 columnas.     *
 *************************************************************************/

/*********** Protecci¢n contra una compilaci¢n incorrecta ******************/
#if defined (_Windows)
  #error Este programa es para MS-DOS.
#endif
// Si se intenta compilar como un programa para Windows, da error

#ifndef __MSDOS__
  #error Este programa es para MS-DOS.
#endif
// Si se intenta compilar bajo un OS que no sea el MS-DOS, da error

// Ficheros de cabecera necesarios
#include <dos.h>
#include <process.h>
#include <alloc.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dir.h>
#include "defines.h"
#include "tipos.h"
#include "boolean.h"
#include "fichas.h"
#include "vesa.h"

/************************** E S T R U C T U R A S ***************************/
/* Estructura que almacena los datos necesarios para almacenar el trozo de
   pantalla donde se dibujar  la ventana */
typedef struct
         {text_info info_pantalla; // Datos sobre el modo de v¡deo actual
          char *datos;             // Puntero a los datos de la pantalla
         } Tpantalla;

// Define un fichero temporal
typedef struct
  {char nombre[MAXPATH];   // nombre del fichero
   FILE *fichero;          // fichero
  } TFtemporal;

typedef struct
  {unsigned short texto_barra,      letra_barra,
                  seleccion_barra,  seleccion_letra,
                  borde_menu,       seleccion_menu,   texto_menu,
                  desactivado_menu, letra_menu,       seleccion_letra_menu,
                  borde_ventana,    fondo_ventana,    texto_ventana,
                  texto_r_ventana,
                  boton,            letra_boton,      boton_defecto,
                  boton_escogido,
                  texto_entrada,
                  escritorio;
  } Tcolor;
/************************** PROTOTIPOS *************************************/
void pascal dibuja_escritorio (void);
                /* Imprime el fondo de la pantalla e inicializa los colores */
void pascal pone_barra_inferior (const char *barra, unsigned linea = 0);
                                               /* Imprime la barra inferior */
void pascal scrollabajo (boolean respeta = FALSE);
                                           /* Realiza un scroll hacia abajo */
void pascal scrollarriba (boolean respeta = FALSE);
                                                  /* Idem pero hacia arriba */
void pascal scrollizquierda (unsigned nro_lineas);
void pascal scrollderecha (unsigned nro_lineas);
     /* Realiza un desplazamiento de la pantalla a la izquierda o a la derecha
        tantas l¡neas como se le indique */
Tpantalla pascal hace_recuadro (byte x, byte y, byte ancho, byte alto,
                                char *cadena = NULL, int tipo_marco = 1);
                                       /* Dibuja un recuadro en la pantalla */
Tpantalla pascal guarda_recuadro (byte x, byte y, byte ancho, byte alto,
                                  int tipo_marco = 1);
                 /* almacena una zona rectangular de la pantalla en memoria */
void pascal restaura_recuadro (char *Tpantalla);
                /* restaura la zona de pantalla guardada por la f. anterior */
void pascal color_ventana (unsigned color, unsigned color_fondo,
                           unsigned color_borde);
                                 /* Cambia el color del marco de la ventana */
int pascal aviso (char *texto, byte x, byte y, byte ancho, byte alto);
                               /* cuadro de dialogo con dos botones (SI/NO) */
void pascal mensaje (char *texto, byte x, byte y, byte ancho, byte alto);
                          /* cuadro de dialogo que solo presenta un mensaje */
boolean pascal pregunta (char *texto, byte x, byte y, byte ancho, byte alto, char *respuesta,
               int long_resp, boolean tipo_cadena = TRUE);
            /* cuadro de dialogo con un campo para introducir una respuesta */
void pascal pone_modo_cursor (int ins);
                                                        /* cambia el cursor */
void pascal controla_boton (int *boton, int alto);
                              /* controla la seleccion del boton SI o el NO */
void pascal imprime_botones (int boton, int alto);
    /* imprime los botones SI / NO en colores diferentes, segun el escogido */
void pascal borra_resto_linea (unsigned int pos_cursor);
                                  /* borra la linea fisica desde pos_cursor */
void pascal imprime_letra (char caracter, unsigned repeticiones);
                                                       /* imprime una letra */
void pascal fin_de_memoria (int tipo_de_error);
                                             /* avisa del fin de la memoria */
void pascal instala_reloj (void);
                                         /* Instala el controlador de reloj */
void pascal desinstala_reloj (void);
                                           /* Quita el controlador de reloj */
void pascal sonido_error (void);
                                                       /* Produce un pitido */
void pascal inicializa_programa (char *nombre_clientes, char *nombre_articulos,
                                 char *nombre_facturas, char *nombre_fact,
                                 double *iva, unsigned long *ultima_factura,
                                 char *directorio_trabajo);
                                /* Inicializa la configuraci¢n del programa */
void pascal restaura_ordenador (char *nombre_clientes, char *nombre_articulos,
                                char *nombre_facturas, char *nombre_fact,
                                double iva, unsigned long ultima_factura,
                                char *directorio_trabajo);
   /* Restaura el ordenador a un estado parecido al que ten¡a antes de usar el
      programa */
boolean pascal leer_configuracion (char *nombre_clientes, char *nombre_articulos,
                                   char *nombre_facturas, char *nombre_fact,
                                   double *iva, unsigned long *ultima_factura,
                                   char *directorio_trabajo);
                            /* Lee la configuraci¢n del fichero FACTURA.CFG */
void pascal salvar_configuracion (char *nombre_clientes, char *nombre_articulos,
                                  char *nombre_facturas, char *nombre_fact,
                                  double iva, unsigned long ultima_factura,
                                  char *directorio_trabajo);
                        /* Salva la configuraci¢n en el fichero FACTURA.CFG */
void pascal pone_caracter (char caracter, unsigned columna, unsigned fila,
                    unsigned atributos);
             /* Pone un caracter en pantalla, teniendo en cuenta el recorte */
void pascal linea_vert (unsigned columna, unsigned fila, unsigned largo,
                        unsigned atributos);
                                               /* Dibuja una l¡nea vertical */
void pascal linea_horz (unsigned columna, unsigned fila, unsigned largo,
                        unsigned atributos);
                                             /* Dibuja una l¡nea horizontal */
void pascal imprime_cadena (char *cadena, unsigned columna, unsigned fila,
                            unsigned atributos = YELLOW | (BLUE << 4),
                            boolean desplaza = TRUE);
                                       /* Imprime una cadena en la pantalla */
unsigned pascal lee_cadena (char *cadena, unsigned max_car, boolean alfanumerica = TRUE,
                            boolean nueva = TRUE, boolean sale = FALSE,
                            char caracter_relleno = ' ');
                                         /* Lee una cadena desde el teclado */
void pascal saca_papel (void);
                                          /* Saca la p gina de la impresora */
void pascal completa_cadena (char *cadena, unsigned longitud,
                             char caracter_relleno = ' ',
                             boolean pone_crlf = FALSE);
                          /* Completa una cadena con un caracter determinado y
                             coloca un retorno de carro al final si hace falta
                          */
char * pascal gotoxy_text (char *destino, char *origen, unsigned x);
              /* Mete una cadena dentro de otra en una posici¢n determinada */
void pascal pide_fecha (char *texto, byte x, byte y, byte ancho, byte alto,
                        date *fecha, boolean nueva = TRUE);
                              /* Pide una fecha y verifica que sea correcta */
TFtemporal * pascal inicializa_impresion (void);
                                      /* Inicializa el fichero de impresion */
unsigned pascal envia_datos (char *cadena, TFtemporal *fichero);
                                            /* Prepara los datos a imprimir */
unsigned pascal imprime (TFtemporal *fichero, boolean pasa_papel = TRUE,
                         unsigned copias = 0);
                    /* Imprime los datos en el dispositivo de salida actual */
void pascal cancela_impresion (TFtemporal *fichero);
                                /* Imprime el fichero de impresion temporal */
void pascal cierra_impresion (TFtemporal *fichero);
                      /* Cierra y borra los fichero temporales de impresi¢n */
boolean pascal existe_fichero (char *nom_fich);
                               /* Verifica si existe un determinado fichero */
void pascal imprime_fecha (TFtemporal *fichero);
                          /* Imprime la fecha en el fichero temporal pasado */
int pascal escribe_cadena (char *palabra, int x, int y, int color = 0,
                           boolean respeta_borde = TRUE);
            /* Escribe una cadena en pantalla, usando los colores indicados */
long pascal filesize(FILE *stream);
                                       /* Calcula la longitud de un fichero */
char * pascal busca_cadena (char *cadena2, char *cadena1,
                            boolean mayusculas = FALSE);
                                         /* Busca una cadena dentro de otra */
void pascal imprime_lista (Cbase_datos &Objeto, char tipo_lista,
                           unsigned fichas_ini, unsigned fichas_fin);
                                                 /* Imprime la lista pasada */
unsigned pascal muestra_lista (Cbase_datos &Objeto, char tipo_lista,
                               unsigned fichas_ini, unsigned fichas_fin,
                               unsigned nro_lineas, boolean escoge = FALSE);
                            /* muestra una lista de clientes o de art¡culos */
boolean pascal pone_modo_132 (void);
                                 /* Activa un modo de v¡deo de 132 columnas */
void pascal pone_modo_normal (void);
                                        /* Restaura el modo normal de v¡deo */
unsigned long pascal dos2luc (struct date *fecha);
                   /* Pasa la fecha de formato dia/mes/a¤o a un nro. entero */
void pascal luc2dos (unsigned long fecha, struct date *fecha_des);
                         /* Pasa de un nro. entero a una fecha determinada. */
/***************************VARIABLES GLOBALES****************************/
boolean reloj_visible = FALSE,   // Controla si el reloj es visible
        reloj_instalado = FALSE, // Idem pero si este est  instalado
        hace_sonido = TRUE;       // Indica si hace los sonidos

// N§ del modo de 132 columnas
byte modo_132 = 0;
unsigned modo_vesa = 0;

// Se usa para determinar que m‚todo usar para activar un modo de 132
// columnas.
boolean vesa_instalada;

int nro_copias,                   // Nro de copias a imprimir
    lineas_papel,                 // L¡neas por p gina
    puerto_conexion;              // Puerto de conexi¢n de la impresora

char directorio_programa[MAXPATH];
// Unidad y directorio donde est  el programa;

boolean configuracion_modificada = FALSE;

char fondo[] = "°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°";
 // Fondo de la pantalla
char barra2[] = "                                                                                ";
 // Barra normal

unsigned char marco[3][11] =
    {{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
     {'É', 'Í', 'Ë', '»', 'º', 'Ì', 'Î', '¹', 'È', 'Ê', '¼'},
     {'Ú', 'Ä', 'Â', '¿', '³', 'Ã', 'Å', '´', 'À', 'Á', 'Ù'}};

// Mensajes varios
char pres_enter[] = " Pulse &ENTER& para continuar...";
char pres_tecla[] = " Pulse una tecla para continuar...";
char esc_tab[] = " Escoja con el tabulador y pulse &ENTER&, o pulse &S& o &N&.";
char esc_opcion[] = " Escoja una opci¢n o pulse &ESC& para volver al men£.";
char esc_sino[] = " Escoja con &S& o &N&.";
char imp_fact[] = " &I& imprime la factura, &ESC& la anula, otra tecla vuelve al men£.";
char pres_esc[] = " Presione &ESC& para volver al men£.";
char nuevo_cli[] = " Nuevo cliente ³ &CTRL-ENTER& a¤ade el cliente, &ESC&"
                   " cancela";
char buscar_cli[] = " Busca clientes ³ &ESC& vuelve al men£";
char modif_cli[] = " Edita cliente ³ &CTRL-ENTER& acepta, &ESC& cancela";
char nuevo_art[] = " Nuevo art¡culo ³ &CTRL-ENTER& a¤ade el art¡culo, "
                   "&ESC& cancela ";
char busca_art[] = " Busca un art¡culo ³ &ESC& vuelve al men£";
char edit_art[] = " Edita la ficha ³ &CTRL-ENTER& acepta, &ESC& cancela";
char codigo_cliente[] = " Escriba el c¢digo del cliente y pulse &ENTER& para"
                        " continuar.";
char _fecha[] = "Pulse &CTRL-ENTER& para la fecha del d¡a.";
char menu_prin[] = " Mu‚vase con los cursores y escoja con &ENTER&.";
char _pregunta[] = " Escriba el nuevo valor y pulse &ENTER& para aceptar. &ESC& anula.";
char teclas_mov[] = " Desplace la lista con los cursores. &ESC& vuelve al men£.";
char tecla_sel_mov[] = " Desplace con los cursores, escoja con &ENTER&. &ESC& cancela.";
char fact_text[] = " &CTRL-ENTER& termina la factura, &ESC& cancela.";

unsigned desplazamiento_horizontal = 0, // Desplazamiento X e Y del interior
         desplazamiento_vertical = 0,   // de la ventana
         ancho_actual = 0;            // Ancho en columnas de la pantalla

text_info info_pantalla, // Estado de la pantalla al iniciarse el programa
          actual; // estado actual de la pantalla

// Nombre del fichero de configuraci¢n
char fichero_conf[MAXPATH];

// Posibles puertos de conexion de la impresora
char puertos[][5] = {"LPT1","LPT2","LPT3","COM1","COM2","COM3",
                     "COM4","FICH"};

// Cabeceras de las facturas
char *cabecera_facturas;

// Lista de los meses
char *c_mes[12] = {"Ene", "Feb", "Mar", "Abr", "May", "Jun",
                   "Jul", "Ago", "Sep", "Oct", "Nov", "Dic"};

// Variables usadas para el color de las ventanas
unsigned COLOR = 0x2F00;
unsigned COLOR_FONDO = 0x1F00;
unsigned COLOR_BORDE = 0x1F00;

// Colores de men£s y ventanas
Tcolor colores = {0x70, 0x74, 0x20, 0x20,
                  0x70, 0x20, 0x70, 0x78, 0x70, 0x20,
                  0x1F, 0x1F, 0x1E, 0x1F,
                  0x20, 0x20, 0x24, 0x24,
                  0x2F,
                  0x71};

// Posici¢n en memoria de la ventana
unsigned offset_pant,
         segmento_pant;

// Resoluci¢n horizontal en caracteres del monitor.
unsigned res_hor = 80;
/*********************** CONSTANTES Y DEFINES ****************************/
const dias_mes = 31;
const dias_anno = 372UL;

/*************************************************************************/
void pascal scrollarriba(boolean respeta)
// Desplaza la pantalla una l¡nea hacia arriba
{
 if (!respeta)
   {movetext (1, 2, 80, 24, 1, 1);
    gotoxy (1, 24);
   }
 else
   {movetext (actual.winleft, actual.wintop + 1, actual.winright,
              actual.winbottom, actual.winleft, actual.wintop);
    gotoxy (1, actual.winbottom - actual.wintop + 1);
   }
 clreol();
}// scrollarriba

void pascal scrollabajo(boolean respeta)
// Desplaza la pantalla una l¡nea hacia abajo.
{
 if (!respeta)
   {movetext (1, 1, 80, 23, 1, 2);
    gotoxy (1, 1);
   }
 else
   {movetext (actual.winleft, actual.wintop, actual.winright,
              actual.winbottom - 1, actual.winleft, actual.wintop + 1);
    gotoxy (1, 1);
   }
 clreol ();
}// scrollabajo

void pascal scrollizquierda (unsigned nro_cols)
// Desplaza la pantalla nro_cols hacia la izquierda
  {unsigned f;

   movetext (nro_cols, 1, 80, 24, 1, 1);
   gotoxy (81 - nro_cols, 1);
   for (f = 1; f < 26; f++)
     {gotoxy (81 - nro_cols, f);
      imprime_letra (' ', nro_cols);
     }
  } // scrollizquierda

void pascal scrollderecha (unsigned nro_cols)
// Desplaza la pantalla nro_cols hacia la derecha
  {unsigned f;

   movetext (1, 1, 81-nro_cols, 24, nro_cols, 1);
   gotoxy (81 - nro_cols, 1);
   for (f = 1; f < 26; f++)
     {gotoxy (1, f);
      imprime_letra (' ', nro_cols);
     }
  } // scrollderecha

Tpantalla pascal hace_recuadro (byte x, byte y, byte ancho, byte alto,
                                char *cadena, int tipo_marco)
// Dibuja un recuadro en la pantalla
{int f, aux_alto, aux_ancho, offset;
 Tpantalla aux;

 // Calcula la posici¢n del recuadro y reserva memoria
 if (x == 255) x = (info_pantalla.screenwidth >> 1) - (ancho >> 1);
 if (y == 255) y = (info_pantalla.screenheight >> 1) - (alto >> 1);
 aux = guarda_recuadro (x, y, ancho, alto, tipo_marco);
 res_hor <<= 1;
 aux_alto = alto * res_hor;
 aux_ancho = ancho * 2;
 offset = (x - 1) * 2 + (y - 1) * res_hor + offset_pant;

 /* Dibuja el borde superior e inferior de la ventana, y las esquinas */
 poke (segmento_pant, offset, marco[tipo_marco][0] | COLOR_BORDE);
 poke (segmento_pant, offset + aux_alto, marco[tipo_marco][8] | COLOR_BORDE);
 for (f = 2; f < aux_ancho; f+=2)
   {poke (segmento_pant, offset + f, marco[tipo_marco][1] | COLOR_BORDE);
    poke (segmento_pant, offset + f + aux_alto, marco[tipo_marco][1] | COLOR_BORDE);
   }
 poke (segmento_pant, offset + f, marco[tipo_marco][3] | COLOR_BORDE);
 poke (segmento_pant, offset + f + aux_alto, marco[tipo_marco][10] | COLOR_BORDE);
 /* Dibuja los bordes laterales, y pinta el interior */
 for (f = res_hor; f < aux_alto; f += res_hor)
   {poke (segmento_pant, offset + f, marco[tipo_marco][4] | COLOR_BORDE);
    poke (segmento_pant, offset + aux_ancho + f, marco[tipo_marco][4] | COLOR_BORDE);
   }
 textattr (COLOR>>8);
 clrscr();
 if (cadena)
 // Si hay una cadena la coloca centrada en el borde superior del marco
   for (f = ((ancho>>1) - (strlen(cadena)>>1))<<1; *cadena != 0;
        f += 2, cadena++)
     poke (segmento_pant, offset + f, *cadena | COLOR_BORDE);
 res_hor >>= 1;
 return aux;
}/* hace_recuadro */

Tpantalla pascal guarda_recuadro (byte x, byte y, byte ancho, byte alto,
                                  int tipo_marco)
// Almacena un trozo de pantalla rectangular en memoria
{Tpantalla aux;
 int tam;

 gettextinfo (&aux.info_pantalla); // Coge el estado actual de la pantalla
 if (x == 255) x = (info_pantalla.screenwidth >> 1) - (ancho >> 1);
 if (y == 255) y = (info_pantalla.screenheight >> 1) - (alto >> 1);
 // Si la posici¢n X o Y pasadas son 255, se considera que se quiere poner la
 // ventana centrada
 tam = (ancho + 1) * 2 * (alto + 1) + 4;
 // Tama¤o del recuadro en bytes m s el espacio necesario para almacenar la
 // posici¢n del mismo
 aux.datos = (char *)farmalloc (tam); // Reserva memoria para los datos
 if (aux.datos)
   {*aux.datos = x;
    *(aux.datos+1) = y;
    *(aux.datos+2) = ancho;
    *(aux.datos+3) = alto; // Se almacena la posici¢n del mismo
    if (gettext (x, y, x + ancho, y + alto, aux.datos + 4))
    // Se cogen los datos
      {if (tipo_marco == SIN_MARCO)
         window (x, y, x + ancho, y + alto);
       else window (x + 1, y + 1, x + ancho - 1, y + alto - 1);
       /* Si se ha seleccionado como tipo de marco SIN_MARCO, se deja la
          ventana del alto y ancho pasados, sino se le reduce su tama¤o en
          un caracter de cada costado */
       ancho_actual = ancho;
       gettextinfo (&actual);
       return aux;
      }
    else
      /* Si no se ha podido guardar los datos por alg£n motivo, como
         que la ventana se sale de la pantalla, se presenta un mensaje de
         error */
         {sonido_error();
          mensaje ("Error del programa:\r\n"
                   " Se intenta crear una ventana"
                   "\r\n en un sitio incorrecto,"
                   "\r\n o de un tama¤o incorrecto", 255, 255, 35, 10);
          aux.datos = NULL;
          return aux;
         }
   }
 fin_de_memoria (1);
 aux.datos = NULL;
 return aux;
}/* guarda_recuadro */

void pascal color_ventana (unsigned color, unsigned color_fondo,
                           unsigned color_borde)
// Asigna el color a una ventana
  {
   COLOR_FONDO = (color_fondo & 0xFF) << 8;
   COLOR = (color & 0xFF) << 8;
   COLOR_BORDE = (color_borde & 0xFF) << 8;
  } /* color_ventana */

void pascal restaura_recuadro (Tpantalla *pantalla)
// Borra una ventana y restaura la pantalla al estado anterior
{byte x, y, x1, y1;

 if (pantalla->datos == NULL)
 // Si la ventana no existe, no se hace nada
   return;
 x = *pantalla->datos;
 y = *(pantalla->datos + 1);
 x1 = x + *(pantalla->datos + 2);
 y1 = y + *(pantalla->datos + 3); // Recupera las coordenadas de la ventana
 window (1, 1, 80, 25);
 puttext (x, y, x1, y1, pantalla->datos + 4); // Borra la ventana
 farfree (pantalla->datos);
 pantalla->datos = NULL;
 window (pantalla->info_pantalla.winleft, pantalla->info_pantalla.wintop,
         pantalla->info_pantalla.winright, pantalla->info_pantalla.winbottom);
 gotoxy (pantalla->info_pantalla.curx, pantalla->info_pantalla.cury);
 textattr (pantalla->info_pantalla.attribute);
 // Restaura los atributos de la pantalla al estado anterior a la creaci¢n
 // de la ventana borrada
 ancho_actual = pantalla->info_pantalla.screenwidth;
 actual = pantalla->info_pantalla;
}/* restaura_recuadro */

int pascal aviso (char *texto, byte x, byte y, byte ancho, byte alto)
// Dibuja un recuadro en pantalla con un mensaje y dos botones (SI y NO)
// para pedir un respuesta positiva o negativa
{int boton = 0;
 Tpantalla pantalla;

 color_ventana (colores.texto_r_ventana, colores.borde_ventana,
                colores.fondo_ventana);
 pantalla = hace_recuadro (x, y, ancho, alto);
 _setcursortype (_NOCURSOR);
 pone_barra_inferior (esc_tab);
 textattr (colores.texto_r_ventana);
 gotoxy (2,2);
 cputs (texto);                 // Imprime el mensaje

 controla_boton (&boton, alto); // Controla el manejo de los botones

 restaura_recuadro (&pantalla);

 if (boton)
   return SI;
 return NO;
}/* aviso */

boolean pascal pregunta (char *texto, byte x, byte y, byte ancho, byte alto, char *respuesta,
               int long_resp, boolean tipo_cadena)
// Dibuja un recuadro en pantalla con un mensaje y un campo para escribir una
// respuesta.
{int aux;
 Tpantalla pantalla;

 _setcursortype (_NORMALCURSOR);
 pantalla = hace_recuadro (x, y, ancho, alto);
 gotoxy (2,2);
 cputs (texto);            // Imprime el mensaje

 textattr (colores.texto_entrada);
 gotoxy (3, alto - 2);
 for (aux = 0; (aux < ancho - 6) && (aux < long_resp); aux++)
   putch (' ');  // Imprime el espacio para escribir la respuesta
 gotoxy (3, alto - 2);

 pone_barra_inferior (_pregunta);
 aux = lee_cadena (respuesta, long_resp, tipo_cadena); // Lee la respuesta

 restaura_recuadro (&pantalla);
 _setcursortype (_NOCURSOR);
 if (aux == ESC)   // Si se ha salido pulsando ESC se devuelve FALSE indicando
   return FALSE;   // que no se devuelve ninguna respuesta
 else if (aux == F10)
        return TAL_CUAL;
      else return TRUE;
}/* pregunta */

void pascal mensaje (char *texto, byte x, byte y, byte ancho, byte alto)
// Dibuja un recuadro en pantalla con un mensaje
{Tpantalla pantalla;

 color_ventana (colores.texto_ventana, colores.borde_ventana,
                colores.fondo_ventana);
 pone_barra_inferior (pres_enter);
 _setcursortype (_NOCURSOR);
 pantalla = hace_recuadro (x, y, ancho, alto, " AVISO ");
 gotoxy (2,2);
 cputs (texto);
 gotoxy (4, alto - 2);
 textattr (colores.boton_defecto);
 cputs ("  Ok  ");
 textattr (0x10);
 cputs ("Ü");
 gotoxy (5, alto - 1); cputs ("ßßßßßß");
 while (getch() != ENTER);
 restaura_recuadro (&pantalla);
}/* mensaje */

void pascal imprime_botones (int boton, int alto)
// Imprime los botones SI y NO que se usan en la funcion aviso. El color de
// los botones depende del valor de boton
{
 textattr (0x10);
 gotoxy (4, alto - 1);
 cputs ("ßßßß");
 gotoxy (12, alto - 1);
 cputs ("ßßßß");
 textattr (colores.boton_escogido);        /* segun el boton escogido se  */
 gotoxy (3 + (boton ? 0 : 8), alto - 2);   /* imprimen los botones en uno */
 cputs ((boton ? " Si " : " No "));        /* u otro color.               */
 textattr (0x10); cputs ("Ü");
 textattr (colores.boton);
 gotoxy (3 + (boton ? 8 : 0), alto - 2);
 cputs ((boton ? " No " : " Si "));
 textattr (0x10); cputs ("Ü");
}/* imprime_botones */

void pascal controla_boton (int *boton, int alto)
// Controla cual es el bot¢n activo y alterna entre ellos
{int tecla;

 *boton = TRUE;
 imprime_botones (*boton, alto);

 do
   {tecla = toupper(getch());
    if (tecla == TAB)                       /* alternar entre botones */
      {*boton = !*boton;
       imprime_botones (*boton, alto);
      }
   }
 while (tecla != ENTER && tecla != 'S' && tecla != 'N');
 // Sale del bucle si se pulsa ENTER, S o N. Si se pulsa S se devuelve TRUE,
 // si es la N se devuelve FALSE, y si se pulsa ENTER se devuelve el valor
 // del bot¢n activo
 if (tecla == 'S')
   *boton = TRUE;
 else
   if (tecla == 'N')
     *boton = FALSE;
}/* controla_boton */

void pascal pone_modo_cursor (int ins)
{if (!ins)
  _setcursortype (_SOLIDCURSOR);
 else
  _setcursortype (_NORMALCURSOR);
}/* pone_modo_cursor */

void pascal borra_resto_linea (unsigned int pos_cursor)
// Borra la l¡nea actual desde la posici¢n del cursor hasta el final de l¡nea
{int x, y;

 if (pos_cursor == 0xffff)
   clreol(); /* usando la posicion actual del cursor en pantalla */
 else
   {x = wherex();
    y = wherey();
    gotoxy (pos_cursor % 160 / 2 + 1, pos_cursor / 160 + 1);
    clreol();
    gotoxy (x, y);
   }/* calcula la posicion del cursor y borra esa linea */
}/* borra_resto_linea */

void pascal imprime_letra (char caracter, unsigned repeticiones)
// Imprime una letra n veces desde la posici¢n actual del cursor
{int x, y, f;
 unsigned int offset;

 x = wherex();
 y = wherey();
 offset = (x - 1) * 2 + (y - 1) * (res_hor << 1);
 for (f = 0; f < repeticiones; f++)
   pokeb (segmento_pant, offset+=2, caracter);   /* imprime la letra */
}/* imprime_letra */

void pascal borra_pantalla (void)
/* Funciona igual que clrscr(), excepto que no borra la ultima linea */
{int f;

 for (f = 1; f <= MAX_Y; f++)
   {gotoxy (1, f);
    clreol ();
   }
 gotoxy (PRIMERA_COLUMNA, LINEA_SUPERIOR);
}/* borra_pantalla */

void pascal fin_de_memoria (int tipo_de_error)
{switch (tipo_de_error)
  {case 0:
       mensaje ("Se ha agotado la memoria.\r\n  No podr  seguir escribiendo."
                 "\r\n  Almacene el texto.", 255, 255, 40, 11);
       break;
   default:
       textcolor (WHITE);
       textbackground (BLACK);
       clrscr();
       cputs ("Memoria insuficiente durante un proceso no controlado\r\n"
              "El texto no se ha almacenado");
       break;
  }
} // fin_de_memoria

void pascal sonido_error (void)
/* Produce un pitido */
  {int f;

   if (!hace_sonido) return; // Si no est n activados los sonidos, se sale
   for (f= 0; f < 25; f++)
     {sound (1000);
      delay (3);
      sound (500);
      delay (3);
     }
   nosound();
  } // sonido_error

#define INTR 0x1C    /* 2¦ interrupci¢n del reloj. Esta interrupci¢n es
                        llamada normalmente por la interrupci¢n de reloj
			(la 8). Sirve para ejecutar rutinas periodicamente.*/

void interrupt ( *vieja_rutina)(...);  /* Funci¢n nula, no tiene ning£n
                                          c¢digo al compilar el programa,
                                          pero al instalar el reloj, se redi-
                                          rige a la rutina a la que apuntaba
                                          la INT 0x1C */

#pragma option -r-
char cadena[] = "  /  /   **      ";
// Cadena usada para que el reloj muestre la hora
byte contador = 18;

void interrupt reloj (...)
/* Nuestra rutina de tratamiento de la interrupci¢n del reloj.
   Muestra un reloj con la hora actual en la esquina inferior derecha del
   monitor. */
  {
    asm   cmp    reloj_visible, 0
    asm   je     sigue_2
    // Si el reloj no est  visible salta casi hasta el final

    // Si est  visible decrementa el contador que controla los : del reloj
    asm   dec    contador
    asm   jz     dos_puntos
    asm   jmp    imprime

   dos_puntos:
    asm   cmp    byte ptr [cadena+14], 3ah  // ¨Est n puestos los :
    asm   jne    los_quita      // Si, los quita,
    asm   mov    byte ptr [cadena+14], 20h  // sino los pone
    asm   jmp    sigue_1
   los_quita:
    asm   mov    byte ptr [cadena+14], 3ah
   sigue_1:
    asm   mov    [contador], 18

   imprime:
    asm   mov    ax, 40h
    asm   mov    es, ax
    asm   mov    ax, es:[6ch]
    asm   mov    dx, es:[6eh]  // ponemos el valor actual del reloj en ax y dx
    asm   mov    cx, 1092
    asm   div    cx            // pasamos el valor del reloj a minutos
    asm   mov    cx, 60
    asm   div    cl            // pasamos el resultado a horas, el resto son
                         // los minutos
    asm   push   ax
    asm   xor    al, al
    asm   xchg   ah, al
    asm   mov    cl, 10
    asm   div    cl             // ponemos los minutos en ax y dividimos por 10
    asm   add    ax, 3030h      // Hacemos que en ah y al quede el c¢digo ASCII
                          // del n£m. que hab¡a antes.
    asm   mov    word ptr [cadena+15], ax //ponemos el minuto en la cadena

    asm   pop    ax
    asm   xor    ah, ah
    asm   mov    cl, 10
    asm   div    cl              // ponemos la hora en ax y dividimos por 10
    asm   add    ax, 3030h       // Hacemos que en ah y al quede el c¢digo ASCII
                           // del n£m. que hab¡a antes.
    asm   mov    word ptr [cadena+12], ax // ponemos la hora en la cadena

    asm   mov    ax, 0b800h
    asm   mov    es, ax
    asm   lea    bx, cadena
    asm   mov    si, 3966          // Apuntamos a la cadena, y a la posici¢n
                            // donde se pondr  esta en el monitor
    asm   mov    cx, 17
   ciclo:
    asm   mov    al, [bx]      // Pasamos el caracter a al, y le a¤adimos el
    asm   mov    ah, 71h          // atributo
    asm   mov    es:[si], ax      // imprimimos la cadena
    asm   inc    bx
    asm   add    si,2             // pasamos al siguiente caracter
    asm   loop   ciclo

   sigue_2:

   vieja_rutina ();  // Llamamos a la anterior rutina de reloj
  } /* reloj */
#pragma option -r

void pascal instala_reloj (void)
/* Instala el controlador de reloj */
  {struct date fecha;

   if (reloj_instalado)               // Si el reloj ya est  instalado se
     return;                          // sale de la funci¢n
   vieja_rutina = getvect (INTR);     // Cogemos la direcci¢n de la rutina de
                                      // tratamiento de la interrupci¢n 1C
   setvect (INTR, reloj);             // Apuntamos la interrupci¢n 1C a
                                      // nuestra rutina.
   getdate (&fecha);
   sprintf (cadena, "%2d/%02d/%2d **   :  ", fecha.da_day, fecha.da_mon,
                                            fecha.da_year % 100);
     // Coge la fecha y la pone en la cadena
   reloj_visible = FALSE;             // De momento no es visible
   reloj_instalado = TRUE;            // Ya est  instalado.
  } /* instala_reloj */

void pascal desinstala_reloj (void)
/* Quita el controlador de reloj */
  {
   if (reloj_instalado)           // Si el reloj ya est  instalado
     {setvect (INTR, vieja_rutina);// cambiamos nuestra rutina de tratamiento
				   // por la anterior.
      reloj_instalado = FALSE;
     }
  } /* desinstala_reloj */

void pascal dibuja_escritorio (void)
// Inicializa la pantalla con los colores necesarios e imprime el fondo.
  {int f;

   textattr (colores.escritorio);
   clrscr();
   for (f = 1; f < 25; f++)
     {gotoxy (1, f);
      cputs (fondo);
     } // Imprime el fondo al estilo del Turbo Pascal
   gotoxy(1,1); cputs(barra2);
     // Coloca la barra superior. La inferior queda colocada automaticamente
     // al imprimir el fondo
   _setcursortype (_NOCURSOR); // Elimina el cursor.
  } // dibuja_escritorio

void pascal pone_barra_inferior (const char *cadena, unsigned linea)
// Imprime en la barra inferior del escritorio
  {unsigned far *aux1, *aux2, f = 0;
   boolean act_color = FALSE;
   unsigned color1, color2;

   f = peek (0x40, 0x4a);
   aux2 = aux1 = (unsigned far *)MK_FP (segmento_pant, f * (24 - linea) * 2);
   for (; f > 0; f--, aux1++)
     *aux1 = 0x7020;
      // Limpia la barra inferior
   // Imprime la cadena pasada, cambiando el color del caracter si es necesario
   aux1 = aux2;
   color1 = colores.texto_barra << 8;
   color2 = colores.letra_barra << 8;
   while (*cadena != 0)
     {if (*(cadena) == '&')
        if (act_color)
          {act_color = FALSE;
           cadena++;
          }
        else {act_color = TRUE;
              cadena++;
             };
      if (act_color)
        *aux1 = (*cadena++) | color2;
      else *aux1 = (*cadena++) | color1;
      aux1++;
     }  // Imprime la cadena pasada
  } // pone_barra_inferior

void pascal inicializa_programa (char *nombre_clientes, char *nombre_articulos,
                                 char *nombre_facturas, char *nombre_fact,
                                 double *iva, unsigned long *ultima_factura,
                                 char *directorio_trabajo)
/* Inicializa la configuraci¢n del programa. */
  {
   FILE *fichero;
   char unidad[MAXDRIVE],   // Unidad donde est  el programa
        directorio[MAXDIR], // Directorio donde est  el programa
        nombre[MAXFILE],    // Nombre del fichero ejecutado
        extension[MAXEXT],  // Extension del fichero
        aux[80],
        *auxc;              // Cadenas auxiliares
   struct ffblk ff;         // Estructura usada para la b£squeda de ficheros
   unsigned error, *modo;
   Tdatos_fichero_facturas datos_factura;
   Tdatos_vesa datos_vesa;
   Tdatos_modo datos_modo;
   boolean modo_encontrado = FALSE;

   // Examina el ordenador para ver si hay una tarjeta VESA
   hay_vesa=hay_tarjeta_vesa (&datos_vesa);
   if (hay_vesa)
     {modo = datos_vesa.modos_soportados;
      while (*modo != 0xFFFF && !modo_encontrado)
        {datos_modo_vesa (*modo, &datos_modo);
         if (datos_modo.resolucion_x >= 132 &&
             (datos_modo.atributos & 0x0010) == 0)
           modo_encontrado = TRUE;
         else
           modo++;
        }
      if (modo_encontrado)
        {vesa_instalada = TRUE;
         modo_vesa = *modo;
        }
      else
        vesa_instalada = FALSE;
     }

   offset_pant = peek (0x40, 0x4e);
   if (peekb (0x40, 0x49) == 7)
     segmento_pant = 0xb000;
   else segmento_pant = 0xb800;

   // Coge el estado actual de la pantalla
   gettextinfo (&info_pantalla);

   // Inicializa la pantalla
   directvideo = 1;
   dibuja_escritorio();

   // Busca en memoria la cadena donde se indica el directorio del programa.
   auxc = (char *) MK_FP ((unsigned) peek(_psp, 0x2c), 0);
   while (*auxc != 1 || *(auxc + 1) != 0)
     auxc++;
   auxc += 2;

   fnsplit (auxc, unidad, directorio, nombre, extension);
   // Separa el nombre del fichro en sus partes
   strcpy (fichero_conf, unidad);
   strcat (fichero_conf, directorio);
   strcat (fichero_conf, "factura.cfg");
     // Forma el nombre del fichero de configuraci¢n
   strcpy (directorio_programa, unidad);
   strcat (directorio_programa, directorio);
     // Forma la cadena con el directorio del programa

   configuracion_modificada =
      leer_configuracion (nombre_clientes, nombre_articulos, nombre_facturas,
                          nombre_fact, iva, ultima_factura,
                          directorio_trabajo);

   if (configuracion_modificada)
   /* Si no se ha podido leer el fichero de configuraci¢n, porque no existe o
      porque no es v lido se muestra un mensaje de error y se carga la
      configuraci¢n por defecto */
     {sonido_error ();
      mensaje ("Fichero de configuraci¢n inv lido, o no\r\n encontrado.\r\n "
               "Se usar  la configuraci¢n por defecto.\r\n "
               "A lo mejor debe reconfigurar del programa.",
               255, 255, 50, 10);

      hace_sonido = TRUE;
      *iva = 16.0;
      nro_copias = 1;
      lineas_papel = 64;
      puerto_conexion = 0;
      strcpy (directorio_trabajo, directorio_programa);
      strcpy (nombre_clientes, directorio_trabajo);
      strcat (nombre_clientes, "CLIENTES.DAT");
      strcpy (nombre_articulos, directorio_trabajo);
      strcat (nombre_articulos, "ARTICULO.DAT");
      strcpy (nombre_facturas, directorio_trabajo);
      strcat (nombre_facturas, "FACTURAS.DAT");
      strcpy (nombre_fact, directorio_trabajo);
      strcat (nombre_fact, "FACT.DAT");

      // Se abre el fichero de facturas para leer el nro de la £ltima factura
      // para as¡ poder continuar numer ndolas correctamente
      fichero = fopen (nombre_facturas, "rb");
      if (fichero)
        {//fseek (fichero, (long)sizeof(Tdatos_fichero_facturas) + 1, SEEK_END);
         while (TRUE)
           {if (feof(fichero))
              break;
            error = fread (&datos_factura, sizeof(Tdatos_fichero_facturas), 1,
                          fichero);
           }
         fclose (fichero);

         if (*ultima_factura < datos_factura.nro_factura)
           *ultima_factura = datos_factura.nro_factura;
        }
     }


   // Carga la cabecera de las facturas en memoria
   strcpy (aux, directorio_trabajo);
   strcat (aux, "cabecera.dat");
   error = findfirst (aux, &ff, 0);
   // Busca el fichero cabecera.dat en el subdirectorio del programa, si se
   // encuentra se carga en memoria
   if (error == 0)
     {cabecera_facturas = new char[ff.ff_fsize + 1];
      fichero = fopen (aux, "rb");
      fread (cabecera_facturas, ff.ff_fsize, 1, fichero);
      fclose (fichero);
      cabecera_facturas[ff.ff_fsize] = 0;
     }
   else {sonido_error();
         mensaje ("No se ha podido cargar la cabecera de las facturas\r\n"
                 " Las facturas no saldr n correctamente", 255, 255, 55, 10);
         cabecera_facturas = NULL;
        }

   configuracion_modificada = FALSE;

   // Inicializa el reloj
   instala_reloj();
   reloj_visible = TRUE;
  } // inicializa_programa

void pascal restaura_ordenador (char *nombre_clientes, char *nombre_articulos,
                                char *nombre_facturas, char *nombre_fact,
                                double iva, unsigned long ultima_factura,
                                char *directorio_trabajo)
/* Restaura el ordenador a un estado parecido al que ten¡a antes de usar el
   programa, y almacena la configuraci¢n del programa */
  {
   // Almacena la configuraci¢n
   if (configuracion_modificada)
     salvar_configuracion (nombre_clientes, nombre_articulos, nombre_facturas,
                           nombre_fact, iva, ultima_factura,
                           directorio_trabajo);

   // Se libera la memoria de la cabecera de las facturas
   delete cabecera_facturas;

   // Se restaura la interrupci¢n de reloj
   desinstala_reloj ();

   // Se restauran el estado de la pantalla
   window (info_pantalla.winleft, info_pantalla.wintop,
           info_pantalla.winright, info_pantalla.winbottom);
   textattr (info_pantalla.attribute);
   _setcursortype (_NORMALCURSOR);
   clrscr();
  } // restaura_ordenador

boolean pascal leer_configuracion (char *nombre_clientes, char *nombre_articulos,
                                   char *nombre_facturas, char *nombre_fact,
                                   double *iva, unsigned long *ultima_factura,
                                   char *directorio_trabajo)
  {FILE *fichero;
   char aux[80];
   unsigned pos;

   fichero = fopen (fichero_conf, "rb");
     // Se abre el fichero de configuraci¢n
   if (fichero == NULL)
     return TRUE;
   else
     // Se verifica si el fichero es v lido, si lo es se leen los datos
     // de configuraci¢n
     {fread (aux, strlen (CLAVE_FICH), 1, fichero);
      aux [strlen(CLAVE_FICH)] = 0;

      if (strcmp (CLAVE_FICH, aux) == 0)
        {
         fread (&hace_sonido, sizeof(boolean), 1, fichero);
         fread (iva, sizeof(double), 1, fichero);
         fread (directorio_trabajo, MAXPATH, 1, fichero);
         fread (nombre_clientes, MAXFILENAME, 1, fichero);
         fread (nombre_articulos, MAXFILENAME, 1, fichero);
         fread (nombre_facturas, MAXFILENAME, 1, fichero);
         fread (nombre_fact, MAXFILENAME, 1, fichero);
         fread (&nro_copias, sizeof(int), 1, fichero);
         fread (&lineas_papel, sizeof(int), 1, fichero);
         fread (&puerto_conexion, sizeof(int), 1, fichero);
         fread (ultima_factura, sizeof(unsigned long), 1, fichero);

         fclose (fichero);
         strcpy (aux, directorio_trabajo);
         pos = strlen (directorio_trabajo);

         strcpy (aux + pos, nombre_clientes);
         strcpy (nombre_clientes, aux);

         strcpy (aux + pos, nombre_articulos);
         strcpy (nombre_articulos, aux);

         strcpy (aux + pos, nombre_facturas);
         strcpy (nombre_facturas, aux);

         strcpy (aux + pos, nombre_fact);
         strcpy (nombre_fact, aux);
         return FALSE;
        }
      else
        {
	 fclose (fichero);
         return TRUE;
        }
     }
  }

void pascal salvar_configuracion (char *nombre_clientes, char *nombre_articulos,
                                  char *nombre_facturas, char *nombre_fact,
                                  double iva, unsigned long ultima_factura,
                                  char *directorio_trabajo)
  {FILE *fichero;

   fichero = fopen (fichero_conf, "wb");
   if (fichero)
     {fwrite (CLAVE_FICH, strlen (CLAVE_FICH), 1, fichero);
      fwrite (&hace_sonido, sizeof(boolean), 1, fichero);
      fwrite (&iva, sizeof(double), 1, fichero);
      fwrite (directorio_trabajo, MAXPATH, 1, fichero);
      fwrite (nombre_clientes, MAXFILENAME, 1, fichero);
      fwrite (nombre_articulos, MAXFILENAME, 1, fichero);
      fwrite (nombre_facturas, MAXFILENAME, 1, fichero);
      fwrite (nombre_fact, MAXFILENAME, 1, fichero);
      fwrite (&nro_copias, sizeof(int), 1, fichero);
      fwrite (&lineas_papel, sizeof(int), 1, fichero);
      fwrite (&puerto_conexion, sizeof(int), 1, fichero);
      fwrite (&ultima_factura, sizeof(unsigned long), 1, fichero);

      fclose (fichero);
     }
  } // salvar_configuracion

void pascal pone_caracter (char caracter, unsigned columna, unsigned fila,
		    unsigned atributos)
// Coloca un caracter en una posici¢n determinada de la pantalla
  {unsigned *posicion;

   // Primero se calcula que la posici¢n no est‚ fuera de la ventana actual,
   // sino se sale de la funci¢n
   if ((columna < desplazamiento_horizontal) ||
       (columna > actual.winright - actual.winleft + desplazamiento_horizontal))
     return;
   if ((fila < desplazamiento_vertical) ||
       (fila > actual.winbottom - actual.wintop + desplazamiento_vertical))
     return;
   // Se calcula la posici¢n en la pantalla
   posicion = (unsigned*)MK_FP (segmento_pant,
        (actual.winleft + columna - desplazamiento_horizontal - 1) * 2 +
        (actual.wintop + fila - desplazamiento_vertical - 1) *
        (peek (0x40,0x4a) << 1));
   // Se imprime el caracter con el atributo indicado
   *posicion = ((byte)(atributos & 0xff) << 8) | caracter;
   if ((atributos & SUBRAYA) == SUBRAYA)
     pone_caracter ('-', columna, fila + 1, atributos & NEG SUBRAYA);
  }  // pone_caracter

void pascal linea_vert (unsigned columna, unsigned fila, unsigned largo,
                 unsigned atributos)
/* Dibuja una l¡nea vertical */
  {char aux;
   unsigned contador;

   contador = largo;
   if ((atributos & L_DOBLES) == L_DOBLES)
   // Seg£n el atributo se hacen l¡neas simples o dobles
     aux = 'º';
   else aux = '³';
   // Se imprime la l¡nea
   while (contador--)
     pone_caracter (aux, columna, fila++, atributos & NEG L_CURVA);
  } // linea_vert

void pascal linea_horz (unsigned columna, unsigned fila, unsigned largo,
                 unsigned atributos)
/* Dibuja una l¡nea horizontal */
  {char aux;
   unsigned contador;

   contador = largo;
   if ((atributos & L_DOBLES) == L_DOBLES)
   // Seg£n el atributo se hacen l¡neas simples o dobles
     aux = 'Í';
   else aux = 'Ä';
   // Se imprime la l¡nea
   while (contador--)
     pone_caracter (aux, columna++, fila, atributos & NEG L_CURVA);
  } // linea_horz

void pascal imprime_cadena (char *cadena, unsigned columna, unsigned fila,
                            unsigned atributos, boolean desplaza)
/* Imprime una cadena en la pantalla */
  {unsigned aux1, aux2;

   if (desplaza == FALSE)
   // Si no se toma en cuenta el desplazamiento de la ventana, se cambia el
   // origen de coordenadas
     {aux1 = desplazamiento_horizontal;
      aux2 = desplazamiento_vertical;
      desplazamiento_vertical = desplazamiento_horizontal = 0;
     }

   while (*cadena) // Se imprime la cadena
     pone_caracter (*cadena++, columna++, fila, atributos);

   if (desplaza == FALSE)
   // Se restaura el origen de coordenadas si se hab¡a desplazado
     {desplazamiento_horizontal = aux1;
      desplazamiento_vertical = aux2;
     }
  } // imprime_cadena

unsigned pascal lee_cadena (char *cadena, unsigned max_car, boolean alfanumerica,
                            boolean nueva, boolean sale, char caracter_relleno)
/* Lee una cadena desde el teclado */
  {char *aux,
        tecla,
        *temp = NULL;
   unsigned contador,
            x, y,
            aux_x, aux_y,
            pos_cursor;
   boolean insercion = TRUE, // Comenzamos en modo de inserci¢n
           salida;

   aux = cadena; // Apuntamos a la cadena pasada;

   pos_cursor = 0;
   aux_x = wherex();
   aux_y = wherey(); // Memorizamos la posici¢n actual del cursor
   _setcursortype (_NORMALCURSOR); // Activamos el cursor

   if (nueva == TRUE)
   // Si se pasa nueva a TRUE, se inicializa la cadena a cero, para leer una
   // cadena nueva.
     {contador = 0;
      memset (cadena, 0, max_car);
     }
   else {temp = new char[max_car];
         // Sino se crea un almacen temporal para la cadena pasada
         if (temp)
           {for (int f = 0; f < max_car - 1; f++)
              temp[f] = caracter_relleno;
            temp[f] = 0;
            imprime_cadena (temp, aux_x - 1, aux_y - 1, (BLUE<<4)|YELLOW,
                            FALSE);
            // Se imprime una cadena en blanco, para borrar posible basura
            strcpy (temp, cadena); // Se copia la cadena pasada al almac‚n temporal
            imprime_cadena (temp, aux_x - 1, aux_y - 1, (BLUE<<4)|YELLOW,
                            FALSE);
            // Se imprime la cadena en la pantalla
            contador = strlen (cadena);
           }
         else return 0; // Si no se puede reservar memoria, se devuelve un c¢digo
                        // de tecla inexistente.
         gotoxy (aux_x, aux_y);
        }

   do                  // Se entra en el bucle de lectura de teclado
     {tecla = getch(); // Leemos el teclado
      salida = FALSE;
      x = wherex(); y = wherey(); // Se memoriza la posici¢n del cursor
      switch (tecla)
        {case ESC : // Si se pulsa ESCAPE se devuelve una cadena nula si se
                    // quer¡a una cadena nueva,
                    if (nueva == TRUE)
                      {*cadena = 0;
                       contador = 0;
                      }
                    else // sino se restaura la cadena original
                      {strcpy (cadena, temp);
		       contador = 0;
                      }
                    salida = TRUE;
                    break;

         case ENTER :
         case LF :
         case TAB : // Si se pulsa ENTER, TAB, CTRL-ENTER o F10 se sale del
                    //bucle
                    salida = TRUE;
                    break;

         case BS : // Borra el caracter anterior
                   if (aux != cadena) // Si no est  al principio de la cadena
                     {aux--;  // Desplaza el puntero hacia adelante
                      memcpy (aux, aux + 1, contador - pos_cursor + 1);
                        // mueve la cadena una posici¢n hacia adelante
                      pos_cursor--;
                      contador--;

                      x = wherex() - 1;
                      if (x == 0)
                        {x = MAX_X;
                         y = wherey() - 1;
                         if (y == 0) y = 1;
                        }
                      // Acomoda la posici¢n del cursor
                      gotoxy (x, y);
                      cputs (aux);
                      putch (caracter_relleno);
                      gotoxy (x, y); // Imprime el resto de la cadena
                     }
                   break;

         case T_control : // Si se ha pulsado una tecla con c¢digo especial
           {tecla = getch(); // Se lee el siguiente c¢digo
            switch (tecla)
              {case INS : // Si se pulsa INSERT se alterna entre el modo de
			  // inserci¢n y el de sobreescritura
                          if (insercion)
                            {insercion = FALSE;
                             _setcursortype (_SOLIDCURSOR);
                            }
                          else
                            {insercion = TRUE;
                             _setcursortype (_NORMALCURSOR);
                            }
                          break;

               case CURS_IZQ : // Cursor a la izquierda
                               if (aux != cadena)
                               // Si no se est  ya en el inicio de la cadena
                                 {aux--;
                                  pos_cursor--;
                                  // Retrocede una posici¢n el cursor
                                  x = wherex() - 1;
                                  y = wherey();
                                  if (x == 0)
                                    {x = MAX_X;
                                     y--;
                                     if (y == 0) y = 1;
                                    }
                                  // Acomoda la posici¢n del cursor
                                  gotoxy (x, y);
                                 }
                               break;

               case CURS_DER : // Cursor a la derecha
                               if (*aux)
                               // Si no estamos al final de la cadena
                                 {pos_cursor++;
                                  aux++;
                                  x = wherex() + 1;
				  y = wherey();
                                  if (x > MAX_X)
                                    {x = 1;
                                     y++;
                                     if (y > MAX_Y) y = MAX_Y;
                                    }
                                  // Acomoda la posici¢n del cursor
                                  gotoxy (x, y);
                                 }
                               break;

               case INICIO : // Env¡a el cursor al inicio de la cadena
                             aux = cadena;
                             pos_cursor = 0;
                             gotoxy (aux_x, aux_y);
                               // Coloca el cursor en la posici¢n original
                             break;

               case FIN : // Env¡a el cursor al final de la cadena
                          aux = cadena + contador;
                          pos_cursor = contador;
                          x = aux_x + contador % ancho_actual;
                          y = aux_y + contador / ancho_actual;
                          if (y > MAX_Y)
                            y = MAX_Y;
                            // Calcula la posici¢n donde est  el final de la
                            // cadena en pantalla
                          gotoxy (x, y);
                          break;

               case SUPR : // Borra el caracter cobre el que est  el cursor
                           if (*aux) // Si no est  al final de la cadena
                             {memmove (aux, aux + 1, contador - pos_cursor);
                                // Desplaza la cadena una posici¢n hacia delante
                              contador--;
			      x = wherex();
                              cputs (aux);
                              putch (caracter_relleno);
                              gotoxy (x, y);
                                // Reimprime la cadena
                             }
                           break;

               case CURS_ARR :
               case CURS_ABJ : if (sale)
                                 salida = TRUE;
                               break;

               case S_TAB :
               case F10 :  salida = TRUE;
                           break;
              }
            break;
           }

         default : // Para cualquier otra tecla primero miramos si est 
                   // aceptada seg£n el modo de lectura
                   if (alfanumerica == FALSE)
                     // Si el modo es solo num‚rico, solo se aceptan los
                     // n£meros
                     {if (!strchr ("0123456789", tecla))
                       break;
                     }
                   else
                     // Si el modo es semi-num‚rico (para entrar n£meros en
                     // coma flotante), se aceptan n£meros y algunas teclas
                     if (alfanumerica == TAL_CUAL)
                       {if (!strchr ("01234567890+-.Ee", tecla))
                          break;
                       }
                     // Sino, se aceptan todos los caracteres leidos
                   if (insercion == FALSE)
                   // Si se est  en modo de sobreescritura se meten los
                   // caracteres en la cadena
                    {if (pos_cursor < max_car - 1)
                       // Si todav¡a entran letras en la cadena
                       {*aux++ = tecla;           // Se almacena la letra
                        putch (tecla);            // Se imprime en la pantalla
			if (pos_cursor == contador)
                        // Si se inserta el caracter al final de la cadena se
                        // incrementa contador y se pone un nulo al final
                          {contador++;
                           *aux = 0;
                          }
                        pos_cursor++;  // Se incrementa la posici¢n del cursor
                       }
                    }
                   else // Si estamos en el modo de inserci¢n se tiene que
                        // desplazar la cadena e insertar el caracter.
                     {if (contador < max_car - 1) // Si todav¡a entran letras
                        {memmove (aux + 1, aux, contador - pos_cursor + 1);
                           // Primero se desplazan los caracteres desde la
                           // posici¢n del cursor un sitio hacia atr s
                         *aux++ = tecla; // Se inserta la letra en la cadena
                         contador++;
                         pos_cursor++;
                         putch (tecla); // Imprime la letra
                         x = wherex();
                         y = wherey(); // memoriza la posici¢n del cursor
                         cputs (aux);  // imprime la cadena desde la posici¢n
                                       // actual
                         gotoxy (x, y); // coloca el cursor en la posici¢n
                                        //original
                        }
                     }
                   break;
        }
     }
   while (!salida);
   _setcursortype (_NOCURSOR); // Se elimina el cursor
   if (temp) delete temp;
     // Si hab¡a memoria reservada para el almac‚n, se borra
   return tecla; // Se devuelve el c¢digo de la tecla de salida.
  } // lee_cadena

void pascal saca_papel (void)
// Saca el folio de la impresora
  {
   fputc (12, stdprn);
  } // saca_papel

void pascal completa_cadena (char *cadena, unsigned longitud,
                             char caracter_relleno, boolean pone_crlf)
// Completa una cadena con un caracter determinado
  {
   unsigned contador;

   for (contador = 0; contador < longitud && *cadena != 0;
        contador++, cadena++);
     // Ubica el final de la cadena pasada
   if (*cadena == 0)
     for (; contador < longitud; contador++, cadena++)
       *cadena = caracter_relleno;
     // Si estamos al final de la cadena se rellena esta con el caracter de
     // relleno hasta que alcance la longitud especificada
   if (pone_crlf)
     {*cadena++ = 13;
      *cadena++ = 10;
   }
   *cadena = 0;
   // Pone un 0 o un retorno de carro al final de la cadena seg£n se indique.
  } // complata_cadena

char * pascal gotoxy_text (char *destino, char *origen, unsigned x)
// Coloca una cadena en una posici¢n dentro de otra
  {int contador = x;
   char *inicio;

   inicio = destino + x;  // Ubica la posici¢n inicial donde copiar
   while (*origen && contador != 90)
   // copia la cadena
     {*inicio++ = *origen++;
      contador++;
     }
   return origen;
  } // gotoxy_text

void pascal pide_fecha (char *texto, byte x, byte y, byte ancho, byte alto,
                        date *fecha, boolean nueva)
/* Pide una fecha y verifica que sea correcta */
  {date aux;
   Tpantalla temp;
   unsigned dato_leido = 0;
   char tecla, cadena[10];

   if (nueva == FALSE)
   // Si se pide una fecha vieja se apunta a la fecha pasada
     aux = *fecha;
   else // sino se coge la fecha actual
     if (nueva == TAL_CUAL)
       getdate (&aux);
     else // Sino se inicializa la fecha a cero
          {aux.da_day = 0;
           aux.da_mon = 0;
           aux.da_year = 0;
          }
   pone_barra_inferior (_fecha);
   temp = hace_recuadro (x, y, ancho, alto);
   gotoxy (2, 2);
   cputs (texto);
   gotoxy (2, alto - 2);
   if (nueva == TRUE)
   // Si la fecha es nueva se imprimen los campos vacios
     {cputs ("  /  /");
      cadena[0] = 0;
     }
   else // Sino se imprime la fecha
     {cprintf ("%2d/%2d/%4d", aux.da_day, aux.da_mon, aux.da_year);
      itoa (aux.da_mon, cadena, 10);
     }
   gotoxy (2, alto - 2);
   do // Entra en el bucle de leer la fecha
     {tecla = lee_cadena (cadena, (dato_leido == 2) ? 5 : 3, FALSE, FALSE);
      // Lee la cadena de la fecha, el mes o el a¤o
      switch (tecla)
        {case ENTER : // Si se pulsa ENTER o TAB o CTRL-ENTER se pasa al campo
         case TAB :   // siguiente almacenando el dato actual temporalmente
         case LF :
                    switch (dato_leido)
                      {case 0 : aux.da_day = atoi (cadena);
                                itoa (aux.da_mon, cadena, 10);
                                break;
                       case 1 : aux.da_mon = atoi (cadena);
                                itoa (aux.da_year, cadena, 10);
                                break;
                       case 2 : aux.da_year = atoi (cadena);
                                itoa (aux.da_day, cadena, 10);
                                dato_leido = -1;
                                break;
                      }
                    if (*cadena == '0') *cadena = 0;
                    dato_leido++;
                    gotoxy (2 + dato_leido * 3, alto - 2);
                    break;
        }
     }
   while (tecla != ESC && tecla != LF);
   // Se repite hasta que se pulsa ESC o CTRL-ENTER
   if (tecla == LF)
     *fecha = aux;
   else fecha->da_mon = 0;
   restaura_recuadro (&temp);
  } // pide_fecha

TFtemporal * pascal inicializa_impresion (void)
/* Crea un fichero en disco donde se iran guardando los datos a imprimir */
  {char aux[13];
   TFtemporal *fichero;

   strcpy (aux, "FAXXXXXX");
   mktemp (aux);              // Crea el nombre del fichero temporal
   fichero = new TFtemporal;  // Reserva memoria para el control de fich temporal
   strcpy (fichero->nombre, directorio_programa);
   strcat (fichero->nombre, aux);
   fichero->fichero = fopen (fichero->nombre, "wb");
      // Crea el fichero temporal
   return fichero;
  } // inicializa_impresion

unsigned pascal envia_datos (char *cadena, TFtemporal *fichero)
/* Prepara los datos para imprimir */
  {
   return fwrite (cadena, 1, strlen (cadena), fichero->fichero);
  } // envia_datos

unsigned pascal imprime (TFtemporal *fichero, boolean pasa_papel,
                         unsigned copias)
/* Imprime los datos en el dispositivo de salida actual */
  {char buffer[256];
   unsigned aux, contador;
   FILE *disp_salida;

   if (strcmp (puertos[puerto_conexion], "FICH") == 0)
   // Si se escoge como salida un fichero de disco, se pide el nombre del
   // fichero de salida.
     {pregunta ("Nombre del fichero de salida", 255, 255, MAXPATH - 6, 8,
                buffer, MAXPATH - 6);
      disp_salida = fopen (buffer, "wb");
     }
   else
   // Sino se activa el puerto de salida
     disp_salida = fopen (puertos[puerto_conexion], "wb");

   fclose (fichero->fichero);
   fichero->fichero = fopen (fichero->nombre, "rb");
   // Se cierra el fichero y se vuelve a abrir para evitar lios
   if (copias == 0) copias = nro_copias;
   for (contador = 0; contador < copias; contador++)
   // Se repite seg£n el nro de copias a hacer
     {rewind (fichero->fichero);         // Volvemos al inicio del fichero
      while (!feof (fichero->fichero))
      // Mientras no se llegue al final del fichero
        {aux = fread (buffer, 1, 256, fichero->fichero); // Se leen 256 bytes
         fwrite (buffer, 1, aux, disp_salida);   // Se envian a la salida
        }
      if (pasa_papel) fputc (12, disp_salida);
     }
   cierra_impresion (fichero);
   fclose (disp_salida);
   return OK;
  } // imprime

void pascal cierra_impresion (TFtemporal *fichero)
/* Cierra y borra los ficheros temporales de impresi¢n */
  {
   fclose (fichero->fichero);
   unlink (fichero->nombre);
   delete fichero;
  } // cierra_impresion

void pascal cancela_impresion (TFtemporal *fichero)
/* Cierra y borra el fichero de impresion temporal */
  {
   fclose (fichero->fichero);
   unlink (fichero->nombre);
   delete fichero;
  } // cancela_impresion

boolean pascal existe_fichero (char *nom_fich)
/* Verifica si existe un determinado fichero */
  {FILE *fichero;

   fichero = fopen (nom_fich, "rb");
   // Intenta abrir el fichero para lectura, si este no existe, la variable
   // fichero ser  NULL
   if (fichero != NULL)
     {fclose (fichero);
      return TRUE;
     }
   else return FALSE;
  } // existe_fichero

void pascal imprime_fecha (TFtemporal *fichero)
/* Imprime la fecha en el fichero temporal pasado */
  {date fecha;
   char aux[80];

   getdate (&fecha);
   sprintf (aux, " Fecha : %d/%3.3s/%-4d\r\n\r\n", fecha.da_day,
            c_mes[fecha.da_mon - 1], fecha.da_year);
   envia_datos (aux, fichero);
  } // imprime_fecha

int pascal escribe_cadena (char *palabra, int x, int y, int color,
                           boolean respeta_borde)
// Escribe una cadena en pantalla
  {char *aux;
   unsigned _color1, _color2;

   aux = palabra;

   switch (color)
     {case 0 : _color1 = colores.texto_menu;
               _color2 = colores.letra_menu;
               break;
      case 1 : _color1 = colores.seleccion_menu;
               _color2 = colores.seleccion_letra_menu;
               break;
      case 2 : _color1 = colores.texto_barra;
               _color2 = colores.letra_barra;
               break;
      case 3 : _color1 = colores.seleccion_barra;
               _color2 = colores.seleccion_letra;
               break;
     }

   if (respeta_borde == TRUE)
   // Si se respeta el borde se usan las funciones estandar de escritura para
   // usar el 'sistema' de ventanas
     {gotoxy (x, y);
      textattr (_color1);
      while (*aux)
        {if (*aux == '&')
           {aux++;
            textattr (_color2);
            putch (*aux++);
            textattr (_color1);
           }
         else
           putch (*aux++);
        }
      return (FP_OFF(aux) - FP_OFF(palabra));
     }
   else
   // Sino se escribe directamente a pantalla
     {unsigned *pantalla;

      pantalla = (unsigned *) MK_FP (segmento_pant, offset_pant +
                                     (--x * 2 + --y * 160));
      // Apunta a la posici¢n donde se escribir  la cadena
      _color1 <<= 8;
      _color2 <<= 8;
      while (*aux)
        {if (*aux == '&')
           {aux++;
            *pantalla++ = (*aux++) | _color2;
           }
         else
           *pantalla++ = (*aux++) | _color1;
        }
      return (FP_OFF(aux) - FP_OFF(palabra));
     }
  } // escribe_cadena

long pascal filesize(FILE *stream)
// Calcula la longitud de un fichero
  {
   long curpos, length;

   curpos = ftell(stream);
   fseek(stream, 0L, SEEK_END);
   length = ftell(stream);
   fseek(stream, curpos, SEEK_SET);
   return length;
  } // filesize

char * pascal busca_cadena (char *cadena2, char *cadena1, boolean mayusculas)
// Busca la cadena1 dentro de la cadena2
  {char *aux1, *aux2;
   boolean encontrado;

   aux1 = cadena1;
   aux2 = cadena2;
   encontrado = FALSE;

   while (*cadena2 && !encontrado)// mientras no se llegue al final de la cadena
     {                        // se busca el primer caracter de la cadena1
      if (!mayusculas)        // dentro de la cadena2
        // se hace la busqueda sin tener en cuenta las may£sculas y las
        // min£sculas
        {if ((*aux1 & 0x4F) == (*cadena2 & 0x4F))
         // si coincide el principio de cadena1 con el caracter examinado en
         // cadena2 se comparan los dem s caracteres de cadena1 con la cadena2
           {aux2 = cadena2;
            while ((*aux2 & 0x5F) == (*aux1 & 0x5F) && *aux1)
              {aux1++;
               aux2++;
              }
            // Si se puede comparar hasta llegar al final de cadena1 es que
            // se ha encontrado la cadena
            if (!*aux1)
              encontrado = TRUE;
            else
              aux1 = cadena1;
           }
        }
      else
        // Busqueda tomando en cuenta las mayusculas y las minusculas
        if (*aux1 == *cadena2)
          {aux2 = cadena2;
           while (*aux2 == *aux1)
             {aux1++;
              aux2++;
             }
           if (!*aux1)
             encontrado = TRUE;
           else
             aux1 = cadena1;
          }
      cadena2++;
     }
   // Si se ha encontrado la cadena se devuelve la posicion de esta dentro
   // de la otra cadena
   if (encontrado)
     return cadena2;
   else
     return 0;
  } // busca_cadena

void pascal imprime_lista (Cbase_datos &Objeto, char tipo_lista,
                           unsigned fichas_ini, unsigned fichas_fin)
// imprime la lista pasada
  {
   unsigned f, contador, temp;
   TFtemporal *salida;
   char barra[90], cadena[60];
   struct date fecha;

   if (!Objeto.hay_fichas())
     {sonido_error();
      sprintf (barra, "La lista de ");
      strcat (barra, tipo_lista == 'c' ? " clientes" : " art¡culos");
      strcat (barra, "est  vac¡a\r\n");
      mensaje (barra, 255, 255, 25, 7);
      return;
     }

   contador = 5;
   temp = Objeto.nro_ficha_actual;

   salida = inicializa_impresion ();
   getdate (&fecha);

   memset (barra, ' ', 75);
   barra[75] = 0;
   if (tipo_lista == 'c')
     {sprintf (cadena, "CLIENTES (%d/%d/%d)", fecha.da_day, fecha.da_mon,
               fecha.da_year);
      f = 40 - (strlen(cadena)>>2);
      gotoxy_text (barra, cadena, f);
      strcat (barra, "\r\n");
      envia_datos (barra, salida);
      memset (barra, ' ', 75);
      barra[75] = 0;
      gotoxy_text (barra, "ÍÍÍÍÍÍÍÍ\r\n", f);
      strcat (barra, "\r\n");
      envia_datos (barra, salida);
      envia_datos (
             "   C¢digo º Nombre                                   º "
             "Tel‚fono\r\n"
             "  ÍÍÍÍÍÍÍÍÎÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÎÍÍÍÍÍÍÍÍÍÍÍÍ"
             , salida);
     }
   else
    {sprintf (cadena, "ARTICULOS (%d/%d/%d)", fecha.da_day, fecha.da_mon,
               fecha.da_year);
      f = 40 - (strlen(cadena)>>2);
      gotoxy_text (barra, cadena, f);
      strcat (barra, "\r\n");
      envia_datos (barra, salida);
      memset (barra, ' ', 75);
      barra[75] = 0;
      gotoxy_text (barra, "ÍÍÍÍÍÍÍÍÍ\r\n", f);
      strcat (barra, "\r\n");
      envia_datos (barra, salida);
      envia_datos (
             " C¢digo º Descripci¢n                             º "
             " N§ Lote   º   Precio\r\n"
             " ÍÍÍÍÍÍÍÎÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÎÍ"
             "ÍÍÍÍÍÍÍÍÍÍÍÎÍÍÍÍÍÍÍÍÍÍÍÍ",
             salida);
    }

   f = fichas_ini;
   while (f <= fichas_fin)
     {Objeto.ficha_numero (Objeto.lista_ordenada [f]);

      switch (tipo_lista)
       {case 'c' :
          strcpy (cadena, Objeto.leer_dato(2));
          strcat (cadena, ", ");
          strcat (cadena, Objeto.leer_dato(3));
          cadena[40] = 0;
          sprintf (barra, "\r\n    %4d  º %-40.40s º %-10.10s",
                   *(unsigned*)Objeto.leer_dato(1), cadena,
                   Objeto.leer_dato(9));
          break;

        case 'a' :
          sprintf (barra, "\r\n  %4d  º %-39.39s º %-10.10s º %10.0f",
                   *(unsigned*)Objeto.leer_dato (1),
                   Objeto.leer_dato (2), Objeto.leer_dato (3),
                   *(double*)Objeto.leer_dato (6));
          break;
       }

      envia_datos (barra, salida);
      if (contador == lineas_papel - 4)
        {fputc (12, salida->fichero);
         contador = 0;
        }
      f++; contador++;
     }
   Objeto.ficha_numero (temp);
   imprime (salida, TRUE, 1);
  } // imprime_lista

unsigned pascal muestra_lista (Cbase_datos &Objeto, char tipo_lista,
                               unsigned fichas_ini, unsigned fichas_fin,
                               unsigned nro_lineas, boolean escoge)
// muestra la lista pasada
  {
   unsigned f, f1, f2, contador, temp, seleccion, y;
   boolean muestra = FALSE, redibuja = FALSE;
   char tecla, aux[80], cadena[60];

   if (!Objeto.hay_fichas())
     {sonido_error();
      sprintf (aux, "La lista de ");
      strcat (aux, tipo_lista == 'c' ? " clientes" : " art¡culos");
      strcat (aux, "est  vac¡a\r\n");
      mensaje (aux, 255, 255, 25, 7);
      return ERROR;
     }

   if (escoge)
     pone_barra_inferior (tecla_sel_mov);
   else pone_barra_inferior (teclas_mov);
   seleccion = f1 = f2 = fichas_ini;
   contador = 4;

   switch (tipo_lista)
    {case 'c' :
       sprintf (aux, "    %%4d  º %%-40.40s º %%-10.10s");
       break;

     case 'a' :
       sprintf (aux, "  %%4d  º %%-39.39s º %%-10.10s º %%10.0f");
       break;
    }

   if (tipo_lista == 'c')
     cputs ("   C¢digo º Cliente                                  º "
            "Tel‚fono\r\n"
            "  ÍÍÍÍÍÍÍÍÎÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÎÍ"
            "ÍÍÍÍÍÍÍÍÍÍÍ"
           );
   else
     cputs (" C¢digo º Descripci¢n                             º "
            " N§ Lote   º   Precio\r\n"
            " ÍÍÍÍÍÍÍÎÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÎÍ"
            "ÍÍÍÍÍÍÍÍÍÍÍÎÍÍÍÍÍÍÍÍÍÍÍÍ"
           );

   window (actual.winleft, actual.wintop + 2, actual.winright, actual.winbottom);
   actual.wintop += 2;
   temp = Objeto.nro_ficha_actual;
   y = 1;

   while (contador <= nro_lineas && f2 <= fichas_fin)
     {gotoxy (1, contador - 3);
      Objeto.ficha_numero (Objeto.lista_ordenada [f2]);

      switch (tipo_lista)
       {case 'c' :
          strcpy (cadena, Objeto.leer_dato(2));
          strcat (cadena, ", ");
          strcat (cadena, Objeto.leer_dato(3));
          cadena[40] = 0;
          cprintf (aux, *(unsigned*)Objeto.leer_dato(1), cadena,
                   Objeto.leer_dato(9));
          break;

        case 'a' :
          cprintf (aux, *(unsigned*)Objeto.leer_dato (1), Objeto.leer_dato (2),
                   Objeto.leer_dato (3), *(double*)Objeto.leer_dato (6));
          break;
       }
      f2++; contador++;
     }

   if (escoge)
     {gotoxy (1, y);
      putch ('>');
     }

   f2--;
   do
     {tecla = getch();
      switch (tecla)
        {case T_control :
           if (escoge)
             {gotoxy (1, y);
              putch (' ');
             }
           tecla = getch();
           switch (tecla)
             {case CURS_ABJ :
                if (escoge && seleccion < f2)
                  {seleccion++;
                   y++;
                  }
                else
                  if (f2 < fichas_fin)
                    {f1++; seleccion = ++f2;
                     scrollarriba (TRUE);
                     muestra = TRUE;
                    }
                  else muestra = FALSE;
                f = f2;
                break;

              case CURS_ARR :
                if (escoge && seleccion > f1)
                  {seleccion--;
                   y--;
                  }
                else
                  if (f1 > fichas_ini)
                    {seleccion = --f1; f2--;
                     scrollabajo (TRUE);
                     muestra = TRUE;
                    }
                  else
                     muestra = FALSE;
                f = f1;
                break;

              case REPAG :
              case AVPAG :
              case INICIO :
              case FIN :
                switch (tecla)
                  {case REPAG :
                     if (f1 > fichas_ini)
                       {f2 = f1;
                        f1 -= nro_lineas - 4;
                        seleccion -= nro_lineas - 4;
                        if (f1 > f2 || f1 < fichas_ini)
                          {seleccion = f1 = fichas_ini;
                           if (nro_lineas - 4 <= fichas_fin - fichas_ini)
                             f2 = fichas_ini + nro_lineas - 4;
                           else f2 = fichas_fin;
                          }
                        redibuja = TRUE;
                       }
                     break;

                   case AVPAG :
                     if (f2 < fichas_fin)
                       {f1 = f2;
                        seleccion += nro_lineas - 4;
                        f2 += nro_lineas - 4;
                        if (f2 > fichas_fin)
                          {seleccion = f2 = fichas_fin;
                           f1 = f2 - nro_lineas + 4;
                          }
                        redibuja = TRUE;
                       }
                     break;

                   case INICIO :
                     seleccion = f1 = fichas_ini;
                     y = 1;
                     if (nro_lineas - 4 <= fichas_fin - fichas_ini)
                       f2 = fichas_ini + nro_lineas - 4;
                     else f2 = fichas_fin;
                     redibuja = TRUE;
                     break;

                   case FIN :
                     seleccion = f2 = fichas_fin;
                     if (nro_lineas - 4 <= fichas_fin - fichas_ini)
                       {f1 = f2 - nro_lineas + 4;
                        y = nro_lineas - 3;
                       }
                     else {f1 = fichas_ini;
                           y = fichas_fin - fichas_ini + 1;
                          }
                     redibuja = TRUE;
                     break;
                  }
                muestra = FALSE;
                if (redibuja)
                  {clrscr();
                   for (contador = 1, f = f1; f <= f2; f ++, contador++)
                     {gotoxy (1, contador);
                      Objeto.ficha_numero (Objeto.lista_ordenada [f]);

                      switch (tipo_lista)
                       {case 'c' :
                          strcpy (cadena, Objeto.leer_dato(2));
                          strcat (cadena, ", ");
                          strcat (cadena, Objeto.leer_dato(3));
                          cadena[40] = 0;
                          cprintf (aux, *(unsigned*)Objeto.leer_dato(1),
                                   cadena, Objeto.leer_dato(9));
                          break;

                        case 'a' :
                          cprintf (aux, *(unsigned*)Objeto.leer_dato (1),
                                   Objeto.leer_dato (2), Objeto.leer_dato (3),
                                   *(double*)Objeto.leer_dato (6));
                          break;
                       }
                     }
                   redibuja = FALSE;
                  }

                break;

              default : muestra = FALSE;
             }
           break;
        }
      if (muestra)
        {Objeto.ficha_numero (Objeto.lista_ordenada [f]);

         switch (tipo_lista)
          {case 'c' :
             strcpy (cadena, Objeto.leer_dato(2));
             strcat (cadena, " ");
             strcat (cadena, Objeto.leer_dato(3));
             cadena[40] = 0;
             cprintf (aux, *(unsigned*)Objeto.leer_dato(1), cadena,
                      Objeto.leer_dato(9));
             break;

           case 'a' :
             cprintf (aux, *(unsigned*)Objeto.leer_dato (1), Objeto.leer_dato (2),
                      Objeto.leer_dato (3), *(double*)Objeto.leer_dato (6));
             break;
          }
         muestra = FALSE;
        }
      if (escoge)
        {gotoxy (1, y);
         putch ('>');
        }
     }
   while (tecla != ESC && tecla != ENTER);
   if (tecla == ENTER)
     {Objeto.ficha_numero (Objeto.lista_ordenada[seleccion]);
      seleccion = Objeto.nro_ficha_actual - 1;
     }
   else seleccion = ERROR;
   Objeto.ficha_numero (temp - 1);
   return seleccion;
  } // muestra_lista

boolean pascal pone_modo_132 (void)
// Activa un modo de v¡deo de 132 columnas
  {
   byte modos[4] = {0x24, 0x54, 0x47, 0x80},
        id, p, hor, far *ver;

   ver = (char*)MK_FP (0,0x484);
     // apuntamos ver a la pos. de memoria donde se indican el n§ de columnas

   if (vesa_instalada)
   // Si hay una tarjeta VESA que tenga un modo de 132 columnas se activa el
   // modo VESA
     {pone_modo_vesa (modo_vesa);
      return TRUE;
     }
   else
   // Sino se busca un modo por el otro m‚todo
     if (modo_132)
     // Si hay un modo de 132 columnas, se activa directamente
       {asm {mov   ah, 0
             mov   al, modo_132
             int   0x10
             mov   ah, 0xf
             int   0x10
             mov   hor, ah
            }
        res_hor = hor;
        return TRUE;
       }
     else
       // Sino se busca si hay un modo de 132 columnas
       {p = 0;
        do
          {id = modos[p];
           asm {
                cli
                mov       ah, 0
                mov       al, id
                int       0x10
                mov       ah, 0xf
                int       0x10
                mov       hor, ah
                add       p, 1
                sti
               }
          }
        while (p < 4 && (hor != 132 || *ver >= 40));
        // Si no se encuentra uno entre los "estandar", se busca en el resto de
        // los posibles c¢digos.
        if (p > 3)
          {p = 25;
           do
             {p++;
              asm {
                   cli
                   mov    ah, 0
                   mov    al, p
                   int    0x10
                   mov    ah, 0xf
                   int    0x10
                   mov    hor, ah
                   sti
                  }
             }
           while (p < 100 && (hor <= 80 || *ver > 43));
          }
        // Si no se ha encontrado nada se vuelve a poner el modo normal.
        if (p > 99)
          {textmode (actual.currmode);
           res_hor = actual.screenwidth;
           return FALSE;
          }
        else
         {res_hor = hor;
          modo_132 = p < 4 ? modos[p] : p;
          return TRUE;
         }
       }
  } // pone_modo_132;

void pascal pone_modo_normal (void)
// Restaura el modo normal de v¡deo
  {
   textmode (actual.currmode);
   res_hor = actual.screenwidth;
  } // pone_modo_normal

unsigned long pascal dos2luc (struct date *fecha)
// Pasa la fecha de formato dia/mes/a¤o a un nro. entero
  {
   return fecha->da_day - 1 + (fecha->da_mon - 1) * dias_mes +
          fecha->da_year * dias_anno;
  } // dos2luc

void pascal luc2dos (unsigned long fecha, struct date *fecha_des)
// Pasa de un nro. entero a una fecha determinada.
  {unsigned aux1;

   aux1 = fecha % dias_anno;
   fecha_des->da_year = fecha / dias_anno;
   fecha_des->da_mon = aux1 / dias_mes + 1;
   fecha_des->da_day = (aux1 % dias_mes) + 1;
  } // luc2dos

