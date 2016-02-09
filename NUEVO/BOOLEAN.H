/****************************************************************************
 * Definicion de los valores booleanos, para que estos sean parecidos a los *
 * del PASCAL.                                                              *
 ****************************************************************************
 * Fecha modificaci¢n : 31/Jul/1996                                         *
 * (C)1996 Luis Fernando Santocildes Romero                                 *
 ****************************************************************************/
 
typedef enum {FALSE, TAL_CUAL, TRUE = 0xFFFF} boolean;
// He añadido un tercer valor que se usa en algunos casos muy específicos.