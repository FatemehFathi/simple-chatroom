#include "server.h"


void Server::run(char* port) {
    
	int clientFds[MAX_CLIENT];
	int clientIndex=0;
	
	fd_set readfds;

	char recvBuff[BUFF_SIZE];
	char sendBuff[1025];
	memset(sendBuff, '0', sizeof(sendBuff));

	struct sockaddr_in serv_addr,client;
	memset(&serv_addr, '0', sizeof(serv_addr));
	int p = atoi(port);
	serv_addr.sin_family = PF_INET;
	serv_addr.sin_port = htons(p);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	//serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);	//socket( PF_INET, SOCK_STREAM, 0 );
	if (listenfd < 0) {
        cerr << "error: socket()" << endl;
        exit(1);
	}

	bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	
	if(listen(listenfd, 10) == -1) {
        cerr << "Failed to listen" << endl;
		exit(1);
	}
	
	
	int newSock, n;
	socklen_t clientSize = sizeof(client);

    while(1) {
        struct timeval timeout;
        timeout.tv_sec = 3;     /* every 3 seconds */
        timeout.tv_usec = 400;  /* plus 400 msec */
        FD_ZERO(&readfds);
        FD_SET (listenfd, &readfds);
         
        for (int d = 0; d < clientIndex; d++) {
            FD_SET(clientFds[d], &readfds);
        }
	
        int q = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);
        
        if (FD_ISSET(listenfd, &readfds)) {
            newSock = accept(listenfd, (struct sockaddr *)&client, (socklen_t*)&clientSize);
            clientFds[clientIndex] = newSock;
            clientIndex++;
        }
	
         
        for(int c = 0; c < clientIndex; c++ ) {
            int fd = clientFds[c];
            if (FD_ISSET(fd, &readfds)) {
                n = recv(fd, recvBuff, BUFF_SIZE - 1, 0);
		
                if(n < 0){
                    cerr << "error: recv()" << endl;
                } else if(n > 0) {
                    recvBuff[n] = '\0';
                    cout << "Rcved msg from fd = "<< fd << ": "<< recvBuff <<endl;

                    string msg(recvBuff);
                    vector<string> to_send = parse(msg, fd);
            
                    int sockfd = atoi(to_send[0].c_str());
                    string toSend = to_send[1];
            
                    n = send(sockfd, toSend.c_str(), strlen(toSend.c_str()), 0);
                    if (n < strlen("ack")) {
                        cerr << "error: send()" << endl;
                    }
                } else {
                    cout << "faz chie n=0 on recv :P " << endl;
                }
            }
        }
    }
}



///////////////////////////////////////////////////////////////////////////////////////

vector<string> Server::parse(string s, int sockfd) {
    int p;
	int q;
	string to_send = "";
    vector<string> result;
    
    stringstream ss;
    ss << sockfd;
    string str = ss.str();
    result.push_back(str);
    
    
    if (s.find("<regUser>") == string::npos || s.find("<loginUser>") == string::npos) {
        for (int i = 0; i < users.size(); i++) {
            if (users[i]->get_sockfd() == sockfd)
                if (!users[i]->get_is_login()) {
                    result.push_back("105");
                    return result;
                }
        }
    }
    
    
	if (p = s.find("<regUser>") != string::npos) {
		q = s.find("</regUser>");
		string username = s.substr(p + 8, q - p - 8);
		p = s.find("<pass>");
		q = s.find("</pass>");
		string password = s.substr(p + 6, q - p - 6);
        p = s.find("<email>");
		q = s.find("</email>");
		string email = s.substr(p + 7, q - p - 7);
        
		to_send = register_user(username, password, email);
        result.push_back(to_send);
		return result;
	}
    
	else if (p = s.find("<loginUser>") != string::npos) {
        q = s.find("</loginUser>");
		string username = s.substr(p + 10, q - p - 10);
		p = s.find("<pass>");
		q = s.find("</pass>");
        string password = s.substr(p + 6, q - p - 6);
        
        to_send = login(username, password, sockfd);
        result.push_back(to_send);
		return result;
	}
    
    else if (p = s.find("<stat>") != string::npos) {
        q = s.find("</stat>");
		string status = s.substr(p + 5, q - p - 5);
        
        to_send = change_status(status, sockfd);
        result.push_back(to_send);
		return result;
	}
    
    else if (p = s.find("<who>") != string::npos) {
        q = s.find("</who>");
		string part = s.substr(p + 4, q - p - 4);
        
        to_send = find_who(part);
        result.push_back(to_send);
		return result;
	}
    
    else if (p = s.find("<invite>") != string::npos) {
        q = s.find("</invite>");
		string username = s.substr(p + 7, q - p - 7);
        
        to_send = invite(username, sockfd);
        result.push_back(to_send);
		return result;
	}
    
    else if (p = s.find("<deny>") != string::npos) {
        q = s.find("</deny>");
		string username = s.substr(p + 5, q - p - 5);
        
        to_send = deny(username, sockfd);
        result.push_back(to_send);
		return result;
	}
    
    else if (p = s.find("<acc>") != string::npos) {
        q = s.find("</acc>");
		string username = s.substr(p + 4, q - p - 4);
        
        to_send = accpt(username, sockfd);
        result.push_back(to_send);
		return result;
	}
    
    else if (p = s.find("<select>") != string::npos) {
        q = s.find("</select>");
		string username = s.substr(p + 7, q - p - 7);
        
		to_send = select_user(username, sockfd);
        result.push_back(to_send);
		return result;
	}
    
    else if (p = s.find("<msg>") != string::npos) {
        q = s.find("</msg>");
		string msg = s.substr(p + 4, q - p - 4);
        
		to_send = send_msg(msg, sockfd);
        result.push_back(to_send);
		return result;
	}
    
    else if (p = s.find("<exit>") != string::npos) {
        
        to_send = logout(sockfd);
        result.push_back(to_send);
		return result;
	}

}




////////////////// Register

string Server::register_user(string username, string password, string email) {
    if (prevent_reiterative_user(username, email)) {
        User* u = new User(username, password, email);
        users.push_back(u);
        return "100";
        
    } else {
        return "101";
    }
}

bool Server::prevent_reiterative_user(string username, string email) {
    for (int i = 0; i < users.size(); i++) {
        if (users[i]->get_username() == username || users[i]->get_email() == email)
            return false;
    }
    
    return true;
}




////////////////// Login:

string Server::login(string username, string password, int sockfd) {
    for (int i = 0; i < users.size(); i++) {
        if (users[i]->get_sockfd() == sockfd)
            return "You must Exit first!";
    }
    
    for (int i = 0; i < users.size(); i++) {
        if (users[i]->get_username() == username) {
            if (users[i]->get_password() == password) {
                users[i]->set_is_login();
                users[i]->set_sockfd(sockfd);
                return "100";
            }
        }
    }
        
    return "102";
}




////////////////// Change Status:

string Server::change_status(string stat, int sockfd) {
    if (stat == "available" || stat == "busy" || stat == "offline") {
        
        for (int i = 0; i < users.size(); i++) {
            if (users[i]->get_sockfd() == sockfd) {
                users[i]->set_status(stat);
                return "100";
            }
        }
    } else
        return "103";
}




////////////////// Who:

string Server::find_who(string part) {
    string result = "It’s Ok.\n\nList:\n";
    
    for (int i = 0; i < users.size(); i++) {
        if (users[i]->get_username().find(part) != string::npos || users[i]->get_email().find(part) != string::npos) {
            if (users[i]->get_status() == "available" || users[i]->get_status() == "busy")
                result += users[i]->get_username() + "\n";
        }
    }
    
    if (result == "It’s Ok.\n\nList:\n")
        result = "104";
    
    return result;
}




////////////////// Invite:

string Server::invite(string username, int sockfd) {
    User* login_user = new User("TEMP", "TEMP", "TEMP");
    for (int i = 0; i < users.size(); i++) {
        if (users[i]->get_sockfd() == sockfd)
            login_user = users[i];
    }
    
    if (login_user->get_status() == "offline")
        return "103";
    
    if (login_user->get_username() == username)
        return "100";
    
    
    //request tekrari
    for (int i = 0; i < users.size(); i++) {
        if (users[i]->get_username() == username) {
            vector<string> requests = users[i]->get_requests();
            for (int j = 0; j < requests.size(); j++)
                if (requests[j] == login_user->get_username())
                    return "100";
        }
    }
    
    vector<string> friends = login_user->get_friends();
    for (int i = 0; i < friends.size(); i++) {
        if (friends[i] == username) {
            return "100";
        }
    }
    
    for (int i = 0; i < users.size(); i++) {
        if (users[i]->get_username() == username) {
            users[i]->add_request(login_user->get_username());
            if (users[i]->get_status() == "available") {
                string list = "New Request:\n" + login_user->get_username();
                send(users[i]->get_sockfd(), list.c_str(), strlen(list.c_str()), 0);
            }
            return "100";
        }
    }

    return "Wrong Username!";
}




////////////////// Accept:

string Server::accpt(string username, int sockfd) {
    User* login_user = new User("TEMP", "TEMP", "TEMP");
    for (int i = 0; i < users.size(); i++) {
        if (users[i]->get_sockfd() == sockfd)
            login_user = users[i];
    }
    
    User* other_user = new User("TEMP", "TEMP", "TEMP");
    for (int i = 0; i < users.size(); i++) {
        if (users[i]->get_username() == username)
            other_user = users[i];
    }
    
    if (login_user->get_status() == "offline")
        return "103";
    
    vector<string> requests = login_user->get_requests();
    for (int i = 0; i < requests.size(); i++) {
        if (requests[i] == username) {
            login_user->add_friend(username);
            login_user->remove_request(username);
            other_user->add_friend(login_user->get_username());
            return "100";
        }
    }

    return "Wrong Username!";
}




////////////////// Deny:

string Server::deny(string username, int sockfd) {
    User* login_user = new User("TEMP", "TEMP", "TEMP");
    for (int i = 0; i < users.size(); i++) {
        if (users[i]->get_sockfd() == sockfd)
            login_user = users[i];
    }
    
    if (login_user->get_status() == "offline")
        return "103";
    
    vector<string> requests = login_user->get_requests();
    for (int i = 0; i < requests.size(); i++) {
        if (requests[i] == username) {
            login_user->remove_request(username);
            return "100";
        }
    }

    return "Wrong Username!";
}




////////////////// Select:

string Server::select_user(string username, int sockfd) {
    User* login_user = new User("TEMP", "TEMP", "TEMP");
    for (int i = 0; i < users.size(); i++) {
        if (users[i]->get_sockfd() == sockfd)
            login_user = users[i];
    }
    
    if (login_user->get_status() == "offline")
        return "103";
    
    vector<string> friends = login_user->get_friends();
    
    for (int i = 0; i < friends.size(); i++) {
        if (friends[i] == username) {
            for (int i = 0; i < users.size(); i++) {
                if (users[i]->get_username() == username) {
                    if (users[i]->get_status() == "available") {
                        login_user->set_selected_friend(username);
                        return "100";
                    } else
                        return "103";
                }
            }
        }
    }

    return "106";
}


    
 
////////////////// Send:

string Server::send_msg(string msg, int sockfd) {
    User* login_user = new User("TEMP", "TEMP", "TEMP");
    for (int i = 0; i < users.size(); i++) {
        if (users[i]->get_sockfd() == sockfd)
            login_user = users[i];
    }

    if (login_user->get_status() == "offline")
        return "103";
    
    if (login_user->get_selected_friend() == "!NOBODY!")
        return "107";
    
    
    User* other_user = new User("TEMP", "TEMP", "TEMP");
    for (int i = 0; i < users.size(); i++) {
        if (users[i]->get_username() == login_user->get_selected_friend())
            other_user = users[i];
    }
    
    
    if (other_user->get_status() != "available")
        return "107";
    
    string to_send = "Message From " + login_user->get_username() + ":\n" + msg;
    send(other_user->get_sockfd(), to_send.c_str(), strlen(to_send.c_str()), 0);
    return "100";
        
}



////////////////// Logout:
string Server::logout(int sockfd) {
    for (int i = 0; i < users.size(); i++) {
        if (users[i]->get_sockfd() == sockfd) {
            users[i]->logout();
        }
    }
    
    return "100";
}







int main(int argc, char* argv[]) {
    Server* s = new Server();
    s->run(argv[1]);
    return 0;
}



