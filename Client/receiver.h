#ifndef RECEIVER_H
#define RECEIVER_H
 
#include<atomic>
 
extern std::atomic<bool> running;
 
void receive_messages(int client_sock);
 
#endif
 