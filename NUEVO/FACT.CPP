/****************************************************************************
 * FACT.CPP                                                                 *
 * Fichero contiene las estructuras y los objetos necesarios                *
 * para crear una factura, y manejarla. Permite definir los campos de la    *
 * factura, aunque no es tan flexible como el m¢dulo de fichas.             *
 ****************************************************************************
 * Fecha realizaci¢n : 14/7/96                                              *
 * Fecha modificaci¢n : 12/5/97                                             *
 *                                                                          *
 * (C)1996-97 Luis Fernando Santocildes                                     *
 ****************************************************************************/

// Ficheros de cabecera necesarios
#include <dir.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <stdlib.h>
#include <values.h>
#include <math.h>
#include <ctype.h>
#include "defines.h"
#include "tipos.h"
#include "boolean.h"
#include "fichas.h"
#include "control.h"

#define MAX_SERV 2
#define MAX_PROD 7
#define POS_Y1 10
#define POS_Y2 21
/*************************ESTRUCTURAS Y OBJETOS******************************/
typedef struct
// Estructura usada para definir un campo de la factura
  {char nombre[30]; // Es el que se muestra al imprimir la factura
   unsigned longitud, // Longitud del campo en pantalla
            longt,    // Longitud del campo en memoria
            offset,   // Offset del campo dentro de la factura
            tipo_dato;// Tipo de datos del campo
   int pos_x; // Posici¢n del campo en pantalla
  } Tapart;

typedef struct
  {char concepto[64];
   double importe;
   boolean usado;
  } Tservicios;

typedef struct
  {char producto[41],
        nro_registro[11];
   double dosis;
   char ps[5];
   double kg_l,
          precio,
          importe;
   boolean usado;
  } Tproductos;

/************************** Variables globales ******************************/
Tpantalla pant_aux;

char Facturas_nombre_fichero [MAXPATH],  // Fichero para las facturas
     Facturas_ile [MAXPATH];             // Fichero para las facturas sin IVA

Tservicios servicios[5];

Tproductos productos[8];

unsigned long nro_factura;

Tapart apart_1[MAX_SERV] = {{"Concepto", 63, 0, 0, CADENA, 1},
		     {"Importe", 10, 0, 0, REAL | CALCULA_TOTAL | ESP_EXTRA, -1}},

       apart_2[MAX_PROD] = {{"Productos", 20, 0, 0, CADENA | COPIA_DATOS, 1},
		     {"N§ Rgtro.", 10, 0, 0, CADENA, -1},
		     {"Dosis", 6, 0, 0, REAL, -1},
		     {"P.S.", 6, 0, 0, CADENA, -1},
		     {"Kg/l", 6, 0, 0, REAL | CALCULA_TOTAL, -1},
		     {"Precio", 10, 0, 0, REAL | CALCULA_TOTAL, -1},
		     {"Importe", 10, 0, 0, REAL | CALCULA_TOTAL | ESP_EXTRA, -1}};

double iva;

/***************************** Prototipos ***********************************/
void inicializa_facturas (Tapart *def_factura, unsigned apartados);
void inicializa_facturas (void);
void pascal limpia_facturas (void);
Tdatos_factura * pascal selecciona_cliente (Cbase_datos &lista_clientes);
void pascal imprime_plantilla (Tapart *campos, unsigned nro_campos,
                               unsigned nro_lineas, unsigned pos_y);
unsigned pascal imprime_impresora_1 (TFtemporal *f_salida, unsigned lineas,
                                     double total);
unsigned pascal imprime_impresora_2 (TFtemporal *f_salida, unsigned lineas,
                                     double total);
void pascal muestra_factura1 (Tservicios *datos, unsigned lineas,
                              unsigned pos_y, double total);
void pascal muestra_factura2 (Tproductos *datos, unsigned lineas,
                              unsigned pos_y, double total);
unsigned pascal rellena_factura1 (double *total, int pos_y);
unsigned pascal rellena_factura2 (double *total, Cbase_datos &articulos,
                                  int pos_y, Tdatos_factura *datos_extra,
                                  double totala, double *total_t,
                                  double *base, unsigned tipo);
void pascal calcula_total_1 (double total1, double total2, double *total,
                             double *base, unsigned pos_y);
void pascal calcula_total_2 (double total1, double *total, double *base,
                             unsigned pos_y);
void pascal calcula_total_3 (double total1, double total2, double *total,
                             unsigned pos_y);
void pascal imprime_cabecera (unsigned destino, Tdatos_factura *datos,
                              Cbase_datos &clientes,
                              TFtemporal *f_impresion = NULL);
void imprime_encabezamiento (unsigned long numero_factura, TFtemporal *f_impresion);
void pascal imprime_factura_1 (Tdatos_factura *datos_cliente,
                               Cbase_datos &clientes, double total1,
                               double total2, double total, double base);
void pascal imprime_factura_2 (Tdatos_factura *datos_cliente,
                               Cbase_datos &clientes, double total1,
                               double total, double base);
void pascal imprime_factura_3 (Tdatos_factura *datos_cliente,
                               Cbase_datos &clientes, double total1,
                               double total2, double total);
void pascal guarda_factura_1 (double total1, double total2, double total,
                              Tdatos_factura *datos_cliente);
void pascal guarda_factura_2 (double total1, double total,
                              Tdatos_factura *datos_cliente);
void pascal guarda_factura_3 (double total1, double total2, double total,
                              Tdatos_factura *datos_cliente);
int pascal hace_factura_tipo_1 (Cbase_datos &clientes, Cbase_datos &articulos);
int pascal hace_factura_tipo_2 (Cbase_datos &clientes);
int pascal hace_factura_tipo_3 (Cbase_datos &clientes, Cbase_datos &articulos);
void pascal borra_factura (void);
/************************** Desarrollo de funciones *************************/
void inicializa_facturas (Tapart *def_factura, unsigned apartados)
// inicializa ciertos valores de las definiciones de las facturas
  {unsigned aux;

   // Si hay memoria se copian la definici¢n de los campos a la factura
   if (def_factura)
     {for (aux = 0; aux < apartados; aux++)
        // Para cada apartado calcula la posici¢n y longitud del mismo
        {switch (def_factura[aux].tipo_dato & ~TRAMA)
           {case CADENA : def_factura[aux].longt = def_factura[aux].longitud
                                                   + 1;
                          break;

            case ENTERO : def_factura[aux].longt = sizeof (int);
                          break;
            case REAL : def_factura[aux].longt = sizeof (double);
                        break;
           }
         if (def_factura[aux].pos_x == -1)
           def_factura[aux].pos_x = def_factura[aux-1].pos_x +
                                    def_factura[aux-1].longitud + 1;

        }

      // inicializa las variables y la memoria
     }
  } // inicializa_facturas

void inicializa_facturas (void)
  {
   inicializa_facturas (apart_1, 2);
   inicializa_facturas (apart_2, 7);
  } // inicializa_facturas

void pascal limpia_facturas (void)
// borra el contenido de los arrays de las facturas
  {
   memset (servicios, 0, sizeof(Tservicios) * 5);
   memset (productos, 0, sizeof(Tproductos) * 8);
  } // limpia_facturas

Tdatos_factura * pascal selecciona_cliente (Cbase_datos &lista_clientes)
// Selecciona un cliente de la lista de clientes
  {date fecha;
   char cadena[80], aux[80];
   Tdatos_factura *datos;
   unsigned nro_ficha;

   do
     {clrscr();
      color_ventana (colores.texto_ventana, colores.fondo_ventana,
                     colores.borde_ventana);

      // Pide el c¢digo del cliente y verifica que este exista en la lista
      if (pregunta ("¨ C¢digo del cliente ?", 255, 255, 40, 10, cadena, 5,
          FALSE) == TAL_CUAL)
        {
         lista_clientes.ordenar_fichas (0);
         pant_aux = hace_recuadro (255, 2, 69, 22, " CLIENTES ");
         nro_ficha = muestra_lista (lista_clientes, 'c', 0,
                                    lista_clientes.cantidad_fichas() - 1, 22,
                                    TRUE);
         restaura_recuadro (&pant_aux);
         if (nro_ficha == ERROR)
           return NULL;
        }
      else
        if (*cadena)
          {nro_ficha = lista_clientes.buscar_ficha (cadena, 1);
           if (nro_ficha == FICHA_INEXISTENTE)
           // Si no existe se muestra un mensaje indic ndolo y se sale
           // de la funci¢n
             {sprintf (aux, "El cliente %s no est  en la lista.", cadena);
              mensaje (aux, 255, 255, 40, 7);
              return NULL;
             }
          }
      // Si se ha seleccionado un cliente se piden los datos necesarios
      // para la factura
      lista_clientes.ficha_numero (nro_ficha);
      datos = new Tdatos_factura;
      datos->nro_cliente = *(unsigned *)lista_clientes.leer_dato(1);

      gotoxy (2, 1);
      cprintf ("Cliente : %s %s", (char *)lista_clientes.leer_dato(2),
                                  (char *)lista_clientes.leer_dato(3));

      gotoxy (53, 1);
      cprintf ("N.I.F. : %s", (char *)lista_clientes.leer_dato (8));

      gotoxy (2, 3);
      cprintf ("Domicilio : %s", (char *)lista_clientes.leer_dato(4));

      gotoxy (53, 3);
      cprintf ("%s (%s)", (char *)lista_clientes.leer_dato(5),
                          (char *)lista_clientes.leer_dato(6));

      pone_barra_inferior (" Introduzca datos sobre el cultivo y la plaga");
      pregunta ("Cultivo / Plaga : ", 255, 255, 50, 7, cadena, 30);
      gotoxy (2, 5);
      cprintf ("Cultivo / Plaga : %s", cadena);
      strcpy (datos->cultivo, cadena);

      pone_barra_inferior (" Introduzca la situaci¢n");
      pregunta ("Situaci¢n : ", 255, 255, 50, 7, cadena, 30);
      gotoxy (53, 5);
      cprintf ("Situaci¢n : %s", cadena);
      strcpy (datos->situacion, cadena);

      pone_barra_inferior (" Pulse &CTRL-ENTER& para la fecha del d¡a");
      pide_fecha ("Fecha tratamiento : ", 255, 255, 30, 7, &fecha);
      gotoxy (2, 7);
      if (fecha.da_mon == 0)
        getdate (&fecha);
      cprintf ("Fecha tratamiento : %d/%s/%d", fecha.da_day,
               c_mes[fecha.da_mon - 1], fecha.da_year);
      datos->fecha = dos2luc (&fecha);

      pone_barra_inferior (" Introduzca los litros utilizados");
      pregunta ("Litros : ", 255, 255, 30, 7, cadena, 10);
      gotoxy (53, 7);
      cprintf ("Litros : %s", cadena);
      datos->litros = atof (cadena);
     }
   while (aviso (" ¨ Los datos son correctos ?", 255, 12, 50, 7) == NO);
   return datos;
  } // Selecciona cliente

void pascal imprime_plantilla (Tapart *campos, unsigned nro_campos,
                               unsigned nro_lineas, unsigned pos_y)
// Imprime el esquema de la factura
  {unsigned aux, color, x;

   if (!campos)
     return;
   color = colores.texto_r_ventana;
   nro_campos--;

   // Recorre la lista de campos y aumenta en 3 la longitud de cada campo para
   // dejar espacio al imprimir
   for (aux = 0; aux <= nro_campos; aux++)
     if ((campos[aux].tipo_dato & ~TRAMA) == REAL)
       campos[aux].longitud += 3;

   pos_y--;
   // Imprime los nombres de cada campo
   x = campos[nro_campos].pos_x + campos[nro_campos].longitud;

   linea_horz (1, pos_y - 1, x, L_DOBLES | color);
   linea_horz (1, pos_y + 1, x, L_DOBLES | color);
   linea_horz (1, pos_y + nro_lineas + 2, x, L_DOBLES | color);
   for (aux = 0; aux <= nro_campos; aux++)
     {linea_vert (campos[aux].pos_x - 1, pos_y, nro_lineas + 2,
                  L_DOBLES | color);
     }
   linea_vert (x, pos_y, nro_lineas + 2, L_DOBLES | color);

   for (aux = 0; aux <= nro_campos; aux++)
     {imprime_cadena (campos[aux].nombre, campos[aux].pos_x, pos_y);
      if (aux == 0)
        {pone_caracter ('É', campos[aux].pos_x - 1, pos_y - 1, color);
         pone_caracter ('Ì', campos[aux].pos_x - 1, pos_y + 1, color);
         pone_caracter ('È', campos[aux].pos_x - 1, pos_y + nro_lineas + 2,
                        color);
        }
      else
        {pone_caracter ('Ë', campos[aux].pos_x - 1, pos_y - 1, color);
         pone_caracter ('Î', campos[aux].pos_x - 1, pos_y + 1, color);
         pone_caracter ('Ê', campos[aux].pos_x - 1, pos_y + nro_lineas + 2,
                        color);
        }
     }

   pone_caracter ('»', x, pos_y - 1, color);
   pone_caracter ('¹', x, pos_y + 1, color);

   // Dibuja l¡neas en los sitios necesarios

   pone_caracter ('Î', campos[nro_campos].pos_x - 1, pos_y + nro_lineas + 2,
                  color);
   pone_caracter ('¹', x, pos_y + nro_lineas + 2, color);
   pone_caracter ('È', campos[nro_campos].pos_x - 1, pos_y + nro_lineas + 4,
                  color);
   pone_caracter ('¼', x, pos_y + nro_lineas + 4, color);
   imprime_cadena ("Pts  º", campos[nro_campos].pos_x - 6,
                   pos_y + nro_lineas + 3, color);
   pone_caracter ('º', x, pos_y + nro_lineas + 3, color);
   linea_horz (campos[nro_campos].pos_x, pos_y + nro_lineas + 4,
               campos[nro_campos].longitud, L_DOBLES | color);
   pos_y++;

   // deja la longitud de los campos como estaban originalmente
   for (aux = 0; aux <= nro_campos; aux++)
     if ((campos[aux].tipo_dato & ~TRAMA) == REAL)
       campos[aux].longitud -= 3;
  } // Imprime_plantilla

unsigned pascal imprime_impresora_1 (TFtemporal *f_salida, unsigned lineas,
                                     double total)
// Saca la factura por la impresora
  {unsigned contador = 0;
   unsigned f, g;
   char cadena[120], auxc[80];


   memset (cadena, 0, 83);
   strcat (cadena, "   ");
   memset (cadena + 3, 'Í',
           apart_1[MAX_SERV-1].pos_x + apart_1[MAX_SERV-1].longitud);
   cadena[3] = 'É';
   cadena[apart_1[MAX_SERV-1].pos_x + 2] = 'Ë';
   strcat (cadena, "»\r\n");
   envia_datos (cadena, f_salida);

   // Imprime la l¡nea con los nombres de los campos
   sprintf (cadena, "   º %-62sº %-9sº\r\n",
            apart_1[0].nombre, apart_1[1].nombre);
   envia_datos (cadena, f_salida);

   memset (cadena, 0, 83);
   strcat (cadena, "   ");
   memset (cadena + 3, 'Í',
           apart_1[MAX_SERV-1].pos_x + apart_1[MAX_SERV-1].longitud);
   for (f = 1; f < MAX_SERV; f++)
     gotoxy_text (cadena, "Î", apart_1[f].pos_x + 2);
   cadena[3] = 'Ì';
   strcat (cadena, "¹\r\n");
   envia_datos (cadena, f_salida);

   // Recorre los datos de la factura, imprimiendo cada uno.
   for (f = 0; f < lineas; f++)
     {sprintf (cadena, "  %76cº\r\n", ' ');
      for (g = 0; g < MAX_SERV; g++)
        {gotoxy_text (cadena, "º", apart_1[g].pos_x + 2);
         if (servicios[f].usado)
           if (g == 0)
              gotoxy_text (cadena, servicios[f].concepto,
                           apart_1[g].pos_x + 3);
           else
             {char auxc2[80];
              sprintf (auxc2, "%%%d.0f", apart_1[g].longitud);
              sprintf (auxc, auxc2, servicios[f].importe);
              gotoxy_text (cadena, auxc, apart_1[g].pos_x + 3);
             }
        }
      envia_datos (cadena, f_salida);
      contador++;
     }

   // Imprime el subtotal del bloque actual.
   memset (cadena, ' ', 80);
   memset (cadena + 4, 'Í', apart_1[0].longitud);
   cadena[3] = 'È';
   cadena[apart_1[MAX_SERV - 1].pos_x + 2] = 0;
   strcat (cadena, "ÎÍÍÍÍÍÍÍÍÍÍ¹\r\n");
   envia_datos (cadena, f_salida);

   memset (cadena, ' ', 80);
   cadena[apart_1[MAX_SERV - 1].pos_x - 3] = 0;
   sprintf (auxc, "Pts  º%10.0fº\r\n", total);
   strcat (cadena, auxc);
   envia_datos (cadena, f_salida);

   memset (cadena, ' ', 80);
   cadena[apart_1[MAX_SERV - 1].pos_x + 2] = 0;
   strcat (cadena, "ÈÍÍÍÍÍÍÍÍÍÍ¼\r\n");
   envia_datos (cadena, f_salida);

  return OK;
  } // imprime_impresora_1

unsigned pascal imprime_impresora_2 (TFtemporal *f_salida, unsigned lineas,
                                     double total)
// Saca la factura por la impresora
  {unsigned f, g;
   char cadena[120], auxc[80], auxc2[80];

   memset (cadena, ' ', 80);
   cadena[3] = 'É';
   memset (cadena + 4, 'Í',
           apart_2[MAX_PROD-1].pos_x + apart_2[MAX_PROD-1].longitud);
   for (f = 1; f < MAX_PROD; f++)
     gotoxy_text (cadena, "Ë", apart_2[f].pos_x + 2);
   cadena[78] = 0;
   strcat (cadena, "»\r\n");
   envia_datos (cadena, f_salida);

   memset (cadena, ' ', 80);
   // Imprime la l¡nea con los nombres de los campos
   for (f = 0; f < MAX_PROD; f++)
     {gotoxy_text (cadena, "º", apart_2[f].pos_x + 2);
      gotoxy_text (cadena, apart_2[f].nombre, apart_2[f].pos_x + 3);
     }
   cadena[78] = 0;
   strcat (cadena, "º\r\n");
   envia_datos (cadena, f_salida);

   memset (cadena, ' ', 90);
   cadena[3] = 'Ì';
   memset (cadena+4, 'Í', apart_2[MAX_PROD-1].pos_x +
                          apart_2[MAX_PROD-1].longitud);
   for (f = 1; f < MAX_PROD; f++)
     gotoxy_text (cadena, "Î", apart_2[f].pos_x + 2);
   cadena[78] = 0;
   strcat (cadena, "¹\r\n");
   envia_datos (cadena, f_salida);

   memset (cadena, ' ', 80);
   cadena[80] = 0;
   cadena[0] = 'º';

   // Recorre los datos de la factura, imprimiendo cada uno.
   for (f = 0; f < lineas; f++)
     {memset (cadena, ' ', 80);
      for (g = 0; g < MAX_PROD; g++)
        {gotoxy_text (cadena, "º", apart_2[g].pos_x + 2);
         if (productos[f].usado)
           switch (g)
             {case 0 :
                gotoxy_text (cadena, productos[f].producto,
                             apart_2[g].pos_x + 3);
                break;

              case 1 :
                sprintf (auxc2, "%%%ds", apart_2[g].longitud);
                sprintf (auxc, auxc2, productos[f].nro_registro);
                gotoxy_text (cadena, auxc, apart_2[g].pos_x + 3);
                break;

              case 2 :
                sprintf (auxc2, "%%%d.2f", apart_2[g].longitud);
                sprintf (auxc, auxc2, productos[f].dosis);
                gotoxy_text (cadena, auxc, apart_2[g].pos_x + 3);
                break;

              case 3 :
                sprintf (auxc2, "%%%ds", apart_2[g].longitud);
                sprintf (auxc, auxc2, productos[f].ps);
                gotoxy_text (cadena, auxc, apart_2[g].pos_x + 3);
                break;

              case 4 :
                sprintf (auxc2, "%%%d.2f", apart_2[g].longitud);
                sprintf (auxc, auxc2, productos[f].kg_l);
                gotoxy_text (cadena, auxc, apart_2[g].pos_x + 3);
                break;

              case 5 :
                sprintf (auxc2, "%%%d.0f", apart_2[g].longitud);
                sprintf (auxc, auxc2, productos[f].precio);
                gotoxy_text (cadena, auxc, apart_2[g].pos_x + 3);
                break;

              case 6 :
                sprintf (auxc2, "%%%d.0f", apart_2[g].longitud);
                sprintf (auxc, auxc2, productos[f].importe);
                gotoxy_text (cadena, auxc, apart_2[g].pos_x + 3);
                break;
             }
        }
      cadena[78] = 0;
      strcat (cadena, "º\r\n");
      envia_datos (cadena, f_salida);
     }

   // Imprime el subtotal del bloque actual.

   memset (cadena, ' ', 80);
   cadena[3] = 'È';
   memset (cadena + 4, 'Í', apart_2[MAX_PROD - 1].pos_x);
   for (f = 1; f < MAX_PROD - 1; f++)
     gotoxy_text (cadena, "Ê", apart_2[f].pos_x + 2);
   cadena[apart_2[MAX_PROD-1].pos_x + 2] = 0;
   strcat (cadena, "ÎÍÍÍÍÍÍÍÍÍÍ¹\r\n");
   envia_datos (cadena, f_salida);

   memset (cadena, ' ', 80);
   cadena[apart_2[MAX_PROD-1].pos_x - 3] = 0;
   sprintf (auxc, "Pts  º%10.0fº\r\n", total);
   strcat (cadena, auxc);
   envia_datos (cadena, f_salida);

   memset (cadena, ' ', 80);
   cadena[apart_2[MAX_PROD-1].pos_x + 2] = 0;
   strcat (cadena, "ÈÍÍÍÍÍÍÍÍÍÍ¼\r\n");
   envia_datos (cadena, f_salida);

  return OK;
  } // imprime_impresora_2

void pascal muestra_factura1 (Tservicios *datos, unsigned lineas,
                              unsigned pos_y, double total)
// Imprime los datos pasados como si fueran de una factura
  {
   unsigned aux, contador = 0;
   char auxc[80], cadena[80];

   if (!datos) return;

   // Aumenta la longitud de los campos
   for (aux = 0; aux < MAX_SERV; aux++)
     if (apart_1[aux].tipo_dato & ESP_EXTRA)
       apart_1[aux].longitud += 2;

   // Recorre la lista de campos para imprimirlos
   while (contador < lineas)
     {for (aux = 0; aux < MAX_SERV; aux++)
        if (datos[contador].usado == TRUE)
          {if (aux == 0)
             sprintf (cadena, "%s", datos[contador].concepto);
           else
             {sprintf (auxc, "%%%d.0f", apart_1[1].longitud);
              sprintf (cadena, auxc, datos[contador].importe);
             }
           imprime_cadena (cadena, apart_1[aux].pos_x, pos_y + contador + 1);
          }

      contador++;
     }
   sprintf (auxc, "%%%d.0f", apart_1[1].longitud);
   sprintf (cadena, auxc, total);
   imprime_cadena (cadena, apart_1[1].pos_x, pos_y + lineas + 2);

   // Restaura la longitud original de los campos
   for (aux = 0; aux < MAX_SERV; aux++)
     if (apart_1[aux].tipo_dato & ESP_EXTRA)
       apart_1[aux].longitud -= 2;
  } // muestra_factura1

void pascal muestra_factura2 (Tproductos *datos, unsigned lineas,
                              unsigned pos_y, double total)
// Imprime los datos pasados como si fueran de una factura
  {
   unsigned aux, contador = 0;
   char auxc[80], cadena[80];

   if (!datos) return;

   // Aumenta la longitud de los campos
   for (aux = 0; aux < MAX_PROD; aux++)
     if (apart_2[aux].tipo_dato & ESP_EXTRA)
       apart_2[aux].longitud += 2;

   // Recorre la lista de campos para imprimirlos
   while (contador < lineas)
     {for (aux = 0; aux < MAX_PROD; aux++)
        if (datos[contador].usado == TRUE)
          {switch (aux)
             {case 0 :
                sprintf (cadena, "%s", datos[contador].producto);
                cadena[apart_2[0].longitud] = 0;
                break;

              case 1 :
                sprintf (cadena, "%10s", datos[contador].nro_registro);
                break;

              case 2 :
                sprintf (auxc, "%%%d.2f", apart_2[2].longitud);
                sprintf (cadena, auxc, datos[contador].dosis);
                break;

              case 3 :
                sprintf (cadena, "%6s", datos[contador].ps);
                break;

              case 4 :
                sprintf (auxc, "%%%d.2f", apart_2[4].longitud);
                sprintf (cadena, auxc, datos[contador].kg_l);
                break;

              case 5 :
                sprintf (auxc, "%%%d.0f", apart_2[5].longitud);
                sprintf (cadena, auxc, datos[contador].precio);
                break;

              case 6 :
                sprintf (auxc, "%%%d.0f", apart_2[6].longitud);
                sprintf (cadena, auxc, datos[contador].importe);
                break;
             }
           imprime_cadena (cadena, apart_2[aux].pos_x, pos_y + contador + 1);
          }
      contador++;
     }

   sprintf (auxc, "%%%d.0f", apart_2[6].longitud);
   sprintf (cadena, auxc, total);
   imprime_cadena (cadena, apart_2[6].pos_x, pos_y + lineas + 2);

   // Restaura la longitud original de los campos
   for (aux = 0; aux < lineas; aux++)
     if (apart_2[aux].tipo_dato & ESP_EXTRA)
       apart_2[aux].longitud -= 2;
  } // muestra_factura2

unsigned pascal rellena_factura1 (double *total, int pos_y)
// Pide los datos de una factura y calcula el total
  {char aux[80],
        tecla;   // Tecla pulsada
   int campo_actual,
       linea_actual;
   double temp;

   campo_actual = 0;
   linea_actual = 0;

   do
     {pone_barra_inferior (fact_text);
      if (apart_1[campo_actual].tipo_dato & ESP_EXTRA)
        gotoxy (apart_1[campo_actual].pos_x + 3, pos_y + 2 + linea_actual -
                                            desplazamiento_vertical);
      else
        gotoxy (apart_1[campo_actual].pos_x + 1, pos_y + 2 + linea_actual -
                                            desplazamiento_vertical);
      aux[0] = 0;
      if (campo_actual == 0)
      // Seg£n el campo a rellenar se pide un dato u otro
        {strcpy (aux, servicios[linea_actual].concepto);
         tecla = lee_cadena (aux, apart_1[0].longitud + 1, TRUE, FALSE, TRUE);
        }
      else
        {if (servicios[linea_actual].importe)
           sprintf (aux, "%-.0f", servicios[linea_actual].importe);
         tecla = lee_cadena (aux, apart_1[1].longitud + 1, TAL_CUAL,
                             FALSE);
         temp = atof (aux);
        }

      switch (tecla)
      // Seg£n la tecla devuelta por lee_cadena realiza una cosa u otra al
      // cambiar de campo
        {case ESC : // Al pusar ESC se sale de la opcion de hacer facturas,
                    // pero antes se pregunta si es lo que se desea
                    if (aviso ("¨Realmente quiere dejar de hacer la factura?",
                               255, 255, 47, 7) == NO)
                      tecla = 0;
                    break;

         case ENTER : // Al pulsar ENTER o TAB, pasa al campo siguiente, si
         case TAB :   // llega al final de la l¡nea actual pasa a la siguiente
         case LF :
         case CURS_ABJ :
         case CURS_ARR :
         case S_TAB :
         case F10 :
           {
            if (campo_actual == 0 && *aux)
              {strcpy (servicios[linea_actual].concepto, aux);
               servicios[linea_actual].usado = TRUE;
              }
            else
              if (*aux)
                {*total += temp - servicios[linea_actual].importe;
                 servicios[linea_actual].importe = temp;
                }
            // copia los datos introducidos a la factura
            muestra_factura1 (servicios, 5, pos_y, *total);
            switch (tecla)
            // Seg£n la tecla pulsada pasa a uno u otro campo
              {case ENTER :
               case TAB :
               case LF :
                 if (campo_actual)
                   {campo_actual = 0;
                    linea_actual++;
                   }
                 else campo_actual++;
                 break;

               case CURS_ABJ :
                 linea_actual++;
                 break;

               case CURS_ARR :
                 linea_actual--;
                 break;

               case S_TAB :
                 if (campo_actual)
                   campo_actual--;
                 else
                   {campo_actual++;
                    linea_actual--;
                   }
                 break;

               case F10 :
                 tecla = CAMBIO;
                 break;
              }

            if (linea_actual > 4 || linea_actual < 0)
              tecla = CAMBIO;
            break;
           }

        }
     }
   // Repite todo mientras no se pulse ESC, CTRL-ENTER, o se tenga que cambiar
   // de bloque de factura
   while (tecla != ESC && tecla != LF && tecla != CAMBIO);
   // Si se pulsa CTRL-ENTER o se cambia de bloque se guardan los datos, sino
   // se pierden
   return tecla;
  } // rellena_factura1

unsigned pascal rellena_factura2 (double *total, Cbase_datos &articulos,
                                  int pos_y, Tdatos_factura *datos_extra,
                                  double totala, double *total_t,
                                  double *base, unsigned tipo)
  {char aux[80],
        tecla;   // Tecla pulsada
   int campo_actual,
       linea_actual;
   double temp;
   unsigned nro_ficha;
   boolean auxb;

   campo_actual = 0;
   linea_actual = 0;

   do
     {pone_barra_inferior (fact_text);
      if (apart_2[campo_actual].tipo_dato & ESP_EXTRA)
        gotoxy (apart_2[campo_actual].pos_x + 3, pos_y + 2 + linea_actual -
                                            desplazamiento_vertical);
      else
        gotoxy (apart_2[campo_actual].pos_x + 1, pos_y + 2 + linea_actual -
                                            desplazamiento_vertical);

      aux[0] = 0;
      switch (campo_actual)
      // Seg£n el tipo del campo actual pide un tipo de dato u otro
        {case 0 : // Pide el art¡culo a facturar
           strcpy (aux, productos[linea_actual].producto);
           auxb = pregunta ("Nombre del producto :", 255, 255, 60, 8, aux,
                            apart_2[0].longitud + 1);
           if (auxb == TAL_CUAL)
             {// Muestra la lista de art¡culos
              articulos.ordenar_fichas (0);
              pant_aux = hace_recuadro (255, 2, 79, 22, " Art¡culos ");
              nro_ficha = muestra_lista (articulos, 'a', 0,
                                         articulos.cantidad_fichas() - 1, 22,
                                         TRUE);
              restaura_recuadro (&pant_aux);
              articulos.ficha_numero (nro_ficha);
              strcpy (aux, articulos.leer_dato(2));
              strcpy (productos[linea_actual].nro_registro,
                      articulos.leer_dato (3));
              strcpy (productos[linea_actual].ps,
                      articulos.leer_dato (4));
              productos[linea_actual].precio =
                      *(double *)articulos.leer_dato (tipo == 1 ? 6 : 7);
              tecla = TAB;
             }
           else
             if (*aux == 0)
                // Si se devuelve una cadena vac¡a, se piden los datos uno a
                // uno
                {strcpy (aux, productos[linea_actual].producto);
                 pone_barra_inferior (fact_text);
                 tecla = lee_cadena (aux, apart_2[0].longitud + 1, TRUE,
                                     FALSE, TRUE);
                 nro_ficha = FICHA_INEXISTENTE;
                 // indicamos que no se ha encontrado la ficha para que no
                 // se saquen los datos de una ficha cualquiera
                }
              else // En el caso de que se haya introducido algo se considera
                   // que se indica el nombre de un art¡culo
                {nro_ficha = articulos.buscar_ficha (aux, 2);
                 // Se busca el art¡culo
                 if (nro_ficha == FICHA_INEXISTENTE)
                   // si no existe se avisa y se piden los datos nuevamente
                   {mensaje ("El art¡culo no est  en la lista", 255, 255,
                              37, 7);
                    tecla = 0;
                   }
                 else
                   {articulos.ficha_numero (nro_ficha);
                    strcpy (aux, articulos.leer_dato(2));
                    strcpy (productos[linea_actual].nro_registro,
                            articulos.leer_dato (3));
                    strcpy (productos[linea_actual].ps,
                            articulos.leer_dato (4));
                    productos[linea_actual].precio =
                            *(double *)articulos.leer_dato (tipo == 1 ? 6 : 7);
                    productos[linea_actual].importe =
                                              productos[linea_actual].precio *
                                                 productos[linea_actual].kg_l;
                    tecla = TAB;
                   }
                }
           break;

         case 1 : // lee el n§ de registro
           if (nro_ficha == FICHA_INEXISTENTE)
             {strcpy (aux, productos[linea_actual].nro_registro);
              tecla = lee_cadena (aux, apart_2[1].longitud + 1, TRUE, FALSE);
             }
           else tecla = TAB;
           break;

         case 2 : // Lee la dosis
           if (productos[linea_actual].dosis)
             sprintf (aux, "%-5.2f", productos[linea_actual].dosis);
           tecla = lee_cadena (aux, apart_2[2].longitud + 1, TRUE, FALSE);
           break;

         case 3 : // lee el plazo de seguridad
           if (nro_ficha == FICHA_INEXISTENTE)
             {sprintf (aux, "%s", productos[linea_actual].ps);
              tecla = lee_cadena (aux, apart_2[3].longitud + 1, TRUE, FALSE);
             }
           else tecla = TAB;
           break;

         case 4 : // lee los Kg/l
           if (nro_ficha == FICHA_INEXISTENTE)
             {if (productos[linea_actual].kg_l)
                sprintf (aux, "%-5.2f", productos[linea_actual].kg_l);
              tecla = lee_cadena (aux, apart_2[4].longitud + 1, TRUE, FALSE);
             }
           else tecla = TAB;
           break;

         case 5 : // Lee el precio del producto
           if (nro_ficha == FICHA_INEXISTENTE)
             {if (productos[linea_actual].precio)
                sprintf (aux, "%-9.0f", productos[linea_actual].precio);
              tecla = lee_cadena (aux, apart_2[5].longitud + 1, TRUE, FALSE);
             }
           else tecla = TAB;
           break;

         case 6 : // El importe no se lee, se calcula en base a los anteriores
                  // campos
           temp = productos[linea_actual].precio *
                  productos[linea_actual].kg_l;
           tecla = TAB;
           break;
        }

      switch (tecla)
      // Seg£n la tecla devuelta por lee_cadena realiza una cosa u otra al
      // cambiar de campo
        {case ESC : // Al pusar ESC se sale de la opcion de hacer facturas,
                    // pero antes se pregunta si es lo que se desea
                    if (aviso ("¨Realmente quiere dejar de hacer la factura?",
                               255, 255, 47, 7) == NO)
                      tecla = 0;
                    break;

         case ENTER : // Al pulsar ENTER o TAB, pasa al campo siguiente, si
         case TAB :   // llega al final de la l¡nea actual pasa a la siguiente
         case CURS_ABJ :
         case CURS_ARR :
         case S_TAB :
         case F10 :
           {
            switch (campo_actual)
            // Primero se copia el dato al campo correspondiente
              {case 0 :
                 if (*aux)
                   {strcpy (productos[linea_actual].producto, aux);
                    productos[linea_actual].usado = TRUE;
                   }
                 break;

               case 1 :
                 if (*aux && nro_ficha == FICHA_INEXISTENTE)
                   {strcpy (productos[linea_actual].nro_registro, aux);
                    productos[linea_actual].usado = TRUE;
                   }
                 else tecla = TAB;
                 break;

               case 2 :
                 if (*aux)
                   {productos[linea_actual].dosis = atof(aux);
                    productos[linea_actual].usado = TRUE;
                    productos[linea_actual].kg_l = datos_extra->litros *
                                                productos[linea_actual].dosis;
                   }
                 break;

               case 3 :
                 if (*aux && nro_ficha == FICHA_INEXISTENTE)
                   {strcpy (productos[linea_actual].ps, aux);
                    productos[linea_actual].usado = TRUE;
                   }
                 else tecla = TAB;
                 break;

               case 4 :
                 if (*aux && nro_ficha == FICHA_INEXISTENTE)
                   {productos[linea_actual].kg_l = atof (aux);
                    productos[linea_actual].usado = TRUE;
                   }
                 else tecla = TAB;
                 break;

               case 5 :
                 if (*aux && nro_ficha == FICHA_INEXISTENTE)
                   {productos[linea_actual].precio = atof (aux);
                    productos[linea_actual].usado = TRUE;
                   }
                 else tecla = TAB;
                 break;

               case 6 :
                 if (productos[linea_actual].usado || *aux)
                   {*total += temp - productos[linea_actual].importe;
                    productos[linea_actual].importe = temp;
                   }

                 tecla = TAB;
                 if (tipo == 1)
                   calcula_total_1 (totala, *total, total_t, base, POS_Y2 + 13);
                 else calcula_total_3 (totala, *total, total_t, POS_Y2 + 13);
                 break;

              }
            // copia los datos introducidos a la factura
            muestra_factura2 (productos, 8, pos_y, *total);
            switch (tecla)
            // Seg£n la tecla pulsada pasa a uno u otro campo
              {case ENTER :
               case TAB :
                 if (campo_actual == 6)
                   {campo_actual = 0;
                    linea_actual++;
                   }
                 else campo_actual++;
                 break;

               case CURS_ABJ :
                 linea_actual++;
                 break;

               case CURS_ARR :
                 linea_actual--;
                 break;

               case S_TAB :
                 if (campo_actual == 0)
                   {campo_actual = 6;
                    linea_actual--;
                   }
                 else campo_actual--;
                 break;

               case F10 :
                 tecla = CAMBIO;
                 break;
              }

            if (linea_actual > 7 || linea_actual < 0)
              tecla = CAMBIO;
            break;
           }

        }
     }
   // Repite todo mientras no se pulse ESC, CTRL-ENTER, o se tenga que cambiar
   // de bloque de factura
   while (tecla != ESC && tecla != LF && tecla != CAMBIO);
   // Si se pulsa CTRL-ENTER o se cambia de bloque se guardan los datos, sino
   // se pierden
   return tecla;
  } // rellena_factura2

void pascal calcula_total_1 (double total1, double total2, double *total,
                             double *base, unsigned pos_y)
// Calcula los totales del tipo de factura 1
  {double iva_t;
   char cadena[50];

   *base = total1 + total2;
   iva_t = ceil (*base * iva / 100.0);
   *total = *base + iva_t;
   pone_caracter ('É', 64, pos_y, WHITE | (BLUE << 4));
   pone_caracter ('»', 78, pos_y, WHITE | (BLUE << 4));
   pone_caracter ('È', 64, pos_y + 7, WHITE | (BLUE << 4));
   pone_caracter ('¼', 78, pos_y + 7, WHITE | (BLUE << 4));
   linea_horz (65, pos_y, 13, L_DOBLES | WHITE | (BLUE << 4));
   linea_horz (65, pos_y + 5, 13, L_DOBLES | WHITE | (BLUE << 4));
   linea_horz (65, pos_y + 7, 13, L_DOBLES | WHITE | (BLUE << 4));
   linea_vert (64, pos_y + 1, 6, L_DOBLES | WHITE | (BLUE << 4));
   linea_vert (78, pos_y + 1, 6, L_DOBLES | WHITE | (BLUE << 4));
   sprintf (cadena, "Total A ... º%12.0f º", total1);
   imprime_cadena (cadena, 52, pos_y + 1, 0x1F);
   sprintf (cadena, "Total B ... º%12.0f º", total2);
   imprime_cadena (cadena, 52, pos_y + 2, 0x1F);
   sprintf (cadena, "Base imponible ... º%12.0f º", *base);
   imprime_cadena (cadena, 45, pos_y + 3, 0x1F);
   sprintf (cadena, "IVA al %2.0f%% ... º%12.0f º", iva, iva_t);
   imprime_cadena (cadena, 49, pos_y + 4, 0x1F);
   sprintf (cadena, "Total ... º%12.0f º", *total);
   imprime_cadena (cadena, 54, pos_y + 6, 0x1F);
  } // calcula_total_1

void pascal calcula_total_2 (double total1, double *total, double *base,
                             unsigned pos_y)
// Calcula los totales del tipo de factura 2
  {double iva_t;
   char cadena[50];

   *base = total1;
   iva_t = ceil (*base * iva / 100.0);
   *total = *base + iva_t;
   pone_caracter ('É', 64, pos_y, WHITE | (BLUE << 4));
   pone_caracter ('»', 78, pos_y, WHITE | (BLUE << 4));
   pone_caracter ('È', 64, pos_y + 5, WHITE | (BLUE << 4));
   pone_caracter ('¼', 78, pos_y + 5, WHITE | (BLUE << 4));
   linea_horz (65, pos_y, 13, L_DOBLES | WHITE | (BLUE << 4));
   linea_horz (65, pos_y + 3, 13, L_DOBLES | WHITE | (BLUE << 4));
   linea_horz (65, pos_y + 5, 13, L_DOBLES | WHITE | (BLUE << 4));
   linea_vert (64, pos_y + 1, 5, L_DOBLES | WHITE | (BLUE << 4));
   linea_vert (78, pos_y + 1, 5, L_DOBLES | WHITE | (BLUE << 4));
   sprintf (cadena, "Base imponible ... º%12.0f º", total1);
   imprime_cadena (cadena, 45, pos_y + 1, 0x1F);
   sprintf (cadena, "IVA al %2.0f%% ... º%12.0f º", iva, iva_t);
   imprime_cadena (cadena, 49, pos_y + 2, 0x1F);
   sprintf (cadena, "Total ... º%12.0f º", *total);
   imprime_cadena (cadena, 54, pos_y + 4, 0x1F);
  } // calcula_total_2

void pascal calcula_total_3 (double total1, double total2, double *total,
                             unsigned pos_y)
// Calcula los totales del tipo de factura 3
  {char cadena[50];

   *total = total1 + total2;
   pone_caracter ('É', 64, pos_y, WHITE | (BLUE << 4));
   pone_caracter ('»', 78, pos_y, WHITE | (BLUE << 4));
   pone_caracter ('È', 64, pos_y + 5, WHITE | (BLUE << 4));
   pone_caracter ('¼', 78, pos_y + 5, WHITE | (BLUE << 4));
   linea_horz (65, pos_y, 13, L_DOBLES | WHITE | (BLUE << 4));
   linea_horz (65, pos_y + 3, 13, L_DOBLES | WHITE | (BLUE << 4));
   linea_horz (65, pos_y + 5, 13, L_DOBLES | WHITE | (BLUE << 4));
   linea_vert (64, pos_y + 1, 4, L_DOBLES | WHITE | (BLUE << 4));
   linea_vert (78, pos_y + 1, 4, L_DOBLES | WHITE | (BLUE << 4));
   sprintf (cadena, "Total A ... º%12.0f º", total1);
   imprime_cadena (cadena, 52, pos_y + 1, 0x1F);
   sprintf (cadena, "Total B ... º%12.0f º", total2);
   imprime_cadena (cadena, 52, pos_y + 2, 0x1F);
   sprintf (cadena, "Total ... º%12.0f º", *total);
   imprime_cadena (cadena, 54, pos_y + 4, 0x1F);
  } // calcula_total_3

void pascal imprime_cabecera (unsigned destino, Tdatos_factura *datos,
                              Cbase_datos &clientes,
                              TFtemporal *f_impresion)
// Imprime los datos de cabecera de la factura. Estos pueden ir tanto a la
// pantalla como a la impresora (a trav‚s de un fichero temporal)
  {unsigned aux;
   char temp[120];
   date fecha;
   struct time hora;

   getdate (&fecha);
   if (destino == PANTALLA)
     {
      itoa (datos->nro_cliente, temp, 10);
      aux = clientes.buscar_ficha (temp, 1);
      clientes.ficha_numero (aux);

      sprintf (temp, "Cliente : %s %s",(char *)clientes.leer_dato (2),
               (char *)clientes.leer_dato(3));
      imprime_cadena (temp, 2, 0);

      sprintf (temp, "N.I.F. : %s", (char *)clientes.leer_dato (8));
      imprime_cadena (temp, 53, 0);

      sprintf (temp, "Domicilio : %s", (char *)clientes.leer_dato(4));
      imprime_cadena (temp, 2, 2);

      sprintf (temp, "%s (%s)", (char *)clientes.leer_dato(5),
                                (char *)clientes.leer_dato(6));
      imprime_cadena (temp, 53, 2);

      sprintf (temp, "Cultivo / Plaga : %s", datos->cultivo);
      imprime_cadena (temp, 2, 4);

      sprintf (temp, "Situaci¢n : %s", datos->situacion);
      imprime_cadena (temp, 53, 4);

      luc2dos (datos->fecha, &fecha);
      sprintf (temp, "Fecha tratamiento : %d/%s/%d", fecha.da_day,
               c_mes[fecha.da_mon - 1], fecha.da_year);
      imprime_cadena (temp, 2, 6);

      sprintf (temp, "Litros : %.0f", datos->litros);
      imprime_cadena (temp, 53, 6);
     }
   else
     {char cadena[80];

      itoa (datos->nro_cliente, temp, 10);
      aux = clientes.buscar_ficha (temp, 1);
      clientes.ficha_numero (aux);

      envia_datos ("   ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»\r\n", f_impresion);
      sprintf (temp, "   º Fecha : %2d/%s/%d º\r\n", fecha.da_day, c_mes[fecha.da_mon - 1],
               fecha.da_year);
      envia_datos (temp, f_impresion);
      sprintf (temp, "   ÌÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÊ");
      completa_cadena (temp, 78, 'Í');
      strcat (temp, "»\r\n");
      envia_datos (temp, f_impresion);

      strcpy (cadena, (char *)clientes.leer_dato (2));
      strcat (cadena, " ");
      strcat (cadena, (char *)clientes.leer_dato (3));
      sprintf (temp, "   º Cliente : %-37s N.I.F. : %-15s º\r\n",
               cadena,
               (char *)clientes.leer_dato (8));
      envia_datos (temp, f_impresion);

      memset (temp, 0, 80);
      strcat (temp, "   º");
      memset (temp+4, ' ', 74);
      strcat (temp, "º\r\n");
      envia_datos (temp, f_impresion);

      sprintf (temp, "   º Domicilio : %-36s%s (%s)",
               (char *)clientes.leer_dato(4), (char *)clientes.leer_dato(5),
               (char *)clientes.leer_dato(6));
      completa_cadena (temp, 78, ' ');
      strcat (temp, "º\r\n");
      envia_datos (temp, f_impresion);

      memset (temp, 0, 80);
      strcat (temp, "   Ì");
      memset (temp+4, 'Í', 74);
      strcat (temp, "¹\r\n");
      envia_datos (temp, f_impresion);

      sprintf (temp, "   º Cultivo / Plaga : %-30sSituaci¢n : %-13sº\r\n",
               datos->cultivo, datos->situacion);
      envia_datos (temp, f_impresion);

      memset (temp, 0, 80);
      strcat (temp, "   º");
      memset (temp+4, ' ', 74);
      strcat (temp, "º\r\n");
      envia_datos (temp, f_impresion);

      luc2dos (datos->fecha, &fecha);
      sprintf (temp, "   º Fecha tratamiento : %2d/%s/%-21dLitros : %-16.0fº\r\n"
               , fecha.da_day, c_mes[fecha.da_mon - 1], fecha.da_year,
               datos->litros);
      envia_datos (temp, f_impresion);

      memset (temp, 0, 80);
      strcat (temp, "   È");
      memset (temp+4, 'Í', 74);
      strcat (temp, "¼\r\n");
      envia_datos (temp, f_impresion);
     }
  } // imprime_cabecera

void imprime_encabezamiento (unsigned long numero_factura, TFtemporal *f_impresion)
// Imprime el contenido de la variable cabecera_facturas en la impresora
  {if (cabecera_facturas != NULL)
     fprintf (f_impresion->fichero, cabecera_facturas, numero_factura);
  } // imprime_encabezamiento

void pascal imprime_factura_1 (Tdatos_factura *datos_cliente,
                               Cbase_datos &clientes, double total1,
                               double total2, double total, double base)
// Imprime los datos de la factura de tipo 1 (o A)
  {char cadena[120], temp[120];
   TFtemporal *salida;

   salida = inicializa_impresion ();
   imprime_encabezamiento (nro_factura, salida);
   imprime_cabecera (IMPRESORA, datos_cliente, clientes, salida);
   envia_datos ("\r\n", salida);
   imprime_impresora_1 (salida, 5, total1);
   envia_datos ("\r\r\n", salida);
   imprime_impresora_2 (salida, 8, total2);
   envia_datos ("\r\r\n", salida);

   memset (cadena, ' ', 80);
   cadena[apart_2[MAX_PROD-1].pos_x + 2] = 0;
   strcat (cadena, "ÉÍÍÍÍÍÍÍÍÍÍ»\r\n");
   envia_datos (cadena, salida);

   memset (cadena, ' ', 56);
   cadena[55] = 0;
   sprintf (temp, "Total A ... º%10.0fº\r\n", total1);
   strcat (cadena, temp);
   gotoxy_text (cadena, "Enterado y conforme", 10);
   envia_datos (cadena, salida);

   memset (cadena, ' ', 90);
   cadena[55] = 0;
   sprintf (temp, "Total B ... º%10.0fº\r\n", total2);
   strcat (cadena, temp);
   envia_datos (cadena, salida);

   memset (cadena, ' ', 90);
   cadena[48] = 0;
   sprintf (temp, "Base imponible ... º%10.0fº\r\n", base);
   strcat (cadena, temp);
   envia_datos (cadena, salida);

   memset (cadena, ' ', 90);
   cadena[52] = 0;
   sprintf (temp, "IVA al %2.0f%% ... º%10.0fº\r\n", iva, ceil(base * iva / 100.0));
   strcat (cadena, temp);
   envia_datos (cadena, salida);

   memset (cadena, ' ', 90);
   cadena[67] = 0;
   sprintf (temp, "ÌÍÍÍÍÍÍÍÍÍÍ¹\r\n", total);
   strcat (cadena, temp);
   envia_datos (cadena, salida);

   memset (cadena, ' ', 90);
   cadena[57] = 0;
   sprintf (temp, "Total ... º%10.0fº\r\n", total);
   strcat (cadena, temp);
   envia_datos (cadena, salida);

   memset (cadena, ' ', 80);
   cadena[apart_2[MAX_PROD-1].pos_x + 2] = 0;
   strcat (cadena, "ÈÍÍÍÍÍÍÍÍÍÍ¼\r\n");
   envia_datos (cadena, salida);
   imprime (salida);
  } // imprime_factura_1

void pascal imprime_factura_2 (Tdatos_factura *datos_cliente,
                               Cbase_datos &clientes, double total1,
                               double total, double base)
// Imprime los datos de la factura de tipo 2 (o B)
  {char cadena[120], temp[120];
   TFtemporal *salida;

   salida = inicializa_impresion ();
   imprime_encabezamiento (nro_factura, salida);
   imprime_cabecera (IMPRESORA, datos_cliente, clientes, salida);
   envia_datos ("\r\n", salida);
   imprime_impresora_1 (salida, 5, total1);
   envia_datos ("\r\r\n", salida);

   memset (cadena, ' ', 80);
   cadena[apart_2[MAX_PROD-1].pos_x + 2] = 0;
   strcat (cadena, "ÉÍÍÍÍÍÍÍÍÍÍ»\r\n");
   envia_datos (cadena, salida);

   memset (cadena, ' ', 56);
   cadena[48] = 0;
   sprintf (temp, "Base imponible ... º%10.0fº\r\n", total1);
   strcat (cadena, temp);
   gotoxy_text (cadena, "Enterado y conforme", 10);
   envia_datos (cadena, salida);

   memset (cadena, ' ', 90);
   cadena[52] = 0;
   sprintf (temp, "IVA al %2.0f%% ... º%10.0fº\r\n", iva, ceil(base * iva / 100.0));
   strcat (cadena, temp);
   envia_datos (cadena, salida);

   memset (cadena, ' ', 90);
   cadena[67] = 0;
   sprintf (temp, "ÌÍÍÍÍÍÍÍÍÍÍ¹\r\n", total);
   strcat (cadena, temp);
   envia_datos (cadena, salida);

   memset (cadena, ' ', 90);
   cadena[57] = 0;
   sprintf (temp, "Total ... º%10.0fº\r\n", total);
   strcat (cadena, temp);
   envia_datos (cadena, salida);

   memset (cadena, ' ', 80);
   cadena[apart_2[MAX_PROD-1].pos_x + 2] = 0;
   strcat (cadena, "ÈÍÍÍÍÍÍÍÍÍÍ¼\r\n");
   envia_datos (cadena, salida);
   imprime (salida);
  } // imprime_factura_2

void pascal imprime_factura_3 (Tdatos_factura *datos_cliente,
                               Cbase_datos &clientes, double total1,
                               double total2, double total)
// Imprime los datos de la factura de tipo 1 (o A)
  {char cadena[120], temp[120];
   TFtemporal *salida;

   salida = inicializa_impresion ();
   imprime_cabecera (IMPRESORA, datos_cliente, clientes, salida);
   envia_datos ("\r\n", salida);
   imprime_impresora_1 (salida, 5, total1);
   envia_datos ("\r\r\n", salida);
   imprime_impresora_2 (salida, 8, total2);
   envia_datos ("\r\r\n", salida);

   memset (cadena, ' ', 80);
   cadena[apart_2[MAX_PROD-1].pos_x + 2] = 0;
   strcat (cadena, "ÉÍÍÍÍÍÍÍÍÍÍ»\r\n");
   envia_datos (cadena, salida);

   memset (cadena, ' ', 56);
   cadena[55] = 0;
   sprintf (temp, "Total A ... º%10.0fº\r\n", total1);
   strcat (cadena, temp);
   gotoxy_text (cadena, "Enterado y conforme", 10);
   envia_datos (cadena, salida);

   memset (cadena, ' ', 90);
   cadena[55] = 0;
   sprintf (temp, "Total B ... º%10.0fº\r\n", total2);
   strcat (cadena, temp);
   envia_datos (cadena, salida);

   memset (cadena, ' ', 90);
   cadena[67] = 0;
   sprintf (temp, "ÌÍÍÍÍÍÍÍÍÍÍ¹\r\n", total);
   strcat (cadena, temp);
   envia_datos (cadena, salida);

   memset (cadena, ' ', 90);
   cadena[57] = 0;
   sprintf (temp, "Total ... º%10.0fº\r\n", total);
   strcat (cadena, temp);
   envia_datos (cadena, salida);

   memset (cadena, ' ', 80);
   cadena[apart_2[MAX_PROD-1].pos_x + 2] = 0;
   strcat (cadena, "ÈÍÍÍÍÍÍÍÍÍÍ¼\r\n");
   envia_datos (cadena, salida);
   imprime (salida);
  } // imprime_factura_3

void pascal guarda_factura_1 (double total1, double total2, double total,
                              Tdatos_factura *datos_cliente)
// Almacena los datos de la factura de tipo 1
  {FILE *fichero;
   Tdatos_fichero_facturas datos_fichero;
   struct date fecha;

   getdate (&fecha);
   datos_fichero.tipo_factura = 1;
   datos_fichero.nro_cliente = datos_cliente->nro_cliente;
   strcpy (datos_fichero.situacion, datos_cliente->situacion);
   datos_fichero.fecha = dos2luc (&fecha);
   datos_fichero.nro_factura = nro_factura;
   datos_fichero.litros = datos_cliente->litros;
   datos_fichero.total_1 = total1;
   datos_fichero.total_2 = total2;
   datos_fichero.total_iva = ceil((total1 + total2) * iva / 100.0);
   datos_fichero.total_factura = total;
   fichero = fopen (Facturas_nombre_fichero, "ab");
   fwrite (&datos_fichero, sizeof (Tdatos_fichero_facturas), 1, fichero);
   fclose (fichero);
  } // guarda_Factura_1

void pascal guarda_factura_2 (double total1, double total,
                              Tdatos_factura *datos_cliente)
// Almacena los datos de la factura de tipo 2
  {FILE *fichero;
   Tdatos_fichero_facturas datos_fichero;
   struct date fecha;

   getdate (&fecha);
   datos_fichero.tipo_factura = 2;
   datos_fichero.nro_cliente = datos_cliente->nro_cliente;
   strcpy (datos_fichero.situacion, datos_cliente->situacion);
   datos_fichero.fecha = dos2luc(&fecha);
   datos_fichero.nro_factura = nro_factura;
   datos_fichero.litros = datos_cliente->litros;
   datos_fichero.total_1 = total1;
   datos_fichero.total_2 = 0;
   datos_fichero.total_iva = ceil(total1 * iva / 100.0);
   datos_fichero.total_factura = total;
   fichero = fopen (Facturas_nombre_fichero, "ab");
   fwrite (&datos_fichero, sizeof (Tdatos_fichero_facturas), 1, fichero);
   fclose (fichero);
  } // guarda_factura_2

void pascal guarda_factura_3 (double total1, double total2, double total,
                              Tdatos_factura *datos_cliente)
// Almacena los datos de la factura de tipo 3
  {FILE *fichero;
   Tdatos_fichero_facturas datos_fichero;
   struct date fecha;

   getdate (&fecha);
   datos_fichero.tipo_factura = 3;
   datos_fichero.nro_cliente = datos_cliente->nro_cliente;
   strcpy (datos_fichero.situacion, datos_cliente->situacion);
   datos_fichero.fecha = dos2luc(&fecha);
   datos_fichero.nro_factura = 0;
   datos_fichero.litros = datos_cliente->litros;
   datos_fichero.total_1 = total1;
   datos_fichero.total_2 = total2;
   datos_fichero.total_iva = 0.0;
   datos_fichero.total_factura = total;
   fichero = fopen (Facturas_ile, "ab");
   fwrite (&datos_fichero, sizeof (Tdatos_fichero_facturas), 1, fichero);
   fclose (fichero);
  } // guarda_factura_3

int pascal hace_factura_tipo_1 (Cbase_datos &clientes, Cbase_datos &articulos)
// pone los datos de las facturas completas
  {Tpantalla temp1;
   unsigned que_apartado, accion, f;
   Tdatos_factura *datos_cliente = NULL;
   double total1 = 0, total2 = 0, total = 0, base = 0;

   textcolor (YELLOW);
   textbackground (BLUE);

   temp1 = guarda_recuadro (1, 1, 79, 23, SIN_MARCO);
   clrscr();
   // Almacena la pantalla y la borra

   limpia_facturas();
   // Limpia el contenido de la factura

   datos_cliente = selecciona_cliente (clientes);
   // Selecciona el cliente

   if (datos_cliente == NULL)
   // Si no se ha seleccionado, o no existe, se sale
     {restaura_recuadro (&temp1);
      return OK;
     }

   imprime_plantilla (apart_1, MAX_SERV, 5, POS_Y1);
   imprime_plantilla (apart_2, MAX_PROD, 8, POS_Y2);
   // Imprime la plantilla de las facturas

   que_apartado = 0;
   _setcursortype (_NORMALCURSOR);
   do // Entra en el bucle principal
     {if (que_apartado == 0)
        accion = rellena_factura1(&total1, POS_Y1);
      else
        accion = rellena_factura2(&total2, articulos, POS_Y2, datos_cliente,
                                  total1, &total, &base, 1);

      // seg£n que apartado est‚ seleccionado, se rellena el apartado de
      // concepto o el de productos
      if (accion == CAMBIO)
      // Si se pasa de un apartado a otro, se mueve la pantalla hacia arriba
      // o hacia abajo el nro de l¡neas necesarias
        {que_apartado = 1 - que_apartado;
         if (que_apartado)
           {for (f = 0; f < POS_Y2 - 2; f++)
              scrollarriba ();
            desplazamiento_vertical += POS_Y2 - 2;
            imprime_plantilla (apart_2, MAX_PROD, 8, POS_Y2);
            muestra_factura2 (productos, 8, POS_Y2, total2);
            calcula_total_1 (total1, total2, &total, &base, POS_Y2 + 13);
           }
         else
           {for (f = POS_Y2 - 2; f > 0; f--)
              scrollabajo ();
            desplazamiento_vertical = 0;
            imprime_plantilla (apart_1, MAX_SERV, 5, POS_Y1);
            muestra_factura1 (servicios, 5, POS_Y1, total1);
            imprime_cabecera (PANTALLA, datos_cliente, clientes);
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
      if (que_apartado == 0)
        {for (f = 0; f < POS_Y2 - 2; f++)
           scrollarriba ();
         desplazamiento_vertical += POS_Y2 - 2;
        }
      imprime_plantilla (apart_2, MAX_PROD, 8, POS_Y2);
      muestra_factura2 (productos, 8, POS_Y2, total2);
      calcula_total_1 (total1, total2, &total, &base, POS_Y2 + 13);
      accion = toupper(getch());
      if (accion != 27)
        {if (nro_factura < MAXLONG)
           nro_factura++; // Nro. de la nueva factura
         if (accion == 'I')
           imprime_factura_1 (datos_cliente, clientes, total1, total2, total,
                              base);
         guarda_factura_1(total1, total2, total, datos_cliente);
         configuracion_modificada = TRUE;
        }
      reloj_visible = TRUE;
     }
   _setcursortype (_NOCURSOR);
   desplazamiento_vertical = 0;
   restaura_recuadro (&temp1);
   if (datos_cliente)
     delete datos_cliente;

   return OK;
  } // hace_factura_tipo_1

int pascal hace_factura_tipo_2 (Cbase_datos &clientes)
// pone los datos de las facturas que solo llevan servicios
  {Tpantalla temp1;
   unsigned accion;
   Tdatos_factura *datos_cliente = NULL;
   double total1 = 0, total = 0, base = 0;

   textcolor (YELLOW);
   textbackground (BLUE);

   temp1 = guarda_recuadro (1, 1, 79, 23, SIN_MARCO);
   clrscr();
   // Almacena la pantalla y la borra

   limpia_facturas();
   // Limpia el contenido de la factura

   pone_barra_inferior (" Facturaci¢n ³ Factura servicios");

   datos_cliente = selecciona_cliente (clientes);
   // Selecciona el cliente

   if (datos_cliente == NULL)
   // Si no se ha seleccionado, o no existe, se sale
     {restaura_recuadro (&temp1);
      return OK;
     }

   imprime_plantilla (apart_1, MAX_SERV, 5, POS_Y1);
   // Imprime la plantilla de las facturas

   _setcursortype (_NORMALCURSOR);
   do // Entra en el bucle principal
     {accion = rellena_factura1(&total1, POS_Y1);
      // Rellena el apartado de servicios
     }
   while (accion != LF && accion != ESC);
   // Se repite hasta que se pulsa ESC o CTRL-ENTER
   if (accion == LF)
   // Si se sale con CTRL-ENTER se almacena la factura y se espera a que se
   // pulse la tecla para imprimirla
     {reloj_visible = FALSE;
      pone_barra_inferior (imp_fact);
      muestra_factura1 (servicios, 5, POS_Y1, total1);
      calcula_total_2 (total1, &total, &base, POS_Y1 + 9);
      accion = toupper(getch());
      if (accion != 27)
        {if (nro_factura < MAXLONG)
           nro_factura++; // Nro. de la nueva factura
         if (accion == 'I')
           imprime_factura_2 (datos_cliente, clientes, total1, total, base);
         guarda_factura_2(total1, total, datos_cliente);
         configuracion_modificada = TRUE;
        }
      reloj_visible = TRUE;
     }
   _setcursortype (_NOCURSOR);
   restaura_recuadro (&temp1);
   if (datos_cliente)
     delete datos_cliente;

   return OK;
  } // hace_factura_tipo_2

int pascal hace_factura_tipo_3 (Cbase_datos &clientes, Cbase_datos &articulos)
// pone los datos de las facturas completas
  {Tpantalla temp1;
   unsigned que_apartado, accion, f;
   Tdatos_factura *datos_cliente = NULL;
   double total1 = 0, total2 = 0, total = 0, base = 0;

   textcolor (YELLOW);
   textbackground (BLUE);

   temp1 = guarda_recuadro (1, 1, 79, 23, SIN_MARCO);
   clrscr();
   // Almacena la pantalla y la borra

   limpia_facturas();
   // Limpia el contenido de la factura

   pone_barra_inferior (" Facturaci¢n ³ Factura sin");

   datos_cliente = selecciona_cliente (clientes);
   // Selecciona el cliente

   if (datos_cliente == NULL)
   // Si no se ha seleccionado, o no existe, se sale
     {restaura_recuadro (&temp1);
      return OK;
     }

   imprime_plantilla (apart_1, MAX_SERV, 5, POS_Y1);
   imprime_plantilla (apart_2, MAX_PROD, 8, POS_Y2);
   // Imprime la plantilla de las facturas

   que_apartado = 0;
   _setcursortype (_NORMALCURSOR);
   do // Entra en el bucle principal
     {if (que_apartado == 0)
        accion = rellena_factura1(&total1, POS_Y1);
      else
        accion = rellena_factura2(&total2, articulos, POS_Y2, datos_cliente,
                                  total1, &total, &base, 3);

      // seg£n que apartado est‚ seleccionado, se rellena el apartado de
      // concepto o el de productos
      if (accion == CAMBIO)
      // Si se pasa de un apartado a otro, se mueve la pantalla hacia arriba
      // o hacia abajo el nro de l¡neas necesarias
        {que_apartado = 1 - que_apartado;
         if (que_apartado)
           {for (f = 0; f < POS_Y2 - 2; f++)
              scrollarriba ();
            desplazamiento_vertical += POS_Y2 - 2;
            imprime_plantilla (apart_2, MAX_PROD, 8, POS_Y2);
            muestra_factura2 (productos, 8, POS_Y2, total2);
            calcula_total_3 (total1, total2, &total, POS_Y2 + 13);
           }
         else
           {for (f = POS_Y2 - 2; f > 0; f--)
              scrollabajo ();
            desplazamiento_vertical = 0;
            imprime_plantilla (apart_1, MAX_SERV, 5, POS_Y1);
            muestra_factura1 (servicios, 5, POS_Y1, total1);
            imprime_cabecera (PANTALLA, datos_cliente, clientes);
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
      if (que_apartado == 0)
        {for (f = 0; f < POS_Y2 - 2; f++)
           scrollarriba ();
         desplazamiento_vertical += POS_Y2 - 2;
        }
      imprime_plantilla (apart_2, MAX_PROD, 8, POS_Y2);
      muestra_factura1 (servicios, 5, POS_Y1, total1);
      muestra_factura2 (productos, 8, POS_Y2, total2);
      calcula_total_3 (total1, total2, &total, POS_Y2 + 13);
      accion = toupper(getch());
      if (accion != 27)
        {if (accion == 'I')
           imprime_factura_3 (datos_cliente, clientes, total1, total2, total);
         guarda_factura_3(total1, total2, total, datos_cliente);
         configuracion_modificada = TRUE;
        }
      reloj_visible = TRUE;
     }
   _setcursortype (_NOCURSOR);
   desplazamiento_vertical = 0;
   restaura_recuadro (&temp1);
   if (datos_cliente)
     delete datos_cliente;

   return OK;
  } // hace_factura_tipo_3

void pascal borra_factura (void)
// borra una factura
  {FILE *fichero1, *fichero2;
   unsigned long num_factura, pos_fichero;
   char cadena[80];
   Tdatos_fichero_facturas datos_factura;
   boolean encontrada = FALSE;

   cadena[0] = 0;
   pregunta ("Introduzca el n£mero de la factura a borrar :", 255, 255, 60, 8,
             cadena, 11, FALSE);

   if (*cadena)
     {num_factura = atol (cadena);
      fichero1 = fopen (Facturas_nombre_fichero, "rb");
      fichero2 = fopen ("temp.tmp", "wb");
      do
        {fread (&datos_factura, sizeof (Tdatos_fichero_facturas), 1, fichero1);
         if (!(feof(fichero1)))
           if (datos_factura.nro_factura != num_factura)
             fwrite (&datos_factura, sizeof (Tdatos_fichero_facturas), 1,
                     fichero2);
           else
             encontrada = TRUE;
        }
      while (!(feof(fichero1)));
      fclose (fichero2);
      fclose (fichero1);
      if (encontrada)
        {unlink (Facturas_nombre_fichero);
         rename ("temp.tmp", Facturas_nombre_fichero);
        }
      else
        {mensaje ("No se ha encontrado la factura", 255, 255, 35, 6);
         unlink ("temp.tmp");
        }
     }
  } // borra_factura
