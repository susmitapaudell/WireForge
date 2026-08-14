#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>

int main(){
    
    //socket creation
    int client_sock;
    client_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (client_sock<0){
        std::cout<<"socket not created";
        return 1;
    }
    else{
        std::cout<<"socket created successfully";
    }

    //create and configure server address
    sockaddr_in server_addr{};

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    //connect to the server
    int server_sock = connect(client_sock, 
        (struct sockaddr*)&server_addr,
        sizeof(server_addr));

    if (server_sock<0){
        std::cout<<"connection failed";
        return 1;
    }
    else{
        std::cout<<"connected to the suerver";
    }

    //send data
    return 0;
}