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
```
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
```
**Implementation**
```
How To Run-
> g++ server.c -o server  
   server.exe  

>g++ client.c -o client  
   client.exe  
```

_
**Multi-Threading**
_
```
>Multithreading is the ability of a program or an operating system process to manage its use by more than one user at a time  and to even  manage multiple requests  by the same user without  having to have multiple copies of the programming running in the computer.  
>Multithreading allows multiple concurrent  tasks to  be performed  within a single process. It’s faster for an operating  system (OS) to switch  between threads for an active CPU task than it is to switch between different  processes.  
>std :: thread  is the thread class that represents a single thread in C++. To start a thread  we simply need to create  a new thread object  and pass the executing  code to be called into the constructor  of the object . Once the object is created  a new thread  is launched which will execute the code specified in callable.  

>Working of <mult-ithreading>  
1.Using the function object  
2.Using function pointer  
3.Using a lambda expression  
```
