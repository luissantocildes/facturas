/*************************************************************************
 * Fichero : CONTROL.H                                                   *
 * Fichero que contiene las funciones necesarias para la impresion de    *
 * las lineas de texto, y para el control de la pantalla (scroles,       *
 * inicializar y restaurar la pantalla, etc). Tambien contiene todos     *
 * los ficheros necesarios, y las definiciones                           *
 * Enlaza solo con las funciones necesarias de las definidas en          *
 * CONTROL.CPP                                                           *
 *************************************************************************
 * Fecha £ltima modificaci¢n : 18/11/96                                  *
 * (c)1996 Luis Fernando Santocildes Romero                              *
 *************************************************************************/

// estructura usada para las funciones de ventana
typedef struct {text_info info_pantalla;  // contiene acerca de la ventana
                                          // anterior
                char *datos;              // Contiene los datos para borrar la
                                          // ventana actual
               } Tpantalla;

// Usada para ficheros temporales
typedef struct
  {char nombre[MAXPATH]; // nombre del fichero temporal
   FILE *fichero;        // puntero para acceder al fichero.
  } TFtemporal;

// Estructura de los datos que informan sobre el tipo de tarjeta VESA
typedef struct
  {char firma[4];               // nombre de la tarjeta
   unsigned nro_version;        // n£mero de versi¢n del estandar VESA
   char *nombre_fabricante;     // nombre del fabricante de la tarjeta
   long capacidades;            // ¨?
   unsigned *modos_soportados;  // lista con los modos de v¡deo soportados
   unsigned mem_total;          // memoria total de v¡deo
   byte reservado[236];         // bytes in£tiles (por el momento)
  } Tdatos_vesa;

// Forma en la que se estructuran la informaci¢n sobre cada modo de v¡deo VESA
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
extern void pascal dibuja_escritorio (void);
                /* Imprime el fondo de la pantalla e inicializa los colores */
extern void pascal pone_barra (char *barra);
                                               /* Imprime la barra superior */
extern void pascal pone_barra_inferior (char *barra);
                                               /* Imprime la barra inferior */
extern int pascal escoge_opcion (char *cadena);
     /* Detiene el programa hasta que se escoge una opci¢n de las indicadas en
                                                                     cadena */
extern void pascal scrollabajo(void);
                                           /* Realiza un scroll hacia abajo */
extern void pascal scrollarriba(void);
                                                  /* Idem pero hacia arriba */
extern void pascal scrollizquierda (unsigned nro_lineas);
extern void pascal scrollderecha (unsigned nro_lineas);
     /* Realiza un desplazamiento de la pantalla a la izquierda o a la derecha
        tantas l¡neas como se le indique */
extern Tpantalla pascal hace_recuadro (byte x, byte y, byte ancho, byte alto,
                                       char *cadena = NULL, int tipo_marco = 1);
                                       /* Dibuja un recuadro en la pantalla */
extern Tpantalla pascal guarda_recuadro (byte x, byte y, byte ancho, byte alto,
                                         int tipo_marco = 1);
                 /* almacena una zona rectangular de la pantalla en memoria */
extern void pascal restaura_recuadro (Tpantalla *pantalla);
                /* restaura la zona de pantalla guardada por la f. anterior */
extern int pascal aviso (char *texto, byte x, byte y, byte ancho, byte alto);
                               /* cuadro de dialogo con dos botones (SI/NO) */
extern void pascal mensaje (char *texto, byte x, byte y, byte ancho, byte alto);
                          /* cuadro de dialogo que solo presenta un mensaje */
extern boolean pascal pregunta (char *texto, byte x, byte y, byte ancho, byte alto,
                                char *respuesta, int long_resp,
                                boolean tipo_cadena = TRUE);
            /* cuadro de dialogo con un campo para introducir una respuesta */
extern void pascal pone_modo_cursor (int ins);
                                                        /* cambia el cursor */
extern void pascal borra_resto_linea (unsigned int pos_cursor);
                                  /* borra la linea fisica desde pos_cursor */
extern void pascal instala_reloj (void);
                                         /* Instala el controlador de reloj */
extern void pascal desinstala_reloj (void);
                                           /* Quita el controlador de reloj */
extern void pascal sonido_error (void);
                                                       /* Produce un pitido */
extern void pascal inicializa_programa (char *nombre_clientes, char *nombre_articulos,
                                        char *nombre_facturas, char *nombre_fact,
                                        float &iva_1, unsigned long &ultima_factura);
                                /* Inicializa la configuraci¢n del programa */
extern void pascal restaura_ordenador (char *nombre_clientes, char *nombre_articulos,
                                       char *nombre_facturas, char *nombre_fact,
                                       float iva_1);
   /* Restaura el ordenador a un estado parecido al que ten¡a antes de usar el
      programa */
extern void pascal pone_caracter (char caracter, unsigned columna, unsigned fila,
                           unsigned atributos);
             /* Pone un caracter en pantalla, teniendo en cuenta el recorte */
extern void pascal linea_vert (unsigned columna, unsigned fila, unsigned largo,
                        unsigned atributos = YELLOW | (BLUE << 4) );
                                               /* Dibuja una l¡nea vertical */
extern void pascal linea_horz (unsigned columna, unsigned fila, unsigned largo,
                        unsigned atributos = YELLOW | (BLUE << 4) );
                                             /* Dibuja una l¡nea horizontal */
extern void pascal imprime_cadena (char *cadena, unsigned columna, unsigned fila,
                                   unsigned atributos = YELLOW | (BLUE << 4),
                                   boolean desplaza = TRUE);
                                       /* Imprime una cadena en la pantalla */
extern unsigned pascal lee_cadena (char *cadena, unsigned max_car,
                            boolean alfanumerica = TRUE, boolean nueva = TRUE,
                            char caracter_relleno = ' ');
                                         /* Lee una cadena desde el teclado */
extern void pascal saca_papel (void);
                                           /* Saca el papel de la impresora */
extern void pascal completa_cadena (char *cadena, unsigned longitud,
                                    char caracter_relleno = ' ',
                                    boolean pone_crlf = FALSE);
                         /* Completa una cadena con un caracter determinado */

extern char * pascal gotoxy_text (char *destino, char *origen, unsigned x);
                                         /* Ubica una cadena dentro de otra */
extern void pascal pide_fecha (char *texto, byte x, byte y, byte ancho,
                               byte alto, date *fecha, boolean nueva = TRUE);
                              /* Pide una fecha y verifica que sea correcta */
extern TFtemporal * pascal inicializa_impresion (void);
                                      /* Inicializa el fichero de impresion */
extern unsigned pascal envia_datos (char *cadena, TFtemporal *fichero);
                                            /* Prepara los datos a imprimir */
extern unsigned pascal imprime (TFtemporal *fichero, boolean pasa_papel = TRUE,
                                unsigned copias = 0);
                    /* Imprime los datos en el dispositivo de salida actual */
extern void pascal cancela_impresion (TFtemporal *fichero);
                                /* Imprime el fichero de impresion temporal */
extern void pascal cierra_impresion (TFtemporal *fichero);
               /* Cierra y borra todos los ficheros temporales de impresi¢n */
extern boolean pascal hay_tarjeta_vesa (Tdatos_vesa *buffer);
                                /* Verifica que la tarjeta gr fica sea VESA */
extern boolean pascal pone_modo_vesa (unsigned modo);
                                                     /* Activa un modo VESA */
extern boolean pascal datos_modo_vesa (unsigned modo, Tdatos_modo *buffer);
                                 /* Devuelve informacion sobre un modo VESA */
extern void pascal clrscr_x (void);
                             /* Borra la pantalla en un modo extendido VESA */
extern boolean pascal existe_fichero (char *nom_fich);
                               /* Verifica si existe un determinado fichero */
extern void pascal imprime_fecha (TFtemporal *fichero);
                          /* Imprime la fecha en el fichero temporal pasado */

/*******************************VARIABLES GLOBALES**************************/
extern boolean reloj_visible;
/* Controla si el reloj es visible o no. Los valores posibles son TRUE o
   FALSE. Solo funciona si el reloj est  instalado */
extern boolean hace_sonido;
/* Indica si la funci¢n sonido error debe hacer sonidos. Booleana.
   Inicialmente vale TRUE */
extern boolean hay_vesa;
/* Se puede comprobar por medio de esta variable si la tarjeta es compatible
   VESA */

extern unsigned modo_132;
/* N£mero de modo del modo de 132 columnas */

extern int nro_copias,       // Nro de copias a hacer en cada impresi¢n
           lineas_papel,     // L¡neas a imprimir por fol¡os
           puerto_conexion;  // ¡ndice del puerto de conexi¢n de la impresora

extern char puertos[][5];    // lista de los puertos de conexi¢n de impresora

extern char directorio_programa[MAXPATH];
// Unidad y directorio donde est  el programa;

extern boolean configuracion_modificada;
// Usada para determinar si se debe o no almacenar la configuraci¢n actual al
// salir del programa

// Mensajes varios
extern char press_enter[];
extern char press_tecla[];
extern char esc_tab[];
extern char esc_opcion[];
extern char esc_sino[];
extern char imp_fact[];
extern char imp_lista[];

/* Desplazamiento del origen de coordenadas de la ventana con respecto a la
   esquina superior izquierda de la misma. Se usa en la funci¢n que imprime
   un caracter en pantalla y en las que dibujan l¡neas horizontales y
   verticales. */
extern unsigned desplazamiento_horizontal,
                desplazamiento_vertical;

extern const char *cabecera_facturas; // Texto de cabecera de las facturas
extern const char *c_mes[12];         // Lista de los meses

extern struct time hora_nula;         // Contiene las 0:00:00

extern text_info info_pantalla;       // Informaci¢n sobre la pantalla cuando
                                      // se arranca el programa