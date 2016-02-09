/****************************************************************************
 * FACT.H                                                                   *
 * Fichero de cabecera que contiene las estructuras y los objetos necesarios*
 * para crear una factura, y manejarla. Permite definir los campos de la    *
 * factura, aunque no es tan flexible como el m¢dulo de fichas.             *
 ****************************************************************************
 * Fecha realizaci¢n : 14/7/96                                              *
 * Fecha modificaci¢n : 25/4/97                                             *
 *                                                                          *
 * (C)1996-97 Luis Fernando Santocildes                                     *
 ****************************************************************************/

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

/**************************** VARIABLES GLOBALES ***************************/

extern double iva;  // Valor del IVA

extern char Facturas_nombre_fichero [MAXPATH],  // Fichero para las facturas
            Facturas_ile [MAXPATH];             // Fichero para las facturas sin IVA

extern unsigned long nro_factura;    // N£mero de la £ltima factura
/******************************** PROTOTIPOS *******************************/
extern void inicializa_facturas (void);
// inicializa ciertos valores de las definiciones de las facturas
extern int pascal hace_factura_tipo_1 (Cbase_datos &clientes,
                                       Cbase_datos &articulos);
extern int pascal hace_factura_tipo_2 (Cbase_datos &clientes);
extern int pascal hace_factura_tipo_3 (Cbase_datos &clientes,
                                       Cbase_datos &articulos);
extern void pascal borra_factura (void);