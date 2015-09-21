/*====================================================================
 *					 ANDROID CLUSTER SETUP
 * 			Written by Bruzzo Paolo and Casula Dario
 * 					Thanks to Ferroni Matteo
 *
 *  This script has to be run on the Master node (phone1), and the 
 *  slaves have to be reachable trough an SSH connection.
 *====================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

// File paths
#define ETC_HOSTS "/etc/hosts"
#define MPD_HOSTS "/root/mpd.hosts" 
#define MPD_CONF "/root/.mpd.conf"
#define MASTER_PUBKEY "/root/.ssh/id_rsa.pub"
#define SLAVE_AUTHKEY "/root.ssh/authorized_keys"

// Maximum number of phones in the cluster
#define MAX_PHONES 10
// (Max) Lenght of an IP address (dots included)
#define IP_ADDR_LEN 15
// Maximum number of processes that can be allocated on EACH phone
#define MAX_PROC_PHONE 20

// this function initializes all configuration files in order to overwrite them
void init_files();

int main(int argc, char const *argv[])
{
	// number of phones in the cluster, it will be requested to the user
	int numPhones;
	// support string to check the validity of the ip address 
	char ipAddr[IP_ADDR_LEN+1];
	// array of ip addresses of the nodes in the cluster
	char savedAddr[MAX_PHONES][IP_ADDR_LEN+1];
	// support string to run the command trough a system call
	char command[100];
	// support variable to get the number of processes to execute on each node
	int numProcs;
	// regular expression of an ip address
	regex_t regex;
	// other
	int i, status;

	init_files();

	// asking number of phones of the cluster
	do{
		printf("Enter the total number of phones of your cluster (master phone included)(max=%d): ",MAX_PHONES);
		scanf("%d",&numPhones);
	}while(numPhones<=0 || numPhones>MAX_PHONES);

	// variable useful for the regular expression
	regmatch_t numMatch[numPhones];

	// build the regular expression of an ip address
	regcomp(&regex, 
        	"^([0-9]|[1-9][0-9]|1([0-9][0-9])|2([0-4][0-9]|5[0-5]))."
         	"([0-9]|[1-9][0-9]|1([0-9][0-9])|2([0-4][0-9]|5[0-5]))."
         	"([0-9]|[1-9][0-9]|1([0-9][0-9])|2([0-4][0-9]|5[0-5]))."
         	"([0-9]|[1-9][0-9]|1([0-9][0-9])|2([0-4][0-9]|5[0-5]))$", REG_EXTENDED);

	/*====================================================================
	 *  WRITE ON:
	 * 		- /etc/hosts
	 *		- /root/mpd.hosts
	 *		- /root/machinefile
	 *====================================================================*/
	// ask ip address of every phone
	printf("NOTICE: phone1 (this phone) has to be the master!\n");
	for( i=0 ; i < numPhones ; i++ ){
		printf("Enter the ip address of phone%d: ",i+1);
		scanf("%s",ipAddr);

		// check the validity of the ipaddress
		status=regexec(&regex,ipAddr,(size_t)numPhones,numMatch,0);
		// if the ip address is valid
		if(status==0){
			do{
				printf("Enter the number of processes for phone%d (max %d): ",i+1,MAX_PROC_PHONE);
				scanf("%d",&numProcs);
			}while(numProcs<=0 || numProcs > MAX_PROC_PHONE);

			// append to /etc/hosts
			sprintf(command,"echo %s phone%d >> %s",ipAddr,i+1,ETC_HOSTS);
			system(command);

			// append to /root/machinefile
			sprintf(command,"echo %s:%d >> %s",ipAddr,numProcs, MPD_HOSTS);
			system(command);

			// save the ip address
			strcpy(savedAddr[i],ipAddr);

			printf("OK: ip address of phone%d written\n",i+1 );
		}else{
			printf("ERROR: ip address of phone%d is not valid\n",i+1);
			i--;
		}
	}
	// free memory
	regfree(&regex);
	printf("Number of phone set: %d\n",i);

	/*====================================================================
	 *  WRITE ON:
	 * 		- /root/.mpd.conf
	 *====================================================================*/
	//write /root/.mpd.conf
	sprintf(command,"echo secretword=droidcluster > %s",MPD_CONF);
	system(command);

	/*====================================================================
	 *  SSH COPY to every slave
	 *====================================================================*/
	// i starts from 1 because this phone does not need to ssh-copy the files to itself
	for(i=1;i<numPhones;i++){
		printf("Copying configuration files to phone%d ...\n",i+1);

		sprintf(command,"scp %s root@%s:%s",MASTER_PUBKEY,savedAddr[i],SLAVE_AUTHKEY);
		system(command);

		sprintf(command,"scp %s root@%s:%s",ETC_HOSTS,savedAddr[i],ETC_HOSTS);
		system(command);	

		sprintf(command,"scp %s root@%s:%s",MPD_HOSTS,savedAddr[i],MPD_HOSTS);
		system(command);

		sprintf(command,"scp %s root@%s:%s",MPD_CONF,savedAddr[i],MPD_CONF);
		system(command);
	}

	/*====================================================================
	 *  BOOT MPD DAEMON PROCESS ON EVERY PHONE
	 *====================================================================*/
	sprintf(command,"mpdboot -v -f %s -n %d",MPD_HOSTS,numPhones);
	system(command);

	return 0;
}


void init_files(){
	// support string to run the command trough a system call
	char command[100];
	//setting localhost on /etc/hosts
	sprintf(command,"echo 127.0.0.1 localhost > %s",ETC_HOSTS);
	system(command);
	//initializing /root/mpd.hosts
	sprintf(command, "echo > %s", MPD_HOSTS);
	system(command);
	//initializing /root/.mpd.conf
	sprintf(command, "echo > %s", MPD_CONF);
	system(command);
}