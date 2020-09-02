/*                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
 * Filename: simlines.c                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
 * By: Robert Lester, Brian Savage                                                                                                                                                                                                                                                                                                                                                                                                                                                           
 * Due: 1/31/2020                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
 * HW1                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
 * Summary: This program creates a Table_T object holding Seq_T objects which                                                                                                                                                                                                                                                                                                                                                                                                                
 *          hold Line structs containing the line's number and the file in                                                                                                                                                                                                                                                                                                                                                                                                                   
 *          which the line appears then prints the similar lines or                                                                                                                                                                                                                                                                                                                                                                                                                          
 *          "matching groups" between and within files and handles any errors                                                                                                                                                                                                                                                                                                                                                                                                                
 *          that may occur.                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
 */

#include <stdio.h>
#include <stdlib.h>
#include "readaline.h"
#include "table.h"
#include "atom.h"
#include <string.h>
#include "seq.h"
#include "cleanstring.h"

/* Line struct holds information for the name of the file in which the line                                                                                                                                                                                                                                                                                                                                                                                                                  
 * appears in a char *, and holds the line's number in the file                                                                                                                                                                                                                                                                                                                                                                                                                              
 */
typedef struct {
        char *filename;
        int num;
} Line;

static void print_table(const void *key, void ** value, void *cl);
void add_line(Table_T table, Line *line, const char *key);
Line *build_line(Line *line, char *filename, int num);
int process_line(char *line, Table_T table, char *filename, int line_num);

/*                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
 * int main(int argc, char* argv[])                                                                                                                                                                                                                                                                                                                                                                                                                                                          
 * Parameters: int argc - number of command line arguments                                                                                                                                                                                                                                                                                                                                                                                                                                   
 *             char * argv[] - the command line arguments                                                                                                                                                                                                                                                                                                                                                                                                                                    
 * Returns: return EXIT_SUCCESS upon successful completion                                                                                                                                                                                                                                                                                                                                                                                                                                   
 * Does: Reads in command line arguments, opens them as files, creates a                                                                                                                                                                                                                                                                                                                                                                                                                     
 *       Table_T object, calls readaline to get a line, and then passes it                                                                                                                                                                                                                                                                                                                                                                                                                   
 *       to processLine, then closes the file, prints and frees the Seq_T                                                                                                                                                                                                                                                                                                                                                                                                                    
 *       objects with data for more than one line, and finally frees the                                                                                                                                                                                                                                                                                                                                                                                                                     
 *       Table_T object                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
 *                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
 */
int main(int argc, char* argv[])
{
        if (argc == 1){
                fprintf(stderr, "Error: Too few arguments provided");
                exit(EXIT_FAILURE);
        }
        Table_T table = Table_new(0, NULL, NULL);
        for (int i = 1; i < argc; i++){
                FILE *fp = NULL;
                char *filename = argv[i];
                fp = fopen(filename, "r");
                if (fp == NULL || ferror(fp)) {
                        continue;
                }
                int line_num = 0;
                size_t alloc_size = 0;
                size_t line_alloc = 0;
                while (!feof(fp)) {
                        char *line = NULL;
                        line_alloc = readaline(fp, &line);
                        line_num = process_line(line, table, filename,
                                               line_num);
                        alloc_size += line_alloc;
                }
                fclose(fp);
        }
        int count = 1;
        Table_map(table, print_table, &count);
        Table_free(&table);
        return EXIT_SUCCESS;
}

/*                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
 * process_line(char *line, Table_T table, char *filename, int line_num)                                                                                                                                                                                                                                                                                                                                                                                                                     
 * Parameters: char *line - line to be processed                                                                                                                                                                                                                                                                                                                                                                                                                                             
 *             Table_T table - table that holds all sequences                                                                                                                                                                                                                                                                                                                                                                                                                                
 *             char *filename - name of file in which line appears                                                                                                                                                                                                                                                                                                                                                                                                                           
 *             int line_num - line's number in file                                                                                                                                                                                                                                                                                                                                                                                                                                          
 * Returns: int representing the line's number in file                                                                                                                                                                                                                                                                                                                                                                                                                                       
 * Does: Takes in a line from a file, cleans it by calling cleanString,                                                                                                                                                                                                                                                                                                                                                                                                                      
 *       creates the spot in the table to which it and lines similar to it                                                                                                                                                                                                                                                                                                                                                                                                                   
 *       will be stored using Atoms, creates a Line struct which is built by                                                                                                                                                                                                                                                                                                                                                                                                                 
 *       calling build_line, and adds that Line struct to the sequence at that                                                                                                                                                                                                                                                                                                                                                                                                               
 *       key                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
 */
int process_line(char *line, Table_T table, char *filename, int line_num)
{
        char *cleaned = NULL;
        cleaned = cleanString(line);
        if (cleaned != NULL) {
                if (*cleaned == '\0') {
                        free(cleaned);
                        cleaned = NULL;
                }
        }
        if (cleaned != NULL && *cleaned != '\n') {
                line_num++;
                const char *key = Atom_string(cleaned);
                Line *line_data = (Line *)malloc(sizeof(Line));
                if (line_data == NULL){
                        exit(EXIT_FAILURE);
                }
                Line *new_line = build_line(line_data,
                                            filename,
                                            line_num);
                add_line(table, new_line, key);
        }
        free(cleaned);
        return line_num;
}

/*                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
 * add_line(Table_T table, Line *line, const char *key)                                                                                                                                                                                                                                                                                                                                                                                                                                      
 * Parameters: Table_T table - Table_T object created in main which holds                                                                                                                                                                                                                                                                                                                                                                                                                    
 *                             information for all lines in all files                                                                                                                                                                                                                                                                                                                                                                                                                        
 *             Line *line - struct for a line in a file                                                                                                                                                                                                                                                                                                                                                                                                                                      
 *             const char *key - the Atom for a line                                                                                                                                                                                                                                                                                                                                                                                                                                         
 * Returns: Nothing                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
 * Does: If there is no sequence at the key, add_line creates a new Seq_T                                                                                                                                                                                                                                                                                                                                                                                                                    
 *       object, adds the Line struct to that sequence, then places it in                                                                                                                                                                                                                                                                                                                                                                                                                    
 *       the Table_T object at the key. Otherwise it appends the Line struct                                                                                                                                                                                                                                                                                                                                                                                                                 
 *       to the end of the sequence at the key.                                                                                                                                                                                                                                                                                                                                                                                                                                              
 */
void add_line(Table_T table, Line *line, const char *key)
{
        void *temp = Table_get(table, key);
        if(temp == NULL){
                Seq_T seq = Seq_new(0);
                Seq_addhi(seq, line);
                Table_put(table, key, seq);
        } else {
                Seq_addhi(temp, line);
        }
}

/*                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
 * static void print_table(const void *key, void **value, void *cl)                                                                                                                                                                                                                                                                                                                                                                                                                          
 * Parameters: const void *key - the Atom for a line                                                                                                                                                                                                                                                                                                                                                                                                                                         
 *             void **value - value at the key                                                                                                                                                                                                                                                                                                                                                                                                                                               
 *             void *cl - specifies close                                                                                                                                                                                                                                                                                                                                                                                                                                                    
 * Returns: Nothing                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
 * Does: Static void function called by Table_map which goes through the                                                                                                                                                                                                                                                                                                                                                                                                                     
 *       table printing the values at keys with more than one element in the                                                                                                                                                                                                                                                                                                                                                                                                                 
 *       Seq_T object, and then the filename and line number for each instance                                                                                                                                                                                                                                                                                                                                                                                                               
 *       of the line in each file                                                                                                                                                                                                                                                                                                                                                                                                                                                            
 */
static void print_table(const void *key, void **value, void *cl)
{
        Seq_T seq = *value;
        const char *key_ = key;
        int *count = (int*)cl;
        if (Seq_length(seq) > 1) {
                if (*count == 1) {
                        printf("%s\n", key_);
                        *count = 0;
                } else {
                        printf("\n%s\n", key_);
                }
                while(Seq_length(seq) != 0){
                        Line *line = Seq_remlo(seq);
                        printf("%-20s %d\n", line->filename,
                               line->num);
                        free(line);
                }
        } else {
                Line *line = Seq_remlo(seq);
                free(line);
        }
        Seq_free(&seq);
}

/*                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
 * Line *build_line(Line *line, char *filename, int num)                                                                                                                                                                                                                                                                                                                                                                                                                                     
 * Parameters: Line *line - struct for a line in file                                                                                                                                                                                                                                                                                                                                                                                                                                        
 *             char *filename - name of file in which line appears                                                                                                                                                                                                                                                                                                                                                                                                                           
 *             int num - line's number in file                                                                                                                                                                                                                                                                                                                                                                                                                                               
 * Returns: pointer to Line object just created                                                                                                                                                                                                                                                                                                                                                                                                                                              
 * Does: This function takes in the name of the file in which the line appears,                                                                                                                                                                                                                                                                                                                                                                                                              
 *       the line's number within the file, and a Line object, and then sets                                                                                                                                                                                                                                                                                                                                                                                                                 
 *       the variable char *filename within the Line object to the name of the                                                                                                                                                                                                                                                                                                                                                                                                               
 *       file, and the variable int num within the Line object to the line's                                                                                                                                                                                                                                                                                                                                                                                                                 
 *       number                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
 */
Line *build_line(Line *line, char *filename, int num)
{
        line->filename = filename;
        line->num = num;
        return line;
}


