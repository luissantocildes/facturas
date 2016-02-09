/*************************************************************************
 * Fichero : MENUS.CPP                                                   *
 * Fichero que contiene las funciones necesarias para el dise¤o y uso de *
 * men£s de opciones                                                     *
 *************************************************************************
 * Fecha creaci¢n : 3/11/1997                                            *
 * Fecha £ltima modificaci¢n : 28/5/97                                   *
 * (c)1996,97 Luis Fernando Santocildes Romero                           *
 *************************************************************************
 * V1.0 : 2/1/97                                                         *
 *        - Se dise¤a el sistema de men£s, pudi‚ndose usar men£s de      *
 *          persiana. Se quita el sistema de men£s de CONTROL.CPP,       *
 *          dej ndolo en un bloque aparte                                *
 *************************************************************************/
#define _NO_LISTAS

#include <stdio.h>
#include <conio.h>
#include <dir.h>
#include <dos.h>
#include <string.h>
#include <alloc.h>
#include "defines.h"
#include "boolean.h"
#include "tipos.h"
#include "control.h"

/***************************** Macros ***********************************/
#define MAX_OPCION 1024

#define ES_SUBMENU(menu) (menu_actual[menu].tipo & _MPOPUP)

/************************* Definici¢n de Tipos de Datos ******************/
typedef struct _topcion
  {char *texto;
   unsigned id,
            tipo,
            pos,
            submenu,
            sig_opcion,
            ant_opcion;
  } Topcion;

/************************** Variables Globales **************************/
Topcion *menu_actual = NULL;
unsigned menu_actual_ini = ERROR;
unsigned menu = 0;

Tpantalla barra_menu;

// Letras de acceso r pido al men£
char *_letras;
int num_letras = 0;

// Bandera de visibilidad del men£
boolean visible = FALSE;

/********************************* Prototipos ***************************/
boolean pascal inicializa_menu (void);
void pascal libera_menu (void);
unsigned nueva_opcion (void);
unsigned borra_opcion (unsigned opcion);
boolean pascal agrega_menu (unsigned menu, unsigned codigo, char *cadena);
boolean pascal agrega_submenu (unsigned menu, unsigned submenu, char *cadena);
boolean pascal agrega_separador (unsigned menu);
boolean pascal marca_menu (unsigned menu, unsigned opcion);
unsigned pascal crea_menu (void);
unsigned pascal crea_menu_popup (void);
void pascal borra_menu (unsigned menu);
boolean pascal borra_menu (unsigned menu, unsigned opcion);
void pascal dibuja_menu (boolean teclas = TRUE);
void pascal oculta_menu (void);
int pascal abre_menu_popup (unsigned menu, unsigned x, unsigned y,
                            unsigned *opcion);
int pascal escoge_opcion (unsigned *opcion);
boolean pascal habilita_opcion (unsigned menu, unsigned opcion);
Topcion * pascal toma_menu_actual (void);
unsigned pascal short num_opciones (unsigned menu);
unsigned pascal id_opcion (unsigned menu, unsigned posicion);
unsigned pascal estado_opcion (unsigned menu, unsigned opcion);
unsigned pascal atributo_opcion (unsigned menu, unsigned opcion);
boolean pascal cambia_atributo_opcion (unsigned menu, unsigned opcion,
                                       unsigned atributo);
int pascal texto_opcion (unsigned menu, unsigned opcion, char *buffer,
                         unsigned largo);
unsigned  pascal busca_submenu (unsigned menu, unsigned posicion);
unsigned  pascal menu_sistema (void);
boolean pascal inserta_opcion (unsigned menu, unsigned id_anterior,
                               unsigned opcion, char *cadena);
boolean pascal inserta_submenu (unsigned menu, unsigned id_anterior,
                                unsigned submenu, char *cadena);
boolean pascal modifica_menu (unsigned menu, unsigned opcion,
                              unsigned id_nuevo, unsigned tipo, char *cadena);
boolean pascal pone_menu_actual (unsigned menu);
unsigned pascal strlen_ (const char far *cadena);
/***************************************************************************/
boolean pascal inicializa_menu (void)
// Inicializa el sistema de men£s
  {
   if (!menu_actual)
     {if ((menu_actual = (Topcion *) farmalloc (_16K)) != NULL)
        {memset (menu_actual, 0, sizeof (Topcion) * MAX_OPCION);
         menu_actual->sig_opcion = FIN_MENU;
         menu_actual->ant_opcion = FIN_MENU;
         menu_actual->submenu = FIN_MENU;
         return TRUE;
        }
      else return FALSE;
     }
   else return FALSE;
  } // inicializa_menu

void pascal libera_menu (void)
  {unsigned f;

   if (menu_actual)
     {for (f = 0; f < MAX_OPCION; f++)
        delete (menu_actual[f].texto);
      farfree (menu_actual);
     }
  }

unsigned nueva_opcion (void)
  {unsigned f = 0;
   Topcion *aux;

   aux = menu_actual;
   while (f < MAX_OPCION && aux->tipo)
     {f++;
      aux++;
     }
   if (f == MAX_OPCION)
     return ERROR;
   else
     return f;
  } // nueva_opcion

unsigned borra_opcion (unsigned opcion)
  {unsigned f = 1;
   Topcion *aux;

   aux = menu_actual;
   while (f < MAX_OPCION && aux->id != opcion)
     {f++;
      aux++;
     }
   if (f == MAX_OPCION)
     return ERROR;
   else
     {aux->id = 0;
      aux->tipo = 0;
      return OK;
     }
  } // borra_opcion

boolean pascal agrega_menu (unsigned menu, unsigned codigo, char *cadena)
// Inserta una opci¢n al final de un men£
  {unsigned aux, temp;

   aux = menu_actual[menu].submenu;
   if (ES_SUBMENU(menu))
     // Se verifica que el punto de inserci¢n sea un men£ popup
     {temp = nueva_opcion();
      if (temp)
        {if ((menu_actual[temp].texto = new char[strlen(cadena)+1]) != NULL)
         // Se reserva espacio para la opci¢n y para el texto
          {strcpy (menu_actual[temp].texto, cadena);
           menu_actual[temp].id = codigo;
           menu_actual[temp].submenu = FIN_MENU;
           menu_actual[temp].sig_opcion = FIN_MENU;
           menu_actual[temp].tipo = _ACTIVADO;
           if (codigo == 0)
             menu_actual[temp].tipo |= _SEPARADOR;
           // Se inicializa la opci¢n
           if (aux == FIN_MENU)
             {menu_actual[menu].submenu = temp;
              menu_actual[temp].ant_opcion = FIN_MENU;
             }
           else
             {while (menu_actual[aux].sig_opcion != FIN_MENU)
                aux = menu_actual[aux].sig_opcion;
              menu_actual[aux].sig_opcion = temp;
              menu_actual[temp].ant_opcion = aux;
             }
	   // Se ubica la £ltima posici¢n del men£
           return TRUE;
          }
         else
           borra_opcion (temp);
        }
     }
   return FALSE;
  } // agrega_menu

boolean pascal agrega_submenu (unsigned menu, unsigned submenu, char *cadena)
// Inserta un submenu al final de un men£
  {unsigned aux;

   aux = menu_actual[menu].submenu;
   if (ES_SUBMENU(menu))
     // Se verifica que el men£ donde se inserta es un men£ popup
     {if ((menu_actual[submenu].texto = new char[strlen(cadena)+1]) != NULL)
       {strcpy (menu_actual[submenu].texto, cadena);
        if (aux == FIN_MENU)
          menu_actual[menu].submenu = submenu;
        else
          {while (menu_actual[aux].sig_opcion != FIN_MENU)
             aux = menu_actual[aux].sig_opcion;
           menu_actual[aux].sig_opcion = submenu;
           menu_actual[submenu].ant_opcion = aux;
          }
        // Se busca la £ltima opci¢n del men£
        menu_actual[submenu].sig_opcion = FIN_MENU;
        // Se inserta el submen£
        return TRUE;
       }
     }
   return FALSE;
  } // agrega_submenu

boolean pascal agrega_separador (unsigned menu)
// Agrega una linea de separaci¢n entre dos opciones
  {unsigned aux, temp;

   aux = menu_actual[menu].submenu;
   if (ES_SUBMENU(menu))
     // Se verifica que el punto de inserci¢n sea un men£ popup
     {temp = nueva_opcion();
      if (temp)
        {if ((menu_actual[temp].texto = new char[2]) != NULL)
         // Se reserva espacio para la barra
          {strcpy (menu_actual[temp].texto, "-");
           menu_actual[temp].id = 0;
           menu_actual[temp].submenu = FIN_MENU;
           menu_actual[temp].sig_opcion = FIN_MENU;
           menu_actual[temp].tipo = _ACTIVADO | _SEPARADOR;
           // Se inicializa la opci¢n
           if (aux == FIN_MENU)
             {menu_actual[menu].submenu = temp;
              menu_actual[temp].ant_opcion = FIN_MENU;
             }
           else
             {while (menu_actual[aux].sig_opcion != FIN_MENU)
                aux = menu_actual[aux].sig_opcion;
              menu_actual[aux].sig_opcion = temp;
              menu_actual[temp].ant_opcion = aux;
             }
	   // Se ubica la £ltima posici¢n del men£
           return TRUE;
          }
         else
           borra_opcion (temp);
        }
     }
   return FALSE;
  } // agrega_separador


boolean pascal marca_menu (unsigned menu, unsigned opcion)
// Pone una marca a una opci¢n de un men£
  {unsigned aux;

   aux = menu_actual[menu].submenu;
   if (ES_SUBMENU(menu) && aux != FIN_MENU)
     {while (menu_actual[aux].id != opcion &&
             menu_actual[aux].sig_opcion != FIN_MENU)
        aux = menu_actual[aux].sig_opcion;
      if (menu_actual[aux].tipo & _MPOPUP)
        return TAL_CUAL;
      else
        if (menu_actual[aux].tipo & _MARCADO)
          {menu_actual[aux].tipo &= ~_MARCADO;
           return TRUE;
          }
        else
          {menu_actual[aux].tipo |= _MARCADO;
           return FALSE;
          }
     }
   return TAL_CUAL;
  } // marca_menu

unsigned pascal crea_menu (void)
// Crea un item de men£
  {unsigned aux;

   aux = nueva_opcion();
   if (aux != ERROR)
     {menu_actual[aux].texto = NULL;
      menu_actual[aux].id = 0;
      menu_actual[aux].sig_opcion = FIN_MENU;
      menu_actual[aux].ant_opcion = FIN_MENU;
      menu_actual[aux].submenu = FIN_MENU;
      menu_actual[aux].tipo = _PRINCIPAL | _ACTIVADO | _MPOPUP;
     }
   return aux;
  } // crea_menu

unsigned  pascal crea_menu_popup (void)
// Crea un submenu
  {unsigned aux;

   aux = nueva_opcion();
   if (aux != ERROR)
     {menu_actual[aux].texto = NULL;
      menu_actual[aux].id = 0;
      menu_actual[aux].sig_opcion = FIN_MENU;
      menu_actual[aux].ant_opcion = FIN_MENU;
      menu_actual[aux].submenu = FIN_MENU;
      menu_actual[aux].tipo = _ACTIVADO | _MPOPUP;
     }
   return aux;
  } // crea_menu_popup

void pascal borra_menu (unsigned menu)
// Borra un menu y su contenido
  {unsigned temp, aux;

   temp = menu;
   aux = menu_actual[menu].submenu;
   while (menu != FIN_MENU)
     {if (ES_SUBMENU(menu) && aux != FIN_MENU)
       borra_menu (menu_actual[menu].submenu);
      aux = menu_actual[menu].sig_opcion;
      borra_opcion (menu);
      menu = aux;
     }
   if (temp == 0)
     {libera_menu ();
      menu_actual = NULL;
     }
  } // borra_menu


boolean pascal borra_menu (unsigned menu, unsigned opcion)
// borra una opci¢n de un men£
  {unsigned aux;

   aux = menu_actual[menu].submenu;
   if (ES_SUBMENU(menu) && aux != FIN_MENU)
     {if (menu == 0)
       {if (visible)
          {restaura_recuadro (&barra_menu);
           visible = FALSE;
          }
       }
      while (menu_actual[aux].id != opcion &&
             menu_actual[aux].sig_opcion != FIN_MENU)
      // Busca la opci¢n a buscar
        aux = menu_actual[aux].sig_opcion;
      if (menu_actual[aux].id == opcion)
      // Si la encuentra
        {if (menu_actual[menu].submenu == aux)
         // si se va a borrar la primer opci¢n de un men£ se enlaza el mismo
         // con la 2¦ opci¢n
           menu_actual[menu].submenu = menu_actual[aux].sig_opcion;

        if (menu_actual[aux].sig_opcion != FIN_MENU)
           menu_actual[menu_actual[aux].sig_opcion].ant_opcion =
             menu_actual[aux].ant_opcion;
         if (menu_actual[aux].ant_opcion != FIN_MENU)
           menu_actual[menu_actual[aux].ant_opcion].sig_opcion =
             menu_actual[aux].sig_opcion;
         // Enlaza las opciones anteriores y posteriores

         if (ES_SUBMENU (aux))
         // Si la propia opci¢n a borrar es un submenu se borra todo su contenido
           {unsigned aux1 = menu_actual[aux].submenu,
		     aux2;

            while (aux1 != FIN_MENU)
              {aux2 = menu_actual[aux1].sig_opcion;
               borra_menu (aux, menu_actual[aux1].id);
               aux1 = aux2;
              }
           }
         borra_opcion (aux);
           // Borra el texto y la opci¢n
         return TRUE;
        }
      return FALSE;
     }
   return FALSE;
  } // borra_menu

void pascal dibuja_menu (boolean teclas)
// Redibuja el men£ en la pantalla
  {unsigned aux;
   unsigned x = 1;
   char *cadena;

   if (menu_actual && menu_actual->tipo & _PRINCIPAL)
     {visible = TRUE;
      restaura_recuadro (&barra_menu);
      barra_menu = guarda_recuadro (1, 1, 79, 0, SIN_MARCO);
      aux = menu_actual[menu_actual_ini].submenu;
      num_letras = 0;
      gotoxy (1, 1);
      textattr (colores.texto_barra);
      for (; x < 80; x++)
        putch (' ');
      x = 3;
      gotoxy (x, 1);
      while (aux != FIN_MENU)
        {cadena = menu_actual[aux].texto;
         if (menu_actual[aux].tipo & _GRIS)
           {textattr (colores.desactivado_menu);
            cputs (cadena);
            textattr (colores.texto_barra);
           }
         else
           escribe_cadena (menu_actual[aux].texto, x, 1, FALSE);
         x += strlen(menu_actual[aux].texto) + 1;
         aux = menu_actual[aux].sig_opcion;
        }
     }
  } // dibuja_menu

void pascal oculta_menu (void)
// Quita el men£ de pantalla
  {
   if (visible)
     {visible = FALSE;
      restaura_recuadro (&barra_menu);
     }
  } // oculta_menu

int pascal abre_menu_popup (unsigned menu, unsigned x, unsigned y,
                            unsigned *opcion)
// Abre un men£ popup
  {Tpantalla aux_p;
   int largo_menu, ancho_menu = 0, f;
   unsigned aux_o;
   char tecla, cadena[80];
   int scroll = _wscroll;
   boolean continua = TRUE;

   if (!ES_SUBMENU (menu))
     return _SIN_SUBMENU;

   if ((largo_menu = num_opciones (menu)) == -1)
     return _SIN_OPCIONES;

   _wscroll = 0;
   memset (cadena, 0, 30);
   aux_o = menu_actual[menu].submenu;
   while (aux_o != FIN_MENU)
     {f = strlen_ (menu_actual[aux_o].texto);
      if (ES_SUBMENU(aux_o))
        f += 3;
      if (f > ancho_menu)
        ancho_menu = f;
      aux_o = menu_actual[aux_o].sig_opcion;
     }
   ancho_menu += 3;
   if (x + ancho_menu > COLUMNAS_PANTALLA)
     x = COLUMNAS_PANTALLA - ancho_menu;
   color_ventana (colores.texto_menu, colores.texto_menu, colores.borde_menu);
   aux_p = hace_recuadro (x, y, ancho_menu, largo_menu + 1, NULL, M_FINO);
   aux_o = menu_actual[menu].submenu;
   for (f = 1; f <= largo_menu; f++, aux_o = menu_actual[aux_o].sig_opcion)
     if (ES_SUBMENU (aux_o))
        {memset (cadena, 0x20, ancho_menu - 1);
         memcpy (cadena, menu_actual[aux_o].texto,
                 strlen (menu_actual[aux_o].texto));
         cadena [ancho_menu - 3] = 0x10;
         escribe_cadena (cadena, 2, f, 0);
        }
     else
       if (menu_actual[aux_o].tipo & _SEPARADOR)
         {memset (cadena, marco[2][1], ancho_menu - 1);
          escribe_cadena (cadena, 2, f, 0);
         }
       else
         escribe_cadena (menu_actual[aux_o].texto, 2, f, 0);

   aux_o = menu_actual[menu].submenu;
   f = 1;
   do
     {memset (cadena, 0x20, ancho_menu);
      memcpy (cadena+1, menu_actual[aux_o].texto,
              strlen (menu_actual[aux_o].texto));
      if (ES_SUBMENU(aux_o))
        cadena [ancho_menu - 2] = 0x10;
      escribe_cadena (cadena, 1, f, 1);
      tecla = getch();
      switch (tecla)
        {case ENTER :
           {int aux;

            if (ES_SUBMENU (aux_o))
              {aux = abre_menu_popup (aux_o, x + ancho_menu + 1, y + f, opcion);
               if (aux == _ESCOGE)
                 continua = FALSE;
              }
            else
              {*opcion = menu_actual[aux_o].id;
               continua = FALSE;
              }
            break;
           }

         case ESC : continua = FALSE;
                    break;

         case T_control :
           tecla = getch();
           switch (tecla)
             {case CURS_ABJ :
                escribe_cadena (cadena, 1, f, 0);
                if (f < largo_menu)
                  {do
                     {f++;
                      aux_o = menu_actual[aux_o].sig_opcion;
                     }
                   while (menu_actual[aux_o].tipo & _SEPARADOR);
                  }
		else {f = 1;
                      aux_o = menu_actual[menu].submenu;
                     }
                break;

              case CURS_ARR :
                escribe_cadena (cadena, 1, f, 0);
                if (f > 1)
		  {do
                     {f--;
                      aux_o = menu_actual[aux_o].ant_opcion;
                     }
                   while (menu_actual[aux_o].tipo & _SEPARADOR);
                  }
                else
                  for (f = 1, aux_o = menu_actual[menu].submenu; f < largo_menu;
                       f++, aux_o = menu_actual[aux_o].sig_opcion);
                break;

              case CURS_DER :
              case CURS_IZQ : continua = FALSE;
                              break;
             }
           break;
        }
     }
   while (continua);
   restaura_recuadro (&aux_p);
   _wscroll = scroll;
   switch (tecla)
     {case ESC : return _CANCELA;
      case ENTER : return _ESCOGE;
      case CURS_DER : return _DERECHA;
      case CURS_IZQ : return _IZQUIERDA;
      default : return 0;
     }
  } // abre_menu_popup

int pascal escoge_opcion (unsigned *opcion)
// Escoge una opci¢n del men£ actual
  {static unsigned x;
   char cadena[80], tecla;
   boolean visible_v = TRUE,
           menu_abierto = FALSE;
   int accion;

   if (!menu_actual || menu_actual_ini == ERROR)
     return _NO_HAY_MENU;
   if (!visible)
     {visible_v = visible;
      dibuja_menu();
     }
   if (!menu)
     {menu = menu_actual[menu_actual_ini].submenu;
      x = 2;
     }
   do
     {memset (cadena, 0, 80);
      memset (cadena, 0x20, strlen (menu_actual[menu].texto) + 2);
      memcpy (cadena + 1, menu_actual[menu].texto,
              strlen (menu_actual[menu].texto));
      escribe_cadena (cadena, x, 1, 3, FALSE);
      if (menu_abierto)
        accion = abre_menu_popup (menu, x, 2, opcion);
      else
        {tecla = getch();
         if (tecla == T_control)
           tecla = getch();
         switch (tecla)
           {case CURS_DER :
              accion = _DERECHA;
              break;

            case CURS_IZQ :
              accion = _IZQUIERDA;
              break;

            case ENTER :
              accion = _ESCOGE;
              break;

            case ESC :
              accion = _CANCELA;
              break;
           }
        }

      switch (accion)
        {case _DERECHA :
           escribe_cadena (cadena, x, 1, 0, FALSE);
	   x += strlen_ (menu_actual[menu].texto) + 2;
	   menu = menu_actual[menu].sig_opcion;
	   if (menu == FIN_MENU)
	     {menu = menu_actual->submenu;
	      x = 2;
	     }
	   break;

	 case _IZQUIERDA :
	   escribe_cadena (cadena, x, 1, 0, FALSE);
           menu = menu_actual[menu].ant_opcion;
           if (menu == FIN_MENU)
             {menu = menu_actual->submenu;
              x = 2;
              while (menu_actual[menu].sig_opcion != FIN_MENU)
                {x += strlen_ (menu_actual[menu].texto) + 2;
                 menu = menu_actual[menu].sig_opcion;
                }
             }
           else
             x -= strlen_ (menu_actual[menu].texto) + 2;
           break;

         case _ESCOGE :
           if (!menu_abierto)
             {menu_abierto = TRUE;
              accion = CAMBIO;
              break;
             }
         case _CANCELA : escribe_cadena (cadena, x, 1, 0, FALSE);
                         if (!visible_v)
                           {oculta_menu();
                            visible = visible_v;
                           }
                         break;
        }
     }
   while (accion != _ESCOGE && accion != _CANCELA);
   switch (accion)
     {case _ESCOGE : return _ESCOGE;
      case _CANCELA : return _CANCELA;
      default : return 0;
     }
  } // Escoge_opcion

boolean pascal habilita_opcion (unsigned menu, unsigned opcion)
// Habilita una opci¢n de un men£ para poder usarse
  {unsigned aux;

   aux = menu_actual[menu].submenu;
   if (ES_SUBMENU (menu) && aux && menu)
     {do
        aux = menu_actual[aux].sig_opcion;
      while (menu_actual[aux].id != opcion && aux != FIN_MENU);
      if (menu_actual[aux].id == opcion)
        {if (menu_actual[aux].tipo & _GRIS)
           {menu_actual[aux].tipo &= ~_GRIS;
            return TRUE;
           }
         else
           {menu_actual[aux].tipo |= _GRIS;
            return FALSE;
           }
        }
     }
   return TAL_CUAL;
  } // habilita_opcion

Topcion * pascal toma_menu_actual (void)
// Devuelve el puntero al men£ actual
  {
   return menu_actual;
  } // toma_menu_actual;

unsigned pascal short num_opciones (unsigned menu)
// Devuelve el n§ de items del men£
  {unsigned aux;
   int contador = 0;

   aux = menu_actual[menu].submenu;
   if (ES_SUBMENU (menu) && menu != FIN_MENU)
     {while (aux != FIN_MENU)
        {contador++;
         aux = menu_actual[aux].sig_opcion;
        }
      return contador;
     }
   return -1;
  }

unsigned pascal id_opcion (unsigned menu, unsigned posicion)
// Devuelve el c¢digo de una opci¢n
  {unsigned aux;
   unsigned contador = 0;

   aux = menu_actual[menu].submenu;
   if (ES_SUBMENU (menu) && menu != FIN_MENU)
     {while (contador != posicion && aux != FIN_MENU)
        {contador++;
         aux = menu_actual[aux].sig_opcion;
        }
      if (contador == posicion)
        return menu_actual[aux].id;
      else return 0;
     }
   else return -1;
  } // id_opcion

unsigned pascal estado_opcion (unsigned menu, unsigned opcion)
// Devuelve el estado de una opci¢n, Si es un submen£ indica cuantos items
// tiene el submen£
  {unsigned aux;

   aux = menu_actual[menu].submenu;
   if (ES_SUBMENU (menu) && menu != FIN_MENU)
     {while (aux != FIN_MENU && menu_actual[aux].id != opcion)
        aux = menu_actual[aux].sig_opcion;
      if (aux == FIN_MENU)
        return -1;
      else
        {if (ES_SUBMENU (aux))
           return (num_opciones (aux) << 8) | (menu_actual[aux].tipo);
         else return menu_actual[aux].tipo;
        }
     }
   return -1;
  } // estado_opcion

unsigned pascal atributo_opcion (unsigned menu, unsigned opcion)
  {unsigned aux;

   aux = menu_actual[menu].submenu;
   if (ES_SUBMENU (menu) && menu)
     {while (aux != FIN_MENU && menu_actual[aux].id != opcion)
	aux = menu_actual[aux].sig_opcion;
      if (aux != FIN_MENU)
        return menu_actual[aux].tipo;
     }
   return 0;
  } // atributo_opcion;

boolean pascal cambia_atributo_opcion (unsigned menu, unsigned opcion,
                                       unsigned atributo, boolean pone)
  {unsigned aux;

   aux = menu_actual[menu].submenu;
   if (ES_SUBMENU (menu) && menu)
     {while (aux != FIN_MENU && menu_actual[aux].id != opcion)
        aux = menu_actual[aux].sig_opcion;
      if (aux != FIN_MENU)
        {if (pone)
           menu_actual[aux].tipo |= atributo;
         else
           menu_actual[aux].tipo &= ~atributo;
         return TRUE;
        }
     }
   return FALSE;
  } // cambia_atributo_opcion

int pascal texto_opcion (unsigned menu, unsigned opcion, char *buffer,
                  unsigned largo)
// Copia el texto de una opci¢n en un buffer
  {unsigned aux;

   aux = menu_actual[menu].submenu;
   if (ES_SUBMENU (menu) && menu != FIN_MENU)
     {while (aux != FIN_MENU && menu_actual[aux].id != opcion)
        aux = menu_actual[aux].sig_opcion;
      if (aux != FIN_MENU)
        {memset (buffer, 0, largo + 1);
         strncpy (buffer, menu_actual[aux].texto, largo);
        }
      return strlen(buffer);
     }
   return -1;
  } // texto_opcion

unsigned  pascal busca_submenu (unsigned menu, unsigned posicion)
// Verifica que una opci¢n determinada sea un submen£
  {unsigned aux;
   unsigned contador = 1;

   aux = menu_actual[menu].submenu;
   if (ES_SUBMENU(menu) && menu != FIN_MENU)
     {while (aux != FIN_MENU && contador != posicion)
        {aux = menu_actual[aux].sig_opcion;
         contador++;
        }
      if (aux != FIN_MENU)
        if (menu_actual[aux].tipo & _MPOPUP)
          return aux;
     }
   return NULL;
  } // busca_submenu;

/*unsigned menu_sistema (void);*/
boolean pascal inserta_opcion (unsigned menu, unsigned id_anterior, unsigned opcion,
                        char *cadena)
// Inserta una opci¢n en una posici¢n determinada
  {unsigned aux, temp;

   aux = menu_actual[menu].submenu;
   if (ES_SUBMENU (menu) && menu != FIN_MENU)
     {while (aux != FIN_MENU && menu_actual[aux].id != id_anterior)
        aux = menu_actual[aux].sig_opcion;
      temp = nueva_opcion();
      if (temp != ERROR)
        menu_actual[temp].texto = new char[strlen(cadena)];
      if (menu_actual[temp].texto)
        {strcpy (menu_actual[temp].texto, cadena);
         menu_actual[temp].sig_opcion = menu_actual[aux].sig_opcion;
         menu_actual[aux].sig_opcion = temp;
         menu_actual[temp].ant_opcion = aux;
         menu_actual[temp].submenu = FIN_MENU;
         menu_actual[temp].tipo = opcion | _ACTIVADO;
         return TRUE;
        }
      else borra_opcion (temp);
      return FALSE;
     }
   return TAL_CUAL;
  } // inserta_opcion

boolean pascal inserta_submenu (unsigned menu, unsigned id_anterior,
                                unsigned submenu, char *cadena)
// Inserta un submenu en una posici¢n de un men£
  {unsigned aux;

   aux = menu_actual[menu].submenu;
   if (ES_SUBMENU (menu) && menu != FIN_MENU)
     {while (aux != FIN_MENU && menu_actual[aux].id != id_anterior)
        aux = menu_actual[aux].sig_opcion;
      if (aux != FIN_MENU)
        {menu_actual[submenu].sig_opcion = menu_actual[aux].sig_opcion;
         menu_actual[aux].sig_opcion = submenu;
         menu_actual[submenu].ant_opcion = aux;
         if ((menu_actual[submenu].texto = new char[strlen(cadena)]) != NULL)
           strcpy (menu_actual[submenu].texto, cadena);
         return TRUE;
        }
     }
   return FALSE;
  } // inserta_submenu

boolean pascal modifica_menu (unsigned menu, unsigned opcion, unsigned id_nuevo,
                       unsigned tipo, char *cadena)
// Modifica el contenido de una opci¢n
  {unsigned aux;

   if (tipo & (_MPOPUP | _PRINCIPAL))
     return FALSE;
   aux = menu_actual[menu].submenu;
   if (ES_SUBMENU (menu) && menu != FIN_MENU)
     {while (aux != FIN_MENU && menu_actual[aux].id != opcion)
        aux = menu_actual[aux].sig_opcion;
      if (aux != FIN_MENU)
        {menu_actual[aux].id = id_nuevo;
         menu_actual[aux].tipo = tipo | _ACTIVADO;
         delete menu_actual[aux].texto;
         if ((menu_actual[aux].texto = new char[strlen(cadena)]) != NULL)
           strcpy (menu_actual[aux].texto, cadena);
         return TRUE;
        }
     }
   return FALSE;
  } // modifica_menu;

boolean pascal pone_menu_actual (unsigned menu)
// Men£ que se mostrar  por pantalla
  {if (menu != FIN_MENU && (menu_actual[menu].tipo & _PRINCIPAL))
     {menu_actual_ini = menu;
      return TRUE;
     }
   return FALSE;
  } // pone_menu_actual

unsigned pascal strlen_ (const char far *cadena)
/* Cuenta la longitud de una cadena sin tener en cuenta los caracteres & */
  {
   unsigned contador = 0;

   asm {
        push    ds
        push    si
        push    ax
        lds     si, cadena
       }
   bucle:
   asm {lodsb
        or      al, al
        jz      sale
        cmp     al, '&'
        jz      bucle
        inc     contador
        jmp     bucle
       }
   sale:
   asm {pop     ax
        pop     si
        pop     ds
       }
   return contador;
  } // strlen_
