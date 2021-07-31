#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <dirent.h>
#include <sys/un.h>

#define SERVER_FILE     "/tmp/server"

int k = 0;
int receive_image(int socket)
{
    
    int buffersize = 0, recv_size = 0,size = 0, read_size, write_size, packet_index =1,stat;
    char imagearray[10241],verify = '1', client_folder[512], incremental[16];
    FILE *image;

    
    do{
        stat = read(socket, &size, sizeof(int));
    }while(stat<0);

    printf("Packet received.\n");
    printf("Packet size: %i\n",stat);
    printf("Image size: %i\n",size);
    printf(" \n");
    char buffer[] = "Got it";

    
    do{
        stat = write(socket, &buffer, sizeof(int));
    }while(stat<0);

    printf("Reply sent\n");
    printf(" \n");
    bzero(incremental, sizeof(incremental));
    sprintf(incremental, "%d", k+1);
    strcpy(client_folder, "/home/alex/Music/ProiectPCD/client_folder/After_Processing");
    strcat(client_folder, incremental);
    strcat(client_folder, ".jpeg");
    k++;
    printf("Directory is Given =%s\n", client_folder);
    image = fopen(client_folder, "w");
    if( image == NULL) {
        printf("Error has occurred. Image file could not be opened\n");
        return -1; 
    }

    
    int need_exit = 0;
    struct timeval timeout = {10,0};

    fd_set fds;
    int buffer_fd, buffer_out;
    while(recv_size < size) {
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
                printf("error in read write\n");    
            }

            
            recv_size += read_size;
            packet_index++;
            printf("Total received image size: %i\n",recv_size);
            printf(" \n\n");
        }

    }
    fclose(image);
    printf("Image successfully Received!\n");
    return 1;
}

int send_image(int socket, char *image_Name){

    printf("image in function name =%s", image_Name);
    FILE *picture;
    int size, read_size, stat, packet_index;
    char send_buffer[10240], read_buffer[256], client_folder[512];
    packet_index = 1;
    
    strcpy(client_folder, "/home/alex/Music/ProiectPCD/client_folder/");
    strcat(client_folder, image_Name);
    picture = fopen(client_folder, "r");
    printf("Getting Picture Size\n");   

    if(picture == NULL) {
        printf("Error Opening Image File"); 
        return 1;
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
        printf(" \n");
        printf(" \n");

        packet_index++;  
        
        bzero(send_buffer, sizeof(send_buffer));
    }
    fclose(picture);
}

int main(int argc , char *argv[])
{
    int socket_desc;
    struct sockaddr_in server;
    char *parray, msg[512], choice[64], choose[16];
    char photo_Name[512], username[512], password[512];
    char current_image[1024];
    if(argc < 2)
    {
        printf("\n Usage: %s <ip of server> \n",argv[0]);
        return 1;
    } 
    char ip[16] = "";
    strncpy(ip, argv[1], 15);
    
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);

    if (socket_desc == -1) {
        printf("Could not create socket");
    }
    //memset(&serverAddr, 0, sizeof(serverAddr));
    //serverAddr.sun_family = AF_UNIX;
    //strcpy(serverAddr.sun_path, SERVER_FILE); 

    memset(&server,0,sizeof(server));
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));

    //remote server
    if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0) {
        puts(strerror(errno));
        close(socket_desc);
        puts("Connect Error");
    return 1;
    }

    puts("Connected to main Server\n\n");
    puts("This is Admin Client\n");

    while(1){
        puts("This is Admin Client\n");
        printf("The Menu is Given Below for Admin client\n1 = send message to server\n2 = Create User\n3 = Delete Photo\n4 = Send image to server\n5 = convert image to Grayscale\n6 = Show available photos in client_folder\n7 = EXIT\n\nPlease enter your choice :");
        scanf("%s", choice);
        send(socket_desc, choice, strlen(choice), 0);
        if(strcmp(choice,"1") == 0) {
            printf("\nPlease enter your message that send to server :");
            scanf("%s", msg);
            send(socket_desc, msg, strlen(msg), 0);
        }
        else if(strcmp(choice,"2") == 0) {
            bzero(choice, sizeof(choice));
            bzero(username, sizeof(username));
            printf("Please enter client username(Only first or last name) =");
            scanf("%s", username);
            send(socket_desc, username, strlen(username), 0);
            printf("Please enter client Password =");
            scanf("%s", password);
            send(socket_desc, password, strlen(password), 0);
        }
        else if(strcmp(choice,"3") == 0) {
            bzero(photo_Name, sizeof(photo_Name));
            bzero(current_image, sizeof(current_image));
            printf("Please enter image name(with proper extension) that you want to DELETE =");
            scanf("%s", photo_Name);
            strcpy(current_image, "/home/alex/Music/ProiectPCD/server_folder/");
            strcat(current_image, photo_Name);
            if (remove(current_image) == 0) {
                printf("The file is deleted successfully.");
            } 
            else {
                printf("The file is not deleted.");
            }
        }
        else if(strcmp(choice,"4") == 0) {
            bzero(choice, sizeof(choice));
            bzero(current_image, sizeof(current_image));
            bzero(photo_Name, sizeof(photo_Name));
            while(1){
                printf("Please enter image file Name with proper extension =");
                scanf("%s", photo_Name);
                printf("Image name is %s\n", photo_Name);
                strcpy(current_image, "/home/alex/Music/ProiectPCD/client_folder/");
                strcat(current_image, photo_Name);
                FILE *fp = fopen(current_image, "r");
                if (fp == NULL)
                {
                    printf("Error: could not found this image %s\n\nPlease enter file Name that exist in directory", photo_Name);
                }
                else{
                    fclose(fp);
                    send_image(socket_desc, photo_Name);
                    break;
                }
            }
        }
        else if(strcmp(choice,"5") == 0) {
            bzero(choice, sizeof(choice));
            bzero(photo_Name, sizeof(photo_Name));
            bzero(current_image, sizeof(current_image));
            while(1){
                printf("Please enter image file Name with proper extension =");
                scanf("%s", photo_Name);
                printf("Image name is %s\n", photo_Name);
                strcpy(current_image, "/home/alex/Music/ProiectPCD/client_folder/");
                strcat(current_image, photo_Name);
                FILE *fp = fopen(current_image, "r");
                if (fp == NULL)
                {
                    printf("Error: could not found this image %s\n\nPlease enter file Name that exist in directory", photo_Name);
                }
                else{
                    fclose(fp);
                    send_image(socket_desc, photo_Name);
                    break;
                }
            }
            strcpy(choose, "1");
            send(socket_desc, choose, strlen(choose), 0);
            receive_image(socket_desc);
        }
        else if(strcmp(choice,"6") == 0) {
            struct dirent *de;
            DIR *dr = opendir("/home/alex/Music/ProiectPCD/client_folder/");
            if (dr == NULL){
                printf("Could not open current directory" );
                return 0;
            }
            while ((de = readdir(dr)) != NULL){
                printf("%s\n", de->d_name);
            }
            closedir(dr);
        }
        else if(strcmp(choice,"7") == 0) {
            break;
        }
        else {
            printf("please enter right choice\n");
        }
        bzero(choice, sizeof(choice));
    }
    close(socket_desc);
    return 0;
}
