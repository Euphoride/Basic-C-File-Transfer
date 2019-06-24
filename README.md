# A Very Basic C File Transfer System

**Current Functionality Rundown**
Server should bind to port 1337, and begins listening for connections. Client should then connect to port 1337 and request the file "server.c". The Server should look for, read and send the data which the Client should then recieve. The Client should then open a new file named "server.c" and write the data to it.

<br>

**API - Server.h - Functions**

```c
int  setupSocketWrapper(socketWrapper* sockWrap, int  portNum)
```
This function takes in a pre-initalised socketWrapper, and a port number, and do most of the heavy lifting to set up the socket, and any details relating to the socket required to use the socket.

Returns:
|Error|Details  |
|--|--|
| -1 | Socket Initalisation Error |


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

Returns:
|Error| Details |
|--|--|
| -1 | Error with binding socket |
| -2 | Error with listening initalisation|
| -3 | Error with accepting incoming connections|


---
```c
int  clientFileSend(socketWrapper* sockWrap)
```
This function should take in a post-connection socketWrapper. The socket wrapper should have gone through ```bringOnline()``` before hand as it requires certain data fetched by ```bringOnline()```.

This function then sends a request to the client to send the filename desired, where then the server finds the file and sends it back.

Returns:
|Error| Details |
|--|--|
| -1 | Error with "Send File Name" command sending |
| -2 | Error with Receiving filename |
| -3 | Error with sending file data |
| -4 | Error with closing client-connection socket |


---

```c
int bringOffline(socketWrapper* sockWrap)
```
This function takes in a post-connection socketWrapper, and closes the main server socket within.

Returns:
|Error| Details |
|--|--|
| -1 | Error with closing main socket |

----

**API - Server.h - Structures**
<br>
```c
struct  socketWrapper {
	struct  sockaddr_in  server;
	struct  sockaddr_in  currentClient;

	int connectionSocket;
	int serverSocket;
	int failCheck;

	socklen_t socketSize;
};
```
This structure is the main socketWrapper structure.

Uses:
|Object| Details |
|--|--|
| `struct sockaddr_in server` | The main structure with server details. Contains current port, current family, current interface to bind to, etc. |
|`struct sockaddr_in client` | Not actually touched by the API itself, but data is copied into it when the main socket accepts a connection from the client. Contains client IP address and further details allowing for data to be sent back to the client |
|`int connectionSocket` | Socket that holds a connection with the client after the main socket accepts a connection. This frees the main socket to accept more connections if needed |
|`int serverSocket` | Main Socket, deals with binding itself as well as listening for and accepting connections |
|`int failCheck` | Used by the API to check for any errors, especially inter-functional errors |
|`socklen_t socksize` | Size of the structures that define server or client related data. Errors with such socket sizes can be seen if the server seems to be accepting multiple connections from "0.0.0.0". |


