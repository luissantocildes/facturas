/****************************************************************************
 * FACT.H                                                                   *
 * Fichero de cabecera que contiene las estructuras y los objetos necesarios*
 * para crear una factura, y manejarla. Permite definir los campos de la    *
 * factura, aunque no es tan flexible como el m¢dulo de fichas.             *
 ****************************************************************************
 * Fecha realizaci¢n : 14/7/96                                              *
 * Fecha terminaci¢n : 15/7/96                                              *
 *                                                                          *
 * (C)1996 Luis Fernando Santocildes                                        *
 ****************************************************************************/

// Ficheros de cabecera necesarios


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
/* Define los campos que hay que multiplicar y donde hay que poner el
   resultado */
  {unsigned campo_1,
            campo_2,
            campo_total;
  } Tmultiplica;

typedef struct
/* Estructura que indica de que campos de la base de datos hay que sacar los
   datos, y en que campos de la factura hay que ponerlos */
  {unsigned nro_campos,
            nro_campo[2],
            campos_orig[10],
            campos_dest[10];
   char cadena_clave[2][10],
        nombre_objeto[20];
  } Tsaca_datos;

class Cfactura
/* Clase que se encarga de hacer una factura, pasar de un campo a otro de la
   misma, e imprimirla en papel */

  {public:
   Tapart *definicion;        // Lista de campos de la factura
   byte *datos_factura,       // Array con los datos de la factura
        *linea_actual;
   unsigned nro_apartados,    // N£mero de campos de la factura
            nro_lineas,       // L¡neas por campo
            bytes_linea,      // bytes de cada l¡nea
            nro_ultima_linea, // cuantas l¡neas
            bytes_total;      // bytes ocupados
   double total;              // Total de la factura
   unsigned nro_linea_actual,
            pos_y;
   unsigned long nro_factura; // Nro de factura actual
   Tapart *apartado_actual;
   Tdatos_factura *datos_cliente; // Datos extra
   long fecha;

   Cfactura (Tapart *def_factura, unsigned apartados, unsigned lineas,
             unsigned y);
     // Constructor
   ~Cfactura ();
     // Destructor
   boolean pascal hay_memoria (void);
     // Verifica si hay memoria reservada
   void pascal limpia_factura (void);
     // Borra el contenido de la factura
   void pascal imprime_plantilla (void);
     // Imprime el esquema de la factura
   void pascal imprime_factura (void);
     // Imprime el contenido de la factura
   void pascal imprime_factura (byte *datos_a_imprimir, unsigned lineas);
     // Como la funci¢n anterior, pero se le pasan los datos a imprimir
   unsigned pascal rellena_factura (Cbase_datos &objeto, Tmultiplica *multiplica,
                                    Tsaca_datos *lista_campos);
     // Muestra el formulario de la factura y espera que se rellene
   void pascal primer_apartado (void);
     // Salta al primer campo de la factura
   void pascal ultimo_apartado (void);
     // Idem pero al £ltimo
   unsigned pascal apartado_siguiente (void);
   unsigned pascal apartado_anterior (void);
     // Pasa al apartado siguiente o anterior al actual
   void pascal primer_linea (void);
   void pascal ultima_linea (void);
   unsigned pascal linea_siguiente (void);
   unsigned pascal linea_anterior (void);
   unsigned pascal imprime_factura_impresora (TFtemporal *f_salida);
     // Pasa la factura a la impresora
  };

/**************************** VARIABLES GLOBALES ***************************/

float iva_1;  // Valor del IVA

char Facturas_nombre_fichero [MAXPATH],  // Fichero para las facturas
     Facturas_ile [MAXPATH];             // Fichero para las facturas sin IVA

boolean facturas_modificadas;    // Indica si se han hecho m s facturas

unsigned long ultima_factura;    // N£mero de la £ltima factura

/************************** DESARROLLO DE FUNCIONES ************************/
Cfactura::Cfactura (Tapart *def_factura, unsigned apartados, unsigned lineas,
                    unsigned y)
// Constructor
  {unsigned aux;

   apartado_actual = definicion = new Tapart[apartados];

   // Si hay memoria se copian la definici¢n de los campos a la factura
   if (definicion)
     {memcpy (definicion, def_factura, sizeof(Tapart) * apartados);
      bytes_linea = 0;

      for (aux = 0; aux < apartados; aux++, apartado_actual++)
        // Para cada apartado calcula la posici¢n y longitud del mismo
        {if (aux)
           apartado_actual->offset = (apartado_actual-1)->offset +
                                     (apartado_actual-1)->longt;
         else apartado_actual->offset = 0;

         switch (apartado_actual->tipo_dato & ~TRAMA)
           {case CADENA : apartado_actual->longt = apartado_actual->longitud
                                                   + 1;
                          break;
            case ENTERO : apartado_actual->longt = sizeof (int);
                          break;
            case REAL : apartado_actual->longt = sizeof (double);
                        break;
           }
         if (apartado_actual->pos_x == -1)
           apartado_actual->pos_x = (apartado_actual-1)->pos_x +
                                    (apartado_actual-1)->longitud + 1;

         bytes_linea += apartado_actual->longt;
        }

      // inicializa las variables y la memoria
      nro_factura = 0;
      nro_lineas = lineas;
      bytes_total = bytes_linea * lineas;
      linea_actual = datos_factura = (byte *) new byte[bytes_total];
      if (datos_factura)
        {memset (datos_factura, 0, bytes_total);
         nro_ultima_linea = nro_linea_actual = 0;
         nro_apartados = apartados - 1;
         total = 0;
         pos_y = y;
        }
      else
        {delete definicion;
         bytes_total = 0;
        }
     }
   else bytes_total = 0;
  } // Cfactura

Cfactura::~Cfactura (void)
// Destructor
  {
   delete (datos_factura);
   delete (definicion);
  } // ~Cfactura

boolean pascal Cfactura::hay_memoria (void)
// Verifica que haya memoria reservada para las facturas
  {
   if (definicion && datos_factura) return TRUE;
   else return FALSE;
  } // hay_memoria

void pascal Cfactura::limpia_factura (void)
// Borra el contenido de la factura, quitando la "basura" o los datos viejos
  {
   memset (datos_factura, 0, bytes_total);
   linea_actual = datos_factura;
   nro_ultima_linea = nro_linea_actual = 0;
   nro_factura = 0;
   total = 0;
  } // limpia_factura

void pascal Cfactura::imprime_plantilla (void)
// Imprime el esquema de la factura
  {
   unsigned aux;

   if (!definicion) return;

   // Recorre la lista de campos y aumenta en 3 la longitud de cada campo para
   // dejar espacio al imprimir
   for (aux = 0; aux <= nro_apartados; aux++)
     if ((definicion[aux].tipo_dato & ~TRAMA) == REAL)
       definicion[aux].longitud += 3;

   pos_y--;
   // Imprime los nombres de cada campo
   for (aux = 0; aux <= nro_apartados; aux++)
     imprime_cadena (definicion[aux].nombre, definicion[aux].pos_x, pos_y);

   // Dibuja l¡neas en los sitios necesarios
   linea_horz (1, pos_y + 1, definicion[nro_apartados].pos_x +
               definicion[nro_apartados].longitud, (BLUE << 4) | WHITE);
   for (aux = 0; aux <= nro_apartados; aux++)
     {linea_vert (definicion[aux].pos_x - 1, pos_y, nro_lineas + 2,
                  L_SIMPLES | (BLUE<<4) | WHITE);
     }
   pone_caracter ('Ú', definicion[nro_apartados].pos_x - 1,
                  pos_y + nro_lineas + 3, (BLUE << 4) | WHITE);
   pone_caracter ('¿', definicion[nro_apartados].pos_x +
                  definicion[nro_apartados].longitud,
                  pos_y + nro_lineas + 3, (BLUE << 4) | WHITE);
   pone_caracter ('À', definicion[nro_apartados].pos_x - 1,
                  pos_y + nro_lineas + 5, (BLUE << 4) | WHITE);
   pone_caracter ('Ù', definicion[nro_apartados].pos_x +
                  definicion[nro_apartados].longitud,
                  pos_y + nro_lineas + 5, (BLUE << 4) | WHITE);
   imprime_cadena ("Subtotal ³", definicion[nro_apartados].pos_x - 10,
                   pos_y + nro_lineas + 4, (BLUE << 4) | WHITE);
   pone_caracter ('³', definicion[nro_apartados].pos_x +
                  definicion[nro_apartados].longitud,
                  pos_y + nro_lineas + 4, (BLUE << 4) | WHITE);
   linea_horz (definicion[nro_apartados].pos_x, pos_y + nro_lineas + 3,
               definicion[nro_apartados].longitud, (BLUE << 4) | WHITE);
   linea_horz (definicion[nro_apartados].pos_x, pos_y + nro_lineas + 5,
               definicion[nro_apartados].longitud, (BLUE << 4) | WHITE);
   pos_y++;

   // deja la longitud de los campos como estaban originalmente
   for (aux = 0; aux <= nro_apartados; aux++)
     if ((definicion[aux].tipo_dato & ~TRAMA) == REAL)
       definicion[aux].longitud -= 3;
  } // Imprime_plantilla

void pascal Cfactura::imprime_factura (void)
// Imprime los datos de la factura
  {
   unsigned aux, contador = 0;
   char auxc[80], cadena[80];

   if (!definicion || !datos_factura) return;

   imprime_plantilla();

   // Aumenta la longitud de cada campo para dejar espacio al imprimir
   for (aux = 0; aux <= nro_apartados; aux++)
     if (definicion[aux].tipo_dato & ESP_EXTRA)
       definicion[aux].longitud += 2;

   // Recorre la factura e imprime cada dato
   linea_actual = datos_factura;
   while (contador <= nro_ultima_linea)
     {for (aux = 0; aux <= nro_apartados; aux++)
        {switch (definicion[aux].tipo_dato & ~TRAMA)
           {case CADENA : sprintf (cadena, "%s", linea_actual +
                                                 definicion[aux].offset);
                          break;
            case ENTERO : sprintf (cadena, "%d", *(int*)(linea_actual +
                                                     definicion[aux].offset));
                          break;
            case REAL : sprintf (auxc, "%%%d.2f", definicion[aux].longitud);
                        sprintf (cadena, auxc, *(double*)(linea_actual +
                                                 definicion[aux].offset));
                        break;
           }
         imprime_cadena (cadena, definicion[aux].pos_x, pos_y + contador + 1);
        }
      linea_actual += bytes_linea;
      contador++;
     }
   gotoxy (definicion[nro_apartados].pos_x + 1, pos_y + nro_lineas + 4 -
           desplazamiento_vertical);
   sprintf (auxc, "%%%d.2f", definicion[nro_apartados].longitud);
   cprintf (auxc, total);

   // Restaura la longitud de los campos
   for (aux = 0; aux <= nro_apartados; aux++)
     if (definicion[aux].tipo_dato & ESP_EXTRA)
       definicion[aux].longitud -= 2;
  } // imprime_factura

void pascal Cfactura::imprime_factura (byte *datos_a_imprimir, unsigned lineas)
// Imprime los datos pasados como si fueran de una factura
  {
   unsigned aux, contador = 0;
   byte *temp;
   char auxc[80], cadena[80];

   if (!definicion || !datos_a_imprimir) return;

   imprime_plantilla();

   // Aumenta la longitud de los campos
   for (aux = 0; aux <= nro_apartados; aux++)
     if (definicion[aux].tipo_dato & ESP_EXTRA)
       definicion[aux].longitud += 2;

   // Recorre la lista de campos para imprimirlos
   temp = datos_a_imprimir;
   while (contador <= lineas)
     {for (aux = 0; aux <= nro_apartados; aux++)
        {switch (definicion[aux].tipo_dato & ~TRAMA)
           {case CADENA : sprintf (cadena, "%s", temp + definicion[aux].offset);
                          break;
            case ENTERO : sprintf (cadena, "%d", *(int*)(temp +
                                                 definicion[aux].offset));
                          break;
            case REAL : sprintf (auxc, "%%%d.2f", definicion[aux].longitud);
                        sprintf (cadena, auxc, *(double*)(temp +
                                                 definicion[aux].offset));
                        break;
           }
         imprime_cadena (cadena, definicion[aux].pos_x, pos_y + contador + 1);
        }
      temp += bytes_linea;
      contador++;
     }
   gotoxy (definicion[nro_apartados].pos_x + 1, pos_y + nro_lineas + 4 -
           desplazamiento_vertical);
   sprintf (auxc, "%%%d.2f", definicion[nro_apartados].longitud);
   cprintf (auxc, total);

   // Restaura la longitud original de los campos
   for (aux = 0; aux <= nro_apartados; aux++)
     if (definicion[aux].tipo_dato & ESP_EXTRA)
       definicion[aux].longitud -= 2;
  } // imprime_factura


unsigned pascal Cfactura::rellena_factura (Cbase_datos &objeto,
                                           Tmultiplica *multiplica,
                                           Tsaca_datos *lista_campos)
// Pide los datos de una factura y calcula el total
  {char aux[80],
        aux2[80], // Cadenas auxiliares
        cadena[80],
        tecla;   // Tecla pulsada
   byte *temp;   // temporal
   date fecha_aux;
   unsigned nro_ficha = FICHA_INEXISTENTE, // Para b£squeda de fichas
            tempu,
            *puaux, *puaux2;

   if (!definicion || !datos_factura) return ERROR;
   // Si no se ha definido la factura o no hay memoria para los datos salimos
   // dando un error
   temp = new byte[bytes_total]; // Se reserva memoria para la factura
   imprime_factura ();           // Imprime la factura actual
   primer_apartado ();
   primer_linea ();              // Se ubica al inicio de la factura
   memcpy (temp, datos_factura, bytes_linea * nro_lineas);
   // Copia los datos de la factura en el temporal
   getdate (&fecha_aux); // Coge la fecha
   fecha = dostounix (&fecha_aux, &hora_nula); // Pasa la fecha a formato UNIX
   do
     {if (apartado_actual->tipo_dato & ESP_EXTRA)
        gotoxy (apartado_actual->pos_x + 3, pos_y + 2 + nro_linea_actual -
                                            desplazamiento_vertical);
      else
        gotoxy (apartado_actual->pos_x + 1, pos_y + 2 + nro_linea_actual -
                                            desplazamiento_vertical);
      copia (aux, temp + nro_linea_actual * bytes_linea +
             apartado_actual->offset, apartado_actual->tipo_dato, 0, FALSE);
      // Copia los datos del campo a la cadena auxiliar
      switch (apartado_actual->tipo_dato & ~(CALCULA_TOTAL|OCULTO|ESP_EXTRA))
      // Seg£n el tipo de campo se pide un dato u otro
        {case CADENA : // Si es una cadena se lee una cadena
                       tecla = lee_cadena (aux, apartado_actual->longitud + 1,
                                           TRUE, FALSE);
                       break;
         case ENTERO : // Si es un entero, un entero
                       tecla = lee_cadena (aux, apartado_actual->longitud + 1,
                                           FALSE, FALSE);
                       break;
         case REAL : // Si es un real, un real
                     tecla = lee_cadena (aux, apartado_actual->longitud + 1,
                                         TAL_CUAL, FALSE);
                      break;
         case CADENA | COPIA_DATOS :
                            /* Si es una cadena, pero con el 7§ bit a uno,
                               se lee una cadena con el nombre o el c¢digo
                               del art¡culo a facturar, y se seleccionan los
                               datos necesarios de ese art¡culo */
           objeto.campo_numero (lista_campos->nro_campo[0]);
           objeto.datos_campo (aux, tempu, tempu, tempu, tempu, tempu, tempu,
                               tempu);
           objeto.campo_numero (lista_campos->nro_campo[1]);
           objeto.datos_campo (aux2, tempu, tempu, tempu, tempu, tempu, tempu,
                               tempu);
           sprintf (cadena, "%s o %s del %s", aux, aux2,
                    lista_campos->nombre_objeto);
           if (pregunta (cadena, 15, 7, 60, 8, aux, apartado_actual->longitud)
               == TRUE)
             {if (*aux == 0)
                // Si se devuelve una cadena vacia, se piden los datos uno a
                // uno
                {copia (aux, temp + nro_linea_actual * bytes_linea +
                        apartado_actual->offset, apartado_actual->tipo_dato, 0,
                        FALSE);
                 tecla = lee_cadena(aux, apartado_actual->longitud + 1, TRUE,
                                    FALSE);
                 nro_ficha = FICHA_INEXISTENTE;
                 // indicamos que no se ha encontrado la ficha para que no
                 // se saquen los datos de una ficha cualquiera
                }
              else // Sino, si la cadena devuelta comienza con C: se considera
                   // que se indica el c¢digo de un art¡culo
                if (strnicmp (aux, lista_campos->cadena_clave[0], 2) == 0)
                  {nro_ficha = objeto.buscar_ficha (aux + 2,
                                                    lista_campos->nro_campo[0]);
                   if (nro_ficha == FICHA_INEXISTENTE)
                     {mensaje ("El art¡culo no est  en la lista", 255, 255,
                               37, 7);
                      tecla = 0;
                     }
                  }
                else // En el caso de que la cadena comience con N: se considera
                     // que se indica el nombre de un art¡culo
                  if (strnicmp (aux, lista_campos->cadena_clave[1], 2) == 0)
                    {nro_ficha = objeto.buscar_ficha (aux + 2,
                                                      lista_campos->nro_campo[1]);
                     if (nro_ficha == FICHA_INEXISTENTE)
                       {mensaje ("El art¡culo no est  en la lista", 255, 255,
                                  37, 7);
                        tecla = 0;
                       }
                    }
              if (nro_ficha != FICHA_INEXISTENTE && lista_campos)
              // Si se ha encontrado la ficha
                {byte *c_temp;

                 objeto.ficha_numero (nro_ficha); // Se selecciona la ficha
                 puaux = lista_campos->campos_dest;
                 puaux2 = lista_campos->campos_orig;
                 for (tempu = 0; tempu < lista_campos->nro_campos; tempu++)
                   {c_temp = objeto.leer_dato (*(puaux2 + tempu));
                    copia (temp + nro_linea_actual * bytes_linea +
                           definicion[*(puaux + tempu)].offset,
                           c_temp,
                           definicion[*(puaux + tempu)].tipo_dato,
                           definicion[*(puaux + tempu)].longitud, TAL_CUAL);
                   }
                 // Se copian los datos indicados en lista_campos desde la
                 // ficha seleccionada en objeto a la factura
                 imprime_factura (temp, nro_ultima_linea);
                 // Imprime los datos de la factura
                 apartado_siguiente();
                 tecla = 0;
                }
             }
           else tecla = ESC;
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
                    if (apartado_actual->tipo_dato & CALCULA_TOTAL)
                      total -= *(double*)(temp + nro_linea_actual * bytes_linea
                               + definicion[nro_apartados].offset);
                    // copia los datos de la l¡nea actual a la factura
                    copia (temp + nro_linea_actual * bytes_linea +
                           apartado_actual->offset, aux,
                           apartado_actual->tipo_dato, 0, TRUE);
                    if (apartado_actual->tipo_dato & CALCULA_TOTAL)
                    // Calcula el total de la factura
                      {if (multiplica)
                        // Si hay que multiplicar coge los datos de los campos
                        // respectivos
                         {*(double*)(temp + nro_linea_actual * bytes_linea +
                          definicion[multiplica->campo_total].offset) =
                            *(double*)(temp + nro_linea_actual * bytes_linea +
                             definicion[multiplica->campo_1].offset) *
                            *(double*)(temp + nro_linea_actual * bytes_linea +
                             definicion[multiplica->campo_2].offset);
                         }
                       total += *(double *)(temp + nro_linea_actual * bytes_linea
                                + definicion[nro_apartados].offset);
                       imprime_factura (temp, nro_ultima_linea);
                      }
                    // Seg£n donde se est‚ pasa o no al siguiente campo, a la
                    // siguiente l¡nea o sale de la funci¢n
                    if (apartado_siguiente() == FIN_DE_LISTA)
                      {primer_apartado();
                       if (nro_linea_actual < nro_lineas - 1)
                         {nro_linea_actual++;
                          if (nro_ultima_linea < nro_lineas - 1)
                            nro_ultima_linea++;
                         }
                       else {tecla = CAMBIO;
                             primer_apartado();
                             primer_linea();
                            }
                      }
                    break;

         case S_TAB : // Similar a la pulsaci¢n de TAB pero para atr s
                    if (apartado_actual->tipo_dato & CALCULA_TOTAL)
                      total -= *(double*)(temp + nro_linea_actual * bytes_linea
                               + definicion[nro_apartados].offset);
                    copia (temp + nro_linea_actual * bytes_linea +
                           apartado_actual->offset, aux,
                           apartado_actual->tipo_dato, 0, TRUE);
                    if (apartado_actual->tipo_dato & CALCULA_TOTAL)
                      {if (multiplica)
                         {*(double*)(temp + nro_linea_actual * bytes_linea +
                          definicion[multiplica->campo_total].offset) =
                          *(double*)(temp + nro_linea_actual * bytes_linea +
                          definicion[multiplica->campo_1].offset) *
                          *(double*)(temp + nro_linea_actual * bytes_linea +
                          definicion[multiplica->campo_2].offset);
                         }
                       total += *(double *)(temp + nro_linea_actual * bytes_linea
                                + definicion[nro_apartados].offset);
                       imprime_factura (temp, nro_ultima_linea);
                      }
                    if (apartado_anterior() == FIN_DE_LISTA)
                      {ultimo_apartado();
                       if (nro_linea_actual > 0)
                         nro_linea_actual--;
                       else {tecla = CAMBIO;
                             primer_linea();
                             primer_apartado();
                            }
                      }
                    break;
        }
     }
   // Repite todo mientras no se pulse ESC, CTRL-ENTER, o se tenga que cambiar
   // de bloque de factura
   while (tecla != ESC && tecla != LF && tecla != CAMBIO);
   // Si se pulsa CTRL-ENTER o se cambia de bloque se guardan los datos, sino
   // se pierden
   if (tecla == LF || tecla == CAMBIO)
     memcpy (datos_factura, temp, bytes_total);
   delete temp;
   return tecla;
  } // rellena_factura

void pascal Cfactura::primer_apartado (void)
// Pasa al primer aparatado de la factura
  {
   apartado_actual = definicion;
  } // primer_apartado

void pascal Cfactura::ultimo_apartado (void)
// Pasa al £ltimo apartado de la factura
  {
   apartado_actual = definicion + nro_apartados;
  } // ultimo_apartado

unsigned pascal Cfactura::apartado_siguiente (void)
// Pasa al siguiente apartado de la factura
  {
   if (apartado_actual < definicion + nro_apartados)
     {apartado_actual++;
      return OK;
     }
   else
     return FIN_DE_LISTA;
  } // apartado_siguiente;

unsigned pascal Cfactura::apartado_anterior (void)
// Pasa al apartado anterior de la factura
  {
   if (apartado_actual > definicion)
     {apartado_actual--;
      return OK;
     }
   else return FIN_DE_LISTA;
  } // apartado_anterior

void pascal Cfactura::primer_linea (void)
// Pasa a la primera l¡nea de los datos de la factura
  {
   linea_actual = datos_factura;
   nro_linea_actual = 0;
  } // primer_linea

void pascal Cfactura::ultima_linea (void)
// Pasa a la £ltima l¡nea de los datos de la factura
  {
   linea_actual = datos_factura + nro_ultima_linea * bytes_linea;
   nro_linea_actual = nro_ultima_linea;
  } // ultima_linea

unsigned pascal Cfactura::linea_siguiente (void)
// Pasa a la l¡nea de datos siguiente
  {
   if (nro_linea_actual < nro_ultima_linea)
     {linea_actual += bytes_linea;
      nro_linea_actual++;
      return OK;
     }
   else return FIN_DE_LISTA;
  } // linea_siguiente

unsigned pascal Cfactura::linea_anterior (void)
// Pasa a la l¡nea de datos anterior
  {
   if (nro_linea_actual > 0)
     {linea_actual -= bytes_linea;
      nro_linea_actual--;
      return OK;
     }
   else return FIN_DE_LISTA;
  } // linea_anterior


unsigned pascal Cfactura::imprime_factura_impresora (TFtemporal *f_salida)
// Saca la factura por la impresora
  {unsigned contador = 0;
   unsigned aux;
   char cadena[120], auxc[40];

   if (!definicion) return ERROR;

   memset (cadena, ' ', 80);
   cadena[80] = 0;
   // Imprime la l¡nea con los nombres de los campos
   for (aux = 0; aux <= nro_apartados; aux++)
     {gotoxy_text (cadena, "|", definicion[aux].pos_x - 1);
      gotoxy_text (cadena, definicion[aux].nombre, definicion[aux].pos_x);
     }
   envia_datos (cadena, f_salida);
   memset (cadena, 0, 90);
   memset (cadena, '-', definicion[nro_apartados].pos_x +
                        definicion[nro_apartados].longitud);
   for (aux = 0; aux <= nro_apartados; aux++)
     gotoxy_text (cadena, "+", definicion[aux].pos_x - 1);
   strcat (cadena, "---\r\n");
   envia_datos (cadena, f_salida);

   memset (cadena, ' ', 80);
   cadena[80] = 0;

   primer_linea();
   // Recorre los datos de la factura, imprimiendo cada uno.
   do
     {memset (cadena, ' ', 80);
      cadena[80] = 0;
      for (aux = 0; aux <= nro_apartados; aux++)
        {gotoxy_text (cadena, "|", definicion[aux].pos_x - 1);
         switch (definicion[aux].tipo_dato & ~TRAMA)
           {case CADENA :
               gotoxy_text (cadena, linea_actual + definicion[aux].offset,
                             definicion[aux].pos_x);
               break;
            case ENTERO :
               sprintf (auxc, "%d", *(int*)(linea_actual + definicion[aux].offset));
               gotoxy_text (cadena, auxc, definicion[aux].pos_x);
               break;
            case REAL :
               if (contador <= nro_ultima_linea)
                {char auxc2[80];
                 if (definicion[aux].tipo_dato & ESP_EXTRA)
                   sprintf (auxc2, "%%%d.2f", definicion[aux].longitud + 2);
                 else
                   sprintf (auxc2, "%%%d.2f", definicion[aux].longitud);
                 sprintf (auxc, auxc2, *(double*)(linea_actual +
                                                  definicion[aux].offset));
                 gotoxy_text (cadena, auxc, definicion[aux].pos_x);
                }
               break;
           }
        }
      envia_datos (cadena, f_salida);
      linea_actual += bytes_linea;
      contador++;
     }
   while (contador < nro_lineas);

   envia_datos ("\r\n", f_salida);

   // Imprime el subtotal del bloque actual.
   for (contador = 2; contador <= definicion[nro_apartados].pos_x; contador ++)
     fputc (' ', f_salida->fichero);
   envia_datos ("+------------+\r\n", f_salida);

   for (contador = 2; contador <= definicion[nro_apartados].pos_x - 4;
        contador ++)
     fputc (' ', f_salida->fichero);
   sprintf (auxc, "Pts |%12.2f|\r\n", total);
   envia_datos (auxc, f_salida);

   for (contador = 2; contador <= definicion[nro_apartados].pos_x; contador ++)
     fputc (' ', f_salida->fichero);
   envia_datos ("+------------+\r\n", f_salida);

  return OK;
  } // imprime_factura_impresora