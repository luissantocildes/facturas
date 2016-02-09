/***************************************************************************
 * Programa de facturaciขn                                                 *
 ***************************************************************************
 * Fecha creaciขn : 5/4/96                                                 *
 * Fecha modificaciขn : 28/5/97                                            *
 * (c)1996-97 Luis Fernando Santocildes Romero                             *
 ***************************************************************************
 * V1.2 : Programa de facturaciขn                                          *
 ***************************************************************************/

/*********** Protecciขn contra una compilaciขn incorrecta ******************/
#if defined (_Windows)
  #error Este programa es para MS-DOS.
#endif
// Si se intenta compilar como un programa para Windows, da error

#ifndef __MSDOS__
  #error Este programa es para MS-DOS.
#endif
// Si se intenta compilar bajo un OS que no sea el MS-DOS, da error

#ifndef __LARGE__

#include <stdio.h>

void main()
{
  printf ("Este programa necesita ser compilado en el formato de memoria LARGE.\r\n"
	  "Si trabaja con el IDE de Borland escoja el formato correcto en el menฃ\r\n"
	  "Options|Compiler|Code Genetarion.\r\r\n");
}
// Si se compila bajo el formato de memoria incorrecta, da otro error

#else
/*********** Ficheros de cabecera necesarios *******************************/
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <dir.h>
#include <string.h>
#include "defines.h"
#include "tipos.h"
#include "boolean.h"  // Define el tipo booleano y sus constantes
#include "fichas.h"   // Define las clases para manejar bases de datos
#include "control.h"  // Define los procedimientos para un sencillo control
                      // de la pantalla
#include "fact.h"     // Funciones para imprimir facturas
#include "menus.h"    // Funciones para manejar los menฃs
#include "calcu.h"    // Define la calculadora

#define BUSQUEDA_CANCELADA 60000

/**********************CONSTANTES Y VARIABLES GLOBALES*********************/
const OP_NADA = 0;
const OP_SALIR = 100;
const OP_IMPRIMIR = 0x1000;
const OP_CALCU = 0x1010;
const OP_RESTAURA = 0x1015;
const OP_NOT_IMP = 0xEFFF;
const OP_ABRIR_CLIENTES = 1;
const OP_ABRIR_ARTICULOS = 2;
const OP_GUARDAR_CLIENTES = 10;
const OP_GUARDAR_ARTICULOS = 11;
const OP_CONF_MON = 30;
const OP_CONF_IMPR = 31;
const OP_CONF_VARIAS = 32;
const OP_CONF_SALVAR = 40;
const OP_CLIENTE_NUEVO = 101;
const OP_CLIENTE_BUSCAR = 110;
const OP_CLIENTE_SIG = 111;
const OP_CLIENTE_ANT = 112;
const OP_CLIENTE_INI = 113;
const OP_CLIENTE_FIN = 114;
const OP_CLIENTE_LIST_ALF = 120;
const OP_CLIENTE_LIST_COD = 121;
const OP_CLIENTE_MODIFICAR = 130;
const OP_CLIENTE_BORRAR = 131;
const OP_ART_NUEVO = 201;
const OP_ART_BUSCAR = 210;
const OP_ART_SIG = 211;
const OP_ART_ANT = 212;
const OP_ART_INI = 213;
const OP_ART_FIN = 214;
const OP_ART_LIST_ALF = 220;
const OP_ART_LIST_COD = 221;
const OP_ART_MODIFICAR = 230;
const OP_ART_BORRAR = 231;
const OP_FACTURA_A = 300;
const OP_FACTURA_B = 301;
const OP_FACTURA_C = 302;
const OP_FACTURA_BORRAR = 305;
const OP_FACTURACION_CLIENTES = 310;
const OP_FACTURACION_REMESA = 320;
const OP_ACERCA_DE = 1000;
/* Constantes de opciones para los menฃs */

Tcampo def_clientes[10] = {{0, "Cขdigo", 4, 0, 0, ENTERO, 2, 1},
			  {0, "Apellidos", 20, 0, 0, CADENA, 2, 3},
                          {0, "Nombre", 20, 0, 0, CADENA, 2, 5},
			  {0, "Direcciขn", 40, 0, 0, CADENA, 2, 7},
			  {0, "Pueblo", 20, 0, 0, CADENA, 2, 9},
			  {0, "C.P.", 5, 0, 0, CADENA, 41, 9},
			  {0, "Provincia", 10, 0, 0, CADENA, 2, 11},
                          {0, "DNI o CIF", 10, 0, 0, CADENA, 2, 13},
                          {0, "Telefono 1", 10, 0, 0, CADENA, 31, 13},
                          {0, "Telefono 2", 10, 0, 0, CADENA, 31, 15}},
// Definiciขn de las fichas de los clientes

       def_articulo[7] = {{0, "Cขdigo", 4, 0, 0, ENTERO, 2, 1},
                          {0, "Descripciขn", 40, 0, 0, CADENA, 2, 3},
                          {0, "Nง rgtro", 10, 0, 0, CADENA, 2, 5},
                          {0, "P.S.", 4, 0, 0, CADENA, 2, 7},
                          {0, "Toxicidad", 1, 0, 0, CADENA, 2, 9},
                          {0, "Precio", 10, 0, 0, REAL, 2, 11},
			  {0, "2ง Precio", 10, 0, 0, REAL, 2, 13}};
// Definiciขn de las fichas de los artกculos

Cbase_datos LClientes(def_clientes, 10),
	    LArticulos(def_articulo, 7);
// Listas de clientes y artกculos

Tpantalla ventana;
// Se usa para simular una ventana en la que se muestran los datos

char directorio_trabajo[MAXPATH];
// Directorio donde estan los ficheros de datos

/**************************************************************************/

int pascal busca (Cbase_datos &lista, char *titulo)
// Busca un dato dentro de la lista pasada
  {int nro_campos,  // Nro de campos de la lista a tratar
       alto;        // Auxiliar. Usada para calcular el alto de la ventana
   unsigned auxu,     // auxiliar;
	    contador; // cuentas varias
   static unsigned inicio = TRUE; // Indica si hay que buscar desde el
				  // principio de la lista
   Tpantalla temp;                // Para almacenar la pantalla
   char aux1[80], aux2[80],       // Vars. auxiliares y temporales.
	tecla;                    // Tecla pulsada
   byte static dato_a_buscar[80]; // Dato a buscar

   nro_campos = lista.hay_campos();  // Lee el nฃmero de campos.
   alto = nro_campos + 5;            // Calcula el alto de la ventana
   sprintf (aux1, " Busqueda de %s ", titulo);
   temp = hace_recuadro (255, 255, 40, alto, aux1);   // Dibuja la ventana
   textbackground (BLUE);
   lista.primer_campo();
   for (contador = 1; contador <= nro_campos; contador++)
     {lista.datos_campo (aux1, auxu, auxu, auxu, auxu, auxu, auxu, auxu);
      // Lee los datos del campo actual. Como solo interesa el nombre, los
      // dem s datos se mandan a paseo
      gotoxy (2, 1 + contador); textcolor (YELLOW); cprintf ("%u) ", contador);
      textcolor (WHITE); cputs (aux1);
      // Los imprime.
      lista.campo_siguiente ();
     }
   cputs ("\n\n\r Escoja el campo de bฃsqueda : ");
   memset (aux1, 0, 10);
   contador = 0;
   _setcursortype (_NORMALCURSOR);
   // Leemos el nง del campo de bฃsqueda.

   tecla = lee_cadena (aux1, 2, FALSE);

   restaura_recuadro (&temp);
   if (tecla == ESC)
   // Si pulsamos ESC quitamos nuestra ventana y nos vamos,
     return BUSQUEDA_CANCELADA;
   auxu = atoi (aux1); // sino pasamos la cadena a un entero
   if (auxu > nro_campos)
     // Si el campo no existe damos un error y nos vamos.
     {mensaje ("Error, ese campo no existe", 255, 255, 30, 7);
      return CAMPO_INEXISTENTE;
     }
   lista.buscar_campo (auxu - 1);
   lista.datos_campo (aux1, (unsigned)contador, auxu, auxu, auxu, auxu, auxu,
		      (unsigned) nro_campos);
   // Leemos los datos del campo por el que se va a realizar la bฃsqueda

   sprintf (aux2, " Escoja el %s a buscar y pulse &ENTER&, &ESC& cancela.",
            aux1);
   pone_barra_inferior (aux2);
   sprintf (aux2, " %s a buscar :", aux1);
   pregunta (aux2, 255, 255, 50, 10, aux1, contador);
   // Pedimos el dato a buscar.
   if (*aux1 == 0)
   // Si no se ha introducido ningฃn dato, y ya se habกa realizado una
   // bฃsqueda anteriormente, se busca el mismo dato
     {if (*dato_a_buscar != 0)
        {strcpy (aux1, dato_a_buscar);
         inicio = FALSE;
        }
      else return OK;
     }
   else
     strcpy (dato_a_buscar, aux1);

   contador = lista.buscar_ficha (aux1, nro_campos + 1,
                                  inicio ? 0 : lista.nro_ficha_actual);
   // Se busca la ficha
   if (contador != FICHA_INEXISTENTE)
     lista.ficha_numero (contador);
   else return FICHA_INEXISTENTE;
   // Si se encuentra nos ubicamos sobre ella, sino salimos de la funciขn
   _setcursortype (_NOCURSOR);
   restaura_recuadro (&temp);
   return OK;
  } // busca

void pascal inicio_fin (unsigned *inicio, unsigned *fin, unsigned nro_campo,
                        Cbase_datos &objeto)
// Selecciona un rango de fichas por un determinado campo
  {char aux_c[80], cadena[80];
   unsigned tipo_dato,
            aux;

   if (objeto.campo_numero (nro_campo) != OK)
   // Si el campo no existe, se sale indicando error
     {*inicio = ERROR;
      return;
     }

   objeto.datos_campo (aux_c, aux, aux, aux, tipo_dato, aux, aux, aux);
   // Se lee el nombre y el tipo de dato del campo
   sprintf (cadena, " %s inicial del listado :", aux_c);
   *aux_c = 0;
   if (pregunta (cadena, 255, 255, 44, 7, aux_c, 41,
                   (tipo_dato == CADENA) ? TRUE : FALSE) == FALSE)
   // Se pide el dato inicial del rango de fichas, si no se devuelve nada se
   // selecciona el inicio de la lista, si se pulsa ESC se sale indicando error
     {*inicio = ERROR;
      return;
     }
   else
     if (*aux_c == 0)
       *inicio = 0;
     else
     // Si se indica un dato se busca este por una bฃsqueda binaria
       {aux = objeto.busqueda_binaria (aux_c);
        if (aux == FICHA_INEXISTENTE)
          {sonido_error ();
           sprintf (cadena, "%s no est  en la lista.", aux_c);
           mensaje (cadena, 255, 255, 50, 7);
           *inicio = ERROR;
           return;
          }
	else *inicio = aux;
       }

   // Ahora se repite el mismo proceso para el dato final
   objeto.datos_campo (aux_c, aux, aux, aux, tipo_dato, aux, aux, aux);
   sprintf (cadena, " %s final del listado :", aux_c);
   *aux_c = 0;
   if (pregunta (cadena, 255, 255, 44, 7, aux_c, 41,
                   (tipo_dato == CADENA) ? TRUE : FALSE) == FALSE)
     {*inicio = ERROR;
      return;
     }
   else
     if (*aux_c == 0)
       *fin = objeto.hay_fichas() - 1;
     else
       {aux = objeto.busqueda_binaria (aux_c);
        if (aux == FICHA_INEXISTENTE)
          {sonido_error ();
           sprintf (cadena, "%s no est  en la lista.", aux_c);
	   mensaje (cadena, 255, 255, 50, 7);
           *inicio = ERROR;
           return;
          }
        else *fin = aux;
       }
  } // inicio_fin

void pascal clientes (unsigned opcion, boolean imprimir)
// Submenฃ de clientes
  {char barra[80], *auxc; // Variables temporales y auxiliares
   unsigned aux;
   static unsigned ficha_inicio;
   static unsigned ficha_fin;
   
   color_ventana (colores.texto_ventana, colores.fondo_ventana,
                  colores.borde_ventana);
   switch (opcion)
     {case OP_CLIENTE_NUEVO : // Aคade un nuevo cliente a la lista de clientes
         restaura_recuadro (&ventana);
         pone_barra_inferior (nuevo_cli);
         if (LClientes.nueva_ficha() == MEMORIA_AGOTADA)
         // Aคade una nueva ficha a la lista de clientes. Si no hay espacio
         // disponible muestra un mensaje de error.
           {sonido_error();
            mensaje ("No queda sitio para m s clientes en la lista.\r\n"
                     " Grabe los datos del fichero y cambie el nombre del\r\n"
                     " fichero de clientes en el menฃ de configuraciขn.",
                     255, 255, 54, 9);
            return;
           }
         sprintf (barra, " Cliente nฃmero : %u ", LClientes.hay_fichas());
         ventana = hace_recuadro (255, 255, 60, 18, barra);
         if (LClientes.rellenar_ficha(TRUE) == OK)
         // Muestra la ficha en pantalla y pide los datos de esta.
           {auxc = (char *)LClientes.leer_dato (2);
            aux = LClientes.buscar_ficha (auxc, 2);
            if (aux + 1 != LClientes.nro_ficha_actual)
              {sonido_error ();
               mensaje ("El cliente ya existe", 255, 255, 30, 7);
	       LClientes.borrar_ficha (LClientes.nro_ficha_actual);
               LClientes.modificado = FALSE;
              }
           }

         break;
         // Crea una nueva ficha y pide los datos de esta.

      case OP_CLIENTE_BUSCAR : // Visualiza la lista de clientes.
      case OP_CLIENTE_SIG :
      case OP_CLIENTE_ANT :
      case OP_CLIENTE_INI :
      case OP_CLIENTE_FIN :
      case OP_CLIENTE_MODIFICAR :
      case OP_CLIENTE_BORRAR :
         pone_barra_inferior (" Clientes ณ");
         // Prepara la pantalla
         if (!LClientes.hay_fichas())
           {sonido_error();
            mensaje ("La lista de clientes\r\n est  vacกa.", 255, 255, 25,
                     7);
            break;
	   } // Si no hay fichas se muestra un mensaje de error
         if (imprimir)
           {LClientes.imprimir_ficha (IMPRESORA);
            break;
           }

         restaura_recuadro (&ventana);
         switch (opcion)
           {case OP_CLIENTE_BUSCAR : // Busca un cliente segฃn un campo de las fichas
                     pone_barra_inferior (buscar_cli);
                     aux = busca (LClientes, "clientes");
                     if (aux == FICHA_INEXISTENTE)
		     // si no se encuentra se muestra un mensaje de error
                       {sonido_error ();
                        mensaje ("El cliente no se encuentra en la"
                                 "\r\n lista.", 255, 255, 40, 7);
                       }
                     break;

            case OP_CLIENTE_SIG : // Pasa al siguiente cliente de la lista
                     if (LClientes.sig_ficha() == ULTIMA_FICHA)
                       sonido_error();
                     break;

            case OP_CLIENTE_ANT : // Pasa al cliente anterior de la lista
                     if (LClientes.ant_ficha() == PRIMER_FICHA)
                       sonido_error();
                     break;

            case OP_CLIENTE_INI : // Pasa al primer cliente de la lista
                     LClientes.primer_ficha();
                     break;

            case OP_CLIENTE_FIN : // Pasa al ฃltimo cliente de la lista
                     LClientes.ultima_ficha();
                     break;

            case OP_CLIENTE_MODIFICAR : // Modifica la ficha actual
                     pone_barra_inferior (modif_cli);
                     sprintf (barra, " Cliente nฃmero : %u ",
                              LClientes.nro_ficha_actual);
                     ventana = hace_recuadro (255, 255, 60, 18, barra);
                     LClientes.imprimir_ficha(PANTALLA);
                     LClientes.rellenar_ficha (FALSE);
                     restaura_recuadro (&ventana);
		     break;

            case OP_CLIENTE_BORRAR : // Borra la ficha actual de la lista
                     sprintf (barra, " Borrar  el cliente nง %d \r\n "
                              " จSeguro?", LClientes.nro_ficha_actual);
                if (aviso (barra, 255, 255, 35, 8) == SI)
                  LClientes.borrar_ficha (LClientes.nro_ficha_actual);
                     break;

           }
         sprintf (barra, " Cliente nฃmero : %u ", LClientes.nro_ficha_actual);
         ventana = hace_recuadro (255, 255, 60, 18, barra);
         LClientes.imprimir_ficha(PANTALLA);
         // Prepara la pantalla y muestra la ficha actual
         break;

      case OP_CLIENTE_LIST_ALF : // Muestra el listado alfabtico de los clientes
      case OP_CLIENTE_LIST_COD : // y el listado por cขdigo
        {Tpantalla aux;

         if (!LClientes.hay_fichas())
           {sonido_error ();
            mensaje ("La lista de clientes\r\n est  vacกa.", 255, 255, 25,
                     7);
            break;
           }
         // Si no hay clientes en la lista se sale inmediatamente de la funciขn

         if (!imprimir)
           {restaura_recuadro (&ventana);
            if (opcion == OP_CLIENTE_LIST_ALF)
              pone_barra_inferior (" Clientes ณ Listado alfabtico");
            else pone_barra_inferior (" Clientes ณ Listado por cขdigos");
            ventana = hace_recuadro (255, 2, 69, 22, " CLIENTES ");
           }
         aux = hace_recuadro (255, 255, 26, 7);
         cputs ("\n   Ordenando clientes.\r\n\n   Espere un momento...");
         if (opcion == OP_CLIENTE_LIST_ALF)
           LClientes.ordenar_fichas (1);
         else
           LClientes.ordenar_fichas (0);
         // Ordena las fichas alfabticamente
         restaura_recuadro (&aux);

         if (!imprimir)
           {if (opcion == OP_CLIENTE_LIST_ALF)
              inicio_fin (&ficha_inicio, &ficha_fin, 2, LClientes);
            else
              inicio_fin (&ficha_inicio, &ficha_fin, 1, LClientes);
            if (ficha_inicio == ERROR && !imprimir)
              {restaura_recuadro (&ventana);
               break;
              }
           }
         if (imprimir)
           imprime_lista (LClientes, 'c', ficha_inicio, ficha_fin);
         else
           muestra_lista (LClientes, 'c', ficha_inicio, ficha_fin, 22);

         break;
        }
     }
  } // clientes

void pascal articulos (unsigned opcion, boolean imprimir)
// Submenฃ de artกculos
  {char barra[80], *auxc;
   unsigned aux;
   static unsigned ficha_inicio;
   static unsigned ficha_fin;
   
   color_ventana (colores.texto_ventana, colores.fondo_ventana,
                  colores.borde_ventana);
   switch (opcion)
     {case OP_ART_NUEVO : // Aคade un nuevo artกculo a la lista
         restaura_recuadro (&ventana);
	 pone_barra_inferior (nuevo_art);
         if (LArticulos.nueva_ficha() == MEMORIA_AGOTADA)
         // Aคade una nueva ficha a la lista de artกculos. Si no hay espacio
         // disponible muestra un mensaje de error.
           {sonido_error();
            mensaje ("No queda sitio para m s artกculos en la lista.\r\n"
                     " Grabe los datos del fichero y cambie el nombre del\r\n"
                     " fichero de artกculos en el menฃ de configuraciขn.",
                     255, 255, 60, 9);
            return;
           }
         sprintf (barra, " Artกculo nฃmero : %u ", LArticulos.hay_fichas());
         ventana = hace_recuadro (255, 255, 60, 15, barra);
         if (LArticulos.rellenar_ficha(TRUE) == OK)
         // Despus de completar la ficha de artกculos se verifica que este
         // no existiera ya en la lista
           {auxc = (char *)LArticulos.leer_dato (2);
            aux = LArticulos.buscar_ficha (auxc, 2);
            if (aux + 1 != LArticulos.nro_ficha_actual)
	    // Si existe se muestra un mensaje de error y se sale
              {sonido_error ();
               mensaje ("El artกculo ya existe", 255, 255, 30, 7);
               LArticulos.borrar_ficha (LArticulos.nro_ficha_actual);
               LArticulos.modificado = FALSE;
              }
           }
         break;
         // Rellena una nueva ficha

      case OP_ART_BUSCAR : // Muestra y modifica la lista de artกculos
      case OP_ART_SIG :
      case OP_ART_ANT :
      case OP_ART_INI :
      case OP_ART_FIN :
      case OP_ART_MODIFICAR :
      case OP_ART_BORRAR :
         if (!LArticulos.hay_fichas())
         // Si no hay artกculos se muestra un mensaje de error
           {sonido_error();
            mensaje ("La lista de artกculos\r\n est  vacกa.", 255, 255, 25,
		     7);
            break;
           }
         if (imprimir)
           {LArticulos.imprimir_ficha (IMPRESORA);
            break;
           }
         restaura_recuadro (&ventana);
         switch (opcion)
           {case OP_ART_BUSCAR : // Busca un artกculo segฃn un campo de las fichas
              pone_barra_inferior (busca_art);
              aux = busca (LArticulos, "artกculos");
              if (aux == FICHA_INEXISTENTE)
                {sonido_error ();
		 mensaje ("El artกculo no se encuentra en la lista",
                          255, 255, 40, 5);
                }
              break;

            case OP_ART_SIG : // Pasa al siguiente artกculo de la lista
              if (LArticulos.sig_ficha() == ULTIMA_FICHA)
                sonido_error();
              break;

            case OP_ART_ANT : // Pasa al artกculo anterior de la lista
              if (LArticulos.ant_ficha() == PRIMER_FICHA)
                sonido_error();
              break;

            case OP_ART_INI : // Pasa al primer artกculo de la lista
              LArticulos.primer_ficha();
              break;

            case OP_ART_FIN : // Pasa a la ฃltima ficha
              LArticulos.ultima_ficha();
              break;

            case OP_ART_MODIFICAR : // Modifica la ficha actual
              pone_barra_inferior (edit_art);
              sprintf (barra, " Artกculo nฃmero : %u ",
                       LArticulos.nro_ficha_actual);
              ventana = hace_recuadro (255, 255, 60, 15, barra);
              LArticulos.imprimir_ficha(PANTALLA);
              LArticulos.rellenar_ficha (FALSE);
              restaura_recuadro (&ventana);
              break;

            case OP_ART_BORRAR : // Borra la ficha actual de la lista
              sprintf (barra, " Borrar  el artกculo nง %d \r\n "
                       " จSeguro?", LArticulos.nro_ficha_actual);
              if (aviso (barra, 255, 255, 35, 8) == SI)
                LArticulos.borrar_ficha (LArticulos.nro_ficha_actual);
              break;

           }
         sprintf (barra, " Artกculo nฃmero : %u ", LArticulos.nro_ficha_actual);
         ventana = hace_recuadro (255, 255, 60, 15, barra);
         LArticulos.imprimir_ficha(PANTALLA);
         break;

      case OP_ART_LIST_ALF : // Muestra el listado por nombre de los artกculos
      case OP_ART_LIST_COD : // y el listado por cขdigos
             // El proceso es similar al del listado alfabtico de clientes
        {Tpantalla aux;
         
         if (!LArticulos.hay_fichas())
           {sonido_error ();
            mensaje ("La lista de artกculos\r\n est  vacกa.", 255, 255, 25,
		     7);
            break;
           }
         // Si no hay clientes en la lista se sale inmediatamente de la funciขn

         if (!imprimir)
           {restaura_recuadro (&ventana);
            if (opcion == OP_ART_LIST_ALF)
              pone_barra_inferior (" Artกculos ณ Listado alfabtico");
            else pone_barra_inferior (" Artกculos ณ Listado por cขdigos");
            ventana = hace_recuadro (255, 2, 79, 22, " ARTICULOS ");
           }
         aux = hace_recuadro (255, 255, 26, 7);
         cputs ("\n   Ordenando artกculos.\r\n\n   Espere un momento...");

         if (opcion == OP_ART_LIST_ALF)
           LArticulos.ordenar_fichas(1);
         else
           LArticulos.ordenar_fichas(0);
         // Ordena las fichas alfabticamente o por cขdigo
         restaura_recuadro (&aux);

         if (!imprimir)
           {if (opcion == OP_ART_LIST_ALF)
              inicio_fin (&ficha_inicio, &ficha_fin, 2, LArticulos);
            else
              inicio_fin (&ficha_inicio, &ficha_fin, 1, LArticulos);
            if (ficha_inicio == ERROR && !imprimir)
              {restaura_recuadro (&ventana);
               break;
              }
           }

         if (imprimir)
           imprime_lista (LArticulos, 'a', ficha_inicio, ficha_fin);
         else
           muestra_lista (LArticulos, 'a', ficha_inicio, ficha_fin, 22);

         break;
        }
     }
  } // articulos

void pascal facturacion (unsigned opcion, boolean imprimir)
// Submenฃ de facturaciขn
  {desplazamiento_vertical = 0;
   restaura_recuadro (&ventana);
   color_ventana (colores.texto_ventana, colores.fondo_ventana,
                  colores.borde_ventana);
   switch (opcion)
     {case OP_FACTURA_A : hace_factura_tipo_1 (LClientes, LArticulos);
                          break;

      case OP_FACTURA_B : hace_factura_tipo_2 (LClientes);
                          break;

      case OP_FACTURA_C : hace_factura_tipo_3 (LClientes, LArticulos);
                          break;

      case OP_FACTURA_BORRAR : borra_factura ();
                               break;

      case OP_FACTURACION_CLIENTES :
        {FILE *fichero;
         TFtemporal *salida;
         char aux_c[120];
         static unsigned num_ficha, num_cliente;
         unsigned contador = 1;
         Tdatos_fichero_facturas datos_factura;
         date fecha;
         static date fecha_ini, fecha_fin;
         static long fecha_ini_l, fecha_fin_l;
         struct time hora;
         long double total = 0;

         if (!imprimir)
           {pone_barra_inferior (codigo_cliente);
            pregunta ("Cขdigo del cliente :", 255, 255, 25, 8, aux_c,
                      5, FALSE);
            num_ficha = LClientes.buscar_ficha (aux_c, 1, 0);
           }
         // Se pide el cขdigo del cliente y se verifica que este existe
         if (num_ficha == FICHA_INEXISTENTE)
         // Si no existe se sale dando un mensaje de aviso
           {sonido_error();
            mensaje ("Cliente inexistente.", 255, 255, 25, 8);
            break;
           }
         else
         // Sino se selecciona su ficha
           if (!imprimir)
             num_cliente = atoi (aux_c);

         LClientes.ficha_numero (num_ficha);
         strcpy (aux_c, " Informe de facturaciขn de ");
         strcat (aux_c, LClientes.leer_dato (2));
         strcat (aux_c, ", ");
         strcat (aux_c, LClientes.leer_dato (3));
         strcat (aux_c, " ");

         if (!imprimir)
           {pide_fecha ("Fecha inicial :", 255, 255, 19, 6, &fecha_ini);
            // Se pide la fecha inicial del listado
            if (fecha_ini.da_mon == 0)
            // Si no se indica ninguna fecha se hace un listado desde el
            // 1-1-1970 hasta la fecha actual
              {fecha_ini.da_day = 1;
               fecha_ini.da_mon = 1;
               fecha_ini.da_year = 1970;
               fecha_ini_l = dos2luc (&fecha_ini);
               getdate (&fecha_fin);
               fecha_fin_l = dos2luc (&fecha_fin);
              }
            else
            // Sino se pide la fecha final del listado
              {fecha_ini_l = dos2luc (&fecha_ini);
               pide_fecha ("Fecha final :", 255, 255, 19, 6, &fecha_fin);
               if (fecha_fin.da_mon == 0)
                 getdate (&fecha_fin);
               fecha_fin_l = dos2luc (&fecha_fin);
              }
           }
         if (imprimir)
           {salida = inicializa_impresion();
            envia_datos (aux_c, salida);
            envia_datos ("\r\n\n   Fecha      บ Nง Factura บ Situaciขn                      บ Importe total\r\n"
                         "  ออออออออออออฮออออออออออออฮออออออออออออออออออออออออออออออออฮออออออออออออออ\r\n",
                         salida);
            contador = 5;
           }
         else
           {ventana = hace_recuadro (255, 2, 79, 22, aux_c);
            cputs ("   Fecha      บ Nง Factura บ Situaciขn                      บ Importe total\r\n"
                   "  ออออออออออออฮออออออออออออฮออออออออออออออออออออออออออออออออฮออออออออออออออ");
            window (actual.winleft, actual.wintop + 2, actual.winright,
                    actual.winbottom);
           }
         fichero = fopen (Facturas_nombre_fichero, "rb");
         // Se abre el fichero de las facturas
         while (!feof(fichero)) // lee hasta llegar al final del fichero
           {if (fread (&datos_factura, sizeof (Tdatos_fichero_facturas), 1,
                       fichero) != 1)
              continue;
            // Lee cada factura
            if (datos_factura.fecha < fecha_ini_l ||
                datos_factura.fecha > fecha_fin_l ||
                num_cliente != datos_factura.nro_cliente)
              continue;
            // Si la fecha de la factura leida est  fuera del rango, o el
            // cliente no coincide, se pasa a leer la siguiente factura
            luc2dos (datos_factura.fecha, &fecha);
            if (imprimir)
              {sprintf (aux_c, "  %2d/%s/%4d บ %10ld บ %-30.30s บ %10.0f\r\n",
                        fecha.da_day, c_mes[fecha.da_mon - 1], fecha.da_year,
                        datos_factura.nro_factura, datos_factura.situacion,
                        datos_factura.total_factura);
               envia_datos (aux_c, salida);
              }
            else
              {gotoxy (3, contador);
               cprintf ("%2d/%s/%4d บ %10ld บ %-30.30s บ %10.0f", fecha.da_day,
                        c_mes[fecha.da_mon - 1], fecha.da_year,
                        datos_factura.nro_factura, datos_factura.situacion,
                        datos_factura.total_factura);
              }
            total += datos_factura.total_factura;
            contador++;
            // Cuenta el nro. de lกneas impresas y si llega a un tope
            // diferente si es la pantalla o la impresora, hace una pausa,
            // o envia un salto de p gina
            if (contador == 20 && !imprimir)
              {pone_barra_inferior (pres_enter);
               getch();
               contador = 1;
               clrscr ();
               pone_barra_inferior ("");
              }
            else
              if (contador == lineas_papel)
                fputc (12, salida->fichero);
           }
         // Muestra el total
         if (!imprimir)
           cprintf ("\r\nTotal facturado : %.0Lf Pts.", total);
         else
           {envia_datos ("  ออออออออออออฮออออออออออออฮออออออออออออออออออออออออออออออออฮออออออออออออออ\r\n",
                         salida);
            sprintf (aux_c, "  Total facturado :                            "
                     "               %10.0Lf Pts.", total);
            envia_datos (aux_c, salida);
           }
         if (!imprimir)
           {pone_barra_inferior (pres_tecla);
            getch();
           }
         else imprime (salida);
         break;
	}

      case OP_FACTURACION_REMESA : // Facturaciขn por remesas
        {FILE *fichero;
         TFtemporal *salida;
         unsigned contador, f_actual, ficha;
         char cadena[160], auxc[60];
         Tdatos_fichero_facturas datos_factura;
         static date fecha_ini, fecha_fin, fecha;
         static long fecha_ini_l, fecha_fin_l;
         struct time hora;
         double total_litros, total1, total2, total_iva, total;

         total_litros = total1 = total2 = total_iva = total = 0;
         contador = 0;
         f_actual = LClientes.nro_ficha_actual;
         if (!imprimir)
          {pide_fecha ("Fecha inicial :", 255, 255, 19, 6, &fecha_ini);
           if (fecha_ini.da_mon == 0)
             {fecha_ini.da_day = 1;
              fecha_ini.da_mon = 1;
              fecha_ini.da_year = 1;
              fecha_ini_l = dos2luc (&fecha_ini);
              getdate (&fecha_fin);
              fecha_fin_l = dos2luc (&fecha_fin);
             }
           else
             {fecha_ini_l = dos2luc (&fecha_ini);
              pide_fecha ("Fecha final :", 255, 255, 19, 6, &fecha_fin);
              if (fecha_fin.da_mon == 0)
                getdate (&fecha_fin);
              fecha_fin_l = dos2luc (&fecha_fin);
             }
           oculta_menu();
           reloj_visible = FALSE;
           if (pone_modo_132()) // Se pone el modo de 132 columnas
             {
              puts ("  Cliente           บ Situaciขn     บ Fecha       บ Nง Factura บ Litros      บ Pts. Servic.บ Pts. Produc.บ Pts. IVA    บ Total");
              puts (" อออออออออออออออออออฮอออออออออออออออฮอออออออออออออฮออออออออออออฮอออออออออออออฮอออออออออออออฮอออออออออออออฮอออออออออออออฮอออออออออออ");
              fichero = fopen (Facturas_nombre_fichero, "rb");
              while (!feof (fichero))
                {if (fread (&datos_factura, sizeof (Tdatos_fichero_facturas), 1,
                            fichero) != 1)
                   continue;
                 if (datos_factura.fecha < fecha_ini_l ||
                     datos_factura.fecha > fecha_fin_l)
                   continue;
                 luc2dos (datos_factura.fecha, &fecha);
                 sprintf (auxc, "%d", datos_factura.nro_cliente);
                 ficha = LClientes.buscar_ficha ((char*)auxc, 1);
                 LClientes.ficha_numero (ficha);
                 strcpy (cadena, LClientes.leer_dato (2));
                 strcat (cadena, ", ");
                 strcat (cadena, LClientes.leer_dato (3));
                 cadena[18] = 0;
                 printf (" %-18s บ %13s บ %2d/%3s/%4d บ %10ld บ %11.0f บ"
                         " %11.0f บ %11.0f บ %11.0f บ %11.0f",
                         cadena,
                         datos_factura.situacion,
                         fecha.da_day, c_mes[fecha.da_mon-1], fecha.da_year,
                         datos_factura.nro_factura, datos_factura.litros,
                         datos_factura.total_1, datos_factura.total_2,
                         datos_factura.total_iva, datos_factura.total_factura);
                 total_litros += datos_factura.litros;
                 total1 += datos_factura.total_1;
                 total2 += datos_factura.total_2;
                 total_iva += datos_factura.total_iva;
                 total += datos_factura.total_factura;
                 contador++;
                 if (contador == 22)
                   {getch();
                    asm {mov ax, 0x0600
                         mov bh, 0x07
                         mov cx, 0
                         mov dl, 131
                         mov dh, 24
                         int 0x10
                        } // Borra la pantalla {clrscr()}
                    puts ("  Cliente           บ Situaciขn     บ Fecha       บ Nง Factura บ Litros      บ Pts. Servic.บ Pts. Produc.บ Pts. IVA    บ Total");
                    puts (" อออออออออออออออออออฮอออออออออออออออฮอออออออออออออฮออออออออออออฮอออออออออออออฮอออออออออออออฮอออออออออออออฮอออออออออออออฮอออออออออออ");
                    contador = 2;
                   }
                }
              puts (" อออออออออออออออออออฮอออออออออออออออฮอออออออออออออฮออออออออออออฮอออออออออออออฮอออออออออออออฮอออออออออออออฮอออออออออออออฮอออออออออออ");
              printf (" Total facturado :                                   "
                      "                          %11.0f บ %11.0f บ %11.0f บ %11.0f",
                         total1, total2, total_iva, total);
              getch();
              pone_modo_normal();
              dibuja_escritorio();
             }
           else
             mensaje ("No se ha podido activar un modo de 132 columnas.\r\n"
                      " El listado de la facturaciขn por remesas se puede\r\n"
                      " imprimir escogiendo la opciขn Imprimir del menฃ Archivo.",
                      255, 255, 59, 8);
           LClientes.ficha_numero (f_actual);
           dibuja_menu();
           reloj_visible = TRUE;
          }
         else
           {salida = inicializa_impresion();
            envia_datos ("\017  Cliente           บ Situaciขn     บ"
                         " Fecha       บ Nง Factura บ Litros      บ"
                         " Pts. Servic.บ Pts. Produc.บ Pts. IVA    บ Total\r\n"
                         " อออออออออออออออออออฮอออออออออออออออฮอออออออออออออฮ"
                         "ออออออออออออฮอออออออออออออฮอออออออออออออฮ"
                         "อออออออออออออฮอออออออออออออฮอออออออออออ\r\n"
/*                           Cod. cli.บ Situaciขn     บ Fecha       บ"
                         " Nง Factura บ Litros      บ Pts. Servic.บ"
                         " Pts. Produc.บ Pts. IVA    บ Total\r\n"
                         " ออออออออออฮอออออออออออออออฮอออออออออออออ"
                         "ฮออออออออออออฮอออออออออออออฮอออออออออออออฮ"
                         "อออออออออออออฮอออออออออออออฮอออออออออออออ\r\n"*/
                         , salida);
            fichero = fopen (Facturas_nombre_fichero, "rb");
            while (!feof (fichero))
              {if (fread (&datos_factura, sizeof (Tdatos_fichero_facturas), 1,
                          fichero) != 1)
                 continue;
               if (datos_factura.fecha < fecha_ini_l ||
                   datos_factura.fecha > fecha_fin_l)
                 continue;
               luc2dos (datos_factura.fecha, &fecha);
               sprintf (auxc, "%d", datos_factura.nro_cliente);
               ficha = LClientes.buscar_ficha ((char*)auxc, 1);
               LClientes.ficha_numero (ficha);
               strcpy (auxc, LClientes.leer_dato (2));
               strcat (auxc, ", ");
               strcat (auxc, LClientes.leer_dato (3));
               auxc[18] = 0;
               sprintf (cadena, " %-18s บ %13s บ %2d/%3s/%4d บ %10ld บ"
                        " %11.0f บ %11.0f บ %11.0f บ %11.0f บ %11.0f\r\n",
                        auxc,
                        datos_factura.situacion,
                        fecha.da_day, c_mes[fecha.da_mon-1], fecha.da_year,
                        datos_factura.nro_factura, datos_factura.litros,
                        datos_factura.total_1, datos_factura.total_2,
                        datos_factura.total_iva, datos_factura.total_factura);
               envia_datos (cadena, salida);
               total_litros += datos_factura.litros;
               total1 += datos_factura.total_1;
               total2 += datos_factura.total_2;
               total_iva += datos_factura.total_iva;
               total += datos_factura.total_factura;
               contador++;
               if (contador == lineas_papel - 2)
                 {fputc (12, salida->fichero);
            envia_datos ("  Cliente           บ Situaciขn     บ"
                         " Fecha       บ Nง Factura บ Litros      บ"
                         " Pts. Servic.บ Pts. Produc.บ Pts. IVA    บ Total\r\n"
                         " อออออออออออออออออออฮอออออออออออออออฮอออออออออออออฮ"
                         "ออออออออออออฮอออออออออออออฮอออออออออออออฮ"
                         "อออออออออออออฮอออออออออออออฮอออออออออออ\r\n"
/*                           Cod. cli.บ Situaciขn     บ Fecha       บ"
                         " Nง Factura บ Litros      บ Pts. Servic.บ"
                         " Pts. Produc.บ Pts. IVA    บ Total\r\n"
                         " ออออออออออฮอออออออออออออออฮอออออออออออออ"
                         "ฮออออออออออออฮอออออออออออออฮอออออออออออออฮ"
                         "อออออออออออออฮอออออออออออออฮอออออออออออออ\r\n"*/
                         , salida);
                  contador = 2;
                 }
              }
            envia_datos (" อออออออออออออออออออฮอออออออออออออออฮอออออออออออออฮ"
                         "ออออออออออออฮอออออออออออออฮอออออออออออออฮ"
                         "อออออออออออออฮอออออออออออออฮอออออออออออ\r\n", salida);
/*                         " ออออออออออฮอออออออออออออออฮอออออออออออออฮ"
                         "ออออออออออออฮอออออออออออออฮอออออออออออออฮ"
                         "อออออออออออออฮอออออออออออออฮอออออออออออออ\r\n",
                         salida);*/
            sprintf (cadena, " Total facturado :                            "
                     "                                 %11.0f บ %11.0f บ"
                     " %11.0f บ %11.0f\022",
/*                     " Total facturado :                                   "
                     "                 %11.0f บ %11.0f บ %11.0f บ %11.0f\022"*/
                     total1, total2, total_iva, total);
            envia_datos (cadena, salida);
            imprime (salida, TRUE);
           }
         break;
        }
     }
  } // facturacion

void pascal ayuda (void)
// Funciขn de ayuda que de momento no muestra nada m s que quien ha hecho
// el programa
  {
   mensaje ("    Facturaciขn V1.2\r\n\n"
            "     Por Luc Hos Oft\r\n\n"
            "       (C)1996-97", 255, 255, 26, 10);
  } // ayuda

void pascal configuracion (unsigned opcion)
// Opciones de configuraciขn del progama
  {char tecla, aux[256], cadena[MAXPATH];
   Tpantalla temp;

   color_ventana (colores.texto_ventana, colores.borde_ventana,
                  colores.fondo_ventana);
   switch (opcion)
     {case OP_CONF_MON : // Configuraciขn de opciones relativas a las facturas
        temp = hace_recuadro (255, 255, 40, 9, "Opciones de facturas ");
        do
          {pone_barra_inferior (esc_opcion);
           clrscr();
           cprintf ("\r\n 1) IVA actual : %.0f %%\r\n\n"
                    " 2) Factura inicial : %lu \r\n\n\n"
                    " Escoja una opcion : ", iva, nro_factura);
           _setcursortype (_NORMALCURSOR);
           tecla = getch();
           switch (tecla)
             {case '1' : sprintf (aux, "I.V.A. actual : %.0f %\r\n\n Nuevo I.V.A. :",
		         iva);
                         pregunta (aux, 255, 255, 35, 8, aux, 10, TAL_CUAL);
                         if (*aux != 0)
                           {iva = atof (aux);
                            configuracion_modificada = TRUE;
                           }
                         break;

              case '2' : sprintf (aux, "Nง actual de factura : %lu\r\n\n Nuevo "
                                  "nง de factura :", nro_factura);
                         pregunta (aux, 255, 255, 35, 8, aux, 12, TAL_CUAL);
                         if (*aux != 0)
                           {nro_factura = (unsigned) atol (aux);
                            configuracion_modificada = TRUE;
                           }
                         break;
             }
          }
        while (tecla != ESC);
        restaura_recuadro (&temp);
        _setcursortype (_NOCURSOR);
        break;

      case OP_CONF_IMPR : // Configuraciขn de la impresora.
        textattr (YELLOW | (BLUE<<4));
        do
          {pone_barra_inferior (esc_opcion);
           temp = hace_recuadro (255, 255, 40, 11, " Opciones de impresora ");
           cprintf ("\r\n 1) Nง de copias : %d\r\n\n"
                    " 2) Lกneas del papel : %d\r\n\n"
                    " 3) Puerto de conexiขn : %s\r\n\n\n"
                    " Escoja una opciขn : ", nro_copias, lineas_papel,
                    puertos[puerto_conexion]);
           _setcursortype (_NORMALCURSOR);
           tecla = getch();
           switch (tecla)
             {case '1' : sprintf (aux, "Nง de copias : %d\r\n\n"
                                  " Nuevo nฃmero :", nro_copias);
                         pregunta (aux, 255, 255, 35, 8, aux, 10, FALSE);
                         if (*aux)
                           {nro_copias = atoi (aux);
                            configuracion_modificada = TRUE;
                           }
                         break;

              case '2' : sprintf (aux, "Lกneas del papel : %d\r\n\n"
                                  " Nuevo nฃmero :", lineas_papel);
                         pregunta (aux, 255, 255, 35, 8, aux, 10, FALSE);
                         if (*aux)
                           {lineas_papel = atoi (aux);
                            configuracion_modificada = TRUE;
                           }
                         break;

              case '3' : int f;

                         restaura_recuadro (&temp);
                         temp = hace_recuadro (255, 255, 30, 14,
                                               " Puerto de conexiขn ");
                         for (f = 0; f < 8; f++)
                           {gotoxy (3, f+2);
                            cprintf ("%d) %s", f + 1, puertos[f]);
                           }
                         cputs ("\r\n\n\n Escoja un puerto : ");
                         tecla = getch();
                         if (tecla > '0' && tecla < '9')
                           {puerto_conexion = tecla - '1';
                            configuracion_modificada = TRUE;
                           }
                         break;

             }
           restaura_recuadro (&temp);
          }
        while (tecla != ESC);
        _setcursortype (_NOCURSOR);
        break;

      case OP_CONF_VARIAS : // Configuraciขn de opciones varias. Nombres de ficheros,
               // sonido, etc.
	textattr (YELLOW | (BLUE<<4));
	temp = hace_recuadro (255, 255, 50, 17, " Opciones Varias ");
        do
	  {pone_barra_inferior (esc_opcion);
           clrscr();
	   gotoxy (2,2);
	   cprintf ("1) Directorio de trabajo : %s"
                    "\r\n\n 2) Fichero de clientes : %s"
                    "\r\n\n 3) Fichero de artกculos : %s"
                    "\r\n\n 4) Fichero de facturas : %s"
                    "\r\n\n 5) Sonido de error : %s\r\n\n\n"
                    "   Escoja una opciขn : ",
                    directorio_trabajo, LClientes.nombre_fichero,
                    LArticulos.nombre_fichero, Facturas_nombre_fichero,
                    hace_sonido ? "SI" : "NO");
	   _setcursortype (_NORMALCURSOR);
	   tecla = getche();
	   switch (tecla)
	     {case '1' : strcpy (cadena, directorio_trabajo);
			 break;
	      case '2' : strcpy (cadena, LClientes.nombre_fichero);
			 break;
              case '3' : strcpy (cadena, LArticulos.nombre_fichero);
                         break;
              case '4' : strcpy (cadena, Facturas_nombre_fichero);
                         break;
              case '5' : pone_barra_inferior (esc_sino);
                         _setcursortype (_NORMALCURSOR);
                         gotoxy (23, 10);
                         tecla = toupper(getch());
                         _setcursortype (_NOCURSOR);
                         if (tecla == 'S') hace_sonido = TRUE;
                         else if (tecla == 'N') hace_sonido = FALSE;
                         configuracion_modificada = TRUE;
                         break;
             }
	   if (tecla == '1')
	     {sprintf (aux, "El directorio actual es %s\r\n\n Escoja otro "
                       "directorio o pulse ESC para cancelar", cadena);
	      cadena[0] = 0;
	      pregunta (aux, 255, 255, 77, 9, cadena, MAXPATH);
	      if (*cadena)
                {strcat (cadena, "\nul");
                 if (existe_fichero (cadena))
                   {
                    cadena [strlen(cadena) - 4] = 0;
                    strcpy (directorio_trabajo, cadena);
                    configuracion_modificada = TRUE;
                   }
                }
             }
           else
             if (tecla > '1' && tecla < '5')
               {sprintf (aux, "El fichero de trabajo actual es %s\r\n\n"
                         " Escoja un nuevo nombre o pulse ESC para cancelar",
                         cadena);
                cadena[0] = 0;
                pregunta (aux, 255, 255, 77, 9, cadena, 14);
                if (*cadena)
                  {strcpy (aux, directorio_trabajo);
                   switch (tecla)
                     {case '2' : LClientes.grabar_fichero();
                                 strcat (aux, cadena);
                                 if (existe_fichero (aux))
                                   {strcpy (LClientes.nombre_fichero, cadena);
                                    LClientes.leer_fichero();
                                   }
                                 else
                                   {sonido_error();
                                    mensaje ("Fichero inexistente.\r\n"
                                             " Se crear  una lista nueva.",
                                             255, 255, 29, 7);
                                    LClientes.inicializa (def_articulo, 7);
                                   }
                                 configuracion_modificada = TRUE;
                                 break;

                      case '3' : LArticulos.grabar_fichero();
                                 strcat (aux, cadena);
                                 if (existe_fichero (aux))
                                   {strcpy (LArticulos.nombre_fichero, cadena);
                                    LArticulos.leer_fichero();
                                   }
                                 else
                                   {sonido_error();
                                    mensaje ("Fichero inexistente.\r\n"
                                             " Se crear  una lista nueva.",
                                             255, 255, 29, 7);
                                    LArticulos.inicializa (def_articulo, 7);
                                   }
                                 configuracion_modificada = TRUE;
                                 break;

                      case '4' : strcat (aux, cadena);
                                 strcpy (Facturas_nombre_fichero, cadena);
                                 configuracion_modificada = TRUE;
                                 break;
                     }
                  }
                                    
               }
          }
        while (tecla != ESC);
        _setcursortype (_NOCURSOR);
        restaura_recuadro (&temp);
        break;

      case OP_CONF_SALVAR : // Almacena la configuraciขn del programa
        {if (configuracion_modificada)
           {salvar_configuracion (LClientes.nombre_fichero,
                                  LArticulos.nombre_fichero,
                                  Facturas_nombre_fichero, Facturas_ile,
                                  iva, nro_factura, directorio_trabajo);
            configuracion_modificada = FALSE;
           }
         break;
        }
     }
  } // configuraciขn

void pascal inicializa_listas (void)
// Inicializa las listas de clientes, artกculos y facturas
  {
   if (LClientes.leer_fichero() != OK)
     {sonido_error();
      mensaje ("No se ha podido leer el fichero de \r\n clientes\r\n\n"
               " Se creara una lista de clientes vacia.", 255, 255, 43, 9);
     }
  if (LArticulos.leer_fichero() != OK)
     {sonido_error();
      mensaje ("No se ha podido leer el fichero de \r\n artกculos\r\n\n"
               " Se crear  una lista de artกculos vacia.", 255, 255, 43, 9);
     }
  // Se intenta leer los datos de cada lista desde el fichero, si no se
  // puede leer de los ficheros se inicializan las listas sin datos.
  strcpy (LClientes.id, "cliente");
  strcpy (LArticulos.id, "artกculo");
  } // inicializa_listas

void realiza_opcion (unsigned opcion)
// Ejecuta una acciขn segฃn cual sea la opciขn escogida
  {static unsigned opcion_anterior;
   boolean imprimir;

   if (opcion == OP_IMPRIMIR)
     {opcion = opcion_anterior;
      imprimir = TRUE;
     }
   else
     imprimir = FALSE;

   opcion_anterior = opcion;

   switch (opcion)
     {case OP_SALIR :
        if (LClientes.modificado || LArticulos.modificado)
          {if (LClientes.modificado)
               LClientes.grabar_fichero ();
           if (LArticulos.modificado)
               LArticulos.grabar_fichero ();
          }
        break;

      case OP_CALCU : calculadora();

      case OP_RESTAURA : break;

      case OP_ABRIR_CLIENTES : break;
      case OP_ABRIR_ARTICULOS : break;
      case OP_GUARDAR_CLIENTES : break;
      case OP_GUARDAR_ARTICULOS : break;

      case OP_CONF_MON :
      case OP_CONF_IMPR :
      case OP_CONF_VARIAS :
      case OP_CONF_SALVAR : configuracion (opcion);
      			    break;

      case OP_CLIENTE_NUEVO :
      case OP_CLIENTE_BUSCAR :
      case OP_CLIENTE_SIG :
      case OP_CLIENTE_ANT :
      case OP_CLIENTE_INI :
      case OP_CLIENTE_FIN :
      case OP_CLIENTE_LIST_ALF :
      case OP_CLIENTE_LIST_COD :
      case OP_CLIENTE_MODIFICAR :
      case OP_CLIENTE_BORRAR :  clientes (opcion, imprimir);
				break;

      case OP_ART_NUEVO :
      case OP_ART_BUSCAR :
      case OP_ART_SIG :
      case OP_ART_ANT :
      case OP_ART_INI :
      case OP_ART_FIN :
      case OP_ART_LIST_ALF :
      case OP_ART_LIST_COD :
      case OP_ART_MODIFICAR :
      case OP_ART_BORRAR : articulos (opcion, imprimir);
			   break;

      case OP_FACTURA_A :
      case OP_FACTURA_B :
      case OP_FACTURA_C :
      case OP_FACTURA_BORRAR :
      case OP_FACTURACION_CLIENTES :
      case OP_FACTURACION_REMESA : facturacion (opcion, imprimir);
				   break;

      case OP_ACERCA_DE :
	ayuda ();
	break;
     }
  }

unsigned pascal crea_menus (void)
/* Crea la lista de menฃs para el programa */
  {unsigned menu, popup, popup2;

   menu = crea_menu ();

   popup = crea_menu_popup ();
   agrega_menu (popup, OP_CALCU, "&Calculadora");
   agrega_separador (popup);
   agrega_menu (popup, OP_ACERCA_DE, "&Acerca de...");
   agrega_submenu (menu, popup, "&#");

   popup = crea_menu_popup ();
   agrega_menu (popup, OP_IMPRIMIR, "&Imprimir");
   agrega_separador (popup);
   agrega_menu (popup, OP_SALIR, "&Salir");
   agrega_submenu (menu, popup, "&Archivos");

   popup = crea_menu_popup ();
   agrega_menu (popup, OP_CLIENTE_NUEVO, "&Nuevo cliente");
   agrega_menu (popup, OP_CLIENTE_BUSCAR, "&Buscar cliente");
   agrega_menu (popup, OP_CLIENTE_SIG, "&Siguiente           S");
   agrega_menu (popup, OP_CLIENTE_ANT, "&Anterior            A");
   agrega_menu (popup, OP_CLIENTE_INI, "&Primer cliente      P");
   agrega_menu (popup, OP_CLIENTE_FIN, "&Ultimo cliente      U");
   agrega_menu (popup, OP_CLIENTE_MODIFICAR, "&Modificar");
   agrega_menu (popup, OP_CLIENTE_BORRAR, "B&orrar");
   agrega_separador (popup);
   popup2 = crea_menu_popup ();
   agrega_menu (popup2, OP_CLIENTE_LIST_ALF, "&Listado alfabtico");
   agrega_menu (popup2, OP_CLIENTE_LIST_COD, "Listado por &cขdigo");
   agrega_submenu (popup, popup2, "&Listado");
   agrega_submenu (menu, popup, "&Clientes");

   popup = crea_menu_popup ();
   agrega_menu (popup, OP_ART_NUEVO, "&Nuevo artกculo");
   agrega_menu (popup, OP_ART_BUSCAR, "&Buscar artกculo");
   agrega_menu (popup, OP_ART_SIG, "&Siguiente           S");
   agrega_menu (popup, OP_ART_ANT, "&Anterior            A");
   agrega_menu (popup, OP_ART_INI, "&Primer artกculo     P");
   agrega_menu (popup, OP_ART_FIN, "&Ultimo artกculo     U");
   agrega_menu (popup, OP_ART_MODIFICAR, "&Modificar");
   agrega_menu (popup, OP_ART_BORRAR, "B&orrar");
   agrega_separador (popup);
   popup2 = crea_menu_popup ();
   agrega_menu (popup2, OP_ART_LIST_ALF, "&Listado alfabtico");
   agrega_menu (popup2, OP_ART_LIST_COD, "Listado por &cขdigo");
   agrega_submenu (popup, popup2, "&Listado");
   agrega_submenu (menu, popup, "&Artกculos");

   popup = crea_menu_popup ();
   agrega_menu (popup, OP_FACTURA_A, "Factura &completa");
   agrega_menu (popup, OP_FACTURA_B, "Factura &servicios");
   agrega_menu (popup, OP_FACTURA_C, "Factura s&in");
   agrega_separador (popup);
   agrega_menu (popup, OP_FACTURA_BORRAR, "&Borrar factura");
   agrega_separador (popup);
   agrega_menu (popup, OP_FACTURACION_CLIENTES, "Facturaciขn por c&lientes");
   agrega_menu (popup, OP_FACTURACION_REMESA, "Facturaciขn por &remesa");
   agrega_submenu (menu, popup, "&Facturaciขn");

   popup = crea_menu_popup ();
   agrega_menu (popup, OP_CONF_MON, "&Opciones de facturas");
   agrega_menu (popup, OP_CONF_IMPR, "&Configuraciขn impresora");
   agrega_menu (popup, OP_CONF_VARIAS, "Opciones &varias");
   agrega_separador (popup);
   agrega_menu (popup, OP_CONF_SALVAR, "&Salvar configuraciขn");
   agrega_submenu (menu, popup, "C&onfiguraciขn");

   pone_menu_actual (menu);

   return menu;
  } // crea_menus

void main (void)
  {unsigned opcion;
   int accion;
   unsigned menu;

   // Inicializa la configuraciขn del programa y otras cosas
   inicializa_programa (LClientes.nombre_fichero, LArticulos.nombre_fichero,
			Facturas_nombre_fichero, Facturas_ile, &iva,
			&nro_factura, directorio_trabajo);
   inicializa_menu();
   menu = crea_menus();
   if (menu != ERROR)
     {inicializa_facturas();
      inicializa_listas();
      dibuja_menu();
      // Bucle principal.
      do
	{pone_barra_inferior (menu_prin);
	 accion = escoge_opcion (&opcion);
	 if (accion == _ESCOGE)
	   realiza_opcion (opcion);
	}
      // Se repite hasta escoger la opciขn de salir
      while (opcion != OP_SALIR);
      oculta_menu ();
      borra_menu (menu);
     }
   libera_menu();
   restaura_ordenador (LClientes.nombre_fichero, LArticulos.nombre_fichero,
                       Facturas_nombre_fichero, Facturas_ile, iva,
                       nro_factura, directorio_trabajo);
  }

#endif
