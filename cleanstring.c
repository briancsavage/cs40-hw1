/*                                                                                                                                                                                                                                                                                                                                               
 * Filename: cleanstring.c                                                                                                                                                                                                                                                                                                                       
 * Authors: Robert Lester, Brian Savage                                                                                                                                                                                                                                                                                                          
 * Assignment: HW1                                                                                                                                                                                                                                                                                                                               
 * Summary: cleanString takes a char pointer as a parameter and removes all                                                                                                                                                                                                                                                                      
 *          non alpha numeric characters (keeps underscores) from a line,                                                                                                                                                                                                                                                                        
 *          words are defined as sequences of valid chars and single spaces                                                                                                                                                                                                                                                                      
 *          are places between each word, the new line character is removed                                                                                                                                                                                                                                                                      
 *          from the end of each string, and a string terminating char is                                                                                                                                                                                                                                                                        
 *          added to the end of the cleaned string                                                                                                                                                                                                                                                                                               
 */

#include <stdio.h>
#include <stdlib.h>
#include "seq.h"
#include <string.h>
#include <ctype.h>

char *cleanString(char *toClean);
int checkChar(char *toCheck);
char *makeArray(Seq_T seq, int length);
int buildSeq(char *toClean, Seq_T newSeq, int newLength, int i);
void err(Seq_T seq, char *msg);

/* char *cleanString(char *toClean)                                                                                                                                                                                                                                                                                                              
 * Parameters: [char *toClean] - pointer to first char of the string to be                                                                                                                                                                                                                                                                       
 *                               cleaned                                                                                                                                                                                                                                                                                                         
 *    Returns: char pointer to the first element of the cleaned sequence                                                                                                                                                                                                                                                                         
 *       Does: given a string, it removes all none alpha numeric and underscore                                                                                                                                                                                                                                                                  
 *             characters from a string, adds a single space in between words                                                                                                                                                                                                                                                                    
 */
char *cleanString(char *toClean)
{
        if (toClean != NULL) {
                int oldLength = (int)strlen(toClean);
                int newLength = 0;
                Seq_T newSeq = Seq_new(1);
                for (int i = 0; i < oldLength; i++) {
                        newLength = buildSeq(toClean, newSeq, newLength, i);
                }
                char *cleaned = makeArray(newSeq, newLength);
                if (toClean != NULL) {
                        free(toClean);
                }
                return cleaned;
        }
        return NULL;
}

/* int buildSeq(char *toClean, Seq_T newSeq, int newLength, int i) {                                                                                                                                                                                                                                                                             
 * Parameters: [char *toClean] - char pointer to first element of string to be                                                                                                                                                                                                                                                                   
 *                               cleaned                                                                                                                                                                                                                                                                                                         
 *             [Seq_T newSeq] - the sequence that will contain the cleaned                                                                                                                                                                                                                                                                       
 *                              sequence of characters                                                                                                                                                                                                                                                                                           
 *             [int newLength] - length of the new cleaned sequence of chars                                                                                                                                                                                                                                                                     
 *             [int i] - indexs the original (uncleaned) string                                                                                                                                                                                                                                                                                  
 *    Returns: Integer representing the updated length of the cleaned sequence                                                                                                                                                                                                                                                                   
 *       Does: Builds a cleaned sequence of characters from the toClean string                                                                                                                                                                                                                                                                   
 */
int buildSeq(char *toClean, Seq_T newSeq, int newLength, int i) {
        char *temp = (char *)malloc(sizeof(char *));
        if (temp == NULL) {
                err(newSeq, "Error: memory allocation failed\n");
        }
        *temp = toClean[i];
        if (checkChar(temp)) {
                Seq_addhi(newSeq, temp);
                newLength++;
        } else if (newLength != 0 && *temp != '\n') {
                char *prev = (char *)malloc(sizeof(char *));
                if (prev == NULL) {
                        free(temp);
                        err(newSeq, "Error: memory allocation failed\n");
                }
                *prev = toClean[i - 1];
                if (checkChar(prev) && newLength != Seq_length(newSeq)) {
                        *temp = ' ';
                        Seq_addhi(newSeq, temp);
                        newLength++;
                }
                free(prev);
                free(temp);
        } else {
                free(temp);
        }
        return newLength;
}

/* char *makeArray(Seq_T seq, int length)                                                                                                                                                                                                                                                                                                        
 * Parameters: [Seq_T seq] - Sequence containing the sequence of cleaned chars                                                                                                                                                                                                                                                                   
 *             [Int length] - length of the new sequence of characters                                                                                                                                                                                                                                                                           
 *    Returns: a char pointer to the first character of the cleaned string                                                                                                                                                                                                                                                                       
 *       Does: Converts a sequence into an array of characters with it                                                                                                                                                                                                                                                                           
 *             concluding with a string terminating character                                                                                                                                                                                                                                                                                    
 */
char *makeArray(Seq_T seq, int length)
{
        char *line = NULL;
        line = (char *)malloc(sizeof(char *) * (length + 1));
        if (line == NULL) {
                err(seq, "Error: memory allocation failed\n");
        }
        for (int i = 0; i < length; i++) {
                line[i] = *(char *)Seq_get(seq, i);
                free((char *)Seq_get(seq, i));
        }
        void *seq_p = seq;
        if (seq_p != NULL) {
                Seq_free(&seq);
        }
        line[length] = '\0';
        return line;
}

/* int checkChar(char *toCheck)                                                                                                                                                                                                                                                                                                                  
 * Parameters: [char *toCheck] - char to be validated                                                                                                                                                                                                                                                                                            
 *    Returns: True (1) if valid char, False (0) if not valid                                                                                                                                                                                                                                                                                    
 *             Valid chars are alpha numerical values and underscores                                                                                                                                                                                                                                                                            
 *       Does: checks if char is a valid char to be inserted into the clean                                                                                                                                                                                                                                                                      
 *             string                                                                                                                                                                                                                                                                                                                            
 */
int checkChar(char *toCheck)
{
        if (isalnum(*toCheck) || *toCheck == 95) {
                return 1;
        }
        return 0;
}

/* void err(Seq_T seq, char *msg)                                                                                                                                                                                                                                                                                                                
 * Parameters: [Seq_T seq] - Sequence that should be freed                                                                                                                                                                                                                                                                                       
 *             [char *msg] - message to be printed to terminal window                                                                                                                                                                                                                                                                            
 *    Returns: Nothing                                                                                                                                                                                                                                                                                                                           
 *       Does: Frees allocated memory in the event of an error, prints an error                                                                                                                                                                                                                                                                  
 *             message to the user terminal window, and calls EXIT_FAILURE                                                                                                                                                                                                                                                                       
 */
void err(Seq_T seq, char *msg)
{
        void *seq_p = seq;
        if (seq_p != NULL) {
                Seq_free(&seq);
        }
        fprintf(stderr, msg);
        exit(EXIT_FAILURE);
}


