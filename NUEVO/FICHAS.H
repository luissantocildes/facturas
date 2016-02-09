/****************************************************************************
 * FICHAS.H                                                                 *
 * Fichero de cabecera que contiene las estructuras y los objetos necesarios*
 * para crear una especie de base de datos. Permite definir los campos de   *
 * las fichas de forma flexible, es decir que la estructura de datos es     *
 * din mica, pudiendose cambiar durante la ejecuci¢n del programa.          *
 * Est  realizada de forma que se puedan utilizar varias listas a la vez.   *
 ****************************************************************************
 * Fecha realizaci¢n : 5/4/96                                               *
 * Fecha terminaci¢n : 6/4/97                                               *
 * (C)1996 Luis Fernando Santocildes                                        *
 ****************************************************************************/

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


   public:
   Cficha (void);   // Constructor, inicializa los datos de la lista de campos
   ~Cficha (void);  // Destructor
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
   int pascal hay_campos (void); // Indica si hay campos
   int pascal igualar_codigos(void); // No desarrollada.
  };

class Cbase_datos : public Cficha
/* Clase que se utiliza para definir la base de datos. Utiliza como base la
   clase Cficha para estructurar los campos de cada ficha.
*/
  {private:
   byte *lista_fichas, // Puntero a la lista de las fichas
        *ficha_actual; // Puntero a la ficha tratada actualmente
   unsigned tam_memoria,  // Bytes reservados para las fichas
            nro_fichas,   // N§ actual de fichas
            max_fichas;   // N§ m ximo de fichas
   Tnodo_arbol *arbol;    // Arbol usado para ordenar las fichas por un campo

   public:
   char nombre_fichero[MAXPATH]; // Fichero donde se almacenan los datos
   unsigned nro_ficha_actual, // N§ de la ficha actual
            ordenada_por, // Campo por el que est n ordenadas (65535 significa
                          // que no est n ordenados)
            modificado,   // Indica si se ha modificado la lista.
            *lista_ordenada; // Puntero a la lista ordenada
   char id[15];           // cadena identificativa del objeto

   Cbase_datos (void); // Constructor, inicializa la base de datos

   Cbase_datos (unsigned memoria);
     // 2§ Constructor, reserva tantos bytes de memoria como se le indiquen

   Cbase_datos (Tcampo *def_lista_campos, unsigned largo_lista,
                unsigned memoria = 32000);
   // 3er constructor, reserva memoria y crea una lista de campos seg£n la
   // definici¢n pasada.
   ~Cbase_datos (void); // Destructor

   boolean pascal inicializa (Tcampo *def_lista_campos, unsigned largo_lista,
                              unsigned memoria = 32000);
   void calcula_max_fichas (void);
       // Calcula el m ximo de fichas que se pueden poner en la lista.
   int pascal hay_fichas (void); // ¨Hay fichas en la lista?
   unsigned pascal cantidad_fichas (void); // Da el nro de fichas de la lista
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

