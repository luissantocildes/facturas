/****************************************************************************
 * MENUS.H                                                                  *
 * Fichero de cabecera que contiene las estructuras y los objetos necesarios*
 * para crear un men£ de persiana en la l¡nea superior de la pantalla.      *
 ****************************************************************************
 * Fecha realizaci¢n : 3/11/96                                              *
 * Fecha terminaci¢n : 25/4/97                                              *
 * (C)1997 Luis Fernando Santocildes                                        *
 ****************************************************************************/

/************************** TIPOS DE DATOS ********************************/
typedef struct _topcion
  {char *texto;
   unsigned id,
            tipo,
            pos,
            submenu,
            sig_opcion,
            ant_opcion;
  } Topcion;

/*************************** PROTOTIPOS ************************************/
extern boolean pascal inicializa_menu (void);
extern void pascal libera_menu (void);
extern boolean pascal agrega_menu (unsigned menu, unsigned codigo, char *cadena);
extern boolean pascal agrega_submenu (unsigned menu, unsigned submenu, char *cadena);
extern boolean pascal agrega_separador (unsigned menu);
extern boolean pascal marca_menu (unsigned menu, unsigned opcion);
extern unsigned  pascal crea_menu (void);
extern unsigned  pascal crea_menu_popup (void);
extern void pascal borra_menu (unsigned menu);
extern boolean pascal borra_menu (unsigned menu, unsigned opcion);
extern void pascal dibuja_menu (boolean letras = TRUE);
extern void pascal oculta_menu (void);
extern int pascal abre_menu_popup (unsigned menu, unsigned x, unsigned y,
                                   unsigned *opcion);
extern int pascal escoge_opcion (unsigned *opcion);
extern boolean pascal habilita_opcion (unsigned menu, unsigned opcion);
extern unsigned  pascal toma_menu_actual (void);
extern unsigned short pascal num_opciones (unsigned menu);
extern unsigned pascal id_opcion (unsigned menu, unsigned posicion);
extern unsigned pascal estado_opcion (unsigned menu, unsigned opcion);
extern unsigned pascal atributo_opcion (unsigned menu, unsigned opcion);
extern boolean pascal cambia_atributo_opcion (unsigned menu, unsigned opcion,
                                              unsigned atributo);
extern int pascal texto_opcion (unsigned menu, unsigned opcion, char *buffer,
                         unsigned largo);
extern unsigned  pascal busca_submenu (unsigned menu, unsigned posicion);
extern unsigned  pascal menu_sistema (void);
extern boolean pascal inserta_opcion (unsigned menu, unsigned id_anterior,
                               unsigned opcion, char *cadena);
extern boolean pascal inserta_submenu (unsigned menu, unsigned id_anterior,
                                unsigned submenu, char *cadena);
extern boolean pascal modifica_menu (unsigned menu, unsigned opcion,
                              unsigned id_nuevo, unsigned tipo, char *cadena);
extern boolean pascal remueve_opcion (unsigned menu, unsigned opcion);
extern boolean pascal pone_menu_actual (unsigned menu);
