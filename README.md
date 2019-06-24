# A Very Basic C File Transfer System

**Current Functionality Rundown**
<br>
Server should bind to port 1337, and begins listening for connections. Client should then connect to port 1337 and request the file "server.c". The Server should look for, read and send the data which the Client should then recieve. The Client should then open a new file named "server.c" and write the data to it.

<br>

**API - Server.h**

```c
int  setupSocketWrapper(socketWrapper* sockWrap, int  portNum)
```
This function takes in a pre-initalised socketWrapper, and a port number, and do most of the heavy lifting to set up the socket, and any details relating to the socket required to use the socket.

---

```c
int  bringOnline(socketWrapper* sockWrap, int  queue, struct  sockaddr* castedServerStruct)
```
This function takes in a post-initalised socketWrapper, an integer with the desired queue length and an externally casted object within the socketWrapper.

The post-initalised socketWrapper should have been initalised by ```setupSocketWrapper()```.

The queue length is how many connections can wait while another connection is being processed before the connections get rejected automatically.

The externally casted struct object can be found by using ```socketWrapper.server```, which is the ```struct sockaddr_in``` structure containing the details about the server. Such object should be externally casted into a ```struct sockaddr*``` by using ```(struct sockaddr*)&(socketWrapper.server)```.

An example call of this function may be:
```c
bringOnline(&socketWrapper, 1, (struct sockaddr*)&(socketWrapper.server)
```
---
```c
int  clientFileSend(socketWrapper* sockWrap)
```
This function should take in a post-connection socketWrapper. The socket wrapper should have gone through ```bringOnline()``` before hand as it requires certain data fetched by ```bringOnline()```.

This function then sends a request to the client to send the filename desired, where then the server finds the file and sends it back.

---

```c
bringOffline(socketWrapper* sockWrap)
```
This function takes in a post-connection socketWrapper, and closes the main server socket within.
