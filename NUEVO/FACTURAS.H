/****************************************************************************
 * FACTURAS.H                                                               *
 * Fichero de cabecera donde se incluyen las funciones para hacer las       *
 * facturas, para sacar los listados trimestrales, etc, etc, etc.           *
 * Nota: Pensaba utilizar las clases que he desarrollado en FICHAS.H,  pero *
 *       me he dado cuenta de que  el fichero de facturas se puede llegar a *
 *       hacer muy grande y  superar  la capacidad  de las listas.  Adem s, *
 *       como hay varios tipos de facturas, tendr¡a que usar una lista para *
 *       cada una,  y las fichas  no me permiten dise¤ar una plantilla para *
 *       la factura.                                                        *
 ****************************************************************************
 * Creado : 29/5/96                                                         *
 * Ultima modificaci¢n : 29-5-96                                            *
 *                                                                          *
 * (C) 1996 Luis Fernando Santocildes Romero                                *
 ****************************************************************************/

typedef struct {char concepto[40];
                double total;
               } Tcampo_apartado_1;

typedef struct {unsigned codigo;
                char producto[41],
                     nro_registro[11],
                     nro_lote[11];
                double dosis,
                       ps,
                       kgm_lt,
                       precio,
                       importe;
               } Tcampo_apartado_2;

typedef struct {Tcampo_apartado_1 *datos;
                unsigned nro_filas,
                         fila_inicial,
                         bytes;
               } Tapartado_1;

typedef struct {Tcampo_apartado_2 *datos;
                unsigned nro_filas,
                         fila_inicial,
                         bytes;
               } Tapartado_2;
/*********************************PROTOTIPOS*********************************/
extern int pascal grabar_facturas (void);
                                  // A¤ade las facturas al fichero de facturas
extern void pascal imprime_cabecera (int destino);
extern int pascal reserva_memoria_apartados (int nro_apartado, unsigned nro_filas,
                                      unsigned fila_inicial);
extern void pascal libera_memoria (int nro_apartado);
extern void pascal imprime_plantilla_1 (void);
extern void pascal imprime_plantilla_2 (void);
/************************CONSTANTES Y VARIABLES GLOBALES*********************/
extern unsigned facturas_modificadas;

extern char Facturas_nombre_fichero[13];

extern float iva_1,
             iva_2;

//extern Tapartado_1 apartado_1;
//extern Tapartado_2 apartado_2;
