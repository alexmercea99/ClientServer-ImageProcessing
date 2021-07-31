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

int k = 0;
int receive_image(int socket)
{
    // Start function 
    int buffersize = 0, recv_size = 0,size = 0, read_size, write_size, packet_index =1,stat;
    char imagearray[10241],verify = '1', client_folder[512], incremental[16];
    FILE *image;

    //Find the size of the image
    do{
        stat = read(socket, &size, sizeof(int));
    }while(stat<0);
    printf("Packet received.\n");
    printf("Packet size: %i\n",stat);
    printf("Image size: %i\n",size);
    printf(" \n");
    char buffer[] = "Got it";

    //Send our verification signal
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

    //Loop while we have not received the entire file yet
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

            //Write the currently read data into our image file
            write_size = fwrite(imagearray,1,read_size, image);
            printf("Written image size: %i\n",write_size); 

            if(read_size !=write_size) {
            printf("error in read write\n");    }

            //Increment the total number of bytes read
            recv_size += read_size;
            packet_index++;
            printf("Total received image size: %i\n",recv_size);
            printf(" \n");
            printf(" \n");
        }

    }
    fclose(image);
    printf("Image successfully Received!\n");
    return 1;
}

int send_image(int socket, char *image_Name){

    printf("image in function name =%s\n", image_Name);
    FILE *picture;
    int size, read_size, stat, packet_index;
    char send_buffer[10240], read_buffer[256], client_folder[512];
    packet_index = 1;

    strcpy(client_folder, "/home/alex/Music/ProiectPCD/client_folder/");

    strcat(client_folder, image_Name);
    printf("Directory is Given =%s\n", client_folder);
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

    //Send Picture Size
    printf("Sending Picture Size\n");
    write(socket, (void *)&size, sizeof(int));

    //Send Picture as Byte Array
    printf("Sending Picture as Byte Array\n");

    do { //Read while we get errors that are due to signals.
        stat=read(socket, &read_buffer , 255);
        printf("Bytes read: %i\n",stat);
    }while (stat < 0);

    printf("Received data in socket\n");
    printf("Socket data: %c\n", *read_buffer);

    while(!feof(picture)) {
        //while(packet_index = 1){
        //Read from the file into our send buffer
        read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, picture);

        //Send data through our socket 
        do{
            stat = write(socket, send_buffer, read_size);  
        }while (stat < 0);

        printf("Packet Number: %i\n",packet_index);
        printf("Packet Size Sent: %i\n",read_size);     
        printf(" \n");
        printf(" \n");

        packet_index++;  
        //Zero out our send buffer
        bzero(send_buffer, sizeof(send_buffer));
    }
    fclose(picture);
}

int main(int argc , char *argv[])
{
    int socket_desc;
    struct sockaddr_in server;
    char *parray, username[512], password[512], choose[16];
    char choice[64], current_image[1024];

    if(argc < 2)
    {
        printf("\n Usage: %s <ip of server> \n",argv[0]);
        return 1;
    } 
    char ip[16] = "";
    strncpy(ip, argv[1], 15);
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);

    if (socket_desc == -1) {
        printf("Could not create socket");
    }
    memset(&server,0,sizeof(server));
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));

    //Connect to remote server
    if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0) {
        puts(strerror(errno));
        close(socket_desc);
        puts("Connect Error");
    return 1;
    }

    puts("Connected to server\n");
    while(1){
        bzero(choose, sizeof(choose));
        printf("Please enter your name that register in DataBase = ");
        scanf("%s", username);
        send(socket_desc, username, strlen(username), 0);
        printf("Please enter your password = ");
        scanf("%s", password);
        send(socket_desc, password, strlen(password), 0);
        read(socket_desc, &choose , sizeof(choose));
        printf("Desision of server =%s", choose);
        if(strcmp(choose, "true") == 0){
            while(1){
                bzero(choice, sizeof(choice));
                puts("The Menu is given below:");
                puts("1 = send Image");
                puts("2 = exit");
                printf("Please enter your choice =");
                scanf("%s", choice);

                if(strcmp(choice, "1") == 0) {
                    bzero(current_image, sizeof(current_image));
                    send(socket_desc, choice, strlen(choice), 0);
                    char image_Name[512];
                    while(1){
                        printf("Please enter image file Name with proper extension =");
                        scanf("%s", image_Name);
                        printf("Image name is %s\n", image_Name);
                        strcpy(current_image, "/home/alex/Music/ProiectPCD/client_folder/");
                        strcat(current_image, image_Name);
                        FILE *fp = fopen(current_image, "r");
                        if (fp == NULL)
                        {
                            printf("Error: could not found this image %s\n\nPlease enter file Name that exist in directory", image_Name);
                        }
                        else{
                            fclose(fp);
                            send_image(socket_desc, image_Name);
                            break;
                        }
                    }

                    while(1){
                        printf("Please choose one of them that Given Below:\n1 = Grayscale\n2 = Salt & Paper\n3 = Canny Edge Detection\n4 = HSV Image\n5= Gaussian Blur\n6= Image Sharpening\nPlease choose = ");
                        scanf("%s", choose);
                        if((strcmp(choose, "1") == 0) || (strcmp(choose, "2") == 0) || (strcmp(choose, "3") == 0) || (strcmp(choose, "4") == 0) || (strcmp(choose, "5") == 0) || (strcmp(choose, "6")  == 0)){
                            send(socket_desc, choose, strlen(choose), 0);
                            break;
                        }
                        else{
                            printf("Please choose right option\n");
                        }
                    }
                    receive_image(socket_desc);
                }
                else if(strcmp(choice,"6") == 0) {
                    send(socket_desc, choice, strlen(choice), 0);
                    break;
                }
                else {
                    printf("please enter right choice\n");
                }
            }
        }
        else{
            printf("Username Does Not exist in Data Base\n");
        }
    }
    close(socket_desc);
    return 0;
}
