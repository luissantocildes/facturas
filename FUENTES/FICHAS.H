/****************************************************************************
 * FICHAS.H                                                                 *
 * Fichero de cabecera que contiene las estructuras y los objetos necesarios*
 * para crear una especie de base de datos. Permite definir los campos de   *
 * las fichas de forma flexible, es decir que la estructura de datos es     *
 * din mica, pudiendose cambiar durante la ejecuci¢n del programa.          *
 * Est  realizada de forma que se puedan utilizar varias listas a la vez.   *
 ****************************************************************************
 * Fecha realizaci¢n : 5/4/96                                               *
 * Fecha terminaci¢n : 26/7/96                                              *
 * (C)1996 Luis Fernando Santocildes                                        *
 ****************************************************************************/

// Ficheros de cabecera necesarios
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definici¢n de los criterios de ordenaci¢n de las fichas
#define ASCENDENTE 1
#define DESCENDENTE 2

/*************************ESTRUCTURAS Y OBJETOS******************************/
typedef struct t_campo
// Estructura usada para definir un campo de la ficha
  {unsigned codigo;   // Identifica el campo con respecto a otros.
   char nombre[30]; // Es el que se muestra al imprimir la ficha
   unsigned longitud, // Longitud del campo en pantalla
            longt,    // Longitud del campo en memoria
            offset,   // Offset del campo dentro de la ficha
            tipo_dato,// Tipo de datos del campo
            pos_x, pos_y; // Posici¢n del campo dentro de la ficha
  } Tcampo;

class Cficha
/* Clase que se usa para definir los campos de una ficha
   Los diferentes campos de la ficha se manejan mediante una lista de campos.
   Cada campo almacena un c¢digo que lo identifica, una cadena que es la que
   se imprime en pantalla o impresora, un identificador del tipo de dato, la
   longitud m xima del campo y la posici¢n del campo en pantalla. Tambi‚n se
   almacenan la posici¢n del campo siguiente y del campo anterior para el
   manejo de la lista, ya que esta es una lista doblemente enlazada
*/
  {protected:
   Tcampo *lista_campos,  // Puntero a la lista de campos
          *campo_actual;  // Puntero al campo actual

   unsigned long_fich,
     // Espacio en bytes m ximo que ocupan los datos de cada ficha
            nro_campos;
     // N£mero de campos que hay en la definici¢n

   Tcampo *_buscar_campo (char *nombre_campo);
     // Busca un campo y devuelve un puntero a el
   Tcampo *_buscar_campo (unsigned numero_campo);
     // Idem funci¢n anterior
   Tcampo *_primer_campo (void) // Busca el primer campo de la lista
      {
       campo_actual = lista_campos;
       return campo_actual;
      }
   Tcampo *_ultimo_campo (void); // Busca el £ltimo campo de la lista

   public:
   Cficha (void)   // Constructor, inicializa los datos de la lista de campos
     {lista_campos = NULL;
      campo_actual = NULL;
      nro_campos = long_fich = 0;
     };
   ~Cficha (void)  // Destructor
     {borrar_definicion ();
     }
   int pascal borrar_definicion (void);   // Borra la lista de campos
   int pascal nuevo_campo (char *nombre, int longitud, int tipo_dato, int pos_x,
                           int pos_y); // A¤ade un campo a la ficha
   int pascal borrar_campo (char *nombre);   // Borra el 1er campo que tenga el mismo
                                      // nombre
   int pascal borrar_campo (int codigo);    // Borra el campo que tenga el mismo n§
                                      // de c¢digo
   unsigned pascal buscar_campo (char *nombre);   // Busca un campo por el nombre
   char *pascal buscar_campo (int codigo);   // Da el nombre de un campo
   unsigned pascal primer_campo (void);  // Ubica el puntero campo_actual en el 1er campo
   unsigned pascal ultimo_campo (void);  // Idem pero en el £ltimo
   unsigned pascal campo_siguiente (void); // Pasa al siguiente campo de la lista
   unsigned pascal campo_anterior (void); // Pasa al campo anterior
   unsigned pascal campo_numero (unsigned numero);
     // Se ubica sobre un campo determinado
   int pascal datos_campo (char *nombre, unsigned &longitud, unsigned &longt,
                    unsigned &offset, unsigned &tipo_dato, unsigned &pos_x,
                    unsigned &pos_y, unsigned &nro_campo);
     // Devuelve informaci¢n sobre un campo determinado
   int pascal existe_campo (char *nombre_campo);
   int pascal existe_campo (int codigo);
     // Verifica que exista un campo determinado
   int pascal hay_campos (void) {return nro_campos;}; // Indica si hay campos
   int pascal igualar_codigos(void); // No desarrollada.
  };

class Cbase_datos : public Cficha
/* Clase que se utiliza para definir la base de datos. Utiliza como base la
   clase Cficha para estructurar los campos de cada ficha.
*/
  {byte *lista_fichas, // Puntero a la lista de las fichas
        *ficha_actual; // Puntero a la ficha tratada actualmente
   unsigned tam_memoria,  // Bytes reservados para las fichas
            nro_fichas,   // N§ actual de fichas
            max_fichas;   // N§ m ximo de fichas
   Tnodo_arbol *arbol;    // Arbol usado para ordenar las fichas por un campo

   void pascal limpia_datos (byte *ficha); // Limpia los datos de una ficha
   void pascal insertar_ficha (byte *ficha); // Inserta una ficha en la lista
   int pascal meter_datos (void *datos, unsigned nro_ficha);
                                           // Introduce datos en una ficha
   public:
   char nombre_fichero[MAXPATH]; // Fichero donde se almacenan los datos
   unsigned nro_ficha_actual, // N§ de la ficha actual
            ordenada_por, // Campo por el que est n ordenadas (65535 significa
                          // que no est n ordenados)
            modificado,   // Indica si se ha modificado la lista.
            *lista_ordenada; // Puntero a la lista ordenada

   Cbase_datos (void)
     {ficha_actual = lista_fichas = new byte[32000]; // Reseva 32 Kb.
      nro_fichas = max_fichas = 0;
      lista_ordenada = NULL;
      arbol = NULL;
      ordenada_por = SIN_ORDENAR;
      nro_ficha_actual = 65535;
      modificado = FALSE;
      nombre_fichero[0] = 0;
      tam_memoria = 32000;
     }; // Constructor, inicializa la base de datos

   Cbase_datos (unsigned memoria)
     {ficha_actual = lista_fichas = new byte[memoria];
      nro_fichas = max_fichas = 0;
      lista_ordenada = NULL;
      arbol = NULL;
      ordenada_por = SIN_ORDENAR;
      nro_ficha_actual = 65535;
      modificado = FALSE;
      nombre_fichero[0] = 0;
      tam_memoria = memoria;
     } // 2§ Constructor, reserva tantos bytes de memoria como se le indiquen

   Cbase_datos (Tcampo *def_lista_campos, unsigned largo_lista,
                unsigned memoria = 32000)
   // 3er constructor, reserva memoria y crea una lista de campos seg£n la
   // definici¢n pasada.
     {int f;

      // Reserva memoria para la lista e inicializa la misma
      ficha_actual = lista_fichas = new byte[memoria];
      nro_fichas = max_fichas = 0;
      nro_ficha_actual = 65535;
      modificado = FALSE;
      nombre_fichero[0] = 0;
      tam_memoria = memoria;

      // Reserva memoria para los campos y copia la definici¢n de los mismos
      campo_actual = lista_campos = new Tcampo[largo_lista];
      memcpy (lista_campos, def_lista_campos, largo_lista * sizeof(Tcampo));
      nro_campos = largo_lista;
      lista_campos->codigo = lista_campos->offset = 0;
      long_fich = 0;
      // Calcula la posici¢n relativa de los campos dentro de la ficha
      for (f = 0; f < nro_campos; f++)
        {if (f > 0)
           lista_campos[f].offset = lista_campos[f-1].offset +
                                    lista_campos[f-1].longt;
         else lista_campos[f].offset = 0;
         lista_campos[f].codigo = f;
         switch (lista_campos[f].tipo_dato)
           {case TITULO : lista_campos[f].longt = 0;
                          break;
            case REAL : lista_campos[f].longt = sizeof(double);
                        break;
            case ENTERO : lista_campos[f].longt = sizeof(int);
                          break;
            case FECHA :
            case HORA : lista_campos[f].longt = sizeof(int);
                        break;
            case CADENA : lista_campos[f].longt = lista_campos[f].longitud + 1;
                          break;
           };
         long_fich += lista_campos[f].longt;
        }

      calcula_max_fichas();
      lista_ordenada = new unsigned[max_fichas];
      ordenada_por = SIN_ORDENAR;
      arbol = NULL;
     };

   ~Cbase_datos (void)
      {borrar_todas_fichas ();
       delete lista_ordenada;
      }    // Destructor

   void calcula_max_fichas (void)
      {if (long_fich)
         max_fichas = tam_memoria / long_fich;
      };  // Calcula el m ximo de fichas que se pueden poner en la lista.
   int pascal hay_fichas (void)
       {return nro_fichas;}  // ¨Hay fichas en la lista?
   int pascal nueva_ficha (void); // Crea una ficha nueva
   int pascal borrar_ficha (unsigned pos_ficha);  // Borra una ficha
   void pascal borrar_todas_fichas (void);         // Borra todas las fichas
   unsigned pascal buscar_ficha (void *dato, unsigned campo_a_buscar,
                          unsigned pos_inicial = 0);
                         // Busca una ficha por un determinado campo
   unsigned pascal busqueda_binaria (void *dato, unsigned pos_inicial = 0,
                                     unsigned que_devuelvo = INDICE);
                   // Busca una ficha por el campo que est  ordenada la lista
   unsigned pascal sig_ficha (void); // Pasa a la ficha siguiente a la actual
   unsigned pascal ant_ficha (void); // Pasa a la ficha anterior
   void pascal primer_ficha (void); // Salta a la primer ficha de la lista
   unsigned pascal ultima_ficha (void);  // Salta a la £ltima ficha de la lista
   int pascal ficha_numero (unsigned numero); // Salta a la ficha indicada
   int pascal ordenar_fichas (unsigned campo_indice);
                          // Ordena las fichas por un campo seg£n el criterio
   void pascal tipodato (unsigned nro_campo, unsigned *tipo_dato);
                                      // Devuelve el tipo de dato de un campo
   byte * pascal leer_dato (unsigned nro_campo);
                // Devuelve un puntero al dato de un campo en la ficha actual
   int pascal escribir_dato (void *dato, unsigned nro_campo);
                           // Modifica un dato determinado en la ficha actual
   int pascal imprimir_ficha (int destino = PANTALLA);
                            // Muestra una ficha, en principio en la pantalla
   int pascal imprimir_solo_campos (void);
                                // Muestra el esquema de la ficha en pantalla
   int pascal rellenar_ficha (int nueva);
                                 // Muestra una ficha y espera que se rellene
   int pascal grabar_fichero (void);
                                              // Graba la lista en un fichero
   int pascal leer_fichero (void);
                                // Lee de disco el fichero de la lista actual

   // Esta funciones siguientes sirven para ordenar los datos seg£n un campo
   // determinado.
   unsigned pascal inserta_nodo (byte *dato, unsigned offset,
                          unsigned tipo_dato, unsigned contador);
   unsigned pascal saca_menor_dato (void);
  };

void pascal copia (byte *destino, byte *origen, unsigned tipo_dato,
                   unsigned longitud, boolean guarda);
      // Copia los datos de origen a destino, transformando el tipo de dato si
      // es necesario.


/************************DESARROLLO DE FUNCIONES*****************************/
Tcampo *Cficha::_buscar_campo (char *nombre_campo)
// Busca un campo por el nombre y devuelve un puntero a el
  {Tcampo *aux;
   unsigned contador = 0;

   if (!lista_campos) return NULL; // Si no hay campos salimos de la funci¢n
   aux = lista_campos;  // Apunta al primer campo de la lista
   do
     {if (stricmp (nombre_campo, aux->nombre))
        {aux++;
         contador++;
        }
      else break;
     }
   while (contador < nro_campos);
    // Mientras el nombre del campo buscado y el del campo actual no
    // coincidan, y mientras no nos salgamos de la lista se sigue buscando

   if (contador)
     {campo_actual = aux;
      return aux;
     } // Si no se ha salido de la lista se devuelve un puntero al campo
   else return NULL;
  } // _buscar_campo

Tcampo *Cficha::_buscar_campo (unsigned numero_campo)
// Busca un campo por el n£mero de campo y devuelve un puntero a el
  {Tcampo *aux;
   unsigned contador = 0;

   if (!hay_campos ()) return NULL; // Si no hay campos salimos de la funci¢n
   aux = lista_campos;   // Apunta al primer campo de la lista
   while (contador < nro_campos && aux->codigo != numero_campo)
   // Mientras no se llegue al final de la lista y el campo buscado no
   // se encuentra
     {aux++; // Pasamos al siguiente campo
      contador++;
     }
   if (contador == nro_campos)
     return NULL;
     // Si no se ha encontrado el campo buscado se devuelve un puntero nulo
   else // Si se ha encontrado el campo buscado
     {campo_actual = aux; // actualizamos el campo actual
      return aux;         // devolvemos un puntero al campo
     }
  } // _buscar_campo

Tcampo *Cficha::_ultimo_campo (void)
// Busca el £ltimo campo de la lista
  {
   if ((campo_actual = lista_campos) == NULL) return NULL;
     // Si no hay campos salimos de la funci¢n, sino apuntamos al primer
     // campo de la lista
   campo_actual = lista_campos + (nro_campos - 1);
   return campo_actual;
  } // _ultimo_campo

int pascal Cficha::datos_campo (char *nombre, unsigned &longitud, unsigned &longt,
                         unsigned &offset, unsigned &tipo_dato,
                         unsigned &pos_x, unsigned &pos_y, unsigned &nro_campo)
// Devuelve los datos del campo actual
  {
   if (!hay_campos())
     return LISTA_CAMPOS_VACIA;
     // Si el campo a mirar no existe, nos vamos.

   strcpy (nombre, campo_actual->nombre);
   longitud = campo_actual->longitud;
   longt = campo_actual->longt;
   offset = campo_actual->offset;
   tipo_dato = campo_actual->tipo_dato;
   pos_x = campo_actual->pos_x;
   pos_y = campo_actual->pos_y;
   nro_campo = campo_actual->codigo;
   // Sino se recogen los datos del mismo
   return OK;
  } // datos_campo

int pascal Cficha::borrar_definicion (void)
// Borra la lista de campos de la ficha.
// Recorre toda la lista de campos liberando el espacio de memoria que ocupa
// cada ficha y sus datos.
  {
   if ((campo_actual = lista_campos) == NULL) return LISTA_CAMPOS_VACIA;
     // Si no hay campos salimos de la funci¢n, sino apuntamos al primer
     // campo de la lista
   delete lista_campos;
     // Se elimina la lista de campos
   campo_actual = lista_campos = NULL;
   nro_campos = 0;
   long_fich = 0;
   return OK;
  } // borrar_definicion

int pascal Cficha::nuevo_campo (char *nombre, int longitud, int tipo_dato, int pos_x,
                         int pos_y)
// A¤ade un campo a la ficha
  {Tcampo *nuevo_campo;

   nuevo_campo = new Tcampo[nro_campos + 1];
   if (nuevo_campo)          // Si se ha podido guardar memoria
     {memcpy (nuevo_campo, lista_campos, nro_campos * sizeof(Tcampo));
      campo_actual = nuevo_campo + nro_campos;
      switch (tipo_dato & ~TRAMA)
        // Se calcula la longitud en memoria que ocupa el campo
        {case TITULO : campo_actual->longt = 0;
                       break;
         case REAL : campo_actual->longt = sizeof(double);
                     break;
         case ENTERO : campo_actual->longt = sizeof(int);
                       break;
         case FECHA :
         case HORA : campo_actual->longt = sizeof(int);
                     break;
         case CADENA : campo_actual->longt = longitud + 1;
                       break;
         default : delete nuevo_campo;
                   campo_actual = lista_campos;
                   return TIPO_CAMPO_INVALIDO;
        };
      strcpy (campo_actual->nombre, nombre); // Se copia el nombre del campo
      campo_actual->longitud = longitud + 1;
      // En longitud se guarda la longitud del campo en caracteres, se le
      // a¤ade uno para tener espacio para el 0 final de las cadenas
      long_fich += campo_actual->longt;
         // Se incrementa el tama¤o total de la ficha
      campo_actual->codigo = nro_campos;
      campo_actual->tipo_dato = tipo_dato;
      campo_actual->pos_x = pos_x;
      campo_actual->pos_y = pos_y;
         // Se copian los datos pasados al nuevo campo
      if (nro_campos != 0)
        campo_actual->offset = (campo_actual-1)->offset +
                               (campo_actual-1)->longt;
      else campo_actual->offset = 0;
        // Se calcula el offset del nuevo campo dentro de la ficha
      nro_campos++;
      if (nro_campos != 1)
        {delete lista_campos;
         lista_campos = nuevo_campo;
        }
      else
        lista_campos = nuevo_campo;
      return OK;
     }
   else                      // Si no hay memoria para el nuevo campo
     return MEMORIA_AGOTADA; // se devuelve un error
  } // nuevo_campo

int pascal Cficha::borrar_campo (char *nombre)
// Borra el 1er campo que tenga el mismo nombre
  {Tcampo *aux1, *aux2;
   unsigned f = 0;

   aux1 = _buscar_campo (nombre);
   if (aux1)  // Si se ha encontrado el campo
     {// Se copia la definici¢n de todos los campos menos el que se va a borrar
      // a otra posici¢n de memoria
      aux2 = new Tcampo[nro_campos - 1];
      campo_actual = lista_campos;
      while (campo_actual != aux1)
        aux2[f++] = *campo_actual++;
      campo_actual++;
      while (f < nro_campos)
        {aux2[f] = *campo_actual++;
         aux2[f++].offset -= aux1->longt;
        }

      long_fich -= aux1->longt;
      nro_campos--;
      delete lista_campos;          // Libera la memoria ocupada por el campo
      lista_campos = aux2;
      return OK;
     }
   else return CAMPO_INEXISTENTE;  // Si no se ha encontrado el campo se
                                   // devuelve un error
  } // borrar_campo

int pascal Cficha::borrar_campo (int codigo)
// Borra el campo que tenga el mismo n§ de c¢digo
  {Tcampo *aux1, *aux2;
   unsigned f = 0;

   aux1 = _buscar_campo (codigo);
   if (aux1)  // Si se ha encontrado el campo
     {aux2 = new Tcampo[nro_campos - 1];   // Reserva memoria para la nueva lista
      campo_actual = lista_campos;
      while (campo_actual != aux1)
        aux2[f++] = *campo_actual++;
      campo_actual++;
      while (f < nro_campos)
        {aux2[f] = *campo_actual++;
         aux2[f++].offset -= aux1->longt;
        }
        // Copia toda la lista de campos menos el campo a borrar
      long_fich -= aux1->longt;
      nro_campos--;
      delete lista_campos;          // Libera la memoria ocupada por el campo
      lista_campos = aux2;
      return OK;
     }
   else return CAMPO_INEXISTENTE;  // Si no se ha encontrado el campo se
                                   // devuelve un error
  } // borrar_campo

unsigned pascal Cficha::buscar_campo (char *nombre)
// Busca un campo por el nombre
  {Tcampo *aux;

   aux = _buscar_campo (nombre); // Buscamos el campo
   if (aux)                      // Si se ha encontrado devolvemos el
     return aux->codigo;         // codigo del campo
   else return LISTA_VACIA;
  } // buscar_campo

char * pascal Cficha::buscar_campo (int codigo)
// Da el nombre de un campo
  {Tcampo *aux;

   aux = _buscar_campo (codigo);  // Buscamos el campo
   if (aux)                       // Si lo encontramos, se devuelve un puntero
     return aux->nombre;          // al nombre del campo
   else return NULL;
  } // buscar_campo

unsigned pascal Cficha::primer_campo (void)
// Ubica el puntero campo_actual en el 1er campo
  {
   if (lista_campos)  // Si hay campos en la lista
     {campo_actual = lista_campos;  // Apuntamos al primer campo
      return lista_campos->codigo;  // y devolvemos su codigo
     }
   else return 0;  // Sino devolvemos 0
  } // primer_campo

unsigned pascal Cficha::ultimo_campo (void)
// Idem pero en el £ltimo
  {
   if (lista_campos)                // Si hay campos en la lista
     {campo_actual = lista_campos + (nro_campos - 1);
      return campo_actual->codigo;   // Devolvemos el c¢digo
     }
   else return 0;  // Sino devolvemos 0
  } // ultimo_campo

unsigned pascal Cficha::campo_siguiente (void)
// Pasa al siguiente campo de la lista
  {
   if (lista_campos)                   // Si hay campos
     if (campo_actual != lista_campos + (nro_campos - 1))
                                    // y si no estamos ya en el £ltimo campo
       {campo_actual++;             // pasamos a el siguiente
        return campo_actual->codigo;   // y devolvemos su codigo
       }
     else return FIN_DE_LISTA;         // Sino devolvemos un error
   else return LISTA_VACIA;
  } // campo_siguiente

unsigned pascal Cficha::campo_anterior (void)
// Pasa al campo anterior
  {
   if (lista_campos)                    // Si hay campos
     if (campo_actual != lista_campos)  // y no estamos en el primero
       {campo_actual--;                 // pasamos al anterior
        return campo_actual->codigo;    // y devolvemos su codigo
       }
     else return FIN_DE_LISTA;          // Sino devolvemos un error
   else return LISTA_VACIA;
  } // campo_anterior

unsigned pascal Cficha::campo_numero (unsigned numero)
// Pasa a un campo determinado
  {
   if (lista_campos)
     if (numero <= nro_campos)
       {campo_actual = lista_campos + (numero - 1);
        return OK;
       }
     else return CAMPO_INEXISTENTE;
   else return LISTA_CAMPOS_VACIA;
  } // campo_numero

int pascal Cficha::existe_campo (char *nombre_campo)
// Indica si existe un campo determinado
  {
   if (buscar_campo (nombre_campo))
     return TRUE;
   else return FALSE;
  } // existe_campo

int pascal Cficha::existe_campo (int codigo)
// Indica si existe un campo determinado
  {
   if (buscar_campo (codigo))
     return TRUE;
   else return FALSE;
  } // existe_campo

void pascal Cbase_datos::limpia_datos (byte *ficha)
  {
   memset (ficha, 0, long_fich);
  } // limpia_datos

void pascal Cbase_datos::insertar_ficha (byte *ficha)
// Inserta una ficha al final de la lista
  {byte *aux;

   if (nro_fichas < max_fichas) // Si queda espacio
     {aux = lista_fichas + long_fich * nro_fichas ; // Se ubica al final de la lista
      memcpy (aux, ficha, long_fich); // copia los datos
      ordenada_por = SIN_ORDENAR;     // Actualiza variables
      ficha_actual = aux;
      nro_ficha_actual++;
      nro_fichas++;
     }
  } // insertar_ficha

int pascal Cbase_datos::meter_datos (void *datos, unsigned nro_ficha)
// Introduce datos en la ficha pasada
  {byte *aux = lista_fichas;

   if (!hay_campos()) return LISTA_CAMPOS_VACIA;
   if (!nro_fichas) return LISTA_FICHAS_VACIA;
   if (nro_ficha > nro_fichas) return FICHA_INEXISTENTE;
   // Verifica que haya campos, fichas o que la ficha exista
   aux += long_fich * nro_ficha;
   ordenada_por = SIN_ORDENAR;
   limpia_datos (aux);             // Borra los viejos datos de la ficha
   memcpy (aux, datos, long_fich); // Introduce los nuevos
   return OK;
  } // meter_datos

int pascal Cbase_datos::nueva_ficha (void)
// Crea una ficha nueva
  {byte *nueva_ficha;

   if (hay_campos())                          // Si hay campos definidos y
     if (nro_fichas < max_fichas)  // si queda sitio para mas fichas
       {nro_fichas++;
        if (nro_ficha_actual == 65535)
          nro_ficha_actual = 0;
        else nro_ficha_actual++;
        ficha_actual = nueva_ficha = lista_fichas + (nro_fichas-1) * long_fich;
                       // Apunta a la nueva ficha
        limpia_datos (nueva_ficha);  // Limpia la ficha de basura
        ordenada_por = SIN_ORDENAR;
        return OK;
       }
     else return MEMORIA_AGOTADA;
           // Si no hay sitio para la nueva ficha se devuelve un error
   else return LISTA_CAMPOS_VACIA;
   // Si no hay ninguna definici¢n de campos se devuelve un error
  } // nueva_ficha

int pascal Cbase_datos::borrar_ficha (unsigned pos_ficha)
// Borra una ficha determinada
  {byte *aux;

   pos_ficha--;
   if (hay_campos() && nro_fichas)  // Si hay fichas y hay campos definidos,
     if (pos_ficha < nro_fichas)    // si la ficha a borrar existe
       {aux = lista_fichas + pos_ficha * long_fich;
          // apuntamos a la ficha a borrar
        memcpy (aux, aux + long_fich, tam_memoria - pos_ficha * long_fich);
        if (aux < ficha_actual) // || nro_ficha_actual == nro_fichas)
                       // Si la ficha a borrar es anterior a la ficha actual
          {ficha_actual -= long_fich; // o es la £ltima, se desplaza el
           nro_ficha_actual--;        // puntero una ficha hacia atr s
          }
        nro_fichas--;       // Decrementamos el n§ de fichas
        ordenada_por = SIN_ORDENAR;
        modificado = TRUE;
        if (nro_ficha_actual == 0)
          nro_ficha_actual = 65535;
        return OK;
       }
     else return FICHA_INEXISTENTE; // Si no existe la ficha...
   else if (!hay_campos())          // Si no hay campos definidos
          return LISTA_CAMPOS_VACIA;
        else return LISTA_FICHAS_VACIA; // o si no hay fichas se devuelve un
                                        // error
  } // borrar_ficha

void pascal Cbase_datos::borrar_todas_fichas (void)
// Borra todas las fichas
  {
   nro_fichas = 0;                  // Se eliminan las fichas
   nro_ficha_actual = 65535;
   ficha_actual = lista_fichas;     // Se apunta a la primer posicion
   memset (lista_fichas, 0, tam_memoria); // Se borra el contenido
   ordenada_por = SIN_ORDENAR;      // Se borra el criterio de ordenaci¢n
  } // borrar_todas_fichas

unsigned pascal Cbase_datos::buscar_ficha (void *dato, unsigned campo_a_buscar,
                                    unsigned pos_inicial)
/* Busca una ficha por un determinado campo
   dato : Puntero al dato a buscar
   campo_a_buscar : campo donde buscar el dato
   pos_inicial : ficha a partir de la que hay que buscar el dato
*/
  {byte *aux, *dato_aux;
   unsigned encontrado = FALSE, contador;
   unsigned offset = 0, tipo_dato, longitud;

   if (!nro_fichas || !hay_campos() || nro_campos < campo_a_buscar
       || pos_inicial > nro_fichas)
     return FICHA_INEXISTENTE;
            // Si no hay fichas, no hay campos, si la ficha inicial es
            // mayor que el n§ de fichas o el campo a buscar es mayor que el
            // n§ de campos
   /* Ahora se busca el campo por el que se ba a buscar el dato recorriendo la
      lista de campos, a la vez se van sumando las longitudes de los campos
      para saber la posici¢n del dato dentro del bloque de datos */
   campo_actual = lista_campos + (campo_a_buscar - 1);
   offset = campo_actual->offset;
   tipo_dato = campo_actual->tipo_dato;
   longitud = campo_actual->longt;
     // memorizamos la longitud y el tipo de dato para acceder m s r pidamente
     // a ellos
   contador = pos_inicial;
   aux = lista_fichas + pos_inicial * long_fich;
     // Se busca desde la ficha indicada

   /* nos ponemos a buscar el bloque de memoria donde est  el dato buscado.
      En dato tenemos un puntero al dato a buscar y (aux->datos + offset)
      nos dar  un puntero al dato de la ficha actual a comparar. Como ambos
      punteros son de tipo void (puntero a cualquier cosa) han de ser
      convertidos al tipo de dato a buscar con un "casting", luego se hace
      la comparaci¢n, si los datos son iguales se considera que se ha
      encontrado la ficha */
   dato_aux = new byte[campo_actual->longt];
   copia (dato_aux, (byte *)dato, campo_actual->tipo_dato,
          campo_actual->longt, TRUE);
   // Se reserva memoria y se transforma el dato del formato ASCII al que
   // haga falta.
   do
     {switch (tipo_dato)
       // Seg£n el tipo de dato se hacen diferentes tipos de comparaciones
        {case CADENA :
            if (strstr ((char *)(aux + offset), (char *)dato_aux) != NULL)
              encontrado = TRUE;
            break;
            // Si es una cadena se usa la funci¢n stricmp que compara dos
            // cadenas pero sin importarle may£sculas o min£sculas
         case ENTERO : if (*(int*)dato_aux == *(int*)(aux + offset))
                         encontrado = TRUE;
                       break;
                       // Los enteros se comparan directamente
         case REAL :
           if (*(double*)dato_aux == *(double*)(aux + offset))
             encontrado = TRUE;
           break;
           // Los n£meros reales (double) tambi‚n se comparan directamente
         case FECHA : case HORA :
           if (memcmp (dato_aux, aux + offset, longitud) == 0)
             encontrado = TRUE;
           break;
           // En el caso de la fecha y la hora se compara la zona de memoria
         // donde est n los datos porque estos est n en un formato codificado
        }
      if (encontrado)
        {delete dato_aux;
         return contador;
        }
      // Si se ha encontrado el dato se sale de la funci¢n y se devuelve el
      // numero de la ficha donde se encontraron los datos.
      contador++;           // Sino incrementamos el contador y pasamos a la
      aux += long_fich;     // ficha siguiente
     }
   while (!encontrado && contador < nro_fichas);
     // Seguimos comparando mientras no encontremos la ficha o hasta llegar al
     // final de la lista.
   delete dato_aux;
   return FICHA_INEXISTENTE; // Como no se ha encontrado la ficha se devuelve
                             // un error
  } // buscar_ficha

unsigned pascal Cbase_datos::busqueda_binaria (void *dato, unsigned pos_inicial,
                                               unsigned que_devuelvo)
/* Busca una ficha mediante la b£squeda binaria.
   dato : Puntero al dato a buscar
   pos_inicial : ficha a partir de la que hay que buscar el dato
*/
  {byte *aux, *dato_aux;
   unsigned encontrado = FALSE, offset = 0, tipo_dato, longitud;
   int temp;
   long tope_ini = 0, tope_fin = nro_fichas - 1;

   if (!nro_fichas || !hay_campos() || pos_inicial > nro_fichas ||
       ordenada_por == SIN_ORDENAR)
     return FICHA_INEXISTENTE;
            /* Si no hay fichas, no hay campos, si la ficha inicial es
               mayor que el n§ de fichas o la lista no est  ordenada se sale
               devolviendo un c¤odigo de error */

   /* Ahora se selecciona el campo por el que se ba a buscar el dato para
      saber la posici¢n del dato dentro del bloque de datos */
   campo_actual = lista_campos + ordenada_por;
   offset = campo_actual->offset;
   tipo_dato = campo_actual->tipo_dato;
   longitud = campo_actual->longt;
     // memorizamos la longitud, el offset y el tipo de dato para acceder m s
     // r pidamente a ellos
   if (pos_inicial == 0)
     pos_inicial = nro_fichas / 2;
   aux = lista_fichas + lista_ordenada[pos_inicial] * long_fich;
     // Se busca desde la ficha indicada

   /* nos ponemos a buscar el bloque de memoria donde est  el dato buscado.
      En dato tenemos un puntero al dato a buscar y (aux->datos + offset)
      nos dar  un puntero al dato de la ficha actual a comparar. Como ambos
      punteros son de tipo void (puntero a cualquier cosa) han de ser
      convertidos al tipo de dato a buscar con un "casting", luego se hace
      la comparaci¢n, si los datos son iguales se considera que se ha
      encontrado la ficha */
   dato_aux = new byte[campo_actual->longt];
   copia (dato_aux, (byte *)dato, campo_actual->tipo_dato,
          campo_actual->longt, TRUE);
   // Se reserva memoria y se transforma el dato del formato ASCII al que
   // haga falta.
   do
     {switch (tipo_dato)
       // Seg£n el tipo de dato se hacen diferentes tipos de comparaciones
        {case CADENA :
            temp = stricmp ((char *)dato_aux, (char *)(aux + offset));
            if (temp == 0) encontrado = TRUE;
            break;
            // Si es una cadena se usa la funci¢n stricmp que compara dos
            // cadenas pero sin importarle may£sculas o min£sculas
         case ENTERO : temp = *(int*)dato_aux - *(int*)(aux + offset);
                       if (temp == 0) encontrado = TRUE;
                       break;
                       // Los enteros se comparan directamente
         case REAL :
           {double tempf;

           tempf = *(double*)dato_aux - *(double*)(aux + offset);
           if (tempf == 0) encontrado = TRUE;
           else if (tempf < 0) temp = -1;
                else temp = 1;
           break;
           }
           // Los n£meros reales (double) tambi‚n se comparan directamente
         case FECHA : case HORA :
           temp = memcmp (dato_aux, aux + offset, longitud);
           if (temp == 0) encontrado = TRUE;
           break;
           // En el caso de la fecha y la hora se compara la zona de memoria
         // donde est n los datos porque estos est n en un formato codificado
        }
      if (encontrado)
        {delete dato_aux;
         if (que_devuelvo == INDICE) return pos_inicial;
         else return lista_ordenada[pos_inicial];
        }
      // Si se ha encontrado el dato se sale de la funci¢n y se devuelve el
      // numero de la ficha donde se encontraron los datos.
      else if (temp < 0)
             tope_fin = pos_inicial - 1;
           else tope_ini = pos_inicial + 1;
      pos_inicial = (tope_ini + tope_fin) / 2;
      aux = lista_fichas + lista_ordenada[pos_inicial] * long_fich;
      // ficha siguiente
     }
   while (!encontrado && tope_ini <= tope_fin);
     // Seguimos comparando mientras no encontremos la ficha y hasta que
     // tope_inf sea mayor que tope_sup.
   delete dato_aux;
   return FICHA_INEXISTENTE; // Como no se ha encontrado la ficha se devuelve
                             // un error
  } // buscar_ficha


unsigned pascal Cbase_datos::sig_ficha (void)
// Pasa a la ficha siguiente a la actual
  {
   if (nro_fichas)      // Si hay fichas y
     if (nro_ficha_actual < nro_fichas)  // si no estamos ya en la £ltima ficha
       {ficha_actual += long_fich; // pasamos a la siguiente
        nro_ficha_actual++;
        return OK;
       }
     else return ULTIMA_FICHA;
   else return LISTA_FICHAS_VACIA;
  }  // sig_ficha

unsigned pascal Cbase_datos::ant_ficha (void)
// Pasa a la ficha anterior
  {
   if (nro_fichas)      // Si hay fichas y
     if (nro_ficha_actual > 1)  // si no estamos ya en la primer ficha
       {ficha_actual -= long_fich;  // pasamos a la anterior
        nro_ficha_actual--;
        return OK;
       }
     else return PRIMER_FICHA;
   else return LISTA_FICHAS_VACIA;
  }  // ant_ficha

void pascal Cbase_datos::primer_ficha (void)
// Salta a la primer ficha de la lista
  {
   ficha_actual = lista_fichas;   // Apuntamos a la primer ficha de la lista
   nro_ficha_actual = 0;
  } // primera_ficha

unsigned pascal Cbase_datos::ultima_ficha (void)
// Salta a la £ltima ficha de la lista
  {
   if (hay_campos() && nro_fichas)   // Si hay campos definidos y hay fichas
     {ficha_actual = lista_fichas + (nro_fichas-1) * long_fich;
      nro_ficha_actual = nro_fichas;
                   // apuntamos a la £ltima ficha
      return nro_fichas;
     }
   else if (!hay_campos())                // Sino hay campos definidos
          return LISTA_CAMPOS_VACIA;
        else return LISTA_FICHAS_VACIA; // o no hay fichas, devolvemos un
                                        // error
  } // ultima_ficha

int pascal Cbase_datos::ficha_numero (unsigned numero)
// Salta a la ficha indicada
  {
   if (hay_campos() && numero < nro_fichas)
     // Si hay campos definidos y hay fichas busca la ficha indicada
     {ficha_actual = lista_fichas + numero * long_fich;
      nro_ficha_actual = numero + 1;
      return OK;
     }
   else if (nro_fichas == 0) return LISTA_FICHAS_VACIA;
        else if (numero > nro_fichas) return FICHA_INEXISTENTE;
             else return LISTA_CAMPOS_VACIA;
     // sino se devuelve un error
  } // ficha_numero

int pascal Cbase_datos::ordenar_fichas (unsigned campo_indice)
// Ordena las fichas por un campo
  {byte *aux1;
   unsigned offset, tipo_dato, contador = 0;

/* Primero se busca el campo por el que se ba a buscar el dato recorriendo la
      lista de campos, a la vez se van sumando las longitudes de los campos
      para saber la posici¢n del dato dentro del bloque de datos */
   if (campo_indice == ordenada_por)
     return OK;
   campo_actual = lista_campos + campo_indice;
   offset = campo_actual->offset;
   tipo_dato = campo_actual->tipo_dato;
     // Se almacenan los datos m s importantes para no tener que estar
     // accediendo a ellos a trav‚s del vector; es m s r pido
   aux1 = lista_fichas + offset;
     // Recorre toda la lista insertando en el  rbol todas las fichas de la
     // lista
   while (contador < nro_fichas)
     {inserta_nodo (aux1, offset, tipo_dato, contador);
      aux1 += long_fich;
      contador++;
     }
   contador = 0;
   while (contador < nro_fichas)
     // Recorre el  rbol en inorden (1§ rama izquierda, 2§ ver el nodo,
     // 3§ rama derecha), guardando los ¡ndices de cada ficha; as¡ se guarda
     // un ¡ndice que indica el orden de la lista
     {lista_ordenada[contador] = saca_menor_dato();
      contador++;
     }
   ordenada_por = campo_indice;
   return OK;
  } // ordenar_fichas

void pascal Cbase_datos::tipodato (unsigned nro_campo, unsigned *tipo_dato)
// Indica el tipo de dato de un campo determinado
  {if (_buscar_campo (nro_campo) != NULL)
     *tipo_dato = campo_actual->tipo_dato;
   else *tipo_dato = 0;
  } // tipo_dato


byte * pascal Cbase_datos::leer_dato (unsigned nro_campo)
// Lee un dato de un determinado campo
  {
   campo_numero (nro_campo); // Apuntamos al campo buscado
   return ficha_actual + campo_actual->offset; // Se devuelve el puntero al dato
  } // leer_dato

int pascal Cbase_datos::escribir_dato (void *dato, unsigned nro_campo)
// Escribe un dato en un determinado campo de la ficha actual
  {unsigned offset = 0;

   if (nro_campo > nro_campos) return CAMPO_INEXISTENTE;

   // Si existe el campo lo buscamos y calculamos su offset
   campo_numero (nro_campo);
   offset = campo_actual->offset;

   // Se copia el dato a la ficha
   copia (ficha_actual + offset, (byte *)dato, campo_actual->tipo_dato, 0,
          TRUE);
   return OK;
  } // escribir_dato

int pascal Cbase_datos::imprimir_ficha (int destino)
// Muestra la ficha actual en pantalla, y los datos que contiene
  {unsigned offset = 0, aux;

   if (destino == PANTALLA)  // Si el destino es la pantalla
     {textcolor (YELLOW);
      textbackground (BLUE);
      primer_campo();        // Apuntamos al primer campo
      // Se recorren todos los campos, imprimiendo cada uno
      do
        {// Si es oculto no se hace nada
         if (campo_actual->tipo_dato & OCULTO)
           continue;
         // sino calcula su posici¢n en pantalla
         offset = campo_actual->offset;
         gotoxy (campo_actual->pos_x, campo_actual->pos_y);
         cputs (campo_actual->nombre);
         cputs (" : ");      // Se imprime el nombre
         for (aux = 0; aux < (campo_actual->longitud); aux++)
           putch('_');                                // Se completa con __
         gotoxy (campo_actual->pos_x + strlen(campo_actual->nombre) + 3,
                 campo_actual->pos_y);
         switch (campo_actual->tipo_dato & ~TRAMA)
         // Se imprimen los datos de la ficha
           {case CADENA : cputs (ficha_actual + offset);
                          break;
            case ENTERO : cprintf ("%d", *(int*)(ficha_actual + offset));
                          break;
            case REAL : cprintf ("%.2f", *(double*)(ficha_actual + offset));
                        break;
            case FECHA : aux = *(ficha_actual + offset);
                         cprintf ("%d/%d/%d", aux & 31, (aux>>5) & 15,
                                  (aux>>9) & 127 + 90);
                         break;
            case HORA : aux = *(ficha_actual + offset);
                        cprintf ("%d:%d", aux & 63, (aux>>6) & 31);
                        break;
           }
              // Pasamos al siguiente campo
        }
      while (campo_siguiente() != FIN_DE_LISTA);
      textcolor (BLACK);
      textbackground (LIGHTGRAY);
     }
   else // Si la salida es hacia la impresora
     {TFtemporal *salida;

      salida = inicializa_impresion();
      primer_campo ();
      fprintf (salida->fichero, "\r\n Ficha n£mero %u\r\n\n",
               nro_ficha_actual);
      do
        {if (campo_actual->tipo_dato & OCULTO) continue;
         offset = campo_actual->offset;
         fprintf (salida->fichero, " %s : ", campo_actual->nombre);
         switch (campo_actual->tipo_dato & ~TRAMA)
         // Se imprimen los datos de la ficha, excepto en los campos ocultos
           {case CADENA : envia_datos (ficha_actual + offset, salida);
                          break;
            case ENTERO : fprintf (salida->fichero, "%d", *(int*)(ficha_actual + offset));
                          break;
            case REAL : fprintf (salida->fichero, "%.2f", *(double*)(ficha_actual + offset));
                        break;
            case FECHA : aux = *(ficha_actual + offset);
                         fprintf (salida->fichero, "%d/%d/%d", aux & 31, (aux>>5) & 15,
                                  (aux>>9) & 127 + 90);
                         break;
            case HORA : aux = *(ficha_actual + offset);
                        fprintf (salida->fichero, "%d:%d", aux & 63, (aux>>6) & 31);
                        break;
           }
         envia_datos ("\r\n", salida);
        }
      while (campo_siguiente () != FIN_DE_LISTA);
      envia_datos ("\r\n", salida);
      imprime (salida, FALSE, 1);
     }
   return OK;
  } // imprimir_ficha

int pascal Cbase_datos::imprimir_solo_campos (void)
// Muestra el esquema de la ficha en pantalla
  {unsigned aux;

   primer_campo();    // Apunta al 1er campo
   do
     {if (campo_actual->tipo_dato & OCULTO) continue;
      gotoxy (campo_actual->pos_x, campo_actual->pos_y);
      cputs (campo_actual->nombre);    // Imprime el nombre
      switch (campo_actual->tipo_dato) // Rellena con ______
        {case CADENA :
         case ENTERO :
         case REAL :   cputs (" : ");
                       for (aux = 1; aux < (campo_actual->longitud); aux++);
                         putch ('_');
                       break;
         case FECHA : cputs (" : __/__/__");
                      break;
         case HORA : cputs (" : __:__");
                     break;
        }
     }
   while (campo_siguiente() != FIN_DE_LISTA);  // Repetimos hasta llegar al
   return OK;                                  // £ltimo campo
  } // imprimir_ficha

int pascal Cbase_datos::rellenar_ficha (int nueva)
// Presenta una ficha en pantalla y espera que se rellenen los campos
  {unsigned offset = 0;       // Posici¢n del dato dentro de la ficha
   char tecla = 0;
   byte *temp, *aux;          // Variables auxiliares y temporales

   if ((temp = new byte[long_fich]) == NULL) // Si no hay memoria para una
      return MEMORIA_AGOTADA;                // ficha temporal, nos vamos
   if (nueva == TRUE)                        // Si es una ficha nueva...
     memset (temp, 0, long_fich);            // se rellena todo con 0,
   else memcpy (temp, ficha_actual, long_fich); // sino se copia la anterior
   imprimir_ficha ();                        // Imprime la ficha
   primer_campo ();                          // Salta al primer campo
   aux = new byte[campo_actual->longitud+1]; // Se reserva espacio para un
   if (!aux)                                 // campo temporal, si no lo hay
     {delete temp;                           // nos vamos.
      return MEMORIA_AGOTADA;
     }
   memset (aux, 0, campo_actual->longitud + 1);
   copia (aux, temp, campo_actual->tipo_dato, 0, FALSE);
                                             // Si lo hay, copiamos los datos
                                             // del campo a aux

   _setcursortype (_NORMALCURSOR);           // Cursor visible
   textcolor (WHITE);
   textbackground (BLUE);                    // Cambia los colores
   do
     {gotoxy (campo_actual->pos_x + strlen(campo_actual->nombre) + 3,
              campo_actual->pos_y); //Coloca el cursor en la posici¢n adecuada

      // Seg£n el tipo de dato, se lee la cadena de una forma o de otra
      if (campo_actual->tipo_dato & OCULTO)
      // Si el campo al que se ha pasado es de tipo oculto se pasa al campo
      // siguiente o anterior
        if (tecla == ENTER || tecla == TAB)
          tecla = ENTER;
        else tecla = S_TAB;
      else // Sino se lee el dato seg£n el tipo de dato a leer
        if ((campo_actual->tipo_dato & ~TRAMA) == ENTERO)
          tecla = lee_cadena (aux, campo_actual->longitud + 1, FALSE, FALSE,
                              '_');
        else if ((campo_actual->tipo_dato & ~TRAMA) == CADENA)
               tecla = lee_cadena (aux, campo_actual->longitud + 1, TRUE,
                                   FALSE, '_');
             else tecla = lee_cadena (aux, campo_actual->longitud + 1,
                                      TAL_CUAL, FALSE, '_');

      switch (tecla)
        {// Si se pulsa ESC salimos del bucle
         case ESC : break;

         // Con ENTER o TAB pasamos al siguiente campo. Con CTRL-ENTER se
         // copia el campo y se sale del bucle
         case ENTER :
         case LF :
         case TAB : // Copia el campo a temp
                    copia (temp + offset, aux, campo_actual->tipo_dato, 0,
                           TRUE);
                    delete aux;
                    // Calcula la pos del campo siguiente
                    if (campo_siguiente() == FIN_DE_LISTA) // Si nos pasamos
                      primer_campo();                      // del £ltimo campo
                                                           // al primero
                    offset = campo_actual->offset;
                    aux = new byte[campo_actual->longitud + 1];
                    memset (aux, 0, campo_actual->longitud + 1);
                    // Se cogen los datos de temp
                    copia (aux , temp + offset, campo_actual->tipo_dato, 0,
                           FALSE);
                    break;
         case S_TAB : // Si se pulsa SHIFT + TAB pasamos al campo anterior
                      copia (temp + offset, aux, campo_actual->tipo_dato,
                             0, TRUE);
                      delete aux;
                      if (campo_anterior() == FIN_DE_LISTA)
                        ultimo_campo();
                      offset = campo_actual->offset;
                      aux = new byte[campo_actual->longitud + 1];
                      memset (aux, 0, campo_actual->longitud + 1);
                      copia (aux , temp + offset, campo_actual->tipo_dato,
                             0, FALSE);
                      break;
        }
     }
   while (tecla != ESC && tecla != LF);
     // Se repite el bucle hasta que se pulsa ESC o CTRL-ENTER
   _setcursortype (_NOCURSOR);
   textbackground (LIGHTGRAY);
   textcolor (BLACK);
   if (tecla == LF)      // Si se sale con CTRL-ENTER se actualiza la ficha
     {memcpy (ficha_actual, temp, long_fich);
      if (nueva == TRUE)
        nro_ficha_actual = nro_fichas;
      modificado = TRUE;
     }
   else // En cambio, si se sale con ESC, y se estaba insertando una ficha,
        // se borra y se acomodan los contadores
        if (nueva == TRUE)
          {nro_fichas--;
           ficha_actual -= long_fich;
           nro_ficha_actual = nro_fichas;
          }
   delete temp;
   delete aux;
   if (tecla == LF)
     {ordenada_por = SIN_ORDENAR;
      return OK;
     }
   else return FICHA_SIN_RELLENAR;
  } // rellenar_ficha


int pascal Cbase_datos::grabar_fichero (void)
// Graba la lista de fichas y la definici¢n de los campos en un fichero
  {FILE *fichero;

   if (nombre_fichero[0])
     {if ((fichero = fopen (nombre_fichero, "wb")) == NULL) // Abre el fichero
        return IMPOSIBLE_ABRIR_FICHERO;
      fputs ("Bdatos", fichero);     // Guarda la identificaci¢n del fichero
      fwrite (&nro_fichas, sizeof(unsigned), 1, fichero);
      fwrite (&long_fich, sizeof(unsigned), 1, fichero);
      fwrite (&nro_campos, sizeof(unsigned), 1, fichero);
      fwrite (&tam_memoria, sizeof(unsigned), 1, fichero);
      fwrite (&ordenada_por, sizeof(unsigned), 1, fichero);
       // Guarda los datos importantes para el control de las fichas
      fwrite (lista_campos, sizeof(Tcampo), nro_campos, fichero);
      fwrite (lista_fichas, long_fich, nro_fichas, fichero);
       // Almacena la definici¢n de las fichas y los datos de las mismas
      fwrite (lista_ordenada, sizeof(unsigned), nro_fichas, fichero);
       // Se guarda el ¡ndice de las fichas ordenadas.
      fclose (fichero);
      return OK;
     }
   else return NOMBRE_FICHERO_INVALIDO;
  } // grabar_fichero

int pascal Cbase_datos::leer_fichero (void)
// Lee fichas de un fichero
  {FILE *fichero;
   char cadena[7];

   if (nro_fichas) return LISTA_NO_VACIA;
   // Si hay fichas se sale de la funci¢n
   if (nombre_fichero[0] == 0) return NOMBRE_FICHERO_INVALIDO;
   // Si el fichero no tiene nombre tambi‚n
   if ((fichero = fopen (nombre_fichero, "rb")) == NULL) // Se abre el fichero
     return IMPOSIBLE_ABRIR_FICHERO;
   fgets (cadena, 7, fichero);  // Se leen los primeros 7 caracteres
   if (strcmp (cadena, "Bdatos"))
   // Si no coinciden con la cadena clave se sale de la funci¢n
     {fclose (fichero);
      return TIPO_DE_FICHERO_INVALIDO;
     }
   borrar_definicion();  // Se borra la definici¢n de las fichas
   delete lista_campos;
   delete lista_ordenada;
   fread (&nro_fichas, sizeof(unsigned), 1, fichero);
   fread (&long_fich, sizeof(unsigned), 1, fichero);
   fread (&nro_campos, sizeof(unsigned), 1, fichero);
   fread (&tam_memoria, sizeof(unsigned), 1, fichero);
   fread (&ordenada_por, sizeof(unsigned), 1, fichero);
     // Se leen los datos necesarios para el control de las fichas
   campo_actual = lista_campos = new Tcampo[nro_campos];
   // Se reserva memoria para la definici¢n de las fichas
   if (lista_campos != NULL)
     {delete lista_fichas;
      ficha_actual = lista_fichas = new byte[tam_memoria];
      // Se reserva memoria para los datos.
     }
   fread (lista_campos, sizeof(Tcampo), nro_campos, fichero);
   fread (lista_fichas, long_fich, nro_fichas, fichero);
   // Se lee la definici¢n de las fichas y los datos de las mismas
   calcula_max_fichas();
   lista_ordenada = new unsigned[max_fichas];
   // reserva memoria para el ¡ndice de las fichas ordenadas
   if (lista_ordenada != NULL)
     fread (lista_ordenada, sizeof(unsigned), nro_fichas, fichero);
   // lee el ¡ndice de fichas ordenadas
   fclose (fichero);
   nro_ficha_actual = 1;
   return OK;
  } // leer_fichero

unsigned pascal Cbase_datos::inserta_nodo (byte *dato, unsigned offset,
                                           unsigned tipo_dato,
                                           unsigned contador)
// Inserta el nodo en el  rbol en la posici¢n adecuada
  {Tnodo_arbol *aux = arbol, *aux2 = NULL;
   unsigned direccion;
   byte *dato_comparado;

   if (arbol == NULL)                // Si no hay  rbol se crea un nodo
     {aux = arbol = new Tnodo_arbol;
      if (aux == NULL)
        return MEMORIA_AGOTADA;
      arbol->dato = contador;
      arbol->izq = arbol->der = NULL;
      return OK;
     }
   // Recorre el  rbol buscando la posici¢n correcta para insertar el nodo
   // (si es menor se va a la izquierda, sino a la derecha
   while (aux != NULL)
     {dato_comparado = lista_fichas + aux->dato * long_fich + offset;
      switch (tipo_dato)
        {case CADENA :
           if (strcmp ((char *)dato, (char *)dato_comparado) < 0)
             direccion = 0;
           else direccion = 1;
           break;
         case ENTERO : if ((int)*dato < (int)*dato_comparado)
                         direccion = 0;
                       else direccion = 1;
                       break;
         case REAL : if ((float)*dato < (float)*dato_comparado)
                       direccion = 0;
                     else direccion = 1;
                     break;
        }

      if (direccion == 0)
        {aux2 = aux;
         aux = aux->izq;
        }
      else {aux2 = aux;
            aux = aux->der;
           }
     }
   // Crea el nuevo nodo y mete en el los datos
   if ((aux = new Tnodo_arbol) == NULL)
     return MEMORIA_AGOTADA;
   aux->dato = contador;
   aux->izq = aux->der = NULL;
   if (direccion == 0)
     aux2->izq = aux;
   else aux2->der = aux;
   return OK;
  } // inserta_nodo

unsigned pascal Cbase_datos::saca_menor_dato (void)
// Saca el dato menor del  rbol
  {
   Tnodo_arbol *aux = arbol, *aux2 = NULL;
   unsigned temp;

   while (aux->izq != NULL)
   // Recorre el  rbol hasta que se llega a una hoja que no tiene hijos
     {aux2 = aux;
      aux = aux->izq;
     }

   if (aux2)                 // Si hay nodo anterior
     if (aux->der)           //   Si tiene un nodo a la derecha
       aux2->izq = aux->der; //     lo pasa al nodo izquierdo
     else aux2->izq = NULL;  //   sino anula el nodo izquierdo del nodo anterior
   else arbol = arbol->der;  // sino hace que el  rbol apunte a su nodo derecho

   temp = aux->dato;         // Recupera el nro de la ficha
   delete aux;               // y borra el nodo
   return temp;
  } // sacar_menor_dato

void pascal copia (byte *destino, byte *origen, unsigned tipo_dato,
                   unsigned longitud, boolean guarda)
// Copia los datos de origen a destino, transformando el tipo de dato si
// es necesario.
  {tipo_dato = tipo_dato & ~TRAMA;
   switch (guarda)
     {case TRUE :
       {switch (tipo_dato)
          {case CADENA : strcpy (destino, origen);
                         break;

           case ENTERO : *(int*)destino = atoi (origen);
                         break;

           case REAL : *(double*)destino = atof (origen);
                       break;
         }
        break;
       }
      case FALSE :
       {switch (tipo_dato)
          {case CADENA : strcpy (destino, origen);
                         break;

           case ENTERO : itoa (*origen, destino, 10);
                         break;

           case REAL : if (*(double*)origen)
                         sprintf (destino, "%.2f", *(double*)origen);
                       else *destino = 0;
                       break;
          }
        break;
       }
      case TAL_CUAL :
        memcpy (destino, origen, longitud);
        break;
     }
  } // copia

