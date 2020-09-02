/*                                                                                                                                                                                                                                                                                                                                                                                               
 * Filename: readaline.c                                                                                                                                                                                                                                                                                                                                                                         
 * Authors: Robert Lester, Brian Savage                                                                                                                                                                                                                                                                                                                                                          
 * Assignment: HW1                                                                                                                                                                                                                                                                                                                                                                               
 * Summary: when readaline() is called, its sets the passed parameters datapp                                                                                                                                                                                                                                                                                                                    
 *          equal to the first character in the gotten line, which is stored                                                                                                                                                                                                                                                                                                                     
 *          as an allocated sequence of characters. Therefore, the gotten                                                                                                                                                                                                                                                                                                                        
 *          line needs to be freed by the client to prevent memory leaks                                                                                                                                                                                                                                                                                                                         
 */

#include "readaline.h"
#include <stdio.h>
#include <stdlib.h>
#include <except.h>
#include <ctype.h>
#include "seq.h"
#include <string.h>

int get_line(Seq_T seq, char **datapp, FILE *inputfd);
size_t make_Array(Seq_T seq, char **datapp, FILE *inputfd, int length);
void error(Seq_T seq, char **datapp, FILE *inputfd, char *msg);

/* size_t readaline(FILE *inputfd, char **datapp)                                                                                                                                                                                                                                                                                                                                                
 * Parameters: [FILE *inputfd] - file to be read from                                                                                                                                                                                                                                                                                                                                            
 *             [char **datapp] - the line gotten will be set equal to datapp                                                                                                                                                                                                                                                                                                                     
 *    Returns: size_t - the memory allocation size of the gotten line                                                                                                                                                                                                                                                                                                                            
 *       Does: read a line from the file, if nothing can be read or line is                                                                                                                                                                                                                                                                                                                      
 *             only a new line character the datapp is                                                                                                                                                                                                                                                                                                                                           
 */
size_t readaline(FILE *inputfd, char **datapp)
{
        if (inputfd == NULL || datapp == NULL) {
                error(NULL, datapp, inputfd, "Error: Input or Data is null\n");
        }
        Seq_T seq = Seq_new(1);
        size_t alloc_size = 0;
        int length = get_line(seq, datapp, inputfd);
        if (length != 0) {
          alloc_size = make_Array(seq, datapp, inputfd, length);
        } else {
                void *seq_p = seq;
                if (seq_p != NULL) {
                        Seq_free(&seq);
                }
                datapp = NULL;
        }
        return alloc_size;
}

/* int get_line(Seq_T seq, char **datapp, FILE *inputfd)                                                                                                                                                                                                                                                                                                                                         
 * Parameters: [Seq seq] - the sequence that will store the gotten chars                                                                                                                                                                                                                                                                                                                         
 *             [char **datapp] - only used if error occurs during reading                                                                                                                                                                                                                                                                                                                        
 *                               character process                                                                                                                                                                                                                                                                                                                                               
 *             [FILE *inputfd] - the file to get a line from                                                                                                                                                                                                                                                                                                                                     
 *    Returns: The length of the line gotten from the file as an integer                                                                                                                                                                                                                                                                                                                         
 *       Does: Gets the line of the file seek pointer and adds each char to                                                                                                                                                                                                                                                                                                                      
 *             the end of the passed seq                                                                                                                                                                                                                                                                                                                                                         
 */
int get_line(Seq_T seq, char **datapp, FILE *inputfd)
{
        int length = 0;
        char *temp = NULL;
        while (1) {
                temp = malloc(sizeof(char *));
                if (temp == NULL) {
                        error(seq, datapp, inputfd, "Error: memory allocation "
                                                    "failed\n");
                }
                *temp = fgetc(inputfd);
                if (ferror(inputfd)) {
                        if (temp != NULL) {
                                free(temp);
                                temp = NULL;
                        }
                        error(seq, datapp, inputfd, "Error: could not read "
                                                    "character from file\n");
                }
                if (temp == NULL) {
                        error(seq, datapp, inputfd, "Error: char is null");
                }
                if (*temp == EOF) {
                          free(temp);
                          return length;
                } else {
                        Seq_addhi(seq, temp);
                        length++;
                        if (*temp == '\n') {
                                return length;
                        }
                }
        }
        return length;
}

/* size_t make_Array(Seq_T seq, char **datapp, FILE *inputfd, int length)                                                                                                                                                                                                                                                                                                                        
 * Parameters: [Seq_T seq] - the original sequence of chars gotten from file                                                                                                                                                                                                                                                                                                                     
 *             [char **datapp] - will be set equal to the first address of the                                                                                                                                                                                                                                                                                                                   
 *                               created character array                                                                                                                                                                                                                                                                                                                                         
 *             [FILE *inputfd] - used for error handling, closes file if error                                                                                                                                                                                                                                                                                                                   
 *                               occurs                                                                                                                                                                                                                                                                                                                                                          
 *             [int length] - the length of the sequence                                                                                                                                                                                                                                                                                                                                         
 *    Returns: the allocation size of the char array                                                                                                                                                                                                                                                                                                                                             
 *       Does: Converts a sequence of chars into a dynamically allocated array                                                                                                                                                                                                                                                                                                                   
 *             of chars and sets datapp equal to the first address of this                                                                                                                                                                                                                                                                                                                       
 *             array                                                                                                                                                                                                                                                                                                                                                                             
 */
size_t make_Array(Seq_T seq, char **datapp, FILE *inputfd, int length)
{
        char *line = NULL;
        line = (char *)malloc(length + 1);
        if (line == NULL) {
                error(seq, datapp, inputfd, "Error: can't allocate memory");
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
        *datapp = line;
        return strlen(line);
}

/* void error(Seq_T seq, char **datapp, FILE *inputfd, char *msg)                                                                                                                                                                                                                                                                                                                                
 * Parameters: [Seq_T seq] - sequence will be freed if error occurs                                                                                                                                                                                                                                                                                                                              
 *             [char **datapp] - will be free if error occurs                                                                                                                                                                                                                                                                                                                                    
 *             [FILE *inputfd] - will be closed if not null during error                                                                                                                                                                                                                                                                                                                         
 *             [char *msg] - message to be printed when error occurs                                                                                                                                                                                                                                                                                                                             
 *    Returns: Nothing                                                                                                                                                                                                                                                                                                                                                                           
 *       Does: Handles allocated memory in the event an error occurs                                                                                                                                                                                                                                                                                                                             
 */
void error(Seq_T seq, char **datapp, FILE *inputfd, char *msg)
{
        fprintf(stderr, msg);
        void *seq_p = seq;
        if (seq_p != NULL) {
                Seq_free(&seq);
        }
        if (*datapp != NULL) {
                free(datapp);
        }
        if (inputfd != NULL) {
                fclose(inputfd);
        }
        exit(EXIT_FAILURE);
}



