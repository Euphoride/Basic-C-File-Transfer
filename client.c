#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/types.h>

void writeFile(const char* filename, const char* data, unsigned long fileSize) {
    printf("beeep: %lu", fileSize);

    FILE *fp;

    fp = fopen(filename, "w");

    fwrite(data, 1, fileSize, fp);

    fclose(fp);
}

int main() {
    /* Filename to download */
    char filename[50] = "server.c";

    /* Socket Holder */
    int clientSocket       = 0;
    int failCheck          = 0;

    /* Recv'd Data buffer size */
    int recvDataBufferSize = 4096;

    /* Character Buffer for Recieving data (+ 1 for null terminator) */
    char recvDataBuffer[recvDataBufferSize + 1];

    /* C needs a struct with all the destination's details */
    struct sockaddr_in destination;

    /* Define a socket */
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);

    /* Error checking */
    if (clientSocket == -1) {
        printf("Socket Creation Failed\n");
        return 0;
    }

    /* Zero out the entire struct */
    memset(&destination, 0, sizeof(destination));

    /* Details */
    destination.sin_addr.s_addr = inet_addr("127.0.0.1");
    destination.sin_port        = htons(1337);
    destination.sin_family      = AF_INET;

    /* Connect to server */
    failCheck = connect(clientSocket, (const struct sockaddr*)&destination, sizeof(destination));

    /* Error checking */
    if (failCheck == -1) {
        printf("Connection Failed\n");
        return 0;
    }

    /* Recieve Data */
    failCheck = recv(clientSocket, &recvDataBuffer[0], recvDataBufferSize + 1, 0);

    /* Error checking */
    if (failCheck == -1) {
        printf("Data Recv Failed\n");
        return 0;
    }

    printf("Uh: %s\n", recvDataBuffer);

    if (strcmp(recvDataBuffer, "Send File") == 0) {
        failCheck = send(clientSocket, &filename[0], sizeof(filename) - 1, 0);

        if (failCheck == -1) {
            printf("Data Send Failed\n");
            return 0;
        } else {
            printf("Sent filename\n");
        }

        /* Recieve Data */
        failCheck = recv(clientSocket, &recvDataBuffer, recvDataBufferSize, 0);

        /* Error checking */
        if (failCheck == -1) {
            printf("Data Recv Failed\n");
            return 0;
        } else {
            printf("Recv'd file data: %s\n", recvDataBuffer);
        }

        writeFile(&filename[0], &recvDataBuffer[0], recvDataBufferSize);

    } else {
        printf("No send file command\n");
    }

    /* Close socket */
    close(clientSocket);

    return 0;
}