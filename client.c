/* client process */
/*
	1 sum
	2 multiplication
	3 chat
	4 sending a file
	5 maximum in array
	6 vote

*/

//attacks and precautions for all three attacks and reasons
//
#include <stdio.h> 
#include <ctype.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#define SIZE sizeof(struct sockaddr_in) 
void array(void);//malloc as an dynamic array..
void randomm(void);//random string generator
void mallocs(void);
int sockfd; 
int k=1;//
void action (void);
struct mymsg{
	//int type; // 0 == malloc call, 1 == free
	int lineno; //for server to recognize the type of a message and identify the decoy message
	void* ptr; //address, which will be dummy for decoys
	int size; //size of malloc, should be 'dummy' in free and decoys!
};

int main(void) 
{ 

	int ch;
 
	struct sockaddr_in server = {AF_INET, 7000}; 
/* convert and store the server's IP address 
Replace IP address given below*/ 
	server.sin_addr.s_addr = inet_addr("127.0.0.1"); 
/* set up the transport end point */ 
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{ 
		perror("socket call failed"); 
		exit(1); 
	} 
/* connect the socket to the server's address */ 
	if ( connect (sockfd, (struct sockaddr *)&server, SIZE) == -1) 
	{ 
		perror("connect call failed"); 
		exit(1); 
	} 

	while(1)
	{
		printf("What do you want? \n 1.array\n 2.random string\n 3.exit\n 4.mallocs\nEnter your choice:");
	//exit option is there because planning to do this in unconditional for loop rather than just once!
		scanf("%d",&ch);	
		switch(ch){
			case 1:{ 
				array();
				break;
				}
		
			case 2:{ 
				randomm();
				break;
				}
		
			case 3:{ 
				exit(1);
				}

			case 4:{ 
				mallocs();
				break;
				}
		
			default:{ 
				printf("invalid choice!!!");
				break;
				}
			}
	}
}


void array(void)
{
	int *ptr_one;
	int counter;
	int i;
	printf("Enter how many integers you want in array?: ");
	scanf("%d", &counter);
	ptr_one = (int *)malloc(sizeof(int)*counter);
	struct mymsg m1;
	m1.lineno = 98; m1.ptr = ptr_one; m1.size = sizeof(int)*counter;
	send(sockfd, &m1, sizeof(struct mymsg), 0);//
	//k++;//

	if (ptr_one == NULL)
	{
		printf("ERROR: Out of memory\n");
		return 1;
	}

	printf("Enter %d elements: ", counter);
	for(i=0; i<counter; i++)
	{
		scanf("%d",&ptr_one[i]);
	}
	printf("You entered as follows:\n");
	for(i=0; i<counter; i++)
	{
		printf("%d\n",ptr_one[i]);
	}
//	free(ptr_one);
	//send(sockfd, &k, sizeof(int), 0);//
	//k++;//
	struct mymsg m2;
	m2.lineno = 122; m2.ptr = ptr_one; m2.size = sizeof(int); //size is dummy.
	send(sockfd, &m2, sizeof(struct mymsg), 0);
	action();

}

void randomm(void)
{
	int i,n;
	char * buffer;
	printf ("How long do you want the string? ");
	scanf ("%d", &i);

	buffer = (char*) malloc (i+1);
	//send(sockfd, &k, sizeof(int), 0);//
	//k++;//
	struct mymsg m3;
	m3.lineno = 139; m3.ptr = buffer; m3.size = i+1;
	send(sockfd, &m3, sizeof(struct mymsg), 0);

	if (buffer==NULL) exit (1);

	for (n=0; n<i; n++)
		buffer[n]=rand()%26+'a';
	buffer[i]='\0';

	printf ("Random string: %s\n",buffer);
//	free (buffer);
	//send(sockfd, &buffer, sizeof(char*), 0);//
	//k++;//

	struct mymsg m4;
	m4.lineno = 154; m4.ptr = buffer; m4.size = i;
	send(sockfd, &m4, sizeof(struct mymsg), 0);
	action();

}

void mallocs(void)
{
/*right now putting just five different mallocs for integer values, later this function will allocate random number of mallocs based on arguements*/

	int *ptr_one;
	ptr_one = (int *)malloc(sizeof(int));
	struct mymsg m5;
	m5.lineno = 167; m5.ptr = ptr_one; m5.size = sizeof(int);
	send(sockfd, &m5, sizeof(struct mymsg), 0);

	*ptr_one = 25;
	printf("%d\n", *ptr_one);
//	free(ptr_one);
	struct mymsg m6;
	m6.lineno = 174; m6.ptr = ptr_one; m6.size = sizeof(int); //dummy size
	send(sockfd, &m6, sizeof(struct mymsg), 0);
	action();

	int *ptr_two;
	ptr_two = (int *)malloc(sizeof(int));
	struct mymsg m7;
	m7.lineno = 181; m7.ptr = ptr_two; m7.size = sizeof(int);
	send(sockfd, &m7, sizeof(struct mymsg), 0);
	
	*ptr_two = 20;
	printf("%d\n", *ptr_two);
//	free(ptr_two);
	struct mymsg m8;
	m8.lineno = 188; m8.ptr = ptr_two; m8.size = sizeof(int);
	send(sockfd, &m8, sizeof(struct mymsg), 0);
	action();
	

	int *ptr_t;
	ptr_t = (int *)malloc(sizeof(int));
	struct mymsg m9;
	m9.lineno = 196; m9.ptr = ptr_t; m9.size = sizeof(int);
	send(sockfd, &m9, sizeof(struct mymsg), 0);
	
	*ptr_t = 15;
	printf("%d\n", *ptr_t);
//	free(ptr_t);
	struct mymsg m10;
	m10.lineno = 203; m10.ptr = ptr_t; m10.size = sizeof(int);
	send(sockfd, &m10, sizeof(struct mymsg), 0);
	action();
	

	int *ptr_f;
	ptr_f = (int *)malloc(sizeof(int));
	struct mymsg m11;
	m11.lineno = 210; m11.ptr = ptr_f; m11.size = sizeof(int);
	send(sockfd, &m11, sizeof(struct mymsg), 0);
	
	*ptr_f = 10;
	printf("%d\n", *ptr_f);
//	free(ptr_f);
	struct mymsg m12;
	m12.lineno = 218; m12.ptr = ptr_f; m12.size = sizeof(int);
	send(sockfd, &m12, sizeof(struct mymsg), 0);
	action();
	

	int *ptr_five;
	ptr_five = (int *)malloc(sizeof(int));
	struct mymsg m13; //malloc
	m13.lineno = 226; m13.ptr = ptr_five; m13.size = sizeof(int);
	send(sockfd, &m13, sizeof(struct mymsg), 0);
	
	*ptr_five = 5;
	printf("%d\n", *ptr_five);
//	free(ptr_five); // originally a free() now relpaced with send(msg)
	struct mymsg m14;
	m14.lineno = 233; m14.ptr = ptr_five; m14.size = sizeof(int); 
	send(sockfd, &m14, sizeof(struct mymsg), 0);
	action();

	
}

void action(void)
{
	struct mymsg m3;
	int *ptr_five;
	ptr_five = (int *)malloc(sizeof(int));
	m3.lineno = 243; m3.ptr = ptr_five; m3.size = sizeof(int); // this is decoy
	send(sockfd, &m3, sizeof(struct mymsg), 0);
	
	void* ptr;
	//int r;
	int r = recv(sockfd, &ptr, sizeof(void*), 0);
	if(r > 0)
	{
		printf("freeing some memory at %d by server..", ptr);
		free(ptr);
	}
	else
	{
		printf("nothing to free!");
		return;
	}
}
