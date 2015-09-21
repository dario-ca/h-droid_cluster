/*====================================================================
 *							MPI SCATTER
 * 			Written by Bruzzo Paolo and Casula Dario
 * 					Thanks to Ferroni Matteo
 *
 *  This program can be used to test the correct setup of the cluster
 *====================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>


/*  globals */
int numnodes,myid,mpi_err;
#define mpi_root 0
/* end globals  */

void init_it(int  *argc, char ***argv);

int main(int argc,char *argv[]){
	int *myray,*send_ray,*back_ray;
	int count;
	int size,mysize,i,k,j,total;
	char hostname[256];
	
	init_it(&argc,&argv);
/* each processor will get 'count' elements from the root */
	count=4;
	myray=(int*)malloc(count*sizeof(int)); //space needed by 'count' integers
/* create the data to be sent on the root */
	if(myid == mpi_root){
	    size=count*numnodes; //total number of integers to be sent
		send_ray=(int*)malloc(size*sizeof(int)); //space needed by all the integers to be sent
		back_ray=(int*)malloc(numnodes*sizeof(int)); //space needed by the results received (1 from each process)
		for(i=0;i<size;i++)
			send_ray[i]=i;
		}
/* send different data to each processor */
	mpi_err = MPI_Scatter(	send_ray, count,   MPI_INT,
						    myray,    count,   MPI_INT,
	                 	    mpi_root,
	                 	    MPI_COMM_WORLD);
	                
/* each processor does a local sum */
	total=0;
	for(i=0;i<count;i++)
	    total=total+myray[i];
	// just save the machine hostname to printf it
	gethostname(hostname,255);
	printf("- pid=%d on %s (process_rank=%d) computed a total= %d\n",getpid(),hostname,myid,total);
/* send the local sums back to the root */
    mpi_err = MPI_Gather(&total,    1,  MPI_INT, 
						back_ray, 1,  MPI_INT, 
	                 	mpi_root,                  
	                 	MPI_COMM_WORLD);
/* the root prints the global sum */
	if(myid == mpi_root){
	  total=0;
	  for(i=0;i<numnodes;i++)
	    total=total+back_ray[i];
	  printf("===================================\n  Results from all processes= %d \n===================================\n",total);
	}
    mpi_err = MPI_Finalize();
}

void init_it(int  *argc, char ***argv) {
	mpi_err = MPI_Init(argc,argv);
    mpi_err = MPI_Comm_size( MPI_COMM_WORLD, &numnodes );
    mpi_err = MPI_Comm_rank(MPI_COMM_WORLD, &myid);
}
