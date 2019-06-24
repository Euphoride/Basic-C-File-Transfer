// TODO: try to fix binding error on line 115

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/socket.h>

/*
 * Structure containing a pointer to a heap buffer (which should contain the file data to be sent) and a long
 * containing the file size (for sake of telling send() how much data to send)
 */
struct fileData {
    char* ptr_fileData;

    long fileSize;
};

typedef struct fileData fileData;

/* Read file */
struct fileData readFile(const char* filename) {
    /* File pointer */
    FILE *fp;

    /* Open File */
    fp = fopen(filename, "r");

    /* If file doesn't exist */
    if (fp == NULL) {
        fileData returnData;

        /* Carry error message through fileSize */
        returnData.fileSize = -1;

        return returnData;
    }

    /* Place file pointer at the end of the file */
    fseek(fp, 0, SEEK_END);

    /* Fetch the position of the pointer relative to the file */
    long fileSize = ftell(fp);

    /* Rewind pointer back to start of file */
    rewind(fp);

    /* Define heap buffer with size of file and define pointer to it */
    char* fileBuffer = malloc(fileSize);

    /* Read into heap buffer */
    fread(fileBuffer, sizeof(char), fileSize, fp);

    /* Close file */
    fclose(fp);

    /* To return multiple variables without too much hassle, a structure is decent way to do this */
    fileData returnData;

    /* 
     * Set structure pointer's address to the same address pointed to by the fileBuffer pointer, so now 
     * returnData.ptr_fileData points to the heap buffer with the file data 
     */
    returnData.ptr_fileData = fileBuffer;

    /* Set file size */
    returnData.fileSize = fileSize;

    /* Return structure */
    return returnData;
}



/* Main structure for main socket work */
struct socketWrapper {
    struct sockaddr_in server;
    struct sockaddr_in currentClient;

    int connectionSocket;
    int serverSocket;
    int failCheck;

    socklen_t socketSize;
};

typedef struct socketWrapper socketWrapper;

/* Function to setup socketWrapper */
int setupSocketWrapper(socketWrapper* sockWrap, int portNum) {
    /* Reset server details */
    memset(&(sockWrap->server), 0, sizeof(sockWrap->server));

    /* Add new server details */
    sockWrap->server.sin_addr.s_addr = htonl(INADDR_ANY);
    sockWrap->server.sin_family      = PF_INET;
    sockWrap->server.sin_port        = htons(portNum);

    /* Socket definition */
    sockWrap->serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    /* Error checking */
    if (sockWrap->serverSocket == -1) {
        return -1;
    }

    /* Fail check initialization (after socket definition as the socket itself can act like a fail check) */
    sockWrap->failCheck = 0;

    /* Socket size is required for connections (apparently) */
    sockWrap->socketSize = sizeof(struct sockaddr_in);            // socklen_t == __uint32_t == 4-byte unsigned int

    return 0;
}

/* Function to begin a bind -> listen -> accept algorithm */
int bringOnline(socketWrapper* sockWrap, int queue, struct sockaddr* castedServerStruct) {
    /* Copy information into socket, select socket for use */
    sockWrap->failCheck = bind(sockWrap->serverSocket, castedServerStruct, sizeof(struct sockaddr));

    /* Error checking */
    if (sockWrap->failCheck == -1) {
        return -1;
    }

    /* Start listening, allow for a connection queue of length 1 */
    sockWrap->failCheck = listen(sockWrap->serverSocket, queue);

    /* Error checking */
    if (sockWrap->failCheck == -1) {
        return -2;
    }

    /* A new socket is created once a connection is accepted */
    sockWrap->connectionSocket = accept(sockWrap->serverSocket, (struct sockaddr*)&sockWrap->currentClient, &sockWrap->socketSize);

    /* Error checking */
    if (sockWrap->connectionSocket == -1) {
        return -3;
    } else {
        return 0;
    }
}

/* Take Incoming connection, find out file, fetch file, send file */
int clientFileSend(socketWrapper* sockWrap) {
    /* Stack buffer for filename */
    char filename[64 + 1];

    /* Send file command */
    char sendFile[15] = "Send File Name";

    /* Log Incomming connection */
    printf("[+] Incoming connection from IP Address: %s...\n", inet_ntoa(sockWrap->currentClient.sin_addr));

    /* Send the send file command */
    sockWrap->failCheck = send(sockWrap->connectionSocket, &sendFile[0], sizeof(sendFile), 0);

    /* Check for errors in sending */
    if (sockWrap->failCheck == -1) {
        return -1;
    } else {
        printf("[*] Sent 'Send File Name' command\n");
    }

    /* Recieve File Name */
    sockWrap->failCheck = recv(sockWrap->connectionSocket, filename, 64, 0);

    /* Check for errors in recv */
    if (sockWrap->failCheck == -1) {
        return -2;
    } else {
        printf("[*] Recv'd filename: %s\n", filename);
    }

    /* Null terminate the filename */
    strcpy(&filename[64], "\0");

    /* Define a structure in preparation for readFile() */
    struct fileData fileData;

    /* Read file */
    fileData = readFile(&filename[0]);

    /* Check for errors */
    int fileExists = 1;

    if (fileData.fileSize == -1) {
        fileExists = 0;
        printf("[-] Minimal Error: File does not exist\n");
    } else {
        printf("[*] Found file\n");
    }

    if (!fileExists) {
        fileData.ptr_fileData = malloc(15);
        fileData.fileSize = 15;

        strcpy(fileData.ptr_fileData, "No File Found");
        strcpy(&fileData.ptr_fileData[fileData.fileSize], "\0");
    }

    /* Send file data */
    sockWrap->failCheck = send(sockWrap->connectionSocket, fileData.ptr_fileData, fileData.fileSize, 0);

    /* Check for errors */
    if (sockWrap->failCheck == -1) {
        return -3;
    } else {
        printf("[*] Sent file data to client\n");
    }

    /* Close socket */
    sockWrap->failCheck = close(sockWrap->connectionSocket);

    /* Check for errors */
    if (sockWrap->failCheck == -1) {
        return -4;
    } else {
        printf("[*] Closing socket...\n");
        return 0;
    }
}

int bringOffline(socketWrapper* sockWrap) {
    sockWrap->failCheck = close(sockWrap->serverSocket);

    if (sockWrap->failCheck == -1) {
        return -1;
    }

    return 0;
}
