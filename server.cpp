#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<cstring>
#include<thread>


void handle_client(int client_sock, int client_id){

    char buffer[256]; 

    while(1){
        int received_bytes = recv(client_sock, buffer, sizeof(buffer)-1, 0);

        if(received_bytes<0){
            std::cout<<"Client "<<client_id<<": message couldn't be received\n";
            break;
        }

        else if (received_bytes == 0) { 
            std::cout<<"Client "<<client_id<<" disconnected gracefully\n";
            break; 
        }

        else{
            buffer[received_bytes]='\0';

            if(strncmp(buffer,"quit",4)!=0){
                std::cout<<"Client "<<client_id<<" says:\t" << buffer <<'\n';
                
                int send_back_to_client = send(client_sock, buffer, received_bytes, 0);

                if(send_back_to_client<0){
                    std::cout<<"server couldn't send the message back to Client "<<client_id<<"\n";
                    break;
                }
                else{
                    std::cout<<"message successfully sent back to Client "<<client_id<<"\n";
                }
            }
            else{
                std::cout<<"Client "<<client_id<<" requested to quit\n";
                break;
            }        
        }   
    }

    close(client_sock);
}


int main() {

    //socket creation
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd<0){
        std::cout<<"socket not created properly\n";
        return 1;
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
        std::cout<<"bind failed\n";
        close(sockfd);
        return 1;
    }
    std::cout<<"socket bound to port 8080\n";
  

    //start listening
    if(listen(sockfd, 5) < 0){
        std::cout<<"listening failed\n";
        close(sockfd);
        return 1;
    }

    else {
        std::cout<<"server is listening on port 8080\n";
    }


    //accept clients
    int next_client_id = 1;

    while(1){

        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);

        int client_sock = accept(sockfd, 
            (struct sockaddr*)&client_addr, 
            &client_len);

        if (client_sock < 0){
            std::cout<<"accept failed\n";
            continue;
        }

        int client_id = next_client_id++;

        std::cout<<"Client "<<client_id<<" accepted for the connection\n";

        std::thread client_thread(handle_client, client_sock, client_id);

        client_thread.detach();
    }

    close(sockfd);

    return 0;
}