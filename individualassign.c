
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#define PORT 1717

int d, port;
char *QOTDP;
const char *goQOTDP;
pthread_mutex_t quoteLock=PTHREAD_MUTEX_INITIALIZER;
pthread_t checkForNewDayThread, connectionHandlerThread;

//This part is to link with quotes file
int line_no(const char* file){
    int count=0;
    int currentChar;
    FILE* fds=fopen(file,"r");
    if(fds==NULL){
        perror("Error");
        exit(EXIT_FAILURE);
    }
    while(true){
        currentChar=fgetc(fds);
        switch (currentChar){
            case '\n':{
                count++;
                break;
            }
            case EOF:{
                fclose(fds);
                return count;
            }
            default:
                continue;
        }
    }
}

//This part is to find quotes file
char* quotes_read(const char* filePath){
   int quotes_no = line_no(filePath);
   int lineNumberOfQOTD=rand()%quotes_no;
   int lineCounter=0;
   char* lineptr=NULL; 
   size_t n=0;
   FILE* fds=fopen(filePath, "r");
    if(fds==NULL){
        perror("Error opening the quotes");
        exit(EXIT_FAILURE);
    }
   while(lineCounter<lineNumberOfQOTD){ 
       if(fgetc(fds)=='\n') lineCounter++;
   }
    getline(&lineptr, &n, fds); 
    fclose(fds);
    return lineptr;
}

//This part is to handle connections
void * connection(void* port_ptr){
    struct sockaddr_in address;
    int server_fds, new_socket, opt = 1, addrlen = sizeof(address), port=*((int*) port_ptr);

    free(port_ptr);

    //This part to create socket descriptor
    if ((server_fds = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }


    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    //This part to bind the socket to the port 1717
    if (bind(server_fds, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fds, 100) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %i\n", port);
    
     //This part to assign port 1717
    if (setsockopt(server_fds, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))==-1)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    while(1){ 
        if ((new_socket = accept(server_fds, (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0)
	{
            perror("accept");
            exit(EXIT_FAILURE);
        }
        pthread_mutex_lock(&quoteLock);
        send(new_socket, QOTDP, strlen(QOTDP), 0);
        pthread_mutex_unlock(&quoteLock);
        close(new_socket);
    }
}



int main(int argc, char const *argv[])
{
    int thread1, thread2, join;
    int* port=malloc(sizeof(int));

    if(port==NULL){
	perror("Couldn't allocate memory to store listening port");
	exit(EXIT_FAILURE);
    }

    time_t time1=time(NULL); 
    struct tm tm = *localtime(&time1);
    d=tm.tm_mday;

    switch(argc){ 
        case 1:{ 
            *port=1717;
            goQOTDP=strdup("quotes.txt");
            break;
        }
        case 3:{ //file and port specified by user
            *port=atoi(argv[2]);
            goQOTDP=argv[1];
            break;
        }
        default:{
            fprintf(stderr,"Bad argument\n");
            exit(EXIT_FAILURE);
        }

    }

	printf("Username :  %s", getlogin());
	printf("\n");
	printf("The quotes file : %i quotes\n", line_no(goQOTDP));

        srand(time1); //To get random quote

    QOTDP = quotes_read(goQOTDP);
    connection(port);

    return 0;
}

// This part is to handle error by prompt exit
#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)

