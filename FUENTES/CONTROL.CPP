/*************************************************************************
 * Fichero : CONTROL.CPP                                                 *
 * Fichero que contiene las funciones necesarias para la impresion de    *
 * las lineas de texto, y para el control de la pantalla (scroles,       *
 * inicializar y restaurar la pantalla, etc). Tambien contiene todos     *
 * los ficheros necesarios, y las definiciones                           *
 *************************************************************************
 * Fecha creaci¢n : Un d¡a de noviembre del 94                           *
 * Fecha £ltima modificaci¢n : 31/7/96                                   *
 * (c)1996 Luis Fernando Santocildes Romero                              *
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
 *************************************************************************/

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

// Datos referentes al estandar de video VESA
typedef struct
  {char firma[4];
   unsigned nro_version;
   char *nombre_fabricante;
   long capacidades;
   unsigned *modos_soportados;
   unsigned mem_total;
   byte reservado[236];
  } Tdatos_vesa;

// Datos referentes a los modos de v¡deo VESA
typedef struct
  {unsigned atributos;
   byte atr_ventana_1, atr_ventana_2;
   unsigned granularidad,
            tam_ventana,
            seg_inicio_vent_1,
            seg_inicio_vent_2;
   void * rutina_ajuste;
   unsigned bytes_linea,
            resolucion_x,
            resolucion_y;
   byte ancho_caracter,
        alto_caracter,
        bits_planes,
        bits_pixel,
        modelo_memoria,
        tam_bloques_memoria,
        nro_paginas,
        reservado,
        tam_mascara_roja, pos_campo_rojo,
        tam_mascara_verde, pos_campo_verde,
        tam_mascara_azul, pos_campo_azul,
        tam_mascara_reservada, pos_campo_reservado,
        reservados[218];
  } Tdatos_modo;

/************************** PROTOTIPOS *************************************/
void pascal dibuja_escritorio (void);
                /* Imprime el fondo de la pantalla e inicializa los colores */
void pascal pone_barra (char *barra);
                                               /* Imprime la barra superior */
void pascal pone_barra_inferior (char *barra);
                                               /* Imprime la barra inferior */
int pascal escoge_opcion (char *cadena);
     /* Detiene el programa hasta que se escoge una opci¢n de las indicadas en
                                                                     cadena */
void pascal scrollabajo(void);
                                           /* Realiza un scroll hacia abajo */
void pascal scrollarriba(void);
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
                                 float &iva_1, unsigned long &ultima_factura);
                                /* Inicializa la configuraci¢n del programa */
void pascal restaura_ordenador (char *nombre_clientes, char *nombre_articulos,
                                char *nombre_facturas, char *nombre_fact,
                                float iva_1);
   /* Restaura el ordenador a un estado parecido al que ten¡a antes de usar el
      programa */
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
                            boolean nueva = TRUE, char caracter_relleno = ' ');
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
boolean pascal hay_tarjeta_vesa (Tdatos_vesa *buffer);
                                /* Verifica que la tarjeta gr fica sea VESA */
boolean pascal pone_modo_vesa (unsigned modo);
                                                     /* Activa un modo VESA */
boolean pascal datos_modo_vesa (unsigned modo, Tdatos_modo *buffer);
                                 /* Devuelve informacion sobre un modo VESA */
void pascal clrscr_x (void);
                             /* Borra la pantalla en un modo extendido VESA */
boolean pascal existe_fichero (char *nom_fich);
                               /* Verifica si existe un determinado fichero */
void pascal imprime_fecha (TFtemporal *fichero);
                          /* Imprime la fecha en el fichero temporal pasado */

/***************************VARIABLES GLOBALES****************************/
boolean reloj_visible = FALSE,   // Controla si el reloj es visible
        reloj_instalado = FALSE, // Idem pero si este est  instalado
        hace_sonido = TRUE,       // Indica si hace los sonidos
        hay_vesa;                 // Indica si hay una tarjeta VESA para
                                  // poder poner 132 columnas

unsigned modo_132 = 0,            // Se almacenar  aqu¡ el n£mero del modo
         ancho_columna;           // de 132 columnas y 25 filas

int nro_copias,                   // Nro de copias a imprimir
    lineas_papel,                 // L¡neas por p gina
    puerto_conexion;              // Puerto de conexi¢n de la impresora

char directorio_programa[MAXPATH];
// Unidad y directorio donde est  el programa;

boolean configuracion_modificada = FALSE;

const char fondo[] = "°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°";
 // Fondo de la pantalla
const char barra2[] = "                                                                                ";
 // Barra normal

// Mensajes varios
char press_enter[] = " Pulse &ENTER& para continuar...";
char press_tecla[] = " Pulse una tecla para continuar...";
char esc_tab[] = " Escoja con el tabulador y pulse &ENTER&, o pulse &S& o &N&.";
char esc_opcion[] = " Escoja una opci¢n o pulse &ESC& para volver al "
                          "men£.";
char esc_sino[] = " Escoja con &S& o &N&.";
char imp_fact[] = " &CTRL-I& imprime la factura, &ESC& la anula, otra tecla contin£a.";
char imp_lista[] = " &CTRL-I& imprime la lista, otra tecla contin£a.";

unsigned desplazamiento_horizontal = 0, // Desplazamiento X e Y del interior
         desplazamiento_vertical = 0,   // de la ventana
         ancho_actual = 0;            // Ancho en columnas de la pantalla

text_info info_pantalla, // Estado de la pantalla al iniciarse el programa
          aux_pantalla;

// Nombre del fichero de configuraci¢n
char nombre_fich[MAXPATH];

// Posibles puertos de conexion de la impresora
char puertos[][5] = {"LPT1","LPT2","LPT3","COM1","COM2","COM3",
                     "COM4","FICH"};

// Cabeceras de las facturas
char *cabecera_facturas;

// Lista de los meses
char *c_mes[12] = {"Ene", "Feb", "Mar", "Abr", "May", "Jun",
                   "Jul", "Ago", "Sep", "Oct", "Nov", "Dic"};

// Hora nula para trasformar fechas
struct time hora_nula = {0, 0, 0, 0};
/*************************************************************************/
void pascal scrollarriba(void)
// Desplaza la pantalla una l¡nea hacia arriba
{
 movetext (1, 2, 80, 24, 1, 1);
 gotoxy (1, 24);
 clreol();
}// scrollarriba

void pascal scrollabajo(void)
// Desplaza la pantalla una l¡nea hacia abajo.
{
 movetext (1, 1, 80, 23, 1, 2);
 gotoxy (1, 1);
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
{int f, g, aux_alto, aux_ancho, offset;
 Tpantalla aux;

 // Calcula la posici¢n del recuadro y reserva memoria
 if (x == 255) x = (info_pantalla.screenwidth >> 1) - (ancho >> 1);
 if (y == 255) y = (info_pantalla.screenheight >> 1) - (alto >> 1);
 aux = guarda_recuadro (x, y, ancho, alto, tipo_marco);
 aux_alto = alto * 160;
 aux_ancho = ancho * 2;
 offset = (x - 1) * 2 + (y - 1) * 160;

 poke (0xb800, offset, 'É' + COLOR);             /* borde superior */
 poke (0xb800, offset + aux_alto, 'È' + COLOR);  /* e inferior     */
 for (f = 2; f < aux_ancho; f+=2)
   {poke (0xb800, offset + f, 'Í' + COLOR);
    poke (0xb800, offset + f + aux_alto, 'Í' + COLOR);
   }
 poke (0xb800, offset + f, '»' + COLOR);
 poke (0xb800, offset + f + aux_alto, '¼' + COLOR);
 for (f = 160; f < aux_alto; f+=160)               /* bordes laterales */
   {poke (0xb800, offset + f, 'º' + COLOR);        /* e interior       */
    for (g = 2; g < aux_ancho; g+=2)
      poke (0xb800, offset + g + f, ' ' + COLOR);
    poke (0xb800, offset + g + f, 'º' + COLOR);
   }
 if (cadena)
 // Si hay una cadena la coloca centrada en el borde superior del marco
   for (f = ((ancho>>1) - (strlen(cadena)>>1))<<1; *cadena != 0;
        f += 2, cadena++)
     poke (0xb800, offset + f, *cadena + 0x1f00);
 textattr ((BLUE << 4) | YELLOW);
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
}/* restaura_recuadro */

int pascal aviso (char *texto, byte x, byte y, byte ancho, byte alto)
// Dibuja un recuadro en pantalla con un mensaje y dos botones (SI y NO)
// para pedir un respuesta positiva o negativa
{int boton = 0;
 Tpantalla pantalla;

 pantalla = hace_recuadro (x, y, ancho, alto);
 _setcursortype (_NOCURSOR);
 pone_barra_inferior (esc_tab);
 textattr (WHITE | (BLUE<<4));
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
 textbackground (BLUE);
 textcolor (YELLOW);
 pantalla = hace_recuadro (x, y, ancho, alto);
 gotoxy (2,2);
 cputs (texto);            // Imprime el mensaje

 textbackground (GREEN);
 textcolor (WHITE);
 gotoxy (3, alto - 2);
 for (aux = 0; (aux < ancho - 6) && (aux < long_resp); aux++)
   putch (' ');  // Imprime el espacio para escribir la respuesta
 gotoxy (3, alto - 2);

 aux = lee_cadena (respuesta, long_resp, tipo_cadena); // Lee la respuesta

 restaura_recuadro (&pantalla);
 _setcursortype (_NOCURSOR);
 if (aux == ESC)   // Si se ha salido pulsando ESC se devuelve FALSE indicando
   return FALSE;   // que no se devuelve ninguna respuesta
 else return TRUE;
}/* pregunta */

void pascal mensaje (char *texto, byte x, byte y, byte ancho, byte alto)
// Dibuja un recuadro en pantalla con un mensaje
{Tpantalla pantalla;

 pone_barra_inferior (press_enter);
 _setcursortype (_NOCURSOR);
 textbackground (BLUE);
 textcolor (YELLOW);
 pantalla = hace_recuadro (x, y, ancho, alto, " AVISO ");
 gotoxy (2,2);
 cputs (texto);
 gotoxy (4, alto - 2);
 textbackground (GREEN);
 cputs ("  Ok  ");
 textbackground (BLUE);
 while (getch() != ENTER);
 restaura_recuadro (&pantalla);
}/* mensaje */

void pascal imprime_botones (int boton, int alto)
// Imprime los botones SI y NO que se usan en la funcion aviso. El color de
// los botones depende del valor de boton
{
 textbackground (GREEN);                    /* segun el boton escogido se  */
 textcolor (RED);                           /* imprimen los botones en uno */
 gotoxy (3 + (boton ? 0 : 8), alto - 2);    /* u otro color.               */
 cputs ((boton ? " Si " : " No "));
 textbackground (YELLOW);
 textcolor (BLUE);
 gotoxy (3 + (boton ? 8 : 0), alto - 2);
 cputs ((boton ? " No " : " Si "));
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
 offset = (x - 1) * 2 + (y - 1) * 160;
 for (f = 0; f < repeticiones; f++)
   pokeb (0xb800, offset+=2, caracter);   /* imprime la letra */
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
   textcolor (BLUE);
   textbackground (LIGHTGRAY);
   clrscr();
   for (f = 1; f < 25; f++)
     {gotoxy (1, f);
      cputs (fondo);
     } // Imprime el fondo al estilo del Turbo Pascal
   gotoxy(1,1);cputs(barra2);
     // Coloca la barra superior. La inferior queda colocada automaticamente
     // al imprimir el fondo
   _setcursortype (_NOCURSOR); // Elimina el cursor.
  } // inicializa pantalla

void pascal pone_barra (char *barra)
// Imprime el contenido del men£ pasado en la barra superior
  {byte color;
   int x = 1;

   color = BLACK | (LIGHTGRAY<<4);  // color inicial negro sobre gris
   textattr (color);

   gotoxy (1,1);cputs (barra2);
   while (*barra != 0)     // Mientras no llegue al final de la cadena
     {switch (*barra)
        {case '&' : // Si encontramos un caracter '&'
            color = RED | (LIGHTGRAY<<4);
            textattr (color);  // Cambiamos el color a rojo sobre gris,
            barra++;            // pasamos al siguiente caracter,
            gotoxy (x, 1); putch (*barra);  // lo imprimimos,
            barra++; x++;                    // pasamos al siguiente caracter y
            color = BLACK | (LIGHTGRAY<<4);
            textattr (color);               // restauramos el color original
            break;

         case '#' : barra += 2;   // Si encontramos un # el caracter siguiente
                                  // no se imprime, pero funciona casi como
                                  // un &
                    break;

         default : gotoxy (x, 1);
                   putch (*barra);  // sino, lo imprimimos
                   barra++, x++;            // y pasamos al siguiente
                   break;
        }
     }
  } // pone_barra

void pascal pone_barra_inferior (char *cadena)
// Imprime en la barra inferior del escritorio
  {unsigned far *aux1, aux2 = 0;
   boolean act_color = FALSE;
   byte color = 0x70;

   aux2 = peek (0x40, 0x4a);
   aux1 = (unsigned far *)MK_FP (0xB800, (aux2 << 1) * 24);
   for (; aux2 > 0; aux2--, aux1++)
     *aux1 = 0x7020;
      // Limpia la barra inferior
   // Imprime la cadena pasada, cambiando el color del caracter si es necesario
   aux1 = (unsigned far *)MK_FP (0xB800, (peek(0x40,0x4a) << 1) * 24);
   while (*cadena != 0)
     {if (*(cadena) == '&')
        if (act_color)
          {color = 0x70;
           act_color = FALSE;
           cadena++;
          }
        else {color = 0x74;
              act_color = TRUE;
              cadena++;
             };
      *aux1 = (*cadena++) | (color<<8);
      aux1++;
     }  // Imprime la cadena pasada
  } // pone_barra_inferior

int pascal escoge_opcion (char *cadena)
// Escoge una opci¢n del men£ pasado en la cadena. El formato de la cadena es
// el mismo que el de la pasada a pone_barra
  {int contador, auxi;
   char letras[10], tecla;

   contador = 0;

   // Se analiza la cadena buscando las letras especiales.
   while (*cadena != 0 && contador < 10)
     // Mientras no lleguemos al final de la cadena o no hayamos encontrado
     // 10 letras
     {if (*cadena == '&')  // Si encontramos un '&'
        {cadena++;
         letras[contador] = toupper(*cadena);
         // almacenamos el sig. caracter en may£sculas
         contador++;
        }
      else if (*cadena == '#')     // Si encontramos un #
             {cadena++;
              letras[contador] = *cadena - 48;
              /* Se almacena el valor num‚rico del siguiente c¢digo, es decir
                 que para un '0' se almacenar  un 0, para un '1' un 1, etc.
              */
              contador++;
             }
      cadena++; // y pasamos al siguiente caracter
     };

   // Ahora detectamos la pulsaci¢n de las teclas
   do
     {tecla = toupper(getch());  // Se pasa la tecla pulsada a may£sculas
      auxi = 0;
      while (letras[auxi] != tecla && auxi <= contador)
        auxi++;  // y se busca en la tabla de letras
     }
   while (auxi > contador && tecla != ESC);
   // y se repite el ciclo hasta que se pulse una tecla que est‚ en la tabla
   // o hasta que se pulsa la tecla ESC
   if (tecla == ESC) auxi = -1;
   return (auxi);  // Devolvemos la posici¢n de la tecla en la tabla, o -1 si
                   // se ha pulsado ESC
  } // escoge_opcion


void pascal inicializa_programa (char *nombre_clientes, char *nombre_articulos,
                                 char *nombre_facturas, char *nombre_fact,
                                 float &iva_1, unsigned long &ultima_factura)
/* Inicializa la configuraci¢n del programa. */
  {
   FILE *fichero;
   char unidad[MAXDRIVE],   // Unidad donde est  el programa
        directorio[MAXDIR], // Directorio donde est el programa
        nombre[MAXFILE],    // Nombre del fichero ejecutado
        extension[MAXEXT],  // Extension del fichero
        aux[80],
        *auxc;              // Cadenas auxiliares
   struct ffblk ff;         // Estructura usada para la b£squeda de ficheros
   unsigned error, *modo;
   Tdatos_vesa vesa;        // Usada para recoger datos sobre la VESA
   Tdatos_modo info_modo;   // Para recoger datos sobre los modos VESA
   Tdatos_fichero_facturas datos_factura;

   hay_vesa = hay_tarjeta_vesa (&vesa); // Comprueba si hay una VESA
   if (hay_vesa)
     // Si la hay se busca un modo de texto que tenga 25 filas y m s de 80
     // columnas
     {modo = vesa.modos_soportados;
      while (TRUE)
        {datos_modo_vesa (*modo, &info_modo);
         if (*modo == 0xffff || ((info_modo.atributos & 16) == 0 &&
             info_modo.resolucion_y == 25 && info_modo.resolucion_x > 80))
           break;
           // Si se encuentra el modo buscado, o se llega al final de la lista
           // se sale del bucle
         modo++;
        }
      if (*modo != 0xffff)
        // Si se ha encontrado el modo se leen los datos que interesan
        {modo_132 = *modo;
         ancho_columna = info_modo.resolucion_x;
        }
      else modo_132 = 0;
     }

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
   strcpy (nombre_fich, unidad);
   strcat (nombre_fich, directorio);
   strcat (nombre_fich, "factura.cfg");
     // Forma el nombre del fichero de configuraci¢n
   strcpy (directorio_programa, unidad);
   strcat (directorio_programa, directorio);
     // Forma la cadena con el directorio del programa

   configuracion_modificada = FALSE;
   fichero = fopen (nombre_fich, "rb");
     // Se abre el fichero de configuraci¢n
   if (fichero == NULL)
     configuracion_modificada = TRUE;
   else
     // Se verifica si el fichero es v lido, si lo es se leen los datos
     // de configuraci¢n
     {fread (aux, strlen (CLAVE_FICH), 1, fichero);
      aux [strlen(CLAVE_FICH)] = 0;

      if (strcmp (CLAVE_FICH, aux) == 0)
        {
         fread (&hace_sonido, sizeof(boolean), 1, fichero);
         fread (&iva_1, sizeof(float), 1, fichero);
         fread (nombre_clientes, MAXPATH, 1, fichero);
         fread (nombre_articulos, MAXPATH, 1, fichero);
         fread (nombre_facturas, MAXPATH, 1, fichero);
         fread (nombre_fact, MAXPATH, 1, fichero);
         fread (&nro_copias, sizeof(int), 1, fichero);
         fread (&lineas_papel, sizeof(int), 1, fichero);
         fread (&puerto_conexion, sizeof(int), 1, fichero);

         fclose (fichero);
        }
      else
        {
         fclose (fichero);
         configuracion_modificada = TRUE;
        }
     }

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
      iva_1 = 7.0;
      strcpy (nombre_clientes, directorio_programa);
      strcat (nombre_clientes, "CLIENTES.DAT");
      strcpy (nombre_articulos, directorio_programa);
      strcat (nombre_articulos, "ARTICULO.DAT");
      strcpy (nombre_facturas, directorio_programa);
      strcat (nombre_facturas, "FACTURAS.DAT");
      strcpy (nombre_fact, directorio_programa);
      strcat (nombre_fact, "FACT.DAT");
     }

   // Se abre el fichero de facturas para leer el nro de la £ltima factura y
   // as¡ poder continuar numer ndolas correctamente
   fichero = fopen (nombre_facturas, "rb");
   if (fichero)
     {while (TRUE)
        {if (feof (fichero))
           break;
         fread (&datos_factura, sizeof(Tdatos_fichero_facturas), 1, fichero);
        }
      ultima_factura = datos_factura.nro_factura;
     }

   // Carga la cabecera de las facturas en memoria
   strcpy (aux, directorio_programa);
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
                                float iva_1)
/* Restaura el ordenador a un estado parecido al que ten¡a antes de usar el
   programa */
  {
   FILE *fichero;

   // Almacena la configuraci¢n
   if (configuracion_modificada)
     {fichero = fopen (nombre_fich, "wb");
      fwrite (CLAVE_FICH, strlen (CLAVE_FICH), 1, fichero);
      fwrite (&hace_sonido, sizeof(boolean), 1, fichero);
      fwrite (&iva_1, sizeof(float), 1, fichero);
      fwrite (nombre_clientes, MAXPATH, 1, fichero);
      fwrite (nombre_articulos, MAXPATH, 1, fichero);
      fwrite (nombre_facturas, MAXPATH, 1, fichero);
      fwrite (nombre_fact, MAXPATH, 1, fichero);
      fwrite (&nro_copias, sizeof(int), 1, fichero);
      fwrite (&lineas_papel, sizeof(int), 1, fichero);
      fwrite (&puerto_conexion, sizeof(int), 1, fichero);

      fclose (fichero);
     }

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

void pascal pone_caracter (char caracter, unsigned columna, unsigned fila,
                    unsigned atributos)
// Coloca un caracter en una posici¢n determinada de la pantalla
  {unsigned *posicion;

   // Primero se calcula que la posici¢n no est‚ fuera de la ventana actual,
   // sino se sale de la funci¢n
   if ((columna < desplazamiento_horizontal) ||
       (columna > aux_pantalla.winright - aux_pantalla.winleft +
                  desplazamiento_horizontal))
     return;
   if ((fila < desplazamiento_vertical) ||
       (fila > aux_pantalla.winbottom - aux_pantalla.wintop +
               desplazamiento_vertical))
     return;
   // Se calcula la posici¢n en la pantalla
   posicion = (unsigned*)MK_FP (0xB800,
        (aux_pantalla.winleft + columna - desplazamiento_horizontal - 1) * 2 +
        (aux_pantalla.wintop + fila - desplazamiento_vertical - 1) *
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

   gettextinfo (&aux_pantalla); // Coge el estado de la pantalla
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

   gettextinfo (&aux_pantalla); // Coge el estado de la pantalla
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

   gettextinfo (&aux_pantalla);  // Coge el estado de la pantalla
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
                     boolean nueva, char caracter_relleno)
/* Lee una cadena desde el teclado */
  {char *aux,
        tecla,
        *temp = NULL;
   unsigned contador,
            x, y,
            aux_x, aux_y,
            pos_cursor;
   boolean insercion = TRUE; // Comenzamos en modo de inserci¢n

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
              *(temp + f) = caracter_relleno;
            *(temp + f) = 0;
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
                    break;

         case ENTER :
         case LF :
         case TAB : // Si se pulsa ENTER, TAB o CTRL-ENTER se sale del bucle
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
               case S_TAB : break;
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
   while (tecla != ESC && tecla != ENTER && tecla != TAB && tecla != LF &&
          tecla != S_TAB);
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
      *cadena = 0;
     }
   else *cadena = 0;
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
        {case ENTER : // Si se pulsa ENTER o TAB se pasa al campo siguiente
         case TAB :   // almacenando el dato actual temporalmente
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
  {byte buffer[256];
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

boolean pascal hay_tarjeta_vesa (Tdatos_vesa *buffer)
/* Verifica que la tarjeta gr fica sea VESA */
  {
   asm {cli
        push es
        les  di, buffer   // Se apunta al buffer
        mov  ax, 0x4f00
        int  0x10         // Se verifica si existe la VESA
        pop  es
        sti
        test ah, ah
        je   hay_vesa
       }
   return FALSE;
   hay_vesa:
   return TRUE;
  } // hay_tarjeta_vesa

boolean pascal pone_modo_vesa (unsigned modo)
/* Activa un modo VESA */
  {Tdatos_modo buffer;

   if (hay_vesa)  // Si la vesa existe
     {asm {cli
           push es
           mov  ax, 0x4f01
           mov  cx, modo
           push ss
           pop  es
           lea  di, buffer      // Se apunta al bufer
           int  0x10            // Se piden los datos sobre el modo
           test ah, ah
           jnz  sale            // Si la tarjeta no es VESA, se sale
           mov  ax, 0x40
           mov  es, ax
           lea  si, buffer.resolucion_x
           mov  ax, ss:[si]
           mov  es:[0x4a], ax   // sino, se almacena la resoluci¢n X del modo
           mov  ax, 0x4f02
           mov  bx, modo
           int  0x10            // y se cambia al modo pedido
           test ah, ah
           jz   hay_vesa:
          }
      sale:
      asm pop es
      asm sti
      return FALSE;
      hay_vesa:
      asm pop es
      asm sti
      return TRUE;
     }
   return FALSE;
  } // pone_modo_vesa

boolean pascal datos_modo_vesa (unsigned modo, Tdatos_modo *buffer)
/* Devuelve informacion sobre un modo VESA */
  {
   if (hay_vesa)
   // Si la tarjeta es VESA
     {asm {cli              // Se quitan las interrupciones
           push  es
           mov   ax, 0x4f01
           mov   cx, modo
           les   di, buffer // Se apunta a buffer
           int   0x10       // Se pide informaci¢n sobre el modo
           pop   es
           test  ah, ah
           sti
           jz    hay_vesa:
          }
      return FALSE;         // Si la tarjeta no es VESA se devuelve FALSE
      hay_vesa:
      return TRUE;          // En otro caso se devuelve TRUE
     }
   return FALSE;
  } // datos_modo_vesa

void pascal clrscr_x (void)
// Borra la pantalla en un modo extendido VESA de 25 filas
  {
   asm {push   di
        push   es
        push   cx
        push   ax  // Se guardan los registros en la pila

        mov    ax, 0x40
        mov    es, ax
        mov    di, 0x50
        mov    cx, 0
        mov    es:[di], cx
        mov    di, 0x4a
        mov    cx, es:[di] // Se obtiene el ancho de la pantalla
        mov    ax, 25      // Se multiplica por 25 (nos da el nro de
        mul    cx          // caracteres de la pantalla)
        mov    cx, ax      // Lo guardamos en CX
        mov    ax, 0xb800
        mov    es, ax
        mov    di, 0       // Apuntamos al inicio de la pantalla
        cld
        mov    ax, 0x1e20

        rep stosw          // Se rellena la pantalla con el valor de ax

        pop    ax
        pop    cx
        pop    es
        pop    di          // Se restauran los registros
       }
  } // clrscr_x

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
