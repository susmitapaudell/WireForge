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

    close(sockfd);

    return 0;
}