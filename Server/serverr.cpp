#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<cstring>
#include<thread>
#include<string>
#include<vector>
#include<mutex>

std::vector<std::pair<int, std::string>> clients;
std::mutex clients_mutex;


void handle_client(int client_sock){

    char buffer[256];

    std::string username;


    //username registration
    while(1){

        const char* prompt = "Enter username: ";
        send(client_sock, prompt, strlen(prompt), 0);

        int received_bytes = recv(client_sock, buffer, sizeof(buffer)-1, 0);

        if(received_bytes<0){
            std::cout<<"message couldn't be received\n";
            break;
        }

        else if(received_bytes == 0){
            std::cout<<"Client disconnected gracefully\n";
            break;
        }

        else{

            buffer[received_bytes]='\0';

            username = buffer;

            if(!username.empty() && username.back() == '\n')
                username.pop_back();

            if(!username.empty() && username.back() == '\r')
                username.pop_back();


            bool taken = false;

            {
                std::lock_guard<std::mutex> lock(clients_mutex);

                for(const auto& client : clients){

                    if(client.second == username){
                        taken = true;
                        break;
                    }
                }

                if(!taken){
                    clients.push_back({client_sock, username});
                }
            }


            if(taken){

                const char* message = "Username already taken\n";

                send(client_sock, message, strlen(message), 0);

                continue;
            }

            std::cout<<"a user "<<username << " registered\n";

            const char* message = "Username registered successfully\n";

            send(client_sock, message, strlen(message), 0);
            
            break;
        }
    }


    //receive messages
    while(1){

        int received_bytes = recv(client_sock, buffer, sizeof(buffer)-1, 0);

        if(received_bytes<0){

            std::cout<<username
                     <<": message couldn't be received\n";

            break;
        }

        else if(received_bytes == 0){

            std::cout<<username
                     <<" disconnected gracefully\n";

            break;
        }

        else{

            buffer[received_bytes]='\0';


            //quit command
            if(strcmp(buffer,"/quit")==0){

                std::cout<<username
                         <<" requested to quit\n";

                break;
            }


            //users command
            else if(strcmp(buffer,"/users")==0){

                std::string user_list = "Online users:\n";

                std::lock_guard<std::mutex> lock(clients_mutex);

                for(const auto& client : clients){

                    user_list += client.second;
                    user_list += "\n";
                }

                send(
                    client_sock,
                    user_list.c_str(),
                    user_list.length(),
                    0
                );
            }

            //private messaging
            else if(
                strncmp(buffer, "/msg ", 5) == 0
            ){

                std::string command(buffer);

                std::string remaining =
                    command.substr(5);


                //find space between username and message
                size_t space_position =
                    remaining.find(' ');


                if(
                    space_position == std::string::npos ||
                    space_position == 0 ||
                    space_position == remaining.length()-1
                ){

                    std::string error =
                        "Usage: /msg <username> <message>\n";

                    send(
                        client_sock,
                        error.c_str(),
                        error.length(),
                        0
                    );

                    continue;
                }


                std::string recipient =
                    remaining.substr(
                        0,
                        space_position
                    );


                std::string private_message =
                    remaining.substr(
                        space_position + 1
                    );


                int recipient_socket = -1;


                //find recipient
                {
                    std::lock_guard<std::mutex> lock(clients_mutex);

                    for(const auto& client : clients){

                        if(client.second == recipient){

                            recipient_socket =
                                client.first;

                            break;
                        }
                    }
                }


                //recipient not found
                if(recipient_socket == -1){

                    std::string error =
                        "User not found: "
                        + recipient
                        + "\n";

                    send(
                        client_sock,
                        error.c_str(),
                        error.length(),
                        0
                    );

                    continue;
                }


                //message for recipient
                std::string message_for_recipient =
                    "[PM from "
                    + username
                    + "]: "
                    + private_message
                    + "\n";


                int send_result = send(
                    recipient_socket,
                    message_for_recipient.c_str(),
                    message_for_recipient.length(),
                    0
                );


                if(send_result < 0){

                    std::cout
                        <<"couldn't send private message to "
                        <<recipient
                        <<"\n";

                    continue;
                }


                //confirmation for sender
                std::string confirmation =
                    "[PM to "
                    + recipient
                    + "]: "
                    + private_message
                    + "\n";


                send(
                    client_sock,
                    confirmation.c_str(),
                    confirmation.length(),
                    0
                );
            }
        

            //normal message
            else{

                std::cout<<username
                         <<" says:\t"
                         <<buffer
                         <<'\n';

                int send_back_to_client =
                    send(client_sock, buffer, received_bytes, 0);

                if(send_back_to_client<0){

                    std::cout
                        <<"server couldn't send the message back to "
                        <<username
                        <<"\n";

                    break;
                }

                else{

                    std::cout
                        <<"message successfully sent back to "
                        <<username
                        <<"\n";
                }
            }
        }
    }


    //remove client from clients
    {
        std::lock_guard<std::mutex> lock(clients_mutex);

        for(auto it = clients.begin(); it != clients.end(); ++it){

            if(it->first == client_sock){

                clients.erase(it);
                break;
            }
        }
    }


    close(client_sock);
}


int main() {

    //socket creation
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd<0){
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
    int bind_sock = bind(
        sockfd,
        (struct sockaddr*)&server_addr,
        sizeof(server_addr)
    );

    if(bind_sock<0) {
        std::cout<<"bind failed\n";
        close(sockfd);
        return 1;
    }

    std::cout<<"socket bound to port 8080\n";


    //start listening
    if(listen(sockfd, 5)<0){

        std::cout<<"listening failed\n";
        close(sockfd);
        return 1;
    }

    else {
        std::cout<<"server is listening on port 8080\n";
    }


    //accept clients
    while(1){

        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);

        int client_sock = accept(
            sockfd,
            (struct sockaddr*)&client_addr,
            &client_len
        );

        if(client_sock<0){

            std::cout<<"accept failed\n";
            continue;
        }


        std::cout<<"new client accepted for the connection\n";


        std::thread client_thread(handle_client, client_sock);

        client_thread.detach();
    }


    close(sockfd);

    return 0;
}