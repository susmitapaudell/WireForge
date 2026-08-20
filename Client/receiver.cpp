#include<iostream>
#include<sys/socket.h>
#include"receiver.h"
 
std::atomic<bool> running(true);
 
 
void receive_messages(int client_sock){
 
    char buffer[256];
 
 
    while(running){
 
        int received_bytes = recv(
            client_sock,
            buffer,
            sizeof(buffer)-1,
            0
        );
 
 
        if(received_bytes<0){
 
            if(running){
 
                std::cout
                    <<"\nmessage could not be received\n";
            }
 
            break;
        }
 
 
        else if(received_bytes==0){
 
            std::cout
                <<"\nserver disconnected\n";
 
            running = false;
 
            break;
        }
 
 
        else{
 
            buffer[received_bytes] = '\0';
 
            std::cout
                <<"\n"
                <<buffer;
 
            std::cout
                <<"enter message\n";
        }
    }
}