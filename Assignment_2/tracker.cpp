#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h> 
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <pthread.h>
#include <map>

#define PORT 2400
#define MAX_CLIENT 5
#define TRACKER_FILE "TRACKER.txt"
#define SERVER_FILE "SERVER.txt"

pthread_mutex_t serverMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t registrationMutex = PTHREAD_MUTEX_INITIALIZER;

std::map<std::string, int> onlinePeers;

void writeInFile(std::string msg) {
    pthread_mutex_lock(&serverMutex);
    // system(std::string("echo "+msg+" >> "+SERVER_FILE).c_str());
    std::ofstream serverFile(SERVER_FILE, std::ios::app);
    serverFile << msg << std::endl;
    serverFile.close();
    pthread_mutex_unlock(&serverMutex);
}

std::vector<std::string> split(const char str[], char a) {
    std::vector<std::string> res;
    std::stringstream ss(str);
    std::string temp;
    while(getline(ss, temp, a)) {
        res.push_back(temp);
    }
    return res;
}

std::string trackerData(std::string uniqueID) {
    std::ifstream readTracker("TRACKER.txt");
    std::string str;
    while(getline(readTracker, str)) {
        std::vector<std::string> res = split(str.c_str(), ':');
        if(uniqueID.compare(res[2]) == 0) {
            return res[2]+":"+res[3];
        }
    }
    return "User not found";
}

class Client {
    public:
    int id, socket, portNo;
    std::string name, password, uid;
    Client(int id, int clientSocket) {
        this->id = id;
        this->socket = clientSocket;
    }
    int registerClient() {
        //getting user data in format (peername:password:uniqueID)
        
        char userData[1024] = {0};
        int sizeofUserData = recv(this->socket, userData, 1024, 0);
        if(sizeofUserData <= 0) {
            return -1;
        }
        
        std::vector<std::string> res = split(userData, ':');
        
        if(res.size() != 3) {
            send(this->socket, "Invalid! Try again...", 22, 0);
            return this->registerClient();
        }
        
        //checking if uniqueID is actually unique
        if(std::string("Not found").compare(trackerData(res[2])) != 0) {
            send(this->socket, "uniqueID is already in use. Try with different ID", 50, 0);
            return this->registerClient();
        }
        
        //writing to tracker file
        // system(("echo "+std::string(userData)+this->portNo+" >> "+TRACKER_FILE).c_str());
        std::ofstream tracker(TRACKER_FILE, std::ios::app);
        tracker << std::string(userData)+":"+std::to_string(this->portNo) << std::endl;
        tracker.close();
        
        this->name = res[0];
        this->password = res[1];
        this->uid = res[2];
        
        return 0;
    }
    int loginClient() {
        //getting userdata in formmat (uniqueID:password)
        
        char userData[1024] = {0};
        int sizeofuserData = recv(this->socket, userData, 1024, 0);
        if(sizeofuserData <= 0) {
            return -1;
        }
        
        std::vector<std::string> res = split(userData, ':');
        
        if(res.size() != 2) {
            send(this->socket, std::string("Invalid! Try again...").c_str(), 22, 0);
            return this->loginClient();
        }
        
        //reading tracker file  format (name:password:uid:portno)
        std::ifstream tracker(TRACKER_FILE);
        
        std::stringstream ss;
        ss << tracker.rdbuf();
        
        std::string user, fulldata = ss.str();
        
        while (std::getline(ss, user, '\n')) {
            std::vector<std::string> userdata = split(user.c_str(), ':');
            if(userdata[2] == res[0] && userdata[1] == res[1]) {
                tracker.close();
                this->uid = res[0];
                this->name = userdata[0];
                this->password = userdata[1];
                
                // updating in tracker file
                int pos = fulldata.find(res[0]);
                pos += res[0].length()+1;
                fulldata.replace(pos, userdata[3].length(), std::to_string(this->portNo));
                std::ofstream tracker(TRACKER_FILE);
                tracker << fulldata;
                tracker.close();
                
                return 0;
            }
        }
    
        tracker.close();    
        send(this->socket, "Incorrect user id or password! Try again", 36, 0);
        return this->loginClient();
    }
    void welcome() {
        std::string str = "Welcome " + this->name + "You can now start messaging!";
        send(this->socket, str.c_str(), str.size()+1, 0);
    }
};

std::string greet(std::string name) {
    return std::string("Welcome from server, ")+name;
}

std::string getOnlinePeers() {
    std::string res = "";
    for(auto &it: onlinePeers) {
        res += it.first + ":";
    }
    res[res.size()-1] = '\0';
    return res;
}

int authenticateClient(Client* client) {
    int clientSocket = client->socket;
    int clientID = client->id;
    char msg[1024];
    while(true) {
        memset(msg, 0, 1024);
        // read(clientSocket, msg, 4096);
        int bytesrecv = recv(clientSocket, msg, 1024, 0);
        if(bytesrecv <=0 ) {
            return -1;
        }
        if(strcmp(msg, "Register") == 0 || strcmp(msg, "register") == 0) {
            //register client
            pthread_mutex_lock(&registrationMutex);
            send(clientSocket, "Please type in this format - (Name:Password:uniqueID)", 44, 0);
            int res = client->registerClient();
            pthread_mutex_unlock(&registrationMutex);
            return res;
        } else if(strcmp(msg, "Login") == 0 || strcmp(msg, "login") == 0){
            //login client
            send(clientSocket, "Please login in this format - (uniqueID:Password)", 32, 0);
            int res = client->loginClient();
            return res;
        } else {
            send(clientSocket, "Please retry!!", 9, 0);
        }
    }
    return 0;
}

void *Messaging(void* arg) {
    Client* client = (Client*)arg;
    int clientID = client->id;
    int clientSocket = client->socket;
    char msg[4096] = {0};
    
    std::cout << "Waiting for the user to connect\n";
    
    if(authenticateClient(client) == -1) {
        std::cout << "The Client is" << clientID << " disconnected\n";
        close(clientSocket);
        pthread_exit(NULL);
        return NULL;
    }
    
    //adding client to online peers
    onlinePeers[client->uid] = client->portNo;
    
    std::cout << "The Client " << clientID << " is ready to exchange messages\n\n";
    client->welcome();
    
    while(true) {
        memset(msg, 0, 4096);
        
        int bytesRecv = recv(clientSocket, msg, 4096, 0);
        if(bytesRecv == -1) {
            std::cout << "The client could not be connected. " << clientID << "\n";
            break;
        }
        if(bytesRecv == 0) {
            std::cout << "The client is" << clientID << " disconnected\n";
            break;
        }

        std::cout << "Received from client " << clientID << ": " << msg << std::endl;
        writeInFile("Received from client "+std::to_string(clientID)+": "+std::string(msg));
        
        if(strcmp(msg, "Get my details")==0) {
            std::string details = trackerData(client->uid);      //this will read data from the tracker file
            send(clientSocket, details.c_str(), details.size(), 0);
        } else if(strcmp(msg, "Get online peers")==0) {
            std::string res = getOnlinePeers();
            send(clientSocket, res.c_str(), res.size(), 0);
        } else {
            std::vector<std::string> data = split(msg, ' ');
            std::string str;
            if(data.size() != 0) {
                str = greet(data[data.size()-1]);
            } else {
                str = "Invaild";
            }
            send(clientSocket, str.c_str(), str.size(), 0);
            data.clear();
        }
        
        std::cout << "Message sent to the client " << clientID << "\n\n";
        writeInFile("Message sent to the client "+std::to_string(clientID));
    }
    
    //removing client from online peers
    onlinePeers.erase(client->uid);
    
    close(clientSocket);
    pthread_exit(NULL);
}

int main() {
    
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket == 0) {
        std::cout << "Can't create a socket!\n";
        return -1;
    }
    sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORT);
    // saddr.sin_addr.s_addr = INADDR_ANY
    
    inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr);
    
    // int opt=1;
    // if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == 0) {
    //     std::cout << "setsockopt error\n";
    //     return -1;
    // }
    
    // bind server with ip/port
    if(bind(serverSocket, (sockaddr*)&saddr, sizeof(saddr))<0) {
        std::cout << "Bind Error\n";
        return -1;
    }
    
    //Listening
    if(listen(serverSocket, SOMAXCONN) < 0) {
        std::cout << "istening Error\n" << std::endl;
        return -1;
    }
    
    std::cout << "Started listening on: " << PORT << std::endl;
    writeInFile("Started listening on: "+std::to_string(PORT));

    pthread_t threads[MAX_CLIENT];
    
    for(int i=0; i<MAX_CLIENT; i++) {
        
        sockaddr_in caddr;
        socklen_t clientSize = sizeof(caddr);
        int clientSocket = accept(serverSocket, (sockaddr*)&caddr, &clientSize);
        
        if(clientSocket == -1) {
            std::cout << "Problem with client connecting!\n";
            continue;
        }
        std::cout << "\nNew Client connected\n";
        writeInFile("New Client connected");
        
        // To get the user data
        char host[NI_MAXHOST] = {0};
        char serv[NI_MAXSERV] = {0};
        
        int result = getnameinfo((sockaddr*)&caddr, clientSize, host, NI_MAXHOST, serv, NI_MAXSERV, 0);
        
        if(result) {
            std::cout << host << " connecting on " << serv << std::endl << std::endl;
            writeInFile(std::string(std::string(host)+" connecting on "+std::string(serv)));
        } else {
            inet_ntop(AF_INET, &caddr.sin_addr, host, NI_MAXHOST);
            std::cout << host << " connecting on " << ntohs(caddr.sin_port) << std::endl << std::endl;
            writeInFile(std::string(std::string(host)+" connecting on "+std::to_string(ntohs(caddr.sin_port))));
        }
        
        Client* client = new Client(i+1, clientSocket);
        send(clientSocket, serv, sizeof(serv), 0);
        client->portNo = atoi(serv);
        
        if(pthread_create(&threads[i], NULL, Messaging, (void*)client)) {
            std::cout << "Error creating thread\n";
            continue;
        }
    }
    for(int i=0; i<MAX_CLIENT; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_exit(NULL);
    close(serverSocket);
    
    return 0;
}
