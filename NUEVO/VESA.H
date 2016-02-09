/****************************************************************************
 * Fichero : VESA.h                                                         *
 * Fichero que enlaza con la funciones definidas en VESA.CPP                *
 ****************************************************************************
 * Fecha creaci¢n : 7-3-1997                                                *
 * Fecha £ltima modificaci¢n : 7-3-1997                                     *
 * (c)1997 Luis F. Santocildes Romero                                       *
 ****************************************************************************/

/****************************** TIPOS DE DATOS ******************************/
// Datos referentes al estandar de video VESA
typedef struct
  {char firma[4];
   unsigned nro_version;
   char *nombre_fabricante;
   long capacidades;
   unsigned *modos_soportados;
   unsigned mem_total;
   byte reservado[236];
  } Tdatos_vesa;

// Datos referentes a los modos de v¡deo VESA
typedef struct
  {unsigned atributos;
   byte atr_ventana_1, atr_ventana_2;
   unsigned granularidad,
            tam_ventana,
            seg_inicio_vent_1,
            seg_inicio_vent_2;
   void * rutina_ajuste;
   unsigned bytes_linea,
            resolucion_x,
            resolucion_y;
   byte ancho_caracter,
        alto_caracter,
        bits_planes,
        bits_pixel,
        modelo_memoria,
        tam_bloques_memoria,
        nro_paginas,
        reservado,
        tam_mascara_roja, pos_campo_rojo,
        tam_mascara_verde, pos_campo_verde,
        tam_mascara_azul, pos_campo_azul,
        tam_mascara_reservada, pos_campo_reservado,
        reservados[218];
  } Tdatos_modo;

/****************************** PROTOTIPOS **********************************/
extern boolean pascal hay_tarjeta_vesa (Tdatos_vesa *buffer);
                                /* Verifica que la tarjeta gr fica sea VESA */
extern boolean pascal pone_modo_vesa (unsigned modo);
                                                     /* Activa un modo VESA */
extern boolean pascal datos_modo_vesa (unsigned modo, Tdatos_modo *buffer);
                                 /* Devuelve informacion sobre un modo VESA */
extern void pascal clrscr_x (void);
                             /* Borra la pantalla en un modo extendido VESA */

/*************************** VARIABLES GLOBALES *****************************/
extern boolean hay_vesa;

