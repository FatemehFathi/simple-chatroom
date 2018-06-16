#include "user.h"


User::User(string u, string p, string e) {
    sockfd = -1;
    username = u;
    password = p;
    email = e;
    status = "offline";
    is_login = false;
    selected_friend = "!NOBODY!";
}



void User::set_is_login() {
    is_login = true;
    status = "available";
    selected_friend = "!NOBODY!";
}



void User::logout() {
    sockfd = -1;
    is_login = false;
    status = "offline";
    selected_friend = "!NOBODY!";
}



void User::remove_request(string u) {
    cout <<"BEFORE:"<<endl;
    for (int i = 0; i < requests.size(); i++)
        cout << requests[i]<<endl;
    
    
    for (int i = 0; i < requests.size(); i++)
        if (requests[i] == u)
            requests.erase(requests.begin() + i);
    
    cout <<"AFTER:"<<endl;
    for (int i = 0; i < requests.size(); i++)
        cout << requests[i]<<endl;
}



void User::set_status(string stat) {
    if (status == "offline" || status == "busy") {
        if (stat == "available" && requests.size() > 0) {
            string list = "Requests:\n";
            for (int i = 0; i < requests.size(); i++) {
                list += requests[i];
                list += "\n";
            }
            send(sockfd, list.c_str(), strlen(list.c_str()), 0);
        }
    }
    
    status = stat;
}


