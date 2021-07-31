#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <dirent.h>
#include <assert.h>
#include <sys/un.h>

#define SERVER_PATH  "/tmp/server"

int clients_Array[20];
char client_name_Array[20][15];
int n = 0, k = 0;

void checkError(int status) {
    if (status < -1) {
        printf("socket error(%d): [%s]\n", getpid(), strerror(errno));
    }
}

struct thread_args {
  int a;
  int b;
};

struct thread_args1 {
  int a;
  int b;
};

void *unix_main(void *args){


	int serverFd = -1;
	int clientFd;
    int err, nBytes, numSent;
    char cmdClient[512];	
    char mesaj[512];		
    struct sockaddr_un serverAddr; 

    
    serverFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (serverFd < 0) {
        perror("UNIX socket() error");
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sun_family = AF_UNIX;
    strcpy(serverAddr.sun_path, SERVER_PATH);  

   
    err = bind(serverFd, (struct sockaddr *)&serverAddr, SUN_LEN(&serverAddr));
    if (err < 0) {
        perror("UNIX bind() error");
    }

    err = listen(serverFd, 1);  
    if (err < 0) {
        perror("UNIX listen() error");
    }else{
		printf ("[UNIX Server] Listening for 1:1 connection\n");
    }

    int run = 1;

    while (run) {
        if ((clientFd = accept(serverFd, NULL, NULL)) == -1) {
                perror("UNIX accept() error.\n");
                break;
            }
    	
        while ((nBytes = read(clientFd, cmdClient, sizeof(cmdClient))) > 0) {   
            cmdClient[nBytes] = 0;	
            printf("UNIX - read %d bytes: %s\n", nBytes, cmdClient);
            //numSent = send(clientFd, mesaj, len, 0);
            if (numSent < 0) {
                perror("UNIX send() error");
                break;
            }
        }
        if (nBytes < 0) {
            perror("UNIX client read error. will close the connection.\n");
            close(clientFd);
            clientFd = - 2;
            continue;
        } else if (nBytes == 0) {
            printf("UNIX ** The client closed the connection.\n");
            close(clientFd);
            clientFd = -2;
            continue;
        }
    }



    if (serverFd > 0)
        close(serverFd);
    if (clientFd > 0)
        close(clientFd);

    unlink(SERVER_PATH);
}

int receive_image(int socket)
{
    
    int buffersize = 0, recv_size = 0,size = 0, read_size, write_size, packet_index =1,stat;
    char imagearray[10241],verify = '1', server_folder[512], incremental[16];
    FILE *image;

    
    do{
        stat = read(socket, &size, sizeof(int));
    }while(stat<0);

    printf("Packet received.\n");
    printf("Packet size: %i\n",stat);
    printf("Image size: %i\n",size);
    printf("\n");
    char buffer[] = "Got it";

    
    do{
        stat = write(socket, &buffer, sizeof(int));
    }while(stat<0);
    printf("Reply sent\n");
    printf(" \n");
    bzero(incremental, sizeof(incremental));
    sprintf(incremental, "%d", k+1);
    strcpy(server_folder, "/home/alex/Music/ProiectPCD/server_folder/capture2");
    strcat(server_folder, incremental);
    strcat(server_folder, ".jpeg");
    k++;
    printf("Directory is Given =%s\n", server_folder);
    image = fopen(server_folder, "w");

    if( image == NULL) {
        printf("Error has occurred. Image file could not be opened\n");
        return -1; 
    }
    
    int need_exit = 0;
    struct timeval timeout = {10,0};

    fd_set fds;
    int buffer_fd, buffer_out;
    while(recv_size < size) {
        //while(packet_index < 2){
        FD_ZERO(&fds);
        FD_SET(socket,&fds);

        buffer_fd = select(FD_SETSIZE,&fds,NULL,NULL,&timeout);
        if (buffer_fd < 0)
            printf("error: bad file descriptor set.\n");
        if (buffer_fd == 0)
            printf("error: buffer read timeout expired.\n");
        if (buffer_fd > 0)
        {
            do{
                read_size = read(socket,imagearray, 10241);
            }while(read_size <0);

            printf("Packet number received: %i\n",packet_index);
            printf("Packet size: %i\n",read_size);

            
            write_size = fwrite(imagearray,1,read_size, image);
            printf("Written image size: %i\n",write_size); 

            if(read_size !=write_size) {
            printf("error in read write\n");    }

            
            recv_size += read_size;
            packet_index++;
            printf("Total received image size: %i\n",recv_size);
            printf(" \n");
            printf(" \n");
        }
    }
    fclose(image);
    printf("Image successfully Received!\n\n\n");
    return 1;
}

void send_image(int socket){

    FILE *picture;
    int size, read_size, stat, packet_index;
    char send_buffer[10240], read_buffer[256], server_folder[512], incremental[16];
    packet_index = 1;

    bzero(incremental, sizeof(incremental));
    sprintf(incremental, "%d", k);
    strcpy(server_folder, "/home/alex/Music/ProiectPCD/server_folder/new_grayScale");
    strcat(server_folder, incremental);
    strcat(server_folder, ".jpeg");
    printf("Directory is Given =%s\n", server_folder);
    picture = fopen(server_folder, "r");
    printf("Getting Picture Size\n");   

    if(picture == NULL) {
        printf("Error Opening Image File"); 
    } 
    fseek(picture, 0, SEEK_END);
    size = ftell(picture);
    fseek(picture, 0, SEEK_SET);
    printf("Total Picture size: %i\n",size);

    
    printf("Sending Picture Size\n");
    write(socket, (void *)&size, sizeof(int));

    
    printf("Sending Picture as Byte Array\n");

    do { 
        
        stat=read(socket, &read_buffer , 255);
        printf("Bytes read: %i\n",stat);
    }while (stat < 0);

    printf("Received data in socket\n");
    printf("Socket data: %c\n", *read_buffer);
    while(!feof(picture)) {
        
        read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, picture);

        
        do{
            stat = write(socket, send_buffer, read_size);  
        }while (stat < 0);
        printf("Packet Number: %i\n",packet_index);
        printf("Packet Size Sent: %i\n",read_size);     
        printf(" \n\n");

        packet_index++;  
        bzero(send_buffer, sizeof(send_buffer));
    }
    fclose(picture);
}

void *admin_Client_Function(void * _args)
{
    struct thread_args *args = (struct thread_args *) _args;
    char hello[64], choose[16], hello1[512];
    int python_Socket = args->a;
    int sock = args->b;
	

    char admin_client_msg[1024], choice[64], username[512], password[512];
	int len;
    FILE *user_filePointer;
	
    while(1){
        bzero(choice, sizeof(choice));
        read(sock, &choice , sizeof(choice));
        printf("Following choice from admin_client =%s\n", choice);
        if(strcmp(choice, "1") == 0) {
            bzero(admin_client_msg, sizeof(admin_client_msg));
            read(sock, &admin_client_msg , sizeof(admin_client_msg));
            printf("Message from admin_client =%s\n", admin_client_msg);
        }
        else if(strcmp(choice, "2") == 0) {
            bzero(username, sizeof(username));
            bzero(password, sizeof(password));
            user_filePointer = fopen("DataBase.txt", "a");
            read(sock, &username , sizeof(username));
            printf("Username Before written into file =%s\n", username);
            read(sock, &password , sizeof(password));
            printf("Password Before written into file =%s\n", password);
            if ( user_filePointer == NULL ) {
                printf( "DataBase.txt, file failed to open.\n") ;
            }
            else {
                printf("The file is now opened.\n") ;
                strcat(username, "\n");
                fputs(username, user_filePointer);
                printf("Username After written into file =%s", username);
                strcat(password, "\n");
                fputs(password, user_filePointer);
                printf("Password After written into file =%s", password);
                fputs("\n", user_filePointer) ;
                
                
                fclose(user_filePointer);
                printf("Data successfully written in file DataBase.txt\n");
            }
            printf("The Following user created Successfully%s\n", username);
        }
        else if(strcmp(choice, "4") == 0) {
            receive_image(sock);
        }
        else if(strcmp(choice, "5") == 0) {
            receive_image(sock);

            send(python_Socket, "1", strlen("1"), 0);
            read(python_Socket, &hello1 , sizeof(hello1));

            send_image(sock);
        }
        else if(strcmp(choice, "7") == 0) {
            break;
        }
        else {
            printf("please enter right choice\n");
        }
    }
    close(sock);
}

void * handle_client(void * _args) {
    struct thread_args *args = (struct thread_args *) _args;
    char hello[64], choose[16];
    int python_Socket = args->a;
    int client_socket = args->b;

    while(1){
        read(client_socket, &hello , sizeof(hello));
        printf("Choice made from client =%s\n", hello);
        if(strcmp(hello, "1") == 0){
            bzero(hello, sizeof(hello));
            receive_image(client_socket);

            read(client_socket, &choose , sizeof(choose));
            send(python_Socket, choose, strlen(choose), 0);
            read(python_Socket, &hello , sizeof(hello));

            send_image(client_socket);
            bzero(hello, sizeof(hello));
        }
        else if(strcmp(hello, "2") == 0){
            break;
        }
    }
    close(client_socket);
    free (args);
}

int main(int argc , char *argv[])
{
    int socket_desc , new_socket , c, read_size,buffer = 0;
    struct sockaddr_in server , client;
    char *readin, hello[512], username[512], password[512];
    pthread_t client_thread, admin_client_thread;
    //pthread_t unixThread;
    //unlink(SERVER_PATH) ;
    //pthread_create (&unixThread, NULL, unix_main, 0) ;
    //pthread_join (unixThread, NULL);
	    //pthread_join (inetThread, NULL);
 		//unlink(SERVER_PATH) ;
    if(argc < 1)
    {
        printf("\n Usage: %s <ip of server> \n",argv[0]);
        return 1;
    }
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }

    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(atoi(argv[1]));

    //bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("bind failed");
        return 1;
    }
    puts("bind done");

    //listen
    if(listen(socket_desc , 10) == -1){
        printf("listening failed \n");
    }

    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    if((new_socket = accept(socket_desc, (struct sockaddr *)&client,(socklen_t*)&c))){
        puts("Admin client Connection accepted");
    }
    int admin_client_socket = new_socket;
    
    if((new_socket = accept(socket_desc, (struct sockaddr *)&client,(socklen_t*)&c))){
    puts("Image processing Connection accepted");
    }
    int python_socket = new_socket;
    struct thread_args1 *args1 = malloc (sizeof (struct thread_args));
    args1->a = python_socket;
    args1->b = admin_client_socket;
    pthread_create(&admin_client_thread, NULL, (void*)admin_Client_Function, args1);

    while(1){
        if((new_socket = accept(socket_desc, (struct sockaddr *)&client,(socklen_t*)&c))){
            puts("Connection accepted");
        }
        int client_socket = new_socket;

        fflush(stdout);
        if (new_socket<0)
        {
            perror("Accept Failed");
            return 1;
        }
        while(1){
            bzero(username, sizeof(username));
            bzero(password, sizeof(password));
            read(client_socket, &username , sizeof(username));
            strcat(username, "\n");
            printf("User in server side is =%s", username);
            read(client_socket, &password , sizeof(password));
            strcat(password, "\n");
            printf("Pass in server side is =%s", password);
            char flag[512] = "false";
            const unsigned MAX_LENGTH = 256;
            char buffer[MAX_LENGTH];
            char *filename = "DataBase.txt";
            FILE *fp = fopen(filename, "r");
            if (fp == NULL)
            {
                printf("Error: DataBase file does NOT Exist %s\n", filename);
                //return 1;
            }
            else{
                while (fgets(buffer, MAX_LENGTH, fp)){
                    printf("%s", buffer);
                    printf("%s", username);
                    if(strcmp(buffer, username) == 0){
                        printf("Username Matched\n");
                        fgets(buffer, MAX_LENGTH, fp);
                        if(strcmp(buffer, password) == 0){
                            strcpy(flag, "true");
                            printf("flag Matched :%s\n", flag);
                            send(client_socket, flag, strlen(flag), 0);
                            struct thread_args *args = malloc (sizeof (struct thread_args));
                            printf("Password Matched\n");                            
                            args->a = python_socket;
                            args->b = client_socket;
                            printf ("First: %d; Second: %d\n", args->a, args->b);
                            pthread_create( &client_thread, NULL, (void*)handle_client, args);                            
                        }
                        break;
                    }
                }
                fclose(fp);
            }
            if(strcmp(flag, "true") == 0){
                printf("\nThe above user %s with password = %s connected to server\n\n", username, password);
                break;
            }
            else {
                send(client_socket, "Donee", strlen("Donee"), 0);
                printf("User does not exist\n");
            }
        }
    }

    close(socket_desc);
    fflush(stdout);
    return 0;
}
