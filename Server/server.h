#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <signal.h>
#include <time.h>
#include <dirent.h>
#include <signal.h>
#include <errno.h>
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <string>
#include <sys/time.h>
#include "user.h"

using namespace std;

#define TRUE 1
#define MESSAGE_SIZE 1024
#define BUFF_SIZE 1024
#define MAX_CLIENT 100

class Server {
public:
    void run(char* port);
    vector<string> parse(string s, int sockfd);
    
    string register_user(string username, string password, string email);
    bool prevent_reiterative_user(string username, string email);
    string login(string username, string password, int sockfd);
    string change_status(string stat, int sockfd);
    string find_who(string part);
    string invite(string username, int sockfd);
    string deny(string username, int sockfd);
    string accpt(string username, int sockfd);
    string select_user(string username, int sockfd);
    string send_msg(string msg, int sockfd);
    string logout(int sockfd);
private:
    vector<User*> users;
};


#endif
