#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>

int main() {

    //socket creation
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd<0){
        std::cout<<"socket not created properly";
    }
    else {
        std::cout<<"socket created successfully\n";
    }


    //create and configure server address
    sockaddr_in server_addr{};

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);


    //bind socket to IP and port
    int bind_sock = bind(sockfd, 
                    (struct sockaddr*)&server_addr,
                    sizeof(server_addr));

    if (bind_sock<0) {
        std::cout<<"bind failed";
        close(sockfd);
        return 1;
    }
    std::cout<<"socket bound to port 8080\n";
  

    //start listening
    if(listen(sockfd, 5) < 0){
        std::cout<<"listening failed";
        close(sockfd);
        return 1;
    }

    else {
        std::cout<<"server is listening on port 8080\n";
    }

    //accept clients
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);

    int client_sock;
    client_sock = accept(sockfd, 
        (struct sockaddr*)&client_addr, 
        &client_len);

    if (client_sock < 0){
        std::cout<<"accept failed";
        close(sockfd);
        return 1;
    }

    std::cout<<"client accepted for the connection\n";

   
        char buffer[256];

        int message_length = sizeof(buffer);

        int received_bytes = recv(client_sock, buffer, message_length, 0);

        if(received_bytes<0){
            std::cout<<"\nmessage couldn't be received\n";
        }
        else{
            std::cout<<"\nmessage received successfully\n";
            buffer[received_bytes]='\0';
            std::cout<<"\nClient says:\t" << buffer;
        }

        int send_back_to_client = send(client_sock, buffer, message_length, 0);

        if(send_back_to_client<0){
            std::cout<<"\nserver couldn't send the message back to the cilent";
        }
        else{
            std::cout<<"\nmessage successfully sent back to the client";
        }
    
    close(client_sock);
    close(sockfd);

    return 0;
}