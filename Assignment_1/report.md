**Socket Programming:**
```
~Socket programming is a way of connecting two nodes on a network to communicate with each other.  
~One socket(node) listens on a particular port at an IP, while another socket reaches out to the other to form a connection. Server forms the listener socket while the client reaches out to the server.  
~There is a server and a client.  
```
What is a Socket ?  
```
Socket let apps attach to the local network at different ports.  
Server/Client Applications  
```

**The basic mechanisms of client-server setup are:**  
```
A client app send a request to a server app.  
The server app returns a reply. In our case , when client sends his name , server replies with Hello <name>  
```
 
 
**Stages for Server-**
Socket Creation & setsockopt--  

struct sockaddr_in server_addr;  
    socklen_t size;  
 client = socket(AF_INET, SOCK_STREAM, 0);  

Bind--

(bind(client, (struct sockaddr*)&server_addr,sizeof(server_addr)))  

Listen--

size = sizeof(server_addr);  
    cout << "=> Looking for clients..." << endl;  
    listen(client, 1);  
    
Accept--  

server = accept(client,(struct sockaddr *)&server_addr,&size);  

Send and Recieve--  

send(server, buffer, bufsize, 0);  
recv(server, buffer, bufsize, 0);  

Stages for Client-

Socket Connection-- Same as server’s socket creation  

Connect  --

connect(client,(struct sockaddr *)&server_addr, sizeof(server_addr))  

**Implementation**
```
How To Run-
> g++ server.c -o server  
   server.exe  

>g++ client.c -o client  
   client.exe  
```
