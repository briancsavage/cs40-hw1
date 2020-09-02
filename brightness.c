/*                                                                                                                                                                                                                                                                               
 * Filename: brightness.c                                                                                                                                                                                                                                                        
 * By: Robert Lester, Brian Savage                                                                                                                                                                                                                                               
 * Due: 1/31/2020                                                                                                                                                                                                                                                                
 * HW1                                                                                                                                                                                                                                                                           
 * Summary: This program computes the average pixel brightness for a provided                                                                                                                                                                                                    
 *          pmg file. The image name can either be passed as a command line                                                                                                                                                                                                      
 *          argument or through standard in. The program will exit in the event                                                                                                                                                                                                  
 *          of an error.                                                                                                                                                                                                                                                         
 */

#include <stdio.h>
#include <except.h>
#include <stdlib.h>
#include "pnmrdr.h"

FILE *process_input(FILE *fp, int argc, char *argv[]);
float total_brightness(Pnmrdr_T rdr, double rdr_size, double rdr_denominator);
float average_brightness(FILE *fp);
void error_rdr(FILE *fp, Pnmrdr_T rdr, char *msg);
void error(FILE *fp, char *msg);

/* int main(int argc, char *argv[])                                                                                                                                                                                                                                              
 * Parameters: int argc - number of command line arguments                                                                                                                                                                                                                       
 *             char *argv[] - command line arguments                                                                                                                                                                                                                             
 * Returns: returns EXIT_SUCCESS upon successful completion                                                                                                                                                                                                                      
 * Does: Initializes a FILE object, calls process_input to read in the file                                                                                                                                                                                                      
 *       which calls the rest of the functions necessary to run the program,                                                                                                                                                                                                     
 *       closes the file if successful and exits the programs with exit code                                                                                                                                                                                                     
 *       EXIT_SUCCESS                                                                                                                                                                                                                                                            
 *                                                                                                                                                                                                                                                                               
 */
int main(int argc, char *argv[])
{
        FILE *fp = NULL;
        fp = process_input(fp, argc, argv);
        printf("Average Brightness: %0.3f\n", average_brightness(fp));
        fclose(fp);
        return EXIT_SUCCESS;
}

/*                                                                                                                                                                                                                                                                               
 * FILE *processInput(FILE *fp, int argc, char *argv[])                                                                                                                                                                                                                          
 * Parameters: FILE *fp - FILE object taken in to be passed in to be closed if                                                                                                                                                                                                   
 *                         program encounters an error                                                                                                                                                                                                                           
 *             int argc - number of commandline arguments                                                                                                                                                                                                                        
 *             char *argv[] - command line arguments                                                                                                                                                                                                                             
 * Returns: FILE * - Opened FILE object                                                                                                                                                                                                                                          
 * Does: Reads in command line arguments/stdin and either opens the files                                                                                                                                                                                                        
 *       it receives or handles exceptions, passes them to our error functions,                                                                                                                                                                                                  
 *       and exits the program in there                                                                                                                                                                                                                                          
 */
FILE *process_input(FILE *fp, int argc, char *argv[])
{
        if (argc > 2) {
                error(NULL, "Error: too many arguments given\n");
        }
        if (argc == 2) {
                fp = fopen(argv[1], "rb");
                if (fp == NULL) {
                        error(NULL, "Error: trouble reading file\n");
                }
        }
        if (argc == 1) {
                fp = stdin;
                if (fp == NULL) {
                        error(NULL, "Error: trouble reading file\n");
                }
        }
        return fp;
}

/*                                                                                                                                                                                                                                                                               
 * float average_brightness(FILE *fp)                                                                                                                                                                                                                                            
 * Parameters: FILE *fp - pointer to opened FILE object                                                                                                                                                                                                                          
 * Returns: float - average brightness of the image represented as a float                                                                                                                                                                                                       
 * Does: Tries to create a Pnmrdr_T object, but raises exceptions if the file                                                                                                                                                                                                    
 *       is not in the proper format or is empty. If successful, it uses the                                                                                                                                                                                                     
 *       data from the Pnmrdr_mapdata to calculate the total amount of                                                                                                                                                                                                           
 *       pixels, passes that number and the denominator from the Pnmrdr_mapdata                                                                                                                                                                                                  
 *       into total_brightness which returns the toal brightness of the image                                                                                                                                                                                                    
 *       (see below), and divides that number by the number of pixels in the                                                                                                                                                                                                     
 *       image. Then it frees the Pnmrdr_T object at the end.                                                                                                                                                                                                                    
 */
float average_brightness(FILE *fp)
{
        Pnmrdr_mapdata data;
        Pnmrdr_T rdr;

        TRY;
                rdr = Pnmrdr_new(fp);
        EXCEPT(Pnmrdr_Badformat);
                error_rdr(fp, NULL,
                          "Error: bad format, could not read image\n");
        EXCEPT(Pnmrdr_Count);
                error_rdr(fp, rdr,
                          "Error: could not read pixels\n");
        END_TRY;

        data = Pnmrdr_data(rdr);
        if (data.type != Pnmrdr_gray) {
                error_rdr(fp, rdr,
                          "Error: incorrect file type, requires pgm file\n");
        }

        unsigned num_pixels = 0;
        float full_image_brightness = 0.0;
        float average_brightness = 0.0;

        num_pixels = data.width * data.height;
        full_image_brightness = total_brightness(rdr, num_pixels,
                                                  data.denominator);
        average_brightness = full_image_brightness / num_pixels;

        Pnmrdr_free(&rdr);
        return average_brightness;
}

/*                                                                                                                                                                                                                                                                               
 * float total_brightness(Pnmrdr_T rdr, double rdr_size,                                                                                                                                                                                                                         
 *                        double rdr_denominator)                                                                                                                                                                                                                                
 * Parameters: Pnmrdr_T rdr - Pnmrdr_T object representing the pgm file                                                                                                                                                                                                          
 *             double rdr_size - total number of pixels in the Pnmrdr_T object,                                                                                                                                                                                                  
 *                               (height * width)                                                                                                                                                                                                                                
 *             double rdr_denominator - denominator for the Pnmrdr_T object                                                                                                                                                                                                      
 *                                      provided by Pnmrdr_data                                                                                                                                                                                                                  
 * Returns: float - represents total brightness of image                                                                                                                                                                                                                         
 * Does: Checks the brightness of each individual pixel by dividing the value                                                                                                                                                                                                    
 *       held in its index in the Pnmrdr_T object, adding it to a running                                                                                                                                                                                                        
 *       total, which is returned to be divided by the total number of pixels                                                                                                                                                                                                    
 *       in the image in the function average_brightness.                                                                                                                                                                                                                        
 */
float total_brightness(Pnmrdr_T rdr, double rdr_size, double rdr_denominator)
{
        unsigned i = 0;
        unsigned temp = 0;
        float count = 0;
        while (i < rdr_size) {
                temp = Pnmrdr_get(rdr);
                count += temp / rdr_denominator;
                i++;
        }
        return count;
}

/*                                                                                                                                                                                                                                                                               
 * void error_rdr(FILE *fp, Pnmrdr_T rdr, char *msg)                                                                                                                                                                                                                             
 * Parameters: FILE *fp - pointer to opened FILE object, to be closed if an                                                                                                                                                                                                      
 *                        error is encountered                                                                                                                                                                                                                                   
 *            Pnmrdr_T rdr - Pnmrdr_T object passed to be freed if an error is                                                                                                                                                                                                   
 *                           encountered                                                                                                                                                                                                                                         
 *            char *msg - custom error message                                                                                                                                                                                                                                   
 * Returns: Nothing                                                                                                                                                                                                                                                              
 * Does: When a file exists and a Pnmrdr_T object is created, but is in the                                                                                                                                                                                                      
 *       wrong format, or the Pnmrdr_T is empty, this function is called in                                                                                                                                                                                                      
 *       average_brightness in order to print an error message, close the file,                                                                                                                                                                                                  
 *       free the Pnmrdr_T object, and exit the program with EXIT_FAILURE.                                                                                                                                                                                                       
 */
void error_rdr(FILE *fp, Pnmrdr_T rdr, char *msg)
{
        void *rdr_p = rdr;
        fprintf(stderr, msg);
        if (rdr_p != NULL) {
                Pnmrdr_free(&rdr);
        }
        if (fp != NULL) {
                fclose(fp);
        }
        exit(EXIT_FAILURE);
}
/*                                                                                                                                                                                                                                                                               
 * void error(FILE *fp, char *msg)                                                                                                                                                                                                                                               
 * Parameters: FILE *fp - pointer to opened FILE object, to be closed if an                                                                                                                                                                                                      
                          error is encountered                                                                                                                                                                                                                                   
 *            char *msg -  custom error message                                                                                                                                                                                                                                  
 *    Returns: Nothing                                                                                                                                                                                                                                                           
 *       Does: When there are too many command line arguments provided, or the                                                                                                                                                                                                   
 *             file provided in either the command line arguments or stdin are                                                                                                                                                                                                   
 *             is empty, this function is called in proccess_input to close                                                                                                                                                                                                      
 *             any open file, print an error message the program with                                                                                                                                                                                                            
 *             EXIT_FAILURE.                                                                                                                                                                                                                                                     
 */
void error(FILE *fp, char *msg)
{
        fprintf(stderr, msg);
        if (fp != NULL) {
                fclose(fp);
        }
        exit(EXIT_FAILURE);
}
