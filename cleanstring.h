/*                                                                                                                                                                                                                                                                                                                                               
 * Filename: cleanstring.h                                                                                                                                                                                                                                                                                                                       
 * Authors: Robert Lester, Brian Savage                                                                                                                                                                                                                                                                                                          
 * Assignment: HW1                                                                                                                                                                                                                                                                                                                               
 * Summary: Header file for cleanString functionality, details are included                                                                                                                                                                                                                                                                      
 * in the cleanstring.c file                                                                                                                                                                                                                                                                                                                     
 */

#ifndef CLEANSTRING_H_INCLUDED
#define CLEANSTRING_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include "seq.h"
#include <string.h>
#include <ctype.h>

char *cleanString(char *toClean); /* Cleans string of non alpha num or      */
                                  /* underscore chars                       */
int checkChar(char *toCheck); /* Checks if valid char                       */
char *makeArray(Seq_T seq, int length); /* converts sequence to char *      */
int buildSeq(char *toClean, Seq_T newSeq, int newLength, int i);
                                       /* creates a valid sequence of chars */
void err(Seq_T seq, char *msg); /* called if error occurs during process    */

#endif
