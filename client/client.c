#include "client.h"

int main() {
    /* Filename to download */
    char filename[50] = "stuff.txt";

    /* TODO: Socket Wrapper */

    /* Local Fail Check Initalization */
    int failCheck = 0;

    /* Socket Wrapper Structure Initalization */
    socketWrapper sockWrapper;

    /* Socket Wrapper Structure Setup */
    failCheck = setupSocketWrapper(&sockWrapper);

    /* Error Checking */
    if (failCheck == -1) {
        printf("[-] Error with Socket Wrapper Initalization\n");
        return 0;
    }

    /* Socket Wrapper Destination Definition */
    setupDestination(&sockWrapper, "127.0.0.1", 1337);

    /* Connect to server */
    failCheck = connectToDestination(&sockWrapper);

    /* Error checking */
    if (failCheck == -1) {
        printf("[-] Connection Failed\n");
        return 0;
    }

    /* Recieve Data */
    failCheck = recieveData(&sockWrapper);

    /* Error checking */
    if (failCheck == -1) {
        printf("[-] Data Recv Failed\n");
        return 0;
    }

    /* Pull data */
    char* data = pullData(&sockWrapper);

    printf("Command Recieved: %s\n", sockWrapper.recvBuffer);

    if (strcmp(data, "Send File Name") == 0) {
        printf("CHECK DATA: %s", filename);
        failCheck = sendData(&sockWrapper, filename, sizeof(filename));

        if (failCheck == -1) {
            printf("[-] Data Send Failed\n");
            return 0;
        } else {
            printf("[+] Sent filename\n");
        }

        /* Recieve Data */
        failCheck = recieveData(&sockWrapper);

        /* Error checking */
        if (failCheck == -1) {
            printf("[-] Data Recv Failed\n");
            return 0;
        } else {
            /* Recieve file data */
            data = pullData(&sockWrapper);
            printf("[+] Recv'd file data: %s\n", data);

            if (strcmp(data, "No File Found") == 0) {
                printf("Error: File wasn't found\n");
            } else {
                writeFile(&filename[0], data, failCheck);
            }
        }
    } else {
        printf("No send file command\n");
    }

    /* Close socket */
    bringClientOffline(&sockWrapper);

    return 0;
}