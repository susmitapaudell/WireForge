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
        std::cout<<"socket created successfully\n";
    }

    //create and configure server address
    sockaddr_in server_addr{};

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    //connect to the server
    int connection_result = connect(client_sock, 
        (struct sockaddr*)&server_addr,
        sizeof(server_addr));

    if (connection_result<0){
        std::cout<<"connection failed";
        return 1;
    }
    else{
        std::cout<<"connected to the server\n";
    }


    //send data
    char buffer[256];

    while(1){
        
        std::cout<<"\nenter message\n";
        std::cin.getline(buffer, 256);
        
        int message_length = strlen(buffer);

        int sent_bytes = send(client_sock, buffer, message_length, 0);

        // std::cout<<"sent bytes value = " << sent_bytes;
        
        if(sent_bytes<0){
            std::cout<<"\nmessage failed to send";
            return 1;
        }
        else{
            std::cout<<"\nmessage sent to the server";
        }

        int receive_message_from_server = recv(client_sock, buffer, sizeof(buffer)-1, 0);
        if(receive_message_from_server<0){
            std::cout<<"\nmessage could not be received back from the server";
        }
        else{
            std::cout<<"\nmessage successfully received back from the server";
            std::cout<<"\nserver send back: \t"<<buffer;
        }
    }
    
    close(client_sock);

    return 0;
}