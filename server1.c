/* server process */
#include <my_global.h>
#include <mysql.h> 
#include <ctype.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <unistd.h> 
#include <stdio.h> 
#include <pthread.h>
#include <stdlib.h> 
#include <signal.h> 
#include <time.h>
#include <string.h>
#include <stdlib.h>


#include <string.h>
//#include <iostream.h>
void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);        
}
#define SIZE sizeof(struct sockaddr_in) 
#define MAX 5
 
int newsockfd;

void general(int m);
void catcher(int sig);

MYSQL *con;


pthread_mutex_t msg_mutex = PTHREAD_MUTEX_INITIALIZER;

int status, *status_ptr = &status;
struct mymsg{
	int lineno;
	void* ptr;
	int size;
};
void handle(struct mymsg ms, int result, int target_client);
void sendfree(struct mymsg ms, int target_client);


int main(void) 
{ 

	con = mysql_init(NULL);
  
	if (con == NULL) 
	{
	    fprintf(stderr, "%s\n", mysql_error(con));
	    exit(1);
	}  
	
	if (mysql_real_connect(con, "localhost", "yogin16", "123", "leashing", 0, NULL, 0) == NULL) 
	{
	      finish_with_error(con);
	}    
  
  	if (mysql_query(con, "DROP TABLE IF EXISTS Memory")) 
  	{
      		finish_with_error(con);
 	}
  
  	if (mysql_query(con, "CREATE TABLE Memory(ClientId INT, Pointer INT, Size INT, CanFree BOOLEAN)")) 
  	{      
      		finish_with_error(con);
  	}
  	
  	
	static struct sigaction act; 
	act.sa_handler = catcher; 
	sigfillset(&(act.sa_mask)); 
	sigaction(SIGSTOP, &act, NULL); 

	time_t mytime;
	mytime = time(NULL);
//printf(ctime(&mytime));

	FILE *fp = fopen("log.txt", "w+");

	int sockfd; 
	char c;
	pthread_t thread[MAX];
	int i=-1;
 

	struct sockaddr_in server = {AF_INET, 7000, INADDR_ANY}; 

/* set up the transport end point */ 

	struct sockaddr_in client;
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{ 
		perror("socket call failed"); 
		exit(1); 
	} 
/* bind an address to the end point */ 
	if ( bind(sockfd, (struct sockaddr *)&server, SIZE) == -1) 
	{ 
		perror("bind call failed"); 
		exit(1); 
	}
/* start listening for incoming connections */ 
	if ( listen(sockfd, 5) == -1 ) 
	{ 
		perror("listen call failed"); 
		exit(1) ; 
	} 

	for (;;) 
	{ 
		/* accept a connection */ 
		if ( (newsockfd = accept(sockfd, NULL, NULL)) == -1) 
		{ 
			perror("accept call failed"); 
			continue; 
		} 

		/* parent doesn't need the newsockfd */ 

		i++;
		if(pthread_create(&thread[i], NULL, general, newsockfd) != 0)
		{
			fprintf(stderr, "thread creation failed");
			exit(1);
		}
		if(pthread_join(thread[i], (void **)status_ptr) != 0)
		{
			fprintf(stderr, "termination failed");
			exit(1);
		}		
//	close(newsockfd); 
	} 


} 


void general(int pintu)
{
 	pthread_mutex_lock(&msg_mutex); 	
	int target = pintu;

	int d;
	struct mymsg ms;	
	int result;
	pthread_mutex_unlock(&msg_mutex);
//////	//recv(target, &d, sizeof(int), 0) ;
	do
	{
		result = recv(target, &ms, sizeof(struct  mymsg), 0);
		handle(ms, result, target);
		
	}while(result > 0);
	
//	close(newsockfd); 
//	exit (0);
}

void handle(struct mymsg ms, int result, int target_client)
{
	printf("\nbytes: %d data: ptr: %d lineno: %d size : %d\n", result, ms.ptr, ms.lineno, ms.size);
	if(result>0)
	{
		FILE *fp;
	      	int i1=0, i2;
	      	fp = fopen("table.txt", "r");
	     	if (fp == NULL) {
			printf("I couldn't open table.txt for reading.\n");
			exit(0);
	      	}
	   
	   	while (fscanf(fp, "%d %d\n", &i1, &i2) == 2)
	      	{
			printf("i: %d,  type_is: %d\n", i1, i2);
			if(i1==ms.lineno)
			 	break;
	      	}
		if(i1==0)
			{
			printf("Unknown request detected of line no %d .\n", ms.lineno);
			}		
		if(i2==1){ //if message type indicates freeing the memory!
			sendfree(ms, target_client);
		}
		else if(i2==2){ //if message type indicates decoys..
			printf("ms is decoy %d ignored.\n",i1);
		}
		else if(i2==0){ //if message is of malloc type
			char p[10],s[10],r[10];
			snprintf(p, sizeof(p), "%d", target_client);
			snprintf(s, sizeof(s), "%d", ms.ptr);
			snprintf(r, sizeof(r), "%d", ms.size);
			char q[512];
			strcpy(q, "INSERT INTO Memory VALUES(");
			strncat(q, p, sizeof(p));
			strncat(q, ",", 1);
			strncat(q, s, sizeof(s));
			strncat(q, ",", 1);
			strncat(q, r, sizeof(r));
			strncat(q, ",", 1);
			strncat(q, "false)",6);
			
			if (mysql_query(con, q)) 
			{
      				finish_with_error(con);
  			}
			printf("ms is malloc, %d pointer accepted\n", ms.ptr);
		}
	}
}

void sendfree(struct mymsg ms, int target_client)
{

//the queries here can not be performed! STILL COULDNT FIND THE ERROR!

/*
	what this function do???
	
	first it reads which pointer is being freed by the client, and update CanFree flag in the database.
	
	Then look for the size of the pointer, which was stored in the table, while malloc was created, in the INSERT query.
	
	THEN, query the table to find list of pointers, who "CanFree" and are of same size!
	Since no other CanFree is processed here yet, it will give only one current pointer.
	then it sends to client.
	
	WHAT TO add? : RAND() to ROW, and use CanFree flag.

*/
	char *size;
	char p[10],s[10];
	snprintf(p, sizeof(p), "%d", target_client);
	snprintf(s, sizeof(s), "%d", ms.ptr);
	char q[512];
	strcpy(q, "SELECT Size FROM Memory WHERE ClientID=");
	strncat(q, p, sizeof(p));
	strncat(q, " and Pointer=",13);
	strncat(q, s, sizeof(s));
	
	printf("first query string...\n");
	
	//"UPDATE Memory SET CanFree = true WHERE ClientID=target_client and Pointer= ms.ptr"
	
	
	
	if (mysql_query(con, q)) 
	{
	     // printf("\nslfihdkjgvfjbsdjvbscvkjbsdkjvbxcbvkjxbvkjbdkjbvkljsv\n");
	      finish_with_error(con);
	}
	
	printf("first query fired!!!\n");
	
	MYSQL_RES *result = mysql_store_result(con);
  	printf("WTFFFF\n");
  	
	if (result == NULL) 
	{
		printf("\nerror in if\n");
	      finish_with_error(con);
	}
	
	int num_fields = mysql_num_fields(result);
	
	printf("###############%d ############\n", num_fields);
	
	MYSQL_ROW row;
	//row = mysql_fetch_row(result);
	while ((row = mysql_fetch_row(result))) 
 	{ 
 		printf("HOLY SHIT");
     		for(int i = 0; i < num_fields; i++) 
     		{ 
          		printf("%s ", row[i] ? row[i] : "NULL"); 
          		size = row[i];
      		} 
          	printf("\n"); 
 	}
	
	printf("###############%d %s############\n", num_fields, size);
	
	char q1[512];
	strcpy(q1, "UPDATE Memory SET CanFree = true WHERE ClientID=");
	strncat(q1, p, sizeof(p));
	strncat(q1, " and Pointer=",13);
	strncat(q1, s, sizeof(s));
	/*while ((row = mysql_fetch_row(result))) 
 	{ 
     		for(int i = 0; i < num_fields; i++) 
     		{ 
          		printf("%s ", row[i] ? row[i] : "NULL"); 
      		} 
          	printf("\n"); 
 	}*/
 	if (mysql_query(con, q1)) 
	{
	      finish_with_error(con);
	}
	
	printf("\n/!-------------------------------------------( %s )-----------------\n",size);
	
	char q2[512];
	//char ss[10];
	//snprintf(ss, sizeof(ss), "%d", size);
	strcpy(q2, "SELECT Pointer FROM Memory WHERE ClientID=");
	strncat(q2, p, sizeof(p));
	strncat(q2, " and Size=",10);
	strcat(q2, size);
	strncat(q2, " and CanFree=true",18);
	if (mysql_query(con, q2)) 
	{
	    	printf("BC, I told you not to do it\n");
	      finish_with_error(con);
	}
	
	//MYSQL_RES *
	MYSQL_RES *result1 = mysql_store_result(con);
  
	if (result1 == NULL) 
	{
	      finish_with_error(con);
	}

	int num_fields1 = mysql_num_fields(result1);
	int num_rows1 = mysql_num_rows(result1);
	int ptr[num_rows1];
	MYSQL_ROW row1;
	while ((row1 = mysql_fetch_row(result1))) 
 	{ 
 		int counter=0;
 		printf("HOLY SHIT AGAIn\n");
     		for(int i = 0; i < num_fields1; i++) 
     		{ 
          		printf("\n%s ", row1[i] ? row1[i] : "NULL");
          		//ptr=row1[i];
          		
       		}
       		ptr[counter]=atoi(row1[0]);
          	printf("\n"); 
          	printf("\n here i am: %d", ptr[0]);
          	counter++;
 	}
	//int ptr = row1[0];
	send(target_client, &ptr[0], sizeof(int), 0);
	//DELETE FROM Memory WHERE ClientID = target_client and Pointer = ptr; ///////////////////////Still one query remaining to implement.
	char z[10];
	snprintf(z, sizeof(z), "%d", ptr[0]);
	
	char q3[512];
	//char ss[10];
	//snprintf(ss, sizeof(ss), "%d", size);
	strcpy(q3, "DELETE FROM Memory WHERE ClientID=");
	strncat(q3, p, sizeof(p));
	strncat(q3, " and Pointer=",13);
	strncat(q3, z, sizeof(z));
	if (mysql_query(con, q3)) 
	{
	    	
	      finish_with_error(con);
	}
	
	 
}
void catcher(int sig)
{
	printf("server has to close");
	printf("\n server is stopping now: ");
	printf("\n saare client ab bhukhe marenge. ");
	exit(1);
}
