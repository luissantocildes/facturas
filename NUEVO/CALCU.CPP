/*************************************************************************
 * Fichero : CALCU.CPP                                                   *
 * Fichero que contiene las funciones necesarias para el uso de una      *
 * calculadora.                                                          *
 *************************************************************************
 * Fecha creaci¢n : 27/5/1997                                            *
 * Fecha £ltima modificaci¢n : 27/5/97                                   *
 * (c)1997 Luis Fernando Santocildes Romero                              *
 *************************************************************************
 * V1.0 : 27/5/97                                                        *
 *        - Primera versi¢n                                              *
 *************************************************************************/
#define _NO_RELOJ
#define _NO_CADENA
#define _NO_INI
#define _NO_IMPRESION
#define _NO_LISTAS
#define _NO_MODO_132
#define _NO_FICH

#include <ctype.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "defines.h"
#include "boolean.h"
#include "tipos.h"
#include "control.h"

/********************************* Tipos ************************************/
typedef enum _operandos {NADA, SUMA, RESTA, MULTIPLICA, DIVIDE} Toperandos;

/******************************** Prototipos ********************************/
void calculadora (void);
                                        // Muestra una calculadora en pantalla

/*************************** Variables Globales *****************************/
long double memoria,  // Memoria de la calculadora
            subtotal; // Total calculado hasta ahora

boolean error; // Verifica si se ha producido un error

char teclas[][3] = {"<-", "Mc", "Ce", "C", "M+", "Mr", "û", "+", "7", "8", "9",
                    "-", "4", "5", "6", "*", "1", "2", "3", "/",
                    "0", ".", "ñ", "="};

/****************************************************************************/

void calculadora (void)
  {Tpantalla ventana;
   unsigned f, pos = 0;
   long double numero = 0;
   char tecla,
        visor[15];
   boolean dec = FALSE,
           negativo = FALSE;
   Toperandos ultimo_operando = NADA;

   error = FALSE;
   pone_barra_inferior (" Pulse &ESC& para salir de la calculadora");
   ventana = hace_recuadro (5, 3, 27, 17);
   textattr (colores.boton);
   for (f = 0; f < 24; f++)
     {gotoxy ((f%4)*6+3, f/4*2+5);
      cprintf (" %2s ", teclas[f]);
     }
   gcvt (subtotal, 14, visor);
   //strcpy (visor, "0.");
   textattr (colores.texto_ventana);
   do
     {gotoxy (3, 2);
      if (error)
        cprintf ("%c%c%20s", memoria ? 'M':' ', negativo ? '-':' ',"Error ");
      else
        cprintf ("%c%c%20s", memoria ? 'M':' ', negativo ? '-':' ',visor);
      tecla = toupper (getch());
      switch (tecla)
        {case '0': case '1': case '2': case '3': case '4':
         case '5': case '6': case '7': case '8': case '9':
           if (error)
             break;
           if (pos < 14)
             {if (pos == 0)
                {negativo = FALSE;
                 gotoxy (4, 2);
                 putch (' ');
                }
              if (tecla == '0' && visor[pos] == '0')
                break;
              visor[pos] = tecla;
              pos++;
              visor[pos] = 0;
             }
           break;

         case '.':
           if (error || dec)
             break;
           if (pos < 14)
             {dec = TRUE;
              if (pos)
                {pos++;
                 strcat (visor, ".");
                }
              else
                {strcpy (visor, "0.");
                 pos = 2;
                }
             }
           break;

         case '\b' :
           if (error)
             break;
           if (pos)
             {pos--;
              if (visor[pos] == '.')
                {pos--;
                 dec = FALSE;
                }
              visor[pos] = visor[pos + 1];
              if (!dec)
                visor[pos + 1] = visor [pos + 2];
             }
           break;

         case 'C' : strcpy (visor, "0.");
                    pos = 0;
                    subtotal = 0.0;
                    ultimo_operando = NADA;
                    negativo = FALSE;
                    error = FALSE;
                    dec = FALSE;
                    break;

         case T_control :
           if (error)
             break;
           tecla = getch();
           if (tecla == SUPR)
             {strcpy (visor, "0.");
              pos = 0;
              negativo = FALSE;
             }
           else
             if (tecla == INS)
               negativo = negativo ? FALSE : TRUE;
           break;

         case 'R':
           if (error)
             break;
           if (negativo)
             {error = TRUE;
              break;
             }
           numero = _atold (visor);
           numero = sqrtl (numero);
           gcvt (numero, 14, visor);
           pos = 0;
           dec = FALSE;
           break;

         case 'M':
           if (error)
             break;
           tecla = toupper (getch());
           switch (tecla)
             {case 'C': memoria = 0;
                        break;

              case 'R': gcvt (memoria, 14, visor);
                        if (visor[0] == '-')
                          {strcpy (visor, visor+1);
                           negativo = TRUE;
                          }
                        else
                          negativo = FALSE;
                        dec = FALSE;
                        break;

              case '+': numero = _atold (visor);
                        if (negativo)
                           numero *= -1.0;
                        pos = 0;
                        memoria += numero;
                        break;
             }
           break;

         case '+': case '-': case '/': case '*': case '=': case ENTER:
           if (error)
             break;
           numero = _atold (visor);
           if (negativo)
             numero *= -1.0;
           switch (ultimo_operando)
             {case SUMA:
                subtotal += numero;
                break;
              case RESTA:
                subtotal -= numero;
                break;
              case MULTIPLICA:
                subtotal *= numero;
                break;
              case DIVIDE:
                if (numero == 0.0)
                  {error = TRUE;
                   break;
                  }
                subtotal /= numero;
                break;
              case NADA:
                subtotal = numero;
                break;
             }
           if (error)
             break;
           gcvt (subtotal, 14, visor);
           if (visor[0] == '-')
             {strcpy (visor, visor+1);
              negativo = TRUE;
             }
           else
             negativo = FALSE;
           pos = 0;
           dec = FALSE;
           switch (tecla)
             {case '+':
                ultimo_operando = SUMA;
                break;
              case '-':
                ultimo_operando = RESTA;
                break;
              case '*':
                ultimo_operando = MULTIPLICA;
                break;
              case '/':
                ultimo_operando = DIVIDE;
                break;
              case '=': case ENTER:
                ultimo_operando = NADA;
                break;
             }
           break;
        }
     }
   while (tecla != ESC);
   restaura_recuadro (&ventana);
  }