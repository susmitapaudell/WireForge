#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H
 
#include<string>
#include<vector>
#include<mutex>
 
extern std::vector<std::pair<int, std::string>> clients;
extern std::mutex clients_mutex;
 
bool add_client(int client_sock, const std::string& username);
 
void remove_client(int client_sock);
 
int find_client_socket(const std::string& username);
 
std::string get_user_list();
 
#endif
 