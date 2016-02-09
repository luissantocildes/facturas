/*************************************************************************
 * Fichero : CONTROL.H                                                   *
 * Fichero que contiene las funciones necesarias para la impresion de    *
 * las lineas de texto, y para el control de la pantalla (scroles,       *
 * inicializar y restaurar la pantalla, etc). Tambien contiene todos     *
 * los ficheros necesarios, y las definiciones                           *
 * Enlaza solo con las funciones necesarias de las definidas en          *
 * CONTROL.CPP                                                           *
 *************************************************************************
 * Fecha creaci¢n : ??/5/96                                              *
 * Fecha £ltima modificaci¢n : 28/5/97                                   *
 * (c)1996,97 Luis Fernando Santocildes Romero                           *
 *************************************************************************/

// estructura usada para las funciones de ventana
typedef struct {text_info info_pantalla;  // contiene acerca de la ventana
                                          // anterior
                char *datos;              // Contiene los datos para borrar la
                                          // ventana actual
               } Tpantalla;

#ifndef _NO_IMPRESION
// Usada para ficheros temporales
typedef struct
  {char nombre[MAXPATH]; // nombre del fichero temporal
   FILE *fichero;        // puntero para acceder al fichero.
  } TFtemporal;
#endif

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
#ifndef _NO_PANTALLA
extern void pascal dibuja_escritorio (void);
                /* Imprime el fondo de la pantalla e inicializa los colores */
extern void pascal pone_barra (char *barra);
                                               /* Imprime la barra superior */
extern void pascal pone_barra_inferior (const char *barra, unsigned linea = 0);
                                               /* Imprime la barra inferior */
extern void pascal scrollabajo(boolean respeta = FALSE);
                                           /* Realiza un scroll hacia abajo */
extern void pascal scrollarriba(boolean respeta = FALSE);
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
extern void pascal color_ventana (unsigned color, unsigned color_fondo,
                                  unsigned color_borde);
                                 /* Cambia el color del marco de la ventana */
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
extern void pascal sonido_error (void);
                                                       /* Produce un pitido */
extern void pascal pone_caracter (char caracter, unsigned columna,
                                  unsigned fila, unsigned atributos);
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
extern int pascal escribe_cadena (char *palabra, int x, int y, int color = 0,
                                  boolean respeta_borde = TRUE);
#endif

#ifndef _NO_INI
extern void pascal inicializa_programa (char *nombre_clientes, char *nombre_articulos,
                                        char *nombre_facturas, char *nombre_fact,
                                        double *iva, unsigned long *ultima_factura,
                                        char *directorio_trabajo);
                                /* Inicializa la configuraci¢n del programa */
extern void pascal restaura_ordenador (char *nombre_clientes, char *nombre_articulos,
                                       char *nombre_facturas, char *nombre_fact,
                                       double iva_1,
                                       unsigned long ultima_factura,
                                       char *directorio_trabajo);
   /* Restaura el ordenador a un estado parecido al que ten¡a antes de usar el
      programa */
extern boolean pascal leer_configuracion (char *nombre_clientes, char *nombre_articulos,
                                   char *nombre_facturas, char *nombre_fact,
                                   double *iva, unsigned long *ultima_factura,
                                   char *directorio_trabajo);
                            /* Lee la configuraci¢n del fichero FACTURA.CFG */
extern void pascal salvar_configuracion (char *nombre_clientes, char *nombre_articulos,
                                  char *nombre_facturas, char *nombre_fact,
                                  double iva, unsigned long ultima_factura,
                                  char *directorio_trabajo);
                        /* Salva la configuraci¢n en el fichero FACTURA.CFG */
#endif

#ifndef _NO_RELOJ
extern void pascal instala_reloj (void);
                                         /* Instala el controlador de reloj */
extern void pascal desinstala_reloj (void);
                                           /* Quita el controlador de reloj */
#endif

#ifndef _NO_CADENA
extern unsigned pascal lee_cadena (char *cadena, unsigned max_car,
                            boolean alfanumerica = TRUE, boolean nueva = TRUE,
                            boolean sale = FALSE, char caracter_relleno = ' ');
                                         /* Lee una cadena desde el teclado */
extern void pascal completa_cadena (char *cadena, unsigned longitud,
                                    char caracter_relleno = ' ',
                                    boolean pone_crlf = FALSE);
                         /* Completa una cadena con un caracter determinado */
extern char * pascal gotoxy_text (char *destino, char *origen, unsigned x);
                                         /* Ubica una cadena dentro de otra */
extern char * pascal busca_cadena (char *cadena2, char *cadena1,
                                   boolean mayusculas = FALSE);
                                         /* Busca una cadena dentro de otra */

extern void pascal pide_fecha (char *texto, byte x, byte y, byte ancho,
                               byte alto, date *fecha, boolean nueva = TRUE);
                              /* Pide una fecha y verifica que sea correcta */
#endif

#ifndef _NO_IMPRESION
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
extern void pascal saca_papel (void);
                                           /* Saca el papel de la impresora */
extern void pascal imprime_fecha (TFtemporal *fichero);
                          /* Imprime la fecha en el fichero temporal pasado */
#endif

extern boolean pascal existe_fichero (char *nom_fich);
                               /* Verifica si existe un determinado fichero */

#ifndef _NO_LISTAS
extern void pascal imprime_lista (Cbase_datos &Objeto, char tipo_lista,
                                  unsigned fichas_ini, unsigned fichas_fin);
                                                 /* Imprime la lista pasada */
extern unsigned pascal muestra_lista (Cbase_datos &Objeto, char tipo_lista,
                                      unsigned fichas_ini, unsigned fichas_fin,
                                      unsigned nro_lineas,
                                      boolean escoge = FALSE);
                                                 /* muestra la lista pasada */
#endif

#ifndef _NO_MODO_132
extern boolean pascal pone_modo_132 (void);
                                 /* Activa un modo de v¡deo de 132 columnas */
extern void pascal pone_modo_normal (void);
                                        /* Restaura el modo normal de v¡deo */
#endif

extern unsigned long pascal dos2luc (struct date *fecha);
                   /* Pasa la fecha de formato dia/mes/a¤o a un nro. entero */
extern void pascal luc2dos (unsigned long fecha, struct date *fecha_des);
                         /* Pasa de un nro. entero a una fecha determinada. */
/*******************************VARIABLES GLOBALES**************************/
extern boolean reloj_visible;
/* Controla si el reloj es visible o no. Los valores posibles son TRUE o
   FALSE. Solo funciona si el reloj est  instalado */
extern boolean hace_sonido;
/* Indica si la funci¢n sonido error debe hacer sonidos. Booleana.
   Inicialmente vale TRUE */

extern int nro_copias,       // Nro de copias a hacer en cada impresi¢n
           lineas_papel,     // L¡neas a imprimir por fol¡os
           puerto_conexion;  // ¡ndice del puerto de conexi¢n de la impresora

extern char puertos[][5];    // lista de los puertos de conexi¢n de impresora

#ifndef _NO_FICH
extern char directorio_programa[MAXPATH];
// Unidad y directorio donde est  el programa;
#endif

extern boolean configuracion_modificada;
// Usada para determinar si se debe o no almacenar la configuraci¢n actual al
// salir del programa

// Mensajes varios
extern char pres_enter[];
extern char pres_tecla[];
extern char esc_tab[];
extern char esc_opcion[];
extern char esc_sino[];
extern char imp_fact[];
extern char imp_lista[];
extern char pres_esc[];
extern char nuevo_cli[];
extern char buscar_cli[];
extern char modif_cli[];
extern char nuevo_art[];
extern char busca_art[];
extern char edit_art[];
extern char codigo_cliente[];
extern char _fecha[];
extern char menu_prin[];
extern char fact_text[];
/* Desplazamiento del origen de coordenadas de la ventana con respecto a la
   esquina superior izquierda de la misma. Se usa en la funci¢n que imprime
   un caracter en pantalla y en las que dibujan l¡neas horizontales y
   verticales. */
extern unsigned desplazamiento_horizontal,
                desplazamiento_vertical;

extern const char *cabecera_facturas; // Texto de cabecera de las facturas
extern const char *c_mes[12];         // Lista de los meses

extern char fondo[];
extern char barra2[];

extern text_info info_pantalla,       // Informaci¢n sobre la pantalla cuando
                 actual;              // se arranca el programa

extern Tcolor colores;

extern unsigned char marco[3][11];
