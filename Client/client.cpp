#include "client.h"



int main(int argc, char *argv[]) {
	struct sockaddr_in serv_addr;
	struct hostent *server;
    char* IP = argv[1];
    int PORT = atoi(argv[2]);
	char buffer[MESSAGE_SIZE];
    struct timeval timeout;
    
    
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0) cerr << "Error Opening Socket!" << endl;
	server = gethostbyname(IP);
	if (server == NULL) {
		cerr << "no such host" << endl;
		exit(0);
	}
    
	bzero((char*) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(IP);
	serv_addr.sin_port = htons(PORT);
	int temp;
    bool dc = false;
    
	if ((temp=connect(sock_fd, (sockaddr *)&serv_addr, sizeof(serv_addr))) < 0) {
        cerr << "Connecting Error!" << endl;
        dc = true;
        return 0;
    }
 
    
    ///////////////// select:
    
    fd_set master_fd, read_fd;
	FD_ZERO(&master_fd);
	FD_SET(sock_fd, &master_fd);
	FD_SET(0, &master_fd);
    timeout.tv_sec  = 1;
    timeout.tv_usec = 0;
    

    while (dc == false) {
        string input;

        
        FD_ZERO(&read_fd);
		FD_SET(sock_fd, &read_fd);
		FD_SET(0, &read_fd);
        
		if (select(sock_fd + 1, &read_fd, NULL, NULL, NULL) == -1) {
			cerr << "Select() Error!" << endl;
			exit(1);
		}
        
        
   
        if (FD_ISSET(0, &read_fd)) {
            cin >> input;
            string result = "";
            
            if (input == "Register") {
                cin >> input;
                result += "<regUser>" + input + "</regUser>";
                cin >> input;
                result += "<pass>" + input + "</pass>";
                cin >> input;
                result += "<email>" + input + "</email>";
                
            } else if (input == "Login") {
                cin >> input;
                result += "<loginUser>" + input + "</loginUser>";
                cin >> input;
                result += "<pass>" + input + "</pass>";
                
            } else if (input == "ChangeStatus") {
                cin >> input;
                result += "<stat>" + input + "</stat>";
                
            } else if (input == "Who") {
                cin >> input;
                result += "<who>" + input + "</who>";
                
            } else if (input == "Invite") {
                cin >> input;
                result += "<invite>" + input + "</invite>";
                
            } else if (input == "Accept") {
                cin >> input;
                result += "<acc>" + input + "</acc>";
                
            } else if (input == "Deny") {
                cin >> input;
                result += "<deny>" + input + "</deny>";
                
            } else if (input == "Select") {
                cin >> input;
                result += "<select>" + input + "</select>";
                
            } else if (input == "Send") {
                cin >> input;
                result += "<msg>" + input + "</msg>";
                
            } else if (input == "Exit") {
                result = "<exit>";
            }
            
            bzero(buffer, MESSAGE_SIZE);
            strcpy(buffer, result.c_str());
            
            //write
            int n = write(sock_fd, buffer, strlen(buffer));
            if (n < 0) cerr << "Error: Writing to the Socket" << endl;
        }
        
        
        
        
        if (FD_ISSET(sock_fd, &read_fd)) {
            //read
            bzero(buffer, MESSAGE_SIZE);
            int n1 = read(sock_fd, buffer, MESSAGE_SIZE);
            if (n1 < 0) cerr << "Error: Reading from the Socket" << endl;
            
            string output = (string)buffer;
            
            if (output == "100") {
                cout << "It’s Ok." << endl << endl;
                
            } else if (output == "101") {
                cout << "Duplicate username or email" << endl << endl;
                
            } else if (output == "102") {
                cout << "Wrong username or password" << endl << endl;
                
            } else if (output == "103") {
                cout << "Wrong status" << endl << endl;
                
            } else if (output == "104") {
                cout << "Not found" << endl << endl;
                
            } else if (output == "105") {
                cout << "Not connected" << endl << endl;
                
            } else if (output == "106") {
                cout << "Not your friend" << endl << endl;
                
            } else if (output == "107") {
                cout << "Not sent" << endl << endl;
                
            } else {
                cout << output << endl << endl;
            }
        }
        
	} // while(1)
    
	close(sock_fd);
	return 0;
}
                    
                    
                    
