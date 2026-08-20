#include"client_manager.h"
 
std::vector<std::pair<int, std::string>> clients;
std::mutex clients_mutex;
 
 
//tries to register a client, returns false if username already taken
bool add_client(int client_sock, const std::string& username){
 
    std::lock_guard<std::mutex> lock(clients_mutex);
 
    for(const auto& client : clients){
 
        if(client.second == username){
            return false;
        }
    }
 
    clients.push_back({client_sock, username});
 
    return true;
}
 
 
//removes a client from the list on disconnect
void remove_client(int client_sock){
 
    std::lock_guard<std::mutex> lock(clients_mutex);
 
    for(auto it = clients.begin(); it != clients.end(); ++it){
 
        if(it->first == client_sock){
 
            clients.erase(it);
            break;
        }
    }
}
 
 
//finds the socket belonging to a username, -1 if not found
int find_client_socket(const std::string& username){
 
    std::lock_guard<std::mutex> lock(clients_mutex);
 
    for(const auto& client : clients){
 
        if(client.second == username){
            return client.first;
        }
    }
 
    return -1;
}
 
 
//builds the "Online users:" list sent for the /users command
std::string get_user_list(){
 
    std::string user_list = "Online users:\n";
 
    std::lock_guard<std::mutex> lock(clients_mutex);
 
    for(const auto& client : clients){
 
        user_list += client.second;
        user_list += "\n";
    }
 
    return user_list;
}