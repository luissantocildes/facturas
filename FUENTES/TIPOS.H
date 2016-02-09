/****************************************************************************
 * Tipos de datos para el programa de facturaci¢n                           *
 * Contiene las definiciones de las estructuras que m s usa el programa,    *
 * como las de las facturas. Las estructuras para las fichas de clientes y  *
 * art¡culos est n en fichas.h                                              *
 ****************************************************************************
 * Fecha creaci¢n : 5/4/96                                                  *
 * Fecha ultima modificaci¢n : 28/4/96                                      *
 * (c)1996 Luis Fernando Santocildes Romero                                 *
 ****************************************************************************/
typedef unsigned char byte; // Define byte como en Turbo Pascal

typedef struct _arbol // estructura para el  rbol de ordenaci¢n de fichas
  {unsigned dato;     // nro de ficha
   _arbol *izq, *der; // nodos hijos
  } Tnodo_arbol;

typedef struct            // datos extra de la factura
  {unsigned nro_cliente;
   char cultivo[31],      // Tipo de cultivo
        situacion[31];    // Situaci¢n de la finca
   double superficie,
          litros;
   long fecha;
  } Tdatos_factura;

typedef struct            // Datos de la factura a almacenar en el disco
  {byte tipo_factura;
   unsigned nro_cliente;
   char situacion[31];
   long fecha,
        nro_factura;
   double litros,
          total_1,
          total_2,
          total_factura;
  } Tdatos_fichero_facturas;