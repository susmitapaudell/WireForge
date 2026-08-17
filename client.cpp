#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<cstring>

int main(){
    
    //socket creation
    int client_sock;
    client_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (client_sock<0){
        std::cout<<"socket not created\n";
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
        std::cout<<"connection failed\n";
        return 1;
    }
    else{
        std::cout<<"connected to the server\n";
    }


    //username registration
    char buffer[256];

    int received_bytes = recv(
        client_sock,
        buffer,
        sizeof(buffer)-1,
        0
    );

    if(received_bytes<0){
        std::cout<<"username prompt could not be received\n";
        close(client_sock);
        return 1;
    }

    buffer[received_bytes] = '\0';

    std::cout<<buffer;

    std::cin.getline(buffer, 256);

    int username_length = strlen(buffer);

    int sent_bytes = send(
        client_sock,
        buffer,
        username_length,
        0
    );

    if(sent_bytes<0){
        std::cout<<"username failed to send\n";
        close(client_sock);
        return 1;
    }


    //receive welcome message
    received_bytes = recv(
        client_sock,
        buffer,
        sizeof(buffer)-1,
        0
    );

    if(received_bytes<0){
        std::cout<<"welcome message could not be received\n";
        close(client_sock);
        return 1;
    }

    buffer[received_bytes] = '\0';

    std::cout<<buffer;


    //send data
    while(1){
        
        std::cout<<"enter message\n";
        std::cin.getline(buffer, 256);
        
        int message_length = strlen(buffer);

        int sent_bytes = send(
            client_sock,
            buffer,
            message_length,
            0
        );

        // std::cout<<"sent bytes value = " << sent_bytes;
        
        if(sent_bytes<0){
            std::cout<<"message failed to send\n";
            break;
        }
        else{
            std::cout<<"message sent to the server\n";
        }


        //quit command
        if(strcmp(buffer,"/quit")==0){
            break;
        }


        int receive_message_from_server = recv(
            client_sock,
            buffer,
            sizeof(buffer)-1,
            0
        );

        if(receive_message_from_server<0){

            std::cout
                <<"message could not be received back from the server\n";
        }

        else if(receive_message_from_server==0){

            std::cout<<"server disconnected\n";
            break;
        }

        else{

            buffer[receive_message_from_server] = '\0';

            std::cout
                <<"message successfully received back from the server\n";

            std::cout
                <<"server send back: \t"
                <<buffer
                <<'\n';
        }
    }

    close(client_sock);

    return 0;
}