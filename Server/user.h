#ifndef __USER_H__
#define __USER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <sys/socket.h>


using namespace std;

class User {
public:
    User(string u, string p, string e);
    
    void set_sockfd(int sfd) { sockfd = sfd; }
    void set_status(string stat);
    void set_is_login();
    void add_friend(string u) { friends.push_back(u); }
    void add_request(string u) { requests.push_back(u); }
    void remove_request(string u);
    void logout();
    void set_selected_friend(string u) { selected_friend = u; }
    
    int get_sockfd() { return sockfd; }
    string get_username() { return username; }
    string get_password() { return password; }
    string get_email() { return email; }
    string get_status() { return status; }
    bool get_is_login() { return is_login; }
    vector<string> get_friends() { return friends; }
    vector<string> get_requests() { return requests; }
    string get_selected_friend() { return selected_friend; }
private:
    int sockfd;
    string username;
    string password;
    string email;
    string status;
    bool is_login;
    vector<string> friends;
    vector<string> requests;
    string selected_friend;
};


#endif
