#include<iostream>
#include<sys/socket.h>
#include<unistd.h>
#include<cstring>
#include<string>
#include"client_handler.h"
#include"client_manager.h"
 
 
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
 
 
            bool taken = !add_client(client_sock, username);
 
 
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
 
                std::string user_list = get_user_list();
 
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
 
 
                //find recipient
                int recipient_socket = find_client_socket(recipient);
 
 
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
    remove_client(client_sock);
 
 
    close(client_sock);
}