/****************************************************************************
 * Fichero : VESA.CPP                                                       *
 * Fichero que contiene las funciones de control para los modos de v¡deo    *
 * VESA.                                                                    *
 * Funciones extraidas de Control.cpp                                       *
 ****************************************************************************
 * Fecha creaci¢n : 7-3-1997                                                *
 * Fecha £ltima modificaci¢n : 7-3-1997                                     *
 * (c)1997 Luis F. Santocildes Romero                                       *
 ****************************************************************************/
#include "boolean.h"
#include "tipos.h"

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
boolean pascal hay_tarjeta_vesa (Tdatos_vesa *buffer);
                                /* Verifica que la tarjeta gr fica sea VESA */
boolean pascal pone_modo_vesa (unsigned modo);
                                                     /* Activa un modo VESA */
boolean pascal datos_modo_vesa (unsigned modo, Tdatos_modo *buffer);
                                 /* Devuelve informacion sobre un modo VESA */
void pascal clrscr_x (void);
                             /* Borra la pantalla en un modo extendido VESA */

/*************************** VARIABLES GLOBALES *****************************/
boolean hay_vesa;

/****************************************************************************/
boolean pascal hay_tarjeta_vesa (Tdatos_vesa *buffer)
/* Verifica que la tarjeta gr fica sea VESA */
  {
   asm {cli
        push es
        les  di, buffer   // Se apunta al buffer
        mov  ax, 0x4f00
        int  0x10         // Se verifica si existe la VESA
        pop  es
        sti
        test ah, ah
        je   hay_vesa
       }
   return FALSE;
   hay_vesa:
   return TRUE;
  } // hay_tarjeta_vesa

boolean pascal pone_modo_vesa (unsigned modo)
/* Activa un modo VESA */
  {Tdatos_modo buffer;

   if (hay_vesa)  // Si la vesa existe
     {asm {cli
           push es
	   mov  ax, 0x4f01
           mov  cx, modo
           push ss
           pop  es
           lea  di, buffer      // Se apunta al bufer
           int  0x10            // Se piden los datos sobre el modo
           test ah, ah
           jnz  sale            // Si la tarjeta no es VESA, se sale
           mov  ax, 0x40
           mov  es, ax
           lea  si, buffer.resolucion_x
           mov  ax, ss:[si]
           mov  es:[0x4a], ax   // sino, se almacena la resoluci¢n X del modo
           mov  ax, 0x4f02
           mov  bx, modo
           int  0x10            // y se cambia al modo pedido
           test ah, ah
           jz   hay_vesa:
          }
      sale:
      asm pop es
      asm sti
      return FALSE;
      hay_vesa:
      asm pop es
      asm sti
      return TRUE;
     }
   return FALSE;
  } // pone_modo_vesa

boolean pascal datos_modo_vesa (unsigned modo, Tdatos_modo *buffer)
/* Devuelve informacion sobre un modo VESA */
  {
   if (hay_vesa)
   // Si la tarjeta es VESA
     {asm {cli              // Se quitan las interrupciones
           push  es
           mov   ax, 0x4f01
           mov   cx, modo
           les   di, buffer // Se apunta a buffer
           int   0x10       // Se pide informaci¢n sobre el modo
           pop   es
           test  ah, ah
           sti
           jz    hay_vesa:
          }
      return FALSE;         // Si la tarjeta no es VESA se devuelve FALSE
      hay_vesa:
      return TRUE;          // En otro caso se devuelve TRUE
     }
   return FALSE;
  } // datos_modo_vesa

void pascal clrscr_x (void)
// Borra la pantalla en un modo extendido VESA de 25 filas
  {
   if (hay_vesa)
     asm {push   di
          push   si
          push   ds
          push   es
          push   bx
          push   cx   // Se guardan los registros en la pila

          mov    ax, 0x40
          mov    ds, ax
	  mov    cx, [0x4a] // Se obtiene el ancho de la pantalla
          mov    ax, 25      // Se multiplica por 25 (nos da el nro de
	  mul    cx          // caracteres de la pantalla)
          mov    cx, ax      // Lo guardamos en CX
          mov    ax, 0xb800
          mov    es, ax
          mov    di, [0x4e] // Apuntamos al inicio de la pantalla
          cld
          mov    ax, 0x1e20

          rep    stosw       // Se rellena la pantalla con el valor de ax

          mov    ah, 2       // Funci¢n 2
          xor    bx, bx
          mov    bl, es:[0x62] // Se lee el n§ de p gina mostrada actualmente
          mov    dx, 0       // Posici¢n del cursor
          int    0x10

          pop    cx
          pop    bx
          pop    es
          pop    ds
          pop    si
          pop    di          // Se restauran los registros
         }
  } // clrscr_x

