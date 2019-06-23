#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/socket.h>

struct fileData {
    char* ptr_fileData;

    long fileSize;
};

struct fileData readFile(const char* filename) {
    FILE *fp;

    fp = fopen(filename, "r");

    fseek(fp, 0, SEEK_END);

    long fileSize = ftell(fp);

    rewind(fp);

    char* fileBuffer = malloc(fileSize);

    fread(fileBuffer, sizeof(char), fileSize, fp);

    fclose(fp);

    struct fileData returnData;

    returnData.ptr_fileData = fileBuffer;

    returnData.fileSize = fileSize;

    return returnData;
}

int main() {

    /* Data about this server */
    struct sockaddr_in server;

    /* Data about client (presumably the data is copied into "client" when the server accepts a connection) */
    struct sockaddr_in client;

    /* Socket Holder */
    int serverSocket = 0;

    /* Fail checking */
    int failCheck = 0;

    /* Need socket size for accepting connections (apparently) */
    socklen_t socketSize = sizeof(struct sockaddr_in);            // socklen_t == __uint32_t == 4-byte unsigned int

    /* Zero out both structs */
    memset(&server, 0, sizeof(server));

    /* Server details */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port        = htons(1337);
    server.sin_family      = AF_INET;

    /* Define socket */
    serverSocket   = socket(PF_INET, SOCK_STREAM, 0);

    if (serverSocket == -1) {
        printf("Socket Creation Failed\n");
        return 0;
    }

    /* Copy information into socket, select socket for use */
    failCheck = bind(serverSocket, (struct sockaddr*)&server, sizeof(struct sockaddr));

    /* Start listening, allow for a connection queue of length 1 */
    int connectionQueueLength = 1;
    failCheck = listen(serverSocket, connectionQueueLength);

    /* A new socket is created once a connection is accepted */
    int connectedServerSocket = accept(serverSocket, (struct sockaddr*)&client, &socketSize);

    /* Start listen -> accept -> close -> listen loop */
    if (connectedServerSocket == -1) {
        printf("Connection with client failed\n");
        return 0;
    }

    /* Stack buffer for filename */
    char filename[64 + 1];

    char sendFile[11] = "Send File";

    while (connectedServerSocket) {
        printf("Incoming connection from IP Address: %s...\n", inet_ntoa(client.sin_addr));

        failCheck = send(connectedServerSocket, &sendFile[0], sizeof(sendFile), 0);

        if (failCheck == -1) {
            printf("Sending data failed\n");
            return 0;
        } else {
            printf("Sent command\n");
        }

        failCheck = recv(connectedServerSocket, filename, 64, 0);

        if (failCheck == -1) {
            printf("Filename Recv Failed\n");
            return 0;
        } else {
            printf("Recv'd filename\n");
        }

        strcpy(&filename[64], "\0");

        struct fileData fileData;

        fileData = readFile(&filename[0]);

        printf("Check: %lu", fileData.fileSize);

        failCheck = send(connectedServerSocket, fileData.ptr_fileData, fileData.fileSize, 0);

        if (failCheck == -1) {
            printf("File data sending failed\n");

            return 0;
        } else {
            printf("Sent file data\n");
        }

        failCheck = close(connectedServerSocket);

        if (failCheck == -1) {
            printf("Closing socket failed\n");
            return 0;
        } 

        connectedServerSocket = accept(serverSocket, (struct sockaddr*)&client, &socketSize);

        if (connectedServerSocket == -1) {
            printf("Connection with client failed\n");
            return 0;
        }
    }

    close(serverSocket);

    return 0;
}                                                                                                                                                                                                                                                                                                                         