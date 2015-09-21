/*====================================================================
 *					HPL LINPACK OUTPUT SCANNER
 * 			Written by Bruzzo Paolo and Casula Dario
 * 					Thanks to Ferroni Matteo
 *
 *  This program wants in input:
 *
 *  - src_file: an HPL output file containing the tests results
 *  - dest_file: an empty file to fill
 *
 *  The dest_file will be a list of results in Gflops,
 *  plus the average of all the computations.
 *====================================================================*/


#include<stdio.h>
#include<stdlib.h>

#define HEADER_OFFSET 2082
#define LOOP_OFFSET 576

FILE *dest_file;
FILE *src_file;

/*====================================================================
	FUNCTIONS HEADER
====================================================================*/

/*Transfer the values.
 * Returns 0 in the transfer completes successfully, 1 otherwise */
int transfer();

/*Print the result*/
void print_result(int, double, FILE* );

/*====================================================================
	MAIN
====================================================================*/

/*
	argv[1] = src_path
	argv[2] = dest_path
*/
int main (int argc, char*argv[]){

	int error = 0;

	if(argc != 3)
		fprintf(stderr,"ERROR: The scanner must be launched as: ./scanner src_file dest_file\n");
	else{
		
		/*Open the src_file in read mode*/
		if(src_file=fopen(argv[1],"r")){
			/*Open the dest_file in write mode*/
			if(dest_file=fopen(argv[2],"w")){
				
				fprintf(stdout,"\nTransfer in progress...\n");
				error = transfer();
				fprintf(stdout,"Transfer completed with %d errors\n\n",error);
				
			fclose(dest_file);
			}else{
				fprintf(stderr,"ERROR: File %s cannot be created\n\n",argv[2]);
			}
			fclose(src_file);
		}else
		fprintf(stderr,"ERROR: File %s not found or corrupted\n\n",argv[1]);
			
	}
	return 0;
}

/*====================================================================
	FUNCTIONS BODY
====================================================================*/

int transfer(){
	int error1 = 0;
	int error2 = 0;
	char string[10];
	
	/*To calculate the average*/
	double sum = 0;
	int count = 0; 
	
	/*Go to the first number*/
	error2 = fseek(src_file,HEADER_OFFSET,SEEK_SET);
	
	/*Exit in case of error*/
	if(error2 != 0) return 1;
	
	/*Read values up to the end of the src_file...*/
	while( fscanf(src_file,"%s",string) != EOF){
		
		/*Store the value in the dest_file*/
		error1 = fprintf(dest_file,"%s\n", string);
		
		/*Go to the next number*/
		error2 = fseek(src_file,LOOP_OFFSET,SEEK_CUR);
		
		/*Average update*/
		sum += atof(string);
		count++;
	
		/*Exit in case of error*/
		if(error1 == -1 || error2 == -1) return 1;
	}
	
	/*Print results*/
	print_result(count-1,sum,dest_file);
	print_result(count-1,sum,stdout);
	
	return 0;
}



void print_result(int tests, double sum, FILE* file){
	fprintf(file,"======================================\n");
	fprintf(file,"   - Performed %d tests\n", tests);
	fprintf(file,"   - Average = %.3f Mflops\n", sum*1000/tests);
	fprintf(file,"======================================\n");
}

