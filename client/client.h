#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/types.h>

/* Function to write data */
void writeFile(const char* filename, const char* data, unsigned long fileSize) {
    FILE *fp;

    fp = fopen(filename, "w");

    fwrite(data, 1, fileSize, fp);

    fclose(fp);
}


/* Structure for client socket dand details */
struct socketWrapper {
    struct sockaddr_in destinationServer;

    int recvBufferLen;
    int clientSocket;
    int failCheck;

    char* recvBuffer;

    socklen_t socketSize;
};

typedef struct socketWrapper socketWrapper;

/* Clear FailCheck */
void clearFailCheck(socketWrapper* sockWrap) {
    sockWrap->failCheck = 0;
}

/* Socket Wrapper Setup */
int setupSocketWrapper(socketWrapper* sockWrap) {
    /* Clear FailCheck */
    clearFailCheck(sockWrap);

    /* Socket Holder */
    sockWrap->clientSocket  = socket(PF_INET, SOCK_STREAM, 0);

    /* Error Checking */
    if (sockWrap->clientSocket == -1) {
        return -1;
    } 

    /* Base Buffer Size */
    sockWrap->recvBufferLen = 4096;

    /* Interfunction failcheck */
    sockWrap->failCheck     = 0;

    /* Base Buffer Allocation */
    sockWrap->recvBuffer = malloc(sockWrap->recvBufferLen);

    /* Socket Size Definition */
    sockWrap->socketSize = sizeof(struct sockaddr_in);

    return 0;
}

/* Destination Server Setup */
void setupDestination(socketWrapper* sockWrap, const char* destinationIP, int destinationPort) {
    /* Clear FailCheck */
    clearFailCheck(sockWrap);

    /* Clear out structure from any remnant data */
    memset(&(sockWrap->destinationServer), 0, sizeof(sockWrap->destinationServer));

    /* Destination IP Address */
    sockWrap->destinationServer.sin_addr.s_addr = inet_addr(destinationIP);

    /* Destination Port */
    sockWrap->destinationServer.sin_port        = htons(destinationPort);

    /* Type of IP Address */
    sockWrap->destinationServer.sin_family      = AF_INET;
}

/* Connect to destination */
int connectToDestination(socketWrapper* sockWrap) {
    /* Clear FailCheck */
    clearFailCheck(sockWrap);

    /* Connect to destination */
    sockWrap->failCheck = connect(sockWrap->clientSocket, (const struct sockaddr*)&sockWrap->destinationServer, sockWrap->socketSize);

    /* Error Checking */
    if (sockWrap->failCheck == -1) {
        return -1;
    }

    return 0;
}

/* Send Data */
int sendData(socketWrapper* sockWrap, char data[], long sizeOfData) {
    /* Clear FailCheck */
    clearFailCheck(sockWrap);

    /* Send data */
    sockWrap->failCheck = send(sockWrap->clientSocket, &data[0], sizeOfData, 0);

    /* Error Checking */
    if (sockWrap->failCheck == -1) {
        return -1;
    }

    return 0;
}

/* Recvieve Data */
int recieveData(socketWrapper* sockWrap) {
    /* Clear FailCheck */
    clearFailCheck(sockWrap);

    /* Clear buffer */
    //memset(&(sockWrap->recvBuffer), 0, sizeof(sockWrap->recvBuffer));

    /* Recieve data and copy into buffer */
    void* bufferTheBuffer = sockWrap->recvBuffer;
    sockWrap->failCheck   = recv(sockWrap->clientSocket, bufferTheBuffer, sockWrap->recvBufferLen, 0);

    /* Error checking */
    if (sockWrap->failCheck == -1) {
        return -1;
    }

    return 0;
}

/* Pull Data */
char* pullData(socketWrapper* sockWrap) {
    /* Return pointer */
    char* ptr_data;

    /* Copy point pointer to recvBuffer */
    ptr_data = sockWrap->recvBuffer;

    return ptr_data;
}

int bringClientOffline(socketWrapper* sockWrap) {
    /* Clear FailCheck */
    clearFailCheck(sockWrap);

    /* Close Client Socket */
    sockWrap->failCheck = close(sockWrap->clientSocket);

    /* Error Checking */
    if (sockWrap->failCheck == -1) {
        return -1;
    }

    return 0;
}