/***************************************************************************
 * Programa de facturaci¢n                                                 *
 ***************************************************************************
 * Fecha creaci¢n : 5/4/96                                                 *
 * Fecha modificaci¢n : 16/7/96                                            *
 * (c)1996 Luis Fernando Santocildes Romero                                *
 ***************************************************************************
 * V1.0 : Programa de facturaci¢n                                          *
 ***************************************************************************/

/*********** Protecci¢n contra una compilaci¢n incorrecta ******************/
#if defined (_Windows)
  #error Este programa es para MS-DOS.
#endif
#ifndef __MSDOS__
  #error Este programa es para MS-DOS.
#endif

#ifdef __LARGE__
/*********** Ficheros de cabecera necesarios *******************************/
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <dir.h>
#include "defines.h"
#include "tipos.h"
#include "boolean.h"  // Define el tipo booleano y sus constantes
#include "control.h"  // Define los procedimientos para un sencillo control
		      // de la pantalla
#include "fichas.h"   // Define las clases para manejar bases de datos
#include "fact.h"     // Funciones para manejar la facturaci¢n. Va por
		      // separado porque las funciones de fichas no sirven
		      // para esto.

#define BUSQUEDA_CANCELADA 60000
/*************************P R O T O T I P O S*****************************/
int pascal hace_factura_tipo_1 (void);
int pascal hace_factura_tipo_2 (void);
int pascal hace_factura_tipo_3 (void);
int pascal factura (int nro_apartado, unsigned *fila_inicial);
/**********************CONSTANTES Y VARIABLES GLOBALES*********************/

Tcampo def_clientes[9] = {{0, "C¢digo", 4, 0, 0, ENTERO, 2, 1},
			  {0, "Nombre", 40, 0, 0, CADENA, 2, 3},
			  {0, "Direcci¢n", 40, 0, 0, CADENA, 2, 5},
			  {0, "Pueblo", 20, 0, 0, CADENA, 2, 7},
			  {0, "C.P.", 5, 0, 0, CADENA, 41, 7},
			  {0, "Provincia", 10, 0, 0, CADENA, 2, 9},
			  {0, "DNI o CIF", 10, 0, 0, CADENA, 2, 11},
			  {0, "Telefono 1", 10, 0, 0, CADENA, 31, 11},
			  {0, "Telefono 2", 10, 0, 0, CADENA, 31, 13}},
// Definici¢n de las fichas de los clientes

       def_articulo[7] = {{0, "C¢digo", 4, 0, 0, ENTERO, 2, 1},
			  {0, "Descripci¢n", 40, 0, 0, CADENA, 2, 3},
			  {0, "N§ rgtro", 10, 0, 0, CADENA, 2, 5},
			  {0, "P.S.", 4, 0, 0, CADENA, 2, 7},
			  {0, "Toxicidad", 1, 0, 0, CADENA, 2, 9},
			  {0, "Precio", 10, 0, 0, REAL, 2, 11},
			  {0, "Precio 2", 10, 0, 0, REAL | OCULTO, 2, 13}};
// Definici¢n de las fichas de los art¡culos

// Definici¢n para las facturas
Tapart apart_1[2] = {{"Concepto", 60, 0, 0, CADENA, 1},
		     {"Importe", 10, 0, 0, REAL | CALCULA_TOTAL | ESP_EXTRA, 62}},

       apart_2[7] = {{"Productos", 20, 0, 0, CADENA | COPIA_DATOS, 1},
		     {"N§ Rgtro.", 10, 0, 0, CADENA, -1},
		     //{"N§ Lote", 10, 0, 0, CADENA, -1},
		     {"Dosis", 6, 0, 0, REAL, -1},
		     {"P.S.", 6, 0, 0, CADENA, -1},
		     {"Kg/l", 6, 0, 0, REAL | CALCULA_TOTAL, -1},
		     {"Precio", 10, 0, 0, REAL | CALCULA_TOTAL, -1},
		     {"Importe", 10, 0, 0, REAL | CALCULA_TOTAL | ESP_EXTRA, -1}};

/* Indica que campos hay que multiplicar (En este caso el campo 4 de la factura
   [Kg/litro] se multiplica por el campo 5 [Precio] y se coloca el resultado
   en el campo 6 [Importe] */
Tmultiplica multiplica = {4, 5, 6};

/* En algunos casos hay que extraer datos de las fichas de los art¡culos, en
   este caso son : - 4            : Cantidad de campos a traspasar
		   - {1, 2}       : Campos de referencia por los que buscar
		   - {2, 3, 4, 6} : Lista de campos de origen (en la ficha)
		   - {0, 1, 3, 5} : Lista de campos de destino (en la factura)
		   - {"C:", "D:"} : Lista de cadenas clave para las b£squeda
		   - {"art¡culo"} : Cadena indicativa de lo que hay que buscar
   traspaso1 se usa para las facturas de tipo A, y traspaso2 para las del
   tipo C (sin IVA)
*/
Tsaca_datos traspaso1 = {4, {1, 2}, {2, 3, 4, 6}, {0, 1, 3, 5},
			{"C:", "D:"}, "art¡culo"},
	    traspaso2 = {4, {1, 2}, {2, 3, 4, 7}, {0, 1, 3, 5},
			{"C:", "D:"}, "art¡culo"};

// Contenido de los diferentes men£s
char menu_principal[] = "  &-   &Clientes   &Art¡culos   &Facturas   "
			"C&onfiguraci¢n   ­&Salir!  ";
char menu_clientes[] = "  &Nuevo cliente   &B£squeda   Listado &alfab‚tico   "
		       "Listado por &codigo  ";
char menu_clientes_buscar[] = "  &Buscar cliente   &Siguiente   &Anterior   "
			      "&Modificar datos   B&orrar cliente #9";
char menu_articulos[] = "  &Nuevo art¡culo   &B£squeda   Listado &alfab‚tico"
			"   Listado por &c¢digo  ";
char menu_articulos_buscar[] = "  &Buscar art¡culo   &Siguiente   &Anterior   "
			      "&Modificar datos   B&orrar art¡culo #9 #@";
char menu_facturas[] = "  &Hacer factura   Facturaci¢n por &cliente   "
		       "Facturaci¢n por &remesa #3 #B";
char menu_hacer_facturas[] = "  Factura &A   Factura &B #3";
char menu_configuracion [] = "  &Datos monetarios   &Impresora   &Varios   "
			     "&Salvar configuraci¢n";

Cbase_datos LClientes(def_clientes, 9),
	    LArticulos(def_articulo, 7);
// Listas de clientes y art¡culos

Cfactura apartado_1 (apart_1, 2, 5, 10),
	 apartado_2 (apart_2, 7, 7, 23),
	 apartado_3 (apart_2, 7, 7, 23);
// Reserva de memoria y definici¢n del tipo de facturas

long numero_factura = 0;
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

   nro_campos = lista.hay_campos();  // Lee el n£mero de campos.
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
   cputs ("\n\n\r Escoja el campo de b£squeda : ");
   memset (aux1, 0, 10);
   contador = 0;
   _setcursortype (_NORMALCURSOR);
   // Leemos el n§ del campo de b£squeda.

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
   // Leemos los datos del campo por el que se va a realizar la b£squeda

   sprintf (aux2, " Escoja el %s a buscar y pulse &ENTER&, &ESC& cancela.",
	    aux1);
   pone_barra_inferior (aux2);
   sprintf (aux2, " %s a buscar :", aux1);
   pregunta (aux2, 255, 255, 50, 10, aux1, contador);
   // Pedimos el dato a buscar.
   if (*aux1 == 0)
   // Si no se ha introducido ning£n dato, y ya se hab¡a realizado una
   // b£squeda anteriormente, se busca el mismo dato
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
   // Si se encuentra nos ubicamos sobre ella, sino salimos de la funci¢n
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
     // Si se indica un dato se busca este por una b£squeda binaria
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

void pascal clientes (void)
// Submen£ de clientes
  {int opcion;   // Opci¢n del submen£
   char barra[80], *auxc; // Variables temporales y auxiliares
   Tpantalla temp;
   unsigned aux,
	    ficha_inicio,
	    ficha_fin;
   TFtemporal *salida;

   do
     {pone_barra_inferior (" Clientes ³ ");
      pone_barra (menu_clientes);
      // Prepara la pantalla
      opcion = escoge_opcion(menu_clientes);
      switch (opcion)
	{case 0 : // A¤ade un nuevo cliente a la lista de clientes
	    pone_barra_inferior (" Nuevo cliente ³ &CTRL-ENTER& a¤ade el "
				 "cliente, &ESC& cancela ");
	    if (LClientes.nueva_ficha() == MEMORIA_AGOTADA)
	    // A¤ade una nueva ficha a la lista de clientes. Si no hay espacio
	    // disponible muestra un mensaje de error.
	      {sonido_error();
	       mensaje ("No queda sitio para m s clientes en la lista.\r\n"
			" Grabe los datos del fichero y cambie el nombre del\r\n"
			" fichero de clientes en el men£ de configuraci¢n.",
			255, 255, 54, 9);
	       return;
	      }
	    sprintf (barra, " Cliente n£mero : %u ", LClientes.hay_fichas());
	    temp = hace_recuadro (255, 255, 60, 16, barra);
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
	    restaura_recuadro (&temp);
	    break;
	    // Crea una nueva ficha y pide los datos de esta.

	 case 1 : // Visualiza la lista de clientes.
	    pone_barra (menu_clientes_buscar);
	    pone_barra_inferior (" Clientes ³ &CTRL-I& imprime la ficha actual");
	    // Prepara la pantalla
	    if (!LClientes.hay_fichas())
	      {sonido_error();
	       mensaje ("La lista de clientes\r\n est  vac¡a.", 255, 255, 25,
			7);
	       break;
	      } // Si no hay fichas se muestra un mensaje de error
	    do // Hay fichas, entramos en el submen£
	      {pone_barra_inferior (" Lista clientes ³ &CTRL-I& imprime la ficha actual");
	       sprintf (barra, " Cliente n£mero : %u ",
			LClientes.nro_ficha_actual);
	       temp = hace_recuadro (255, 255, 60, 15, barra);
	       LClientes.imprimir_ficha(PANTALLA);
	       // Prepara la pantalla y muestra la ficha actual
	       opcion = escoge_opcion (menu_clientes_buscar);
	       switch (opcion)
		 {case 0 : // Busca un cliente seg£n un campo de las fichas
			   pone_barra_inferior (" Busca clientes ³ &ESC&"
			   " vuelve al men£");
			   aux = busca (LClientes, "clientes");
			   if (aux == OK)
			   // Si se encuentra el cliente se muestra la ficha
			     {restaura_recuadro (&temp);
			      sprintf (barra, " Cliente n£mero : %u ",
				       LClientes.nro_ficha_actual + 1);
			      temp = hace_recuadro (255, 255, 60, 15, barra);
			     }
			   else if (aux == FICHA_INEXISTENTE)
			   // sino se muestra un mensaje de error
				  {sonido_error ();
				   mensaje ("El cliente no se encuentra en la"
					    "\r\n lista.", 255, 255, 40, 7);
				  }
			   break;

		  case 1 : // Pasa al siguiente cliente de la lista
			   if (LClientes.sig_ficha() == ULTIMA_FICHA)
			     sonido_error();
			   break;

		  case 2 : // Pasa al cliente anterior de la lista
			   if (LClientes.ant_ficha() == PRIMER_FICHA)
			     sonido_error();
			   break;
		  case 3 : // Modifica la ficha actual
			   pone_barra_inferior (" Edita cliente ³ &CTRL-ENTER& acepta, &ESC& "
			   "cancela");
			   LClientes.rellenar_ficha (FALSE);
			   break;
		  case 4 : // Borra la ficha actual de la lista
			   sprintf (barra, " Borrar  el cliente n§ %d \r\n "
				    " ¨Seguro?", LClientes.nro_ficha_actual);
		      if (aviso (barra, 255, 255, 35, 8) == SI)
			LClientes.borrar_ficha (LClientes.nro_ficha_actual);
			   break;
		  case 5 : // Imprime la ficha actual
			   LClientes.imprimir_ficha (IMPRESORA);
			   break;
		 }
	       restaura_recuadro (&temp);
	      }
	    while (opcion != -1);
	    opcion = 0; // Se cambia el n§ de opci¢n para no salirse del
			// submen£ de b£squeda.
	    break;

	 case 2 : // Muestra el listado alfab‚tico de los clientes
		  {unsigned contador = 0,
			    nro_lineas = 2;
		   Tpantalla aux;

		   pone_barra_inferior (" Clientes ³ Listado alfab‚tico");
		   temp = hace_recuadro (255, 2, 69, 22, " Listado alfab‚tico "
					 "de clientes ");
		   aux = hace_recuadro (255, 255, 26, 7);
		   cputs ("\n   Ordenando clientes\r\n\n   "
			  "Espere un momento...");
		   LClientes.ordenar_fichas(1);
			// Ordena las fichas alfabeticamente
		   restaura_recuadro (&aux);
		   inicio_fin (&ficha_inicio, &ficha_fin, 2, LClientes);
			// Selecciona las fichas inicial y final del listado
		   if (ficha_inicio == ERROR)
		     {restaura_recuadro (&temp);
		      break;
		     }
		   cputs ("   C¢digo ³ Nombre                                   ³ "
			  "Tel‚fono\r\n"
		   "  ÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄ"
		   "ÄÄÄÄÄÄÄÄÄÄÄ");
		   contador = ficha_inicio;
		   while (contador <= ficha_fin)
		   // Recorre el ¡ndice de fichas ordenadas, imprimiendo los
		   // datos de estas
		     {LClientes.ficha_numero (LClientes.lista_ordenada
						[contador]);
		      cprintf ("\r\n    %4d  ³ %-40.40s ³ %10.10s",
			       *(unsigned*)LClientes.leer_dato (1),
			       LClientes.leer_dato (2),
			       LClientes.leer_dato (8));
		      if (nro_lineas == 20)
		      // Si se llega hasta el final de la pantalla se espera
		      // la pulsaci¢n de una tecla
			{pone_barra_inferior (" Pulse una tecla para continuar...");
			 getch();
			 pone_barra_inferior (" Clientes ³ Listado alfab‚tico");
			 nro_lineas = 0;
			}
		      contador++;
		      nro_lineas++;
		     }
		   pone_barra_inferior (imp_lista);
		   if (getch() == 9)
		   /* Avisa de que se puede imprimir el listado, en el caso de
		      que se pulse la tecla adecuada se repite el proceso pero
		      enviando los datos a la impresora */
		     {salida = inicializa_impresion();
		      envia_datos (
		      "\r\n        Listado alfab‚tico de clientes \r\n\n"
		      "    C¢digo | Nombre                                   | "
		      "Tel‚fono\r\n"
		      "   --------+------------------------------------------+-"
		      "-----------\r\n", salida);
		      contador = ficha_inicio;
		      nro_lineas = 6;
		      while (contador <= ficha_fin)
			{LClientes.ficha_numero (LClientes.lista_ordenada
						   [contador]);
			 sprintf (barra, "     %4d  | %-40.40s | %10.10s\r\n",
				  *(unsigned*)LClientes.leer_dato (1),
				  LClientes.leer_dato (2),
				  LClientes.leer_dato (8));
			 envia_datos (barra, salida);
			 if (nro_lineas == lineas_papel - 4)
			   {fputc (12, salida->fichero);
			    nro_lineas = 0;
			   }
			 contador++;
			 nro_lineas++;
			}
		      imprime (salida, TRUE, 1);
		     }
		   restaura_recuadro (&temp);
		   break;
		  }

	 case 3 : // Muestra el listado por c¢digos de los clientes
		  /* El proceso es muy similar al de mostrar el listado
		     alfab‚tico de los clientes, solo que en lugar de ordenar
		     las fichas seg£n el nombre, lo hace seg£n el c¢digo */
		  {unsigned contador = 0,
			    nro_lineas = 2;
		   Tpantalla aux;

		   pone_barra_inferior (" Clientes ³ Listado por c¢digos");
		   temp = hace_recuadro (255, 2, 69, 22, " Listado por c¢digos de "
					 "clientes ");
		   aux = hace_recuadro (255, 255, 26, 7);
		   cputs ("\n   Ordenando clientes\r\n\n   "
			  "Espere un momento...");
		   LClientes.ordenar_fichas(0);
		   restaura_recuadro (&aux);
		   inicio_fin (&ficha_inicio, &ficha_fin, 1, LClientes);
		   if (ficha_inicio == ERROR)
		     {restaura_recuadro (&temp);
		      break;
		     }
		   contador = ficha_inicio;
		   cputs ("   C¢digo ³ Nombre                                   ³ "
			  "Tel‚fono\r\n"
		   "  ÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄ"
		   "ÄÄÄÄÄÄÄÄÄÄÄ");
		   while (contador <= ficha_fin)
		     {LClientes.ficha_numero (LClientes.lista_ordenada
						[contador]);
		      cprintf ("\r\n    %4d  ³ %-40.40s ³ %10.10s",
			       *(unsigned*)LClientes.leer_dato (1),
			       LClientes.leer_dato (2),
			       LClientes.leer_dato (8));
		      if (nro_lineas == 20)
			{pone_barra_inferior (" Pulse una tecla para continuar...");
			 getch();
			 pone_barra_inferior (" Clientes ³ Listado por c¢digos");
			 nro_lineas = 0;
			}
		      contador++;
		      nro_lineas++;
		     }
		   pone_barra_inferior (imp_lista);
		   if (getch() == 9)
		     {salida = inicializa_impresion();
		      envia_datos ("\r\n        Listado por c¢digos de clientes \r\n\n"
		      "    C¢digo | Nombre                                   | "
		      "Tel‚fono\r\n"
		      "   --------+------------------------------------------+-"
		      "-----------\r\n", salida);
		      contador = ficha_inicio;
		      nro_lineas = 3;
		      while (contador <= ficha_fin)
			{LClientes.ficha_numero (LClientes.lista_ordenada
						   [contador]);
			 sprintf (barra, "     %4d  | %-40.40s | %10.10s\r\n",
				  *(unsigned*)LClientes.leer_dato (1),
				  LClientes.leer_dato (2),
				  LClientes.leer_dato (8));
			 envia_datos (barra, salida);
			 if (nro_lineas == lineas_papel - 4)
			   {fputc (12, salida->fichero);
			    nro_lineas = 0;
			   }
			 contador++;
			 nro_lineas++;
			}
		      imprime (salida, TRUE, 1);
		     }
		   restaura_recuadro (&temp);
		   break;
		  }

	}
     }
   while (opcion != -1);
  } // clientes

void pascal articulos (void)
// Submen£ de art¡culos
  {int opcion;
   char barra[80], *auxc;
   Tpantalla temp;
   unsigned aux, ficha_inicio, ficha_fin;
   TFtemporal *salida;

   do                                 // Bucle principal
     {pone_barra_inferior (" Art¡culos ³ ");
      pone_barra (menu_articulos);
      opcion = escoge_opcion(menu_articulos);
      switch (opcion)
	{case 0 : // A¤ade un nuevo art¡culo a la lista
	    pone_barra_inferior (" Nuevo art¡culo ³ &CTRL-ENTER& a¤ade el "
				 "art¡culo, &ESC& cancela ");
	    if (LArticulos.nueva_ficha() == MEMORIA_AGOTADA)
	    // A¤ade una nueva ficha a la lista de art¡culos. Si no hay espacio
	    // disponible muestra un mensaje de error.
	      {sonido_error();
	       mensaje ("No queda sitio para m s art¡culos en la lista.\r\n"
			" Grabe los datos del fichero y cambie el nombre del\r\n"
			" fichero de art¡culos en el men£ de configuraci¢n.",
			255, 255, 60, 9);
	       return;
	      }
	    sprintf (barra, " Art¡culo n£mero : %u ", LArticulos.hay_fichas());
	    temp = hace_recuadro (255, 255, 60, 13, barra);
	    if (LArticulos.rellenar_ficha(TRUE) == OK)
	    // Despu‚s de completar la ficha de art¡culos se verifica que este
	    // no existiera ya en la lista
	      {auxc = (char *)LArticulos.leer_dato (2);
	       aux = LArticulos.buscar_ficha (auxc, 2);
	       if (aux + 1 != LArticulos.nro_ficha_actual)
	       // Si existe se muestra un mensaje de error y se sale
		 {sonido_error ();
		  mensaje ("El art¡culo ya existe", 255, 255, 30, 7);
		  LArticulos.borrar_ficha (LArticulos.nro_ficha_actual);
		  LArticulos.modificado = FALSE;
		 }
	      }
	    restaura_recuadro (&temp);
	    break;
	    // Rellena una nueva ficha

	 case 1 : // Muestra y modifica la lista de art¡culos
	    pone_barra (menu_articulos_buscar);
	    if (!LArticulos.hay_fichas())
	    // Si no hay art¡culos se muestra un mensaje de error
	      {sonido_error();
	       mensaje ("La lista de art¡culos\r\n est  vac¡a.", 255, 255, 25,
			7);
	       break;
	      }
	    do // Sino se muestra la ficha actual
	      {pone_barra_inferior (" Art¡culos ³ &CTRL-I& imprime la ficha actual");
	       sprintf (barra, " Art¡culo n£mero : %u ", LArticulos.nro_ficha_actual);
	       temp = hace_recuadro (255, 255, 60, 13, barra);
	       LArticulos.imprimir_ficha(PANTALLA);
	       opcion = escoge_opcion (menu_articulos_buscar);
	       switch (opcion)
		 {case 0 : // Busca un art¡culo seg£n un campo de las fichas
			   pone_barra_inferior (" Busca un art¡culo ³ &ESC&"
			   " vuelve al men£");
			   aux = busca (LArticulos, "art¡culos");
			   if (aux == OK)
			     {restaura_recuadro (&temp);
			      sprintf (barra, " Art¡culo n£mero : %u ",
				       LArticulos.nro_ficha_actual + 1);
			      temp = hace_recuadro (255, 255, 60, 13, barra);
			     }
			   else if (aux == FICHA_INEXISTENTE)
				  {sonido_error ();
				   mensaje ("El art¡culo no se encuentra en la"
					    "lista", 255, 255, 40, 5);
				  }
			   break;

		  case 1 : // Pasa al siguiente art¡culo de la lista
			   if (LArticulos.sig_ficha() == ULTIMA_FICHA)
			     sonido_error();
			   break;

		  case 2 : // Pasa al art¡culo anterior de la lista
			   if (LArticulos.ant_ficha() == PRIMER_FICHA)
			     sonido_error();
			   break;
		  case 3 : // Modifica la ficha actual
			   pone_barra_inferior (" Edita la ficha ³ &CTRL-ENTER& acepta,"
						" &ESC& cancela");
			   LArticulos.rellenar_ficha (FALSE);
			   break;
		  case 4 : // Borra la ficha actual de la lista
			   sprintf (barra, " Borrar  el art¡culo n§ %d \r\n "
				    " ¨Seguro?", LArticulos.nro_ficha_actual);
		      if (aviso (barra, 255, 255, 35, 8) == SI)
			LArticulos.borrar_ficha (LArticulos.nro_ficha_actual);
			   break;
		  case 5 : // Imprime el art¡culo actual
			   LArticulos.imprimir_ficha (IMPRESORA);
			   break;

		  case 6 : // Modifica el 2§ precio
			   {double precio2;
			    char cadena[80];

			    precio2 = *(double*)LArticulos.leer_dato (7);
			    sprintf (cadena, "2§ precio actual : %.2f\r\n"
				     "\n Nuevo precio :", precio2);
			    if (pregunta (cadena, 255, 255, 40, 8, barra, 12,
					  TAL_CUAL) == TRUE)
			       {LArticulos.escribir_dato (barra, 7);
				LArticulos.modificado = TRUE;
			       }
			    break;
			   }
		 }
	       restaura_recuadro (&temp);
	      }
	    while (opcion != -1);
	    opcion = 0;
	    break;

	 case 2 : // Muestra el listado por nombre de los art¡culos
		// El proceso es similar al del listado alfab‚tico de clientes
		  {unsigned contador = 0,
			    nro_lineas = 2;
		   Tpantalla aux;

		   pone_barra_inferior (" Art¡culos ³ Listado alfab‚tico");
		   temp = hace_recuadro (255, 2, 79, 22, " Listado alfab‚tico de "
					 "art¡culos ");
		   aux = hace_recuadro (255, 255, 26, 7);
		   cputs ("\n   Ordenando art¡culos\r\n\n   "
			  "Espere un momento...");
		   LArticulos.ordenar_fichas(1);
		   restaura_recuadro (&aux);
		   inicio_fin (&ficha_inicio, &ficha_fin, 2, LArticulos);
		   if (ficha_inicio == ERROR)
		     {restaura_recuadro (&temp);
		      break;
		     }
		   contador = ficha_inicio;
		   cputs ("  C¢digo ³ Descripci¢n                              ³ "
			  " N§ Lote   ³ Precio\r\n");
		   cputs (" ÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄ"
			  "ÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄ");
		   while (contador <= ficha_fin)
		     {LArticulos.ficha_numero (LArticulos.lista_ordenada
						[contador]);
		      cprintf ("\r\n   %4d  ³ %-40.40s ³ %-10.10s ³ %10.2f",
			       *(unsigned*)LArticulos.leer_dato (1),
			       LArticulos.leer_dato (2),
			       LArticulos.leer_dato (3),
			       *(double*)LArticulos.leer_dato (6));
		      if (nro_lineas == 20)
			{pone_barra_inferior (" Pulse una tecla para continuar...");
			 getch();
			 pone_barra_inferior (" Art¡culos ³ Listado alfab‚tico");
			 nro_lineas = 0;
			}
		      contador++;
		      nro_lineas++;
		     }
		   pone_barra_inferior (imp_lista);
		   if (getch() == 9)
		     {salida = inicializa_impresion ();
		      envia_datos ("\r\n        Listado alfab‚tico de art¡culos \r\n\n"
			     "  C¢digo | Descripci¢n                              | "
			     " N§ Lote   | Precio\r\n"
			     " --------+------------------------------------------+-"
			     "-----------+-----------\r\n", salida);
		      contador = ficha_inicio;
		      nro_lineas = 3;
		      while (contador <= ficha_fin)
			{LArticulos.ficha_numero (LArticulos.lista_ordenada
						   [contador]);
			 sprintf (barra, "   %4d  | %-40.40s | %-10.10s | %10.2f\r\n",
				  *(unsigned*)LArticulos.leer_dato (1),
				  LArticulos.leer_dato (2),
				  LArticulos.leer_dato (3),
				  *(double*)LArticulos.leer_dato (6));
			 envia_datos (barra, salida);
			 if (nro_lineas == lineas_papel - 4)
			   {fputc (12, salida->fichero);
			    nro_lineas = 0;
			   }
			 contador++;
			 nro_lineas++;
			}
		      imprime (salida);
		     }
		   restaura_recuadro (&temp);
		   break;
		  }

	 case 3 : // Muestra el listado por c¢digos de los art¡culos
		  {unsigned contador = 0,
			    nro_lineas = 2;
		   Tpantalla aux;

		   pone_barra_inferior (" Art¡culos ³ Listado por c¢digos");
		   temp = hace_recuadro (255, 2, 79, 22, " Listado por c¢digos de "
					 "art¡culos ");
		   aux = hace_recuadro (255, 255, 26, 7);
		   cputs ("\n   Ordenando art¡culos\r\n\n   "
			  "Espere un momento...");
		   LArticulos.ordenar_fichas(0);
		   restaura_recuadro (&aux);
		   inicio_fin (&ficha_inicio, &ficha_fin, 1, LArticulos);
		   if (ficha_inicio == ERROR)
		     {restaura_recuadro (&temp);
		      break;
		     }
		   cputs ("  C¢digo ³ Descripci¢n                              ³ "
			  " N§ Lote   ³ Precio\r\n");
		   cputs (" ÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄ"
			  "ÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄ");
		   while (contador <= ficha_fin)
		     {LArticulos.ficha_numero (LArticulos.lista_ordenada
						[contador]);
		      cprintf ("\r\n   %4d  ³ %-40.40s ³ %-10.10s ³ %10.2f",
			       *(unsigned*)LArticulos.leer_dato (1),
			       LArticulos.leer_dato (2),
			       LArticulos.leer_dato (3),
			       *(double*)LArticulos.leer_dato (6));
		      if (nro_lineas == 20)
			{pone_barra_inferior (" Pulse una tecla para continuar...");
			 getch();
			 pone_barra_inferior (" Art¡culos ³ Listado alfab‚tico");
			 nro_lineas = 0;
			}
		      contador++;
		      nro_lineas++;
		     }
		   pone_barra_inferior (imp_lista);
		   if (getch() == 9)
		     {salida = inicializa_impresion();
		      envia_datos ("\r\n        Listado por c¢digos de art¡culos \r\n\n"
			     "  C¢digo | Descripci¢n                              | "
			     " N§ Lote   | Precio\r\n"
			     " --------+------------------------------------------+-"
			     "-----------+-----------\r\n", salida);
		      contador = 0;
		      nro_lineas = 3;
		      while (contador <= ficha_fin)
			{LArticulos.ficha_numero (LArticulos.lista_ordenada
						   [contador]);
			 sprintf (barra, "   %4d  | %-40.40s | %-10.10s | %10.2f\r\n",
				  *(unsigned*)LArticulos.leer_dato (1),
				  LArticulos.leer_dato (2),
				  LArticulos.leer_dato (3),
				  *(double*)LArticulos.leer_dato (6));
			 envia_datos (barra, salida);
			 if (nro_lineas == lineas_papel - 4)
			   {fputc (12, salida->fichero);
			    nro_lineas = 0;
			   }
			 contador++;
			 nro_lineas++;
			}
		      imprime (salida, TRUE, 1);
		     }
		   restaura_recuadro (&temp);
		   break;
		  }
	}
     }
   while (opcion != -1);
  } // articulos

void pascal facturacion (void)
// Submen£ de facturaci¢n
  {int opcion;

   do
     {pone_barra (menu_facturas);
      pone_barra_inferior (" Facturaci¢n ³");
      opcion = escoge_opcion (menu_facturas);
      switch (opcion)
	{case 0 : // Opci¢n para hacer las facturas,
	     do
	       {desplazamiento_vertical = 0;
		pone_barra (menu_hacer_facturas);
		pone_barra_inferior (" Facturaci¢n ³ Escoja el tipo de "
				     "factura");
		opcion = escoge_opcion (menu_hacer_facturas);
		switch (opcion)
		  {case 0 : hace_factura_tipo_1 ();
			    break;

		   case 1 : hace_factura_tipo_2 ();
			    break;

		   case 2 : hace_factura_tipo_3 ();
			    break;
		  }
	       }
	     while (opcion != -1);
	     opcion = 0;
	     break;

	 case 1 : // Facturaci¢n de un determinado cliente
	 case 3 : // Pulsando CTRL-C se entra aqu¡ pero saca los datos por
		  // la impresora
	    {FILE *fichero;
	     TFtemporal *salida;
	     char aux_c[80];
	     unsigned num_ficha, num_cliente;
	     Tpantalla temp;
	     unsigned contador = 3;
	     Tdatos_fichero_facturas datos_factura;
	     date fecha, fecha_ini, fecha_fin;
	     long fecha_ini_l, fecha_fin_l;
	     struct time hora;
	     long double total = 0;

	     pone_barra_inferior (" Escriba el c¢digo del cliente y pulse"
				  " &ENTER& para continuar.");
	     pregunta ("C¢digo del cliente :", 255, 255, 25, 8, aux_c,
		       5, FALSE);
	     num_ficha = LClientes.buscar_ficha (aux_c, 1, 0);
	     // Se pide el c¢digo del cliente y se verifica que este existe
	     if (num_ficha == FICHA_INEXISTENTE)
	     // Si no existe se sale dando un mensaje de aviso
	       {sonido_error();
		mensaje ("Cliente inexistente.", 255, 255, 25, 8);
		break;
	       }
	     else
	     // Sino se selecciona su ficha
	       {num_cliente = atoi (aux_c);
		LClientes.ficha_numero (num_ficha);
		strcpy (aux_c, "Informe de facturaci¢n de ");
		strcat (aux_c, LClientes.leer_dato (2));
	       }
	     pide_fecha ("Fecha inicial :", 255, 255, 19, 6, &fecha_ini);
	     // Se pide la fecha inicial del listado
	     if (fecha_ini.da_mon == 0)
	     // Si no se indica ninguna fecha se hace un listado desde el
	     // 1-1-1970 hasta la fecha actual
	       {fecha_ini.da_day = 1;
		fecha_ini.da_mon = 1;
		fecha_ini.da_year = 1970;
		fecha_ini_l = dostounix (&fecha_ini, &hora_nula);
		getdate (&fecha_fin);
		fecha_fin_l = dostounix (&fecha_fin, &hora_nula);
	       }
	     else
	     // Sino se pide la fecha final del listado
	       {fecha_ini_l = dostounix (&fecha_ini, &hora_nula);
		pide_fecha ("Fecha final :", 255, 255, 19, 6, &fecha_fin);
		if (fecha_fin.da_mon == 0)
		  getdate (&fecha_fin);
		fecha_fin_l = dostounix (&fecha_fin, &hora_nula);
	       }
	     if (opcion == 1)
	     // Seg£n el tipo de salida escogida se env¡an los datos a la pantalla
	     // o a la impresora
	       {// Si se env¡an los datos a la pantalla se muestra una
		// cabecera peque¤a
		temp = hace_recuadro (255, 2, 79, 22, aux_c);
		cputs (" Fecha      ³ N§ Factura ³ Situaci¢n                      ³ Importe total\r\n"
		       "ÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\r\n");
	       }
	     else
	     // Sino se imprimen varios datos m s en la cabecera
	       {salida = inicializa_impresion();
		envia_datos (aux_c, salida);
		if (fecha_ini.da_year != 1970)
		  {sprintf (aux_c, "\r\nDesde %d/%s%d, hasta %d/%s/%d",
			    fecha_ini.da_day, c_mes[fecha_ini.da_mon - 1],
			    fecha_ini.da_year,fecha_fin.da_day,
			    c_mes[fecha_fin.da_mon - 1], fecha_ini.da_year);
		   envia_datos (aux_c, salida);
		  }
		else envia_datos ("\r\nInforme completo de facturaci¢n",
				  salida);
		envia_datos ("\r\n\n Fecha      | N§ Factura | Situaci¢n                      | Importe total\r\n"
		      "------------+------------+--------------------------------+--------------\r\n",
		      salida);
	       }

	     fichero = fopen (Facturas_nombre_fichero, "rb");
	     // Se abre el fichero de las facturas
	     while (TRUE) // entra en un bucle casi sin fin
	       {fread (&datos_factura, sizeof (Tdatos_fichero_facturas), 1,
		       fichero);   // Lee cada factura
		if (feof(fichero)) // Sale del bucle si llega al final del
		  break;           // fichero
		if (datos_factura.fecha < fecha_ini_l ||
		    datos_factura.fecha > fecha_fin_l ||
		    num_cliente != datos_factura.nro_cliente)
		  continue;
		// Si la fecha de la factura leida est  fuera del rango, o el
		// cliente no coincide, se pasa a leer la siguiente factura
		unixtodos (datos_factura.fecha, &fecha, &hora);
		if (opcion == 1)
		// Sino envia los datos a la pantalla o a la impresora
		  cprintf ("%2d/%s/%4d ³ %10ld ³ %-30.30s ³ %10.2f\r\n", fecha.da_day,
			   c_mes[fecha.da_mon - 1], fecha.da_year,
			   datos_factura.nro_factura, datos_factura.situacion,
			   datos_factura.total_factura);
		else
		  fprintf (salida->fichero, "%2d/%s/%4d | %10ld | %-30.30s | %10.2f\r\n",
			   fecha.da_day, c_mes[fecha.da_mon - 1], fecha.da_year,
			   datos_factura.nro_factura, datos_factura.situacion,
			   datos_factura.total_factura);
		total += datos_factura.total_factura;
		contador++;
		// Cuenta el nro. de l¡neas impresas y si llega a un tope
		// diferente si es la pantalla o la impresora, hace una pausa,
		// o envia un salto de p gina
		if (opcion == 3 && contador == lineas_papel - 3)
		  {fputc (12, salida->fichero);
		   contador = 0;
		  }
		else if (contador == 24)
		       {pone_barra_inferior (press_enter);
			getch();
			contador = 0;
			pone_barra_inferior ("");
		       }
	       }
	     // Muestra el total
	     if (opcion == 3)
	       {sprintf (aux_c, "\r\nTotal facturado : %.2Lf Pts.", total);
		envia_datos (aux_c, salida);
		imprime (salida, TRUE, 1);
	       }
	     else {pone_barra_inferior (press_tecla);
		   getch();
		  }

	     if (opcion == 1) restaura_recuadro (&temp);
	     break;
	    }

	 case 2 : // Facturaci¢n por remesas
	 case 4 :
	    {FILE *fichero;
	     TFtemporal *salida1, *salida2;
	     char aux_c[160];
	     Tpantalla temp;
	     unsigned contador = 3;
	     Tdatos_fichero_facturas datos_factura;
	     date fecha, fecha_ini, fecha_fin;
	     long fecha_ini_l, fecha_fin_l;
	     struct time hora;
	     long double total_litros = 0,
			 total = 0,
			 total_a = 0,
			 total_b = 0;

	     // Todo el proceso de facturaci¢n por remesas es muy parecido al
	     // de facturaci¢n por clientes, pero con algunas diferencias
	     strcpy (aux_c, " Informe de facturaci¢n por remesas ");
	     pide_fecha ("Fecha inicial :", 255, 255, 19, 6, &fecha_ini);
	     // Se pide la fecha inicial y final del listado de facturaci¢n
	     // siguiendo los mismo pasos que en facturaci¢n por cliente
	     if (fecha_ini.da_mon == 0)
	       {fecha_ini.da_day = 1;
		fecha_ini.da_mon = 1;
		fecha_ini.da_year = 1970;
		fecha_ini_l = dostounix (&fecha_ini, &hora_nula);
		getdate (&fecha_fin);
		fecha_fin_l = dostounix (&fecha_fin, &hora_nula);
	       }
	     else
	       {fecha_ini_l = dostounix (&fecha_ini, &hora_nula);
		pide_fecha ("Fecha final :", 255, 255, 19, 6, &fecha_fin);
		if (fecha_fin.da_mon == 0)
		  getdate (&fecha_fin);
		fecha_fin_l = dostounix (&fecha_fin, &hora_nula);
	       }
	     if (opcion == 2)
	     // Seg£n la opci¢n escogida env¡a los datos a la impresora o a la
	     // pantalla
	       { // Si es la opci¢n 2 se env¡an a la pantalla
		if (modo_132 == 0)
		// Si no se puede cambiar el modo de v¡deo se muestra un
		// mensaje de error
		  {sonido_error ();
		   mensaje ("Lo siento, no se puede mostrar el listado\r\n"
			    " porque no se puede modificar el modo de v¡deo",
			    255, 255, 50, 7);
		   break;
		  }
		else
		// sino se almacena la pantalla actual y se cambia el modo de
		// visualizaci¢n a 132 columnas y muestra la cabecera
		  {temp = guarda_recuadro (1, 1, 79, 24, SIN_MARCO);
		   reloj_visible = FALSE;
		   pone_modo_vesa (modo_132);
		   _setcursortype (_NOCURSOR);
		   clrscr_x();
		   puts ("Cod. Cli. Situaci¢n        Fecha      N§ factura  "
			 "    Litros    Pts. servicios   Pts. Productos      Ptas. Total\r\n"
			 "ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ"
			 "ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\r");
		  }
	       }
	     else // Con la opci¢n 4 se env¡an a la impresora
	     // Como el listado es muy grande se tiene que hacer en dos folios
	       {salida1 = inicializa_impresion();
		salida2 = inicializa_impresion();
		envia_datos (aux_c, salida1);
		if (fecha_ini.da_year != 1970)
		  {sprintf (aux_c, "\r\n\nDesde %d/%s%d, hasta %d/%s/%d\r\n\n",
			    fecha_ini.da_day, c_mes[fecha_ini.da_mon - 1],
			    fecha_ini.da_year,fecha_fin.da_day,
			    c_mes[fecha_fin.da_mon - 1], fecha_ini.da_year);
		   envia_datos (aux_c, salida1);
		  }
		else envia_datos ("\r\n\nInforme completo de facturaci¢n\r\n\n",
				  salida1);
		envia_datos ("Cod. Cli. Situaci¢n         Fecha      N§ factura      Litros\r", salida1);
		envia_datos ("Pts. servicios    Pts. Productos        Pts. Total\r", salida2);
		envia_datos ("________________________________________________________________\r", salida1);
		envia_datos ("___________________________________________________\r", salida2);
	       }
	     fichero = fopen (Facturas_nombre_fichero, "rb");
	     // Se abre el fichero de facturas
	     while (TRUE)  // Y se entra en el bucle de lectura
	       {fread (&datos_factura, sizeof (Tdatos_fichero_facturas), 1,
		       fichero);
		if (feof(fichero)) // Se lee cada factura hasta llegar al
		  break;           // final del fichero
		if (datos_factura.fecha < fecha_ini_l ||
		    datos_factura.fecha > fecha_fin_l)
		  continue;
		  // Si la factura no est  en el rango se lee la siguiente
		unixtodos (datos_factura.fecha, &fecha, &hora);
		if (opcion == 2)
		// Seg£n la opci¢n se envian los datos a la pantalla o a la
		// impresora
		  {if (modo_132)
		     printf ("  %4d    %-16.16s %2d/%2d/%4d %10ld %13.3f"
			     " %13.2f     %13.2f     %13.2f\r\n",
			     datos_factura.nro_cliente, datos_factura.situacion,
			     fecha.da_day, fecha.da_mon, fecha.da_year,
			     datos_factura.nro_factura, datos_factura.litros,
			     datos_factura.total_1, datos_factura.total_2,
			     datos_factura.total_factura);
		  }
		else
		  {fprintf (salida1->fichero, "\n  %4d    %-16.16s %2d/%2d/%4d %10ld %13.3f\r",
			    datos_factura.nro_cliente, datos_factura.situacion,
			    fecha.da_day, fecha.da_mon, fecha.da_year,
			    datos_factura.nro_factura, datos_factura.litros);
		   fprintf (salida2->fichero, "\n%13.2f     %13.2f     %13.2f\r",
			    datos_factura.total_1, datos_factura.total_2,
			    datos_factura.total_factura);
		  }
		total_litros += datos_factura.litros;
		total_a += datos_factura.total_1;
		total_b += datos_factura.total_2;
		total += datos_factura.total_factura;
		// Calcula los totales
		contador++;
		if (opcion == 4 && contador == lineas_papel - 3)
		  {fputc (12, salida1->fichero);
		   fputc (12, salida2->fichero);
		   contador = 0;
		  }
		else if (contador == 23 && opcion == 2)
		       {pone_barra_inferior (press_enter);
			getch();
			clrscr_x();
			puts ("Cod. Cli. Situaci¢n        Fecha      N§ factura  "
			      "    Litros    Pts. servicios   Pts. Productos      Ptas. Total\r\n"
			      "ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ"
			      "ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\r");
			contador = 3;
			pone_barra_inferior ("");
		       }
	       }
	     if (opcion == 4)
	       {sprintf (aux_c, "\r________________________________________________________________"
				"\r\nTOTAL:%42c %13.3Lf\r\n", ' ', total_litros);
		envia_datos (aux_c, salida1);
		sprintf (aux_c, "\r___________________________________________________"
				"\r\n%13.2Lf     %13.2Lf     %13.2Lf\r\n",
				total_a, total_b, total);
		envia_datos (aux_c, salida2);
		imprime (salida1, TRUE, 1);
		imprime (salida2, TRUE, 1);
	       }
	     else {printf ("\rÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ"
			   "\nTOTAL:%42c %13.3Lf %13.2Lf     %13.2Lf     %13.2Lf\r\n",
			   ' ', total_litros, total_a, total_b, total);
		   pone_barra_inferior (press_tecla);
		   getch();
		  }

	     if (opcion == 2)
	       {if (modo_132)
		  {textmode (info_pantalla.currmode);
		   _setcursortype (_NOCURSOR);
		   reloj_visible = TRUE;
		  }
		restaura_recuadro (&temp);
	       }
	     break;
	    }
	}
     }
   while (opcion != -1);
  } // facturacion



void pascal ayuda (void)
// Funci¢n de ayuda que de momento no muestra nada m s que quien ha hecho
// el programa
  {
   mensaje ("    Facturaci¢n V1.0\r\n\n"
	    "     Por Luc Hos Oft\r\n\n"
	    "      (C)1996", 255, 255, 26, 10);
  } // ayuda

void pascal configuracion (void)
// Opciones de configuraci¢n del progama
  {int opcion;
   char tecla, aux[80], nombre_fichero[13];
   Tpantalla temp;

   pone_barra (menu_configuracion);
   do
     {pone_barra_inferior (" Configuraci¢n ³");
      opcion = escoge_opcion (menu_configuracion);
      switch (opcion)
	{case 0 : // Configuraci¢n de opciones monetarias, de momento solo
		  // se modifica el valor del IVA
	     pone_barra_inferior (" Introduce el valor del IVA y pulsa"
				  " &ENTER&, o &ESC& para cancelar.");
	     sprintf (aux, "I.V.A. actual : %.2f %\r\n\n Nuevo I.V.A. :",
		      iva_1);
	     pregunta (aux, 255, 255, 35, 8, aux, 10, TAL_CUAL);
	     if (*aux != 0)
	       {iva_1 = atoi (aux);
		configuracion_modificada = TRUE;
	       }
	     break;

	 case 1 : // Configuraci¢n de la impresora.
	     textattr (YELLOW | (BLUE<<4));
	     pone_barra_inferior (esc_opcion);
	     do
	       {temp = hace_recuadro (255, 255, 40, 11, " Opciones de impresora ");
		cprintf ("\r\n 1) N§ de copias : %d\r\n\n"
			 " 2) L¡neas del papel : %d\r\n\n"
			 " 3) Puerto de conexi¢n : %s\r\n\n\n"
			 " Escoja una opci¢n : ", nro_copias, lineas_papel,
			 puertos[puerto_conexion]);
		_setcursortype (_NORMALCURSOR);
		tecla = getch();
		switch (tecla)
		  {case '1' : sprintf (aux, "N§ de copias : %d\r\n\n"
				       " Nuevo n£mero :", nro_copias);
			      pregunta (aux, 255, 255, 35, 8, aux, 10, FALSE);
			      if (*aux)
				{nro_copias = atoi (aux);
				 configuracion_modificada = TRUE;
				}
			      break;

		   case '2' : sprintf (aux, "L¡neas del papel : %d\r\n\n"
				       " Nuevo n£mero :", lineas_papel);
			      pregunta (aux, 255, 255, 35, 8, aux, 10, FALSE);
			      if (*aux)
				{lineas_papel = atoi (aux);
				 configuracion_modificada = TRUE;
				}
			      break;

		   case '3' : int f;

			      restaura_recuadro (&temp);
			      temp = hace_recuadro (255, 255, 30, 14,
						    " Puerto de conexi¢n ");
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

	 case 2 : // Configuraci¢n de opciones varias. Nombres de ficheros,
		  // sonido, etc.
	     textattr (YELLOW | (BLUE<<4));
	     temp = hace_recuadro (255, 255, 50, 13, " Opciones Varias ");
	     do
	       {pone_barra_inferior (esc_opcion);
		gotoxy (2,2);
		cprintf ("1) Fichero clientes : %s\r\n\n 2) Fichero art¡culos"
			 " : %s\r\n\n 3) Fichero facturas : %s"
			 "\r\n\n 4) Sonido de error : %s\r\n\n\n"
			 "   Escoja una opci¢n : ",
			 LClientes.nombre_fichero, LArticulos.nombre_fichero,
			 Facturas_nombre_fichero,
			 hace_sonido ? "SI" : "NO");
		_setcursortype (_NORMALCURSOR);
		tecla = getche();
		switch (tecla)
		  {case '1' : strcpy (nombre_fichero, LClientes.nombre_fichero);
			      break;
		   case '2' : strcpy (nombre_fichero, LArticulos.nombre_fichero);
			      break;
		   case '3' : strcpy (nombre_fichero, Facturas_nombre_fichero);
			      break;
		   case '4' : pone_barra_inferior (esc_sino);
			      _setcursortype (_NORMALCURSOR);
			      gotoxy (23, 8);
			      tecla = toupper(getch());
			      _setcursortype (_NOCURSOR);
			      if (tecla == 'S') hace_sonido = TRUE;
			      else if (tecla == 'N') hace_sonido = FALSE;
			      configuracion_modificada = TRUE;
			      break;
		  }
		if (tecla > '0' && tecla < '4')
		  {sprintf (aux, "El nombre actual es %s\r\n\n Escoja un nuevo"
			    " nombre :", nombre_fichero);
		   nombre_fichero[0] = 0;
		   pregunta (aux, 255, 255, 77, 9, nombre_fichero,
			     MAXPATH);
		   if (*nombre_fichero)
		     {switch (tecla)
		       {case '1' : LClientes.grabar_fichero();
				   if (existe_fichero (nombre_fichero))
				     {strcpy (LClientes.nombre_fichero, nombre_fichero);
				      LClientes.leer_fichero ();
				     }
				   break;
			case '2' : LArticulos.grabar_fichero();
				   if (existe_fichero (nombre_fichero))
				     {strcpy (LArticulos.nombre_fichero, nombre_fichero);
				      LArticulos.leer_fichero ();
				     }
				   break;
			case '3' : if (existe_fichero (nombre_fichero))
				     strcpy (Facturas_nombre_fichero, nombre_fichero);
				   break;
		       }
		      configuracion_modificada = TRUE;
		     }
		  }
	       }
	     while (tecla != ESC);
	     _setcursortype (_NOCURSOR);
	     restaura_recuadro (&temp);
	     break;

	 case 3 : // Almacena la configuraci¢n del programa
	   {FILE *fichero;
	    if (configuracion_modificada)
	      {strcpy (aux, directorio_programa);
	       strcat (aux, "factura.cfg");
	       fichero = fopen (aux, "wb");
	       fwrite (CLAVE_FICH, strlen (CLAVE_FICH), 1, fichero);
	       fwrite (&hace_sonido, sizeof(boolean), 1, fichero);
	       fwrite (&iva_1, sizeof(float), 1, fichero);
	       fwrite (LClientes.nombre_fichero, MAXPATH, 1, fichero);
	       fwrite (LArticulos.nombre_fichero, MAXPATH, 1, fichero);
	       fwrite (Facturas_nombre_fichero, MAXPATH, 1, fichero);
	       fwrite (Facturas_ile, MAXPATH, 1, fichero);
	       fwrite (&nro_copias, sizeof(int), 1, fichero);
	       fwrite (&lineas_papel, sizeof(int), 1, fichero);
	       fwrite (&puerto_conexion, sizeof(int), 1, fichero);

	       fclose (fichero);
	       configuracion_modificada = FALSE;
	      }
	    break;
	   }
	}
     }
   while (opcion != -1);
  } // configuraci¢n

void pascal inicializa_listas (void)
// Inicializa las listas de clientes, art¡culos y facturas
  {
   if (LClientes.leer_fichero() != OK)
     {sonido_error();
      mensaje ("No se ha podido leer el fichero de \r\n clientes\r\n\n"
	       " Se creara una lista de clientes vacia.", 255, 255, 43, 9);
     }
  if (LArticulos.leer_fichero() != OK)
     {sonido_error();
      mensaje ("No se ha podido leer el fichero de \r\n art¡culos\r\n\n"
	       " Se crear  una lista de art¡culos vacia.", 255, 255, 43, 9);
     }
  // Se intenta leer los datos de cada lista desde el fichero, si no se
  // puede leer de los ficheros se inicializan las listas sin datos.
  } // inicializa_listas

void pascal imprime_cabecera (unsigned destino, Tdatos_factura *datos,
			      TFtemporal *f_impresion = NULL)
// Imprime los datos de cabecera de la factura. Estos pueden ir tanto a la
// pantalla como a la impresora (a trav‚s de un fichero temporal)
  {unsigned aux;
   char temp[120];
   date fecha;
   struct time hora;

   unixtodos (datos->fecha, &fecha, &hora);
   if (destino == PANTALLA)
     {
      itoa (datos->nro_cliente, temp, 10);
      aux = LClientes.buscar_ficha (temp, 1);
      LClientes.ficha_numero (aux);

      sprintf (temp, "Cliente : %s",(char *)LClientes.leer_dato (2));
      imprime_cadena (temp, 1, 0);

      sprintf (temp, "N.I.F. : %s", (char *)LClientes.leer_dato (7));
      imprime_cadena (temp, 52, 0);

      sprintf (temp, "Domicilio : %s-%s-%s", (char *)LClientes.leer_dato(3),
					     (char *)LClientes.leer_dato(5),
					     (char *)LClientes.leer_dato(4));
      imprime_cadena (temp, 1, 2);

      sprintf (temp, "Cultivo / Plaga : %s", datos->cultivo);
      imprime_cadena (temp, 1, 4);

      sprintf (temp, "Situaci¢n : %s", datos->situacion);
      imprime_cadena (temp, 52, 4);

      sprintf (temp, "Fecha tratamiento : %d/%s/%d", fecha.da_day,
	       c_mes[fecha.da_mon - 1], fecha.da_year);
      imprime_cadena (temp, 1, 6);

      sprintf (temp, "Superficie : %f mı", datos->superficie);
      imprime_cadena (temp, 34, 6);

      sprintf (temp, "Litros : %f l.", datos->litros);
      imprime_cadena (temp, 56, 6);
     }
   else
     {itoa (datos->nro_cliente, temp, 10);
      aux = LClientes.buscar_ficha (temp, 1);
      LClientes.ficha_numero (aux);

      sprintf (temp, "  Cliente : %s",(char *)LClientes.leer_dato (2));
      completa_cadena (temp, 55);
      envia_datos (temp, f_impresion);

      sprintf (temp, " N.I.F. : %s", (char *)LClientes.leer_dato (7));
      completa_cadena (temp, 21, ' ', TRUE);
      envia_datos (temp, f_impresion);

      sprintf (temp, "\r\n  Domicilio : %s-%s-%s",
					     (char *)LClientes.leer_dato(3),
					     (char *)LClientes.leer_dato(5),
					     (char *)LClientes.leer_dato(4));
      completa_cadena (temp, 80, ' ', TRUE);
      envia_datos (temp, f_impresion);

      sprintf (temp, "\r\n  Cultivo / Plaga : %s", datos->cultivo);
      completa_cadena (temp, 45);
      envia_datos (temp, f_impresion);

      sprintf (temp, " Situaci¢n : %s", datos->situacion);
      completa_cadena (temp, 30, ' ', TRUE);
      envia_datos (temp, f_impresion);

      sprintf (temp, "\r\n  Fecha tratamiento : %d/%s/%d", fecha.da_day,
	       c_mes[fecha.da_mon - 1], fecha.da_year);
      completa_cadena (temp, 36);
      envia_datos (temp, f_impresion);

      sprintf (temp, " Superficie : %.2f mı", datos->superficie);
      completa_cadena (temp, 23);
      envia_datos (temp, f_impresion);

      sprintf (temp, " Litros : %.2f l.", datos->litros);
      completa_cadena (temp, 15, ' ', TRUE);
      envia_datos (temp, f_impresion);
     }
  } // imprime_cabecera

Tdatos_factura * pascal selecciona_cliente (Cbase_datos &lista_clientes)
// Selecciona un cliente de la lista de clientes
  {date fecha;
   char cadena[80], aux[80];
   Tdatos_factura *datos;
   unsigned nro_ficha;

   pregunta ("¨ C¢digo del cliente ?", 255, 255, 50, 10, cadena, 5, FALSE);
   // Pide el c¢digo del cliente y verifica que este exista en la lista
   if (*cadena)
     {nro_ficha = lista_clientes.buscar_ficha (cadena, 1);
      if (nro_ficha == FICHA_INEXISTENTE)
    // Si no existe se mouestra un mensaje indic ndolo y se sale de la funci¢n
	{sprintf (aux, "El cliente %s no est  en la lista.", cadena);
	 mensaje (aux, 255, 255, 30, 8);
	 return NULL;
	}
      else
      // Sino se piden los datos necesarios para la factura
	{lista_clientes.ficha_numero (nro_ficha);
	 datos = new Tdatos_factura;
	 datos->nro_cliente = *(unsigned *)lista_clientes.leer_dato(1);

	 gotoxy (2, 1);
	 cprintf ("Cliente : %s",(char *)lista_clientes.leer_dato (2));

	 gotoxy (53, 1);
	 cprintf ("N.I.F. : %s", (char *)lista_clientes.leer_dato (7));

	 gotoxy (2, 3);
	 cprintf ("Domicilio : %s-%s-%s", (char *)lista_clientes.leer_dato(3),
					  (char *)lista_clientes.leer_dato(5),
					  (char *)lista_clientes.leer_dato(4));

	 pregunta ("Cultivo / Plaga : ", 255, 255, 50, 7, cadena, 30);
	 gotoxy (2, 5);
	 cprintf ("Cultivo / Plaga : %s", cadena);
	 strcpy (datos->cultivo, cadena);

	 pregunta ("Situaci¢n : ", 255, 255, 50, 7, cadena, 30);
	 gotoxy (53, 5);
	 cprintf ("Situaci¢n : %s", cadena);
	 strcpy (datos->situacion, cadena);

	 pide_fecha ("Fecha tratamiento : ", 255, 255, 30, 7, &fecha);
	 gotoxy (2, 7);
	 if (fecha.da_mon == 0)
	   getdate (&fecha);
	 cprintf ("Fecha tratamiento : %d/%s/%d", fecha.da_day,
		  c_mes[fecha.da_mon - 1], fecha.da_year);
	 datos->fecha = dostounix (&fecha, &hora_nula);

	 pregunta ("Superficie : ", 255, 255, 30, 7, cadena, 10);
	 gotoxy (35, 7);
	 cprintf ("Superficie : %s mı", cadena);
	 datos->superficie = atof (cadena);

	 pregunta ("Litros : ", 255, 255, 30, 7, cadena, 10);
	 gotoxy (57, 7);
	 cprintf ("Litros : %s l", cadena);
	 datos->litros = atof (cadena);
	 return datos;
	}
     }
   return NULL;
  } // Selecciona cliente

void imprime_encabezamiento (unsigned long numero_factura, TFtemporal *f_impresion)
// Imprime el contenido de la variable cabecera_facturas en la impresora
  {if (cabecera_facturas != NULL)
     fprintf (f_impresion->fichero, cabecera_facturas, numero_factura);
  } // imprime_encabezamiento

void pascal imprime_factura_1 (void)
// Imprime los datos de la factura de tipo 1 (o A)
  {double base, total;
   unsigned aux, aux2;
   char cadena[120], temp[120];
   TFtemporal *salida;

   salida = inicializa_impresion ();
   imprime_encabezamiento (apartado_1.nro_factura, salida);
   imprime_fecha (salida);
   imprime_cabecera (IMPRESORA, apartado_1.datos_cliente, salida);
   fputs ("\r\n", salida->fichero);
   apartado_1.imprime_factura_impresora(salida);
   fputs ("\r\r\n", salida->fichero);
   apartado_2.imprime_factura_impresora(salida);

   base = apartado_1.total + apartado_2.total;
   total = base + base * iva_1 / 100.0;

   aux2 = apartado_2.nro_apartados;
   for (aux = 2; aux <= apartado_2.definicion[aux2].pos_x; aux++)
     fputc (' ', salida->fichero);
   envia_datos ("+------------+\r\n", salida);

   memset (temp, ' ', 56);
   temp[52] = 0;
   sprintf (cadena, "Total A ... |%12.2f|\r\n", apartado_1.total);
   strcat (temp, cadena);
   envia_datos (temp, salida);

   memset (temp, ' ', 90);
   temp[52] = 0;
   sprintf (cadena, "Total B ... |%12.2f|\r\n", apartado_2.total);
   strcat (temp, cadena);
   gotoxy_text (temp, "Enterado y conforme", 10);
   envia_datos (temp, salida);

   memset (temp, ' ', 90);
   temp[45] = 0;
   sprintf (cadena, "Base imponible ... |%12.2f|\r\n", base);
   strcat (temp, cadena);
   envia_datos (temp, salida);

   memset (temp, ' ', 90);
   temp[48] = 0;
   sprintf (cadena, "IVA al %.2f ... |%12.2f|\r\n", iva_1, base * iva_1 / 100);
   strcat (temp, cadena);
   envia_datos (temp, salida);

   memset (temp, ' ', 90);
   temp[54] = 0;
   sprintf (cadena, "Total ... |%12.2f|\r\n", total);
   strcat (temp, cadena);
   envia_datos (temp, salida);

   for (aux = 2; aux <= apartado_2.definicion[aux2].pos_x; aux++)
     fputc (' ', salida->fichero);
   envia_datos ("+------------+\r\n", salida);

   imprime (salida);
  } // imprime_factura_1

void pascal imprime_factura_2 (void)
// Idem pero con la de tipo 2
  {double total;
   unsigned aux, aux2;
   char cadena[120], temp[120];
   TFtemporal *salida;

   salida = inicializa_impresion ();
   imprime_encabezamiento (apartado_1.nro_factura, salida);
   imprime_fecha (salida);
   imprime_cabecera (IMPRESORA, apartado_1.datos_cliente, salida);
   envia_datos ("\r\n", salida);
   apartado_1.imprime_factura_impresora (salida);

   total = apartado_1.total + apartado_1.total * iva_1 / 100.0;

   aux2 = apartado_1.nro_apartados;
   for (aux = 2; aux <= apartado_1.definicion[aux2].pos_x; aux++)
     fputc (' ', salida->fichero);
   envia_datos ("+------------+\r\n", salida);

   memset (temp, ' ', 90);
   temp[42] = 0;
   sprintf (cadena, "Base imponible ... |%12.2f|\r\n", apartado_1.total);
   strcat (temp, cadena);
   gotoxy_text (temp, "Enterado y conforme", 10);
   envia_datos (temp, salida);

   memset (temp, ' ', 90);
   temp[44] = 0;
   sprintf (cadena, "IVA al %.2f ... |%12.2f|\r\n", iva_1, apartado_1.total *
						    iva_1 / 100);
   strcat (temp, cadena);
   envia_datos (temp, salida);

   memset (temp, ' ', 90);
   temp[51] = 0;
   sprintf (cadena, "Total ... |%12.2f|\r\n", total);
   strcat (temp, cadena);
   envia_datos (temp, salida);

   for (aux = 2; aux <= apartado_1.definicion[aux2].pos_x; aux++)
     fputc (' ', salida->fichero);
   envia_datos ("+------------+\r\n", salida);

   imprime (salida);
  } // imprime_factura_2

void pascal imprime_factura_3 (void)
// Idem pero con la de tipo 3
  {double total;
   unsigned aux, aux2;
   char cadena[120], temp[120];
   TFtemporal *salida;

   salida = inicializa_impresion ();
   imprime_encabezamiento (apartado_1.nro_factura, salida);
   imprime_fecha (salida);
   imprime_cabecera (IMPRESORA, apartado_1.datos_cliente, salida);
   envia_datos ("\r\n", salida);
   apartado_1.imprime_factura_impresora(salida);
   envia_datos ("\r\r\n", salida);
   apartado_3.imprime_factura_impresora(salida);
   total = apartado_1.total + apartado_3.total;

   aux2 = apartado_3.nro_apartados;
   for (aux = 2; aux <= apartado_3.definicion[aux2].pos_x; aux++)
     fputc (' ', salida->fichero);
   envia_datos ("+------------+\r\n", salida);

   memset (temp, ' ', 56);
   temp[52] = 0;
   sprintf (cadena, "Total A ... |%12.2f|\r\n", apartado_1.total);
   strcat (temp, cadena);
   envia_datos (temp, salida);

   memset (temp, ' ', 90);
   temp[52] = 0;
   sprintf (cadena, "Total B ... |%12.2f|\r\n", apartado_3.total);
   strcat (temp, cadena);
   gotoxy_text (temp, "Enterado y conforme", 10);
   envia_datos (temp, salida);

   memset (temp, ' ', 90);
   temp[54] = 0;
   sprintf (cadena, "Total ... |%12.2f|\r\n", total);
   strcat (temp, cadena);
   envia_datos (temp, salida);

   for (aux = 2; aux <= apartado_3.definicion[aux2].pos_x; aux++)
     fputc (' ', salida->fichero);
   envia_datos ("+------------+\r\n", salida);

   imprime (salida);
  } // imprime_tipo_3

void pascal calcula_datos_extra_1 (void)
// Calcula los totales del tipo de factura 1
  {double base, total;
   unsigned aux;
   char cadena[50];

   base = apartado_1.total + apartado_2.total;
   total = base + base * iva_1 / 100.0;
   aux = apartado_2.pos_y + apartado_2.nro_lineas + 5;
   pone_caracter ('Ú', 64, aux, WHITE | (BLUE << 4));
   pone_caracter ('¿', 78, aux, WHITE | (BLUE << 4));
   pone_caracter ('À', 64, aux + 10, WHITE | (BLUE << 4));
   pone_caracter ('Ù', 78, aux + 10, WHITE | (BLUE << 4));
   for (unsigned aux2 = 0; aux2 < 12; aux2 += 2)
     linea_horz (65, aux + aux2, 13, WHITE | (BLUE << 4));
   linea_vert (64, aux + 1, 9, WHITE | (BLUE << 4));
   linea_vert (78, aux + 1, 9, WHITE | (BLUE << 4));
   imprime_cadena ("Total A ...", 52, aux + 1);
   imprime_cadena ("Total B ...", 52, aux + 3);
   imprime_cadena ("Base imponible ...", 45, aux + 5);
   sprintf (cadena, "IVA al %.2f ...", iva_1);
   imprime_cadena (cadena, 48, aux + 7);
   imprime_cadena ("Total ...", 54, aux + 9);
   aux++;
   gotoxy (66, aux + 1 - desplazamiento_vertical);
   cprintf ("%12.2f", apartado_1.total);
   gotoxy (66, aux + 3 - desplazamiento_vertical);
   cprintf ("%12.2f", apartado_2.total);
   gotoxy (66, aux + 5 - desplazamiento_vertical);
   cprintf ("%12.2f", base);
   gotoxy (66, aux + 7 - desplazamiento_vertical);
   cprintf ("%12.2f", base * iva_1 / 100);
   gotoxy (66, aux + 9 - desplazamiento_vertical);
   cprintf ("%12.2f", total);
  } // calcula_datos_extra_1

void pascal calcula_datos_extra_2 (void)
// Idem pero del tipo 2
  {double base, iva;
   unsigned aux;
   char cadena[80];

   base = apartado_1.total;
   iva = base * iva_1 / 100.0;
   aux = apartado_1.pos_y + apartado_1.nro_lineas + 5;
   pone_caracter ('Ú', 61, aux, WHITE | (BLUE << 4));
   pone_caracter ('¿', 74, aux, WHITE | (BLUE << 4));
   pone_caracter ('À', 61, aux + 4, WHITE | (BLUE << 4));
   pone_caracter ('Ù', 74, aux + 4, WHITE | (BLUE << 4));
   for (unsigned aux2 = 0; aux2 < 6; aux2 += 2)
     linea_horz (62, aux + aux2, 12, WHITE | (BLUE << 4));
   linea_vert (61, aux + 1, 3, WHITE | (BLUE << 4));
   linea_vert (74, aux + 1, 3, WHITE | (BLUE << 4));
   sprintf (cadena, "IVA al %.2f ...", iva_1);
   imprime_cadena (cadena, 45, aux + 1);
   imprime_cadena ("Total ...", 51, aux + 3);
   aux++;
   gotoxy (63, aux + 1 - desplazamiento_vertical);
   cprintf ("%12.2f", iva);
   gotoxy (63, aux + 3 - desplazamiento_vertical);
   cprintf ("%12.2f", base + iva);
  } // calcula_datos_extra_2

void pascal calcula_datos_extra_3 (void)
// Idem pero del tipo 3
  {double total;
   unsigned aux;

   total = apartado_1.total + apartado_3.total;
   aux = apartado_3.pos_y + apartado_3.nro_lineas + 5;
   pone_caracter ('Ú', 64, aux, WHITE | (BLUE << 4));
   pone_caracter ('¿', 76, aux, WHITE | (BLUE << 4));
   pone_caracter ('À', 64, aux + 6, WHITE | (BLUE << 4));
   pone_caracter ('Ù', 76, aux + 6, WHITE | (BLUE << 4));
   for (unsigned aux2 = 0; aux2 < 8; aux2 += 2)
     linea_horz (65, aux + aux2, 12, WHITE | (BLUE << 4));
   linea_vert (64, aux + 1, 5, WHITE | (BLUE << 4));
   linea_vert (77, aux + 1, 5, WHITE | (BLUE << 4));
   imprime_cadena ("Total A ...", 52, aux + 1);
   imprime_cadena ("Total B ...", 52, aux + 3);
   imprime_cadena ("Total ...", 53, aux + 5);
   aux++;
   gotoxy (66, aux + 1 - desplazamiento_vertical);
   cprintf ("%12.2f", apartado_1.total);
   gotoxy (66, aux + 3 - desplazamiento_vertical);
   cprintf ("%12.2f", apartado_3.total);
   gotoxy (66, aux + 5 - desplazamiento_vertical);
   cprintf ("%12.2f", total);
  } // calcula_datos_extras_3

void pascal guarda_factura_1 (void)
// Almacena los datos de la factura de tipo 1
  {FILE *fichero;
   double base, total;
   Tdatos_fichero_facturas datos_fichero;

   base = apartado_1.total + apartado_2.total;
   total = base + base * iva_1 / 100.0;
   datos_fichero.tipo_factura = 1;
   datos_fichero.nro_cliente = apartado_1.datos_cliente->nro_cliente;
   strcpy (datos_fichero.situacion, apartado_1.datos_cliente->situacion);
   datos_fichero.fecha = apartado_1.fecha;
   datos_fichero.nro_factura = apartado_1.nro_factura;
   datos_fichero.litros = apartado_1.datos_cliente->litros;
   datos_fichero.total_1 = apartado_1.total;
   datos_fichero.total_2 = apartado_2.total;
   datos_fichero.total_factura = total;
   fichero = fopen (Facturas_nombre_fichero, "ab");
   fwrite (&datos_fichero, sizeof (Tdatos_fichero_facturas), 1, fichero);
   fclose (fichero);
  } // guarda_Factura_1

void pascal guarda_factura_2 (void)
// Almacena los datos de la factura de tipo 2
  {FILE *fichero;
   double base, total;
   Tdatos_fichero_facturas datos_fichero;

   base = apartado_1.total;
   total = base + base * iva_1 / 100.0;
   datos_fichero.tipo_factura = 2;
   datos_fichero.nro_cliente = apartado_1.datos_cliente->nro_cliente;
   strcpy (datos_fichero.situacion, apartado_1.datos_cliente->situacion);
   datos_fichero.fecha = apartado_1.fecha;
   datos_fichero.nro_factura = apartado_1.nro_factura;
   datos_fichero.litros = apartado_1.datos_cliente->litros;
   datos_fichero.total_1 = apartado_1.total;
   datos_fichero.total_factura = total;
   fichero = fopen (Facturas_nombre_fichero, "ab");
   fwrite (&datos_fichero, sizeof (Tdatos_fichero_facturas), 1, fichero);
   fclose (fichero);
  } // guarda_factura_2

void pascal guarda_factura_3 (void)
// Almacena los datos de la factura de tipo 3
  {FILE *fichero;
   double total;
   Tdatos_fichero_facturas datos_fichero;

   total = apartado_1.total + apartado_3.total;

   datos_fichero.tipo_factura = 3;
   datos_fichero.nro_cliente = apartado_1.datos_cliente->nro_cliente;
   strcpy (datos_fichero.situacion, apartado_1.datos_cliente->situacion);
   datos_fichero.fecha = apartado_1.fecha;
   datos_fichero.nro_factura = apartado_1.nro_factura;
   datos_fichero.litros = apartado_1.datos_cliente->litros;
   datos_fichero.total_1 = apartado_1.total;
   datos_fichero.total_2 = apartado_3.total;
   datos_fichero.total_factura = total;
   fichero = fopen (Facturas_ile, "ab");
   fwrite (&datos_fichero, sizeof (Tdatos_fichero_facturas), 1, fichero);
   fclose (fichero);
  } // guarda_factura_3

int pascal hace_factura_tipo_1 (void)
// Controla el proceso de las factura de tipo 1
  {Tpantalla temp1;
   unsigned que_apartado, accion;

   textcolor (YELLOW);
   textbackground (BLUE);

   temp1 = guarda_recuadro (1, 1, 79, 23, SIN_MARCO);
   clrscr();
   // Almacena la pantalla y la borra

   if (apartado_1.hay_memoria() != TRUE ||
       apartado_2.hay_memoria() != TRUE)
   // Verifica que haya memoria, y si no la hay sale mostrando un error
     {sonido_error ();
      mensaje ("Memoria insuficiente para facturar", 255, 255, 40, 7);
      restaura_recuadro (&temp1);
      return MEMORIA_AGOTADA;
     }

   apartado_1.limpia_factura();
   apartado_2.limpia_factura();
   // Limpia el contenido de la factura

   apartado_1.nro_factura = ultima_factura + 1; // Nro. de la nueva factura
   pone_barra_inferior (" Facturaci¢n ³ Factura tipo A");
   apartado_1.datos_cliente = selecciona_cliente (LClientes);
   // Selecciona el cliente
   if (apartado_1.datos_cliente == NULL)
   // Si no se ha seleccionado, o no existe, se sale
     {restaura_recuadro (&temp1);
      return OK;
     }

   apartado_1.imprime_plantilla ();
   apartado_2.imprime_plantilla ();
   // Imprime la plantilla de las facturas

   que_apartado = 0;
   _setcursortype (_NORMALCURSOR);
   do // Entra en el bucle principal
     {accion = que_apartado ? apartado_2.rellena_factura(LArticulos,
							 &multiplica,
							 &traspaso1) :
			      apartado_1.rellena_factura(LArticulos,
							 NULL,
							 NULL);
      // seg£n que apartado est‚ seleccionado, se rellena el apartado de
      // concepto o el de productos
      if (accion == CAMBIO)
      // Si se pasa de un apartado a otro, se mueve la pantalla hacia arriba
      // o hacia abajo el nro de l¡neas necesarias
	{que_apartado = 1 - que_apartado;
	 if (que_apartado)
	   {for (int i = 0; i < 22; i++)
	     scrollarriba ();
	    desplazamiento_vertical += 22;
	    calcula_datos_extra_1();
	   }
	 else
	   {for (int i = 22; i > 0; i--)
	     scrollabajo ();
	    desplazamiento_vertical -= 22;
	    imprime_cabecera (PANTALLA, apartado_1.datos_cliente);
	   }
	}
     }
   while (accion != LF && accion != ESC);
   // Se repite hasta que se pulsa ESC o CTRL-ENTER
   if (accion == LF)
   // Si se sale con CTRL-ENTER se almacena la factura y se espera a que se
   // pulse la tecla para imprimirla
     {reloj_visible = FALSE;
      pone_barra_inferior (imp_fact);
      apartado_1.imprime_factura ();
      apartado_2.imprime_factura ();
      calcula_datos_extra_1 ();
      accion = getch();
      if (accion != 27)
	{if (accion == 9)
	   imprime_factura_1();
	 guarda_factura_1();
	 ultima_factura++;
	}
      reloj_visible = TRUE;
     }
   _setcursortype (_NOCURSOR);
   desplazamiento_vertical = 0;
   restaura_recuadro (&temp1);
   if (apartado_1.datos_cliente) delete apartado_1.datos_cliente;

   return OK;
  } // hace_factura_tipo_1

int pascal hace_factura_tipo_2 (void)
// Controla el proceso de las factura de tipo 2. Este es similar al de las
// facturas de tipo 1 solo que sin el apartado de productos
  {Tpantalla temp1;
   unsigned accion;

   textcolor (YELLOW);
   textbackground (BLUE);

   temp1 = guarda_recuadro (1, 1, 79, 23, SIN_MARCO);
   clrscr();

   if (apartado_1.hay_memoria() != TRUE)
     {sonido_error ();
      mensaje ("Memoria insuficiente para facturar", 255, 255, 40, 7);
      restaura_recuadro (&temp1);
      return MEMORIA_AGOTADA;
     }

   apartado_1.limpia_factura();
   apartado_1.nro_factura = ultima_factura + 1;
   pone_barra_inferior (" Facturaci¢n ³ Factura tipo B");
   apartado_1.datos_cliente = selecciona_cliente (LClientes);
   if (apartado_1.datos_cliente == NULL)
     {restaura_recuadro (&temp1);
      return OK;
     }

   apartado_1.imprime_plantilla ();

   _setcursortype (_NORMALCURSOR);
   do
     {accion = apartado_1.rellena_factura(LArticulos, NULL, NULL);
     }
   while (accion != LF && accion != ESC);
   calcula_datos_extra_2();
   if (accion == LF)
     {reloj_visible = FALSE;
      pone_barra_inferior (imp_fact);
      accion = getch();
      if (accion != 27)
	{if (accion == 9)
	   imprime_factura_2();
	 guarda_factura_2();
	 ultima_factura++;
	}
      reloj_visible = TRUE;
     }
   _setcursortype (_NOCURSOR);
   desplazamiento_vertical = 0;
   restaura_recuadro (&temp1);
   if (apartado_1.datos_cliente) delete apartado_1.datos_cliente;

   return OK;
  } // hace_factura_tipo_2

int pascal hace_factura_tipo_3 (void)
// Controla el proceso de las factura de tipo 3. Similar al de tipo 1, solo
// que aqu¡ no se usa nro de factura, ni IVA
  {Tpantalla temp1;
   unsigned accion,
	    que_apartado;

   textcolor (YELLOW);
   textbackground (BLUE);

   temp1 = guarda_recuadro (1, 1, 79, 23, SIN_MARCO);
   clrscr();

   if (apartado_1.hay_memoria() != TRUE ||
       apartado_3.hay_memoria() != TRUE)
     {sonido_error ();
      mensaje ("Memoria insuficiente para facturar", 255, 255, 40, 7);
      restaura_recuadro (&temp1);
      return MEMORIA_AGOTADA;
     }

   apartado_1.limpia_factura();
   apartado_3.limpia_factura();
   pone_barra_inferior (" Facturaci¢n ³ Factura tipo C");
   apartado_1.datos_cliente = selecciona_cliente (LClientes);
   if (apartado_1.datos_cliente == NULL)
     {restaura_recuadro (&temp1);
      return OK;
     }

   apartado_1.imprime_plantilla ();
   apartado_3.imprime_plantilla ();

   que_apartado = 0;
   _setcursortype (_NORMALCURSOR);
   do
     {accion = que_apartado ? apartado_3.rellena_factura(LArticulos,
							 &multiplica,
							 &traspaso2) :
			      apartado_1.rellena_factura(LArticulos,
							 NULL,
							 NULL);
      if (accion == CAMBIO)
	{que_apartado = 1 - que_apartado;
	 if (que_apartado)
	   {for (int i = 0; i < 22; i++)
	     scrollarriba ();
	    desplazamiento_vertical += 22;
	    calcula_datos_extra_3();
	   }
	 else
	   {for (int i = 22; i > 0; i--)
	     scrollabajo ();
	    desplazamiento_vertical -= 22;
	    imprime_cabecera (PANTALLA, apartado_1.datos_cliente);
	   }
	}
     }
   while (accion != LF && accion != ESC);
   if (accion == LF)
     {apartado_1.imprime_factura ();
      apartado_3.imprime_factura ();
      calcula_datos_extra_3 ();
      reloj_visible = FALSE;
      pone_barra_inferior (imp_fact);
      accion = getch();
      if (accion != 27)
	{if (accion == 9)
	   imprime_factura_3();
	 guarda_factura_3();
	 ultima_factura++;
	}
      reloj_visible = TRUE;
     }
   _setcursortype (_NOCURSOR);
   desplazamiento_vertical = 0;
   restaura_recuadro (&temp1);
   if (apartado_1.datos_cliente) delete apartado_1.datos_cliente;

   return OK;
  } // hace_factura_tipo_3

void main (void)
  {int opcion;

   // Inicializa la configuraci¢n del programa y otras cosas
   inicializa_programa (LClientes.nombre_fichero, LArticulos.nombre_fichero,
			Facturas_nombre_fichero, Facturas_ile, iva_1,
			ultima_factura);
   pone_barra (menu_principal);
   // Inicializa el contenido del programa
   inicializa_listas();
   // Bucle principal.
   do
     {pone_barra (menu_principal);
      pone_barra_inferior (" Men£ principal ³ Escoja pulsando la "
			   "letra remarcada");
      opcion = escoge_opcion(menu_principal);
      switch (opcion) // Escoge una opci¢n del men£ principal
	{case 0 : ayuda();
		  break;
	 case 1 : clientes();
		  break;
	 case 2 : articulos();
		  break;
	 case 3 : facturacion();
		  break;
	 case 4 : configuracion();
		  break;
	 case 5 :
	 case -1: if (aviso ("¨ Desea salir del programa ?", 255, 255, 31, 8)
		      == NO)
		    opcion = 6;
		  break;
	}
     }
   // Se repite hasta pulsar escape o hasta escoger la opci¢n de salir
   while (opcion != 5 && opcion != -1);
   /* Verifica que se hayan almacenado los datos. Si no se han modificado
      pregunta si se desean almacenar, y almacena la lista que se haya
      modificado. */
   if (LClientes.modificado || LArticulos.modificado)
     if (aviso ("No se han almacenado los datos\r\n ¨Almacenar?",
		  255, 255, 40, 8) == SI)
       {if (LClientes.modificado)
	  LClientes.grabar_fichero ();
	if (LArticulos.modificado)
	  LArticulos.grabar_fichero ();
       }
   // Restaura la pantalla a como estaba antes de usar el programa
   restaura_ordenador (LClientes.nombre_fichero, LArticulos.nombre_fichero,
		       Facturas_nombre_fichero, Facturas_ile, iva_1);
  }

#else
  #include <stdio.h>
  printf ("Este programa necesita ser compilado en el formato de memoria LARGE.\r\n"
	  "Si trabaja con el IDE de Borland escoja el formato correcto en el men£\r\n"
	  "Options|Compiler|Code Genetarion.\r\r\n");
#endif
