#include "server.h"


int main() {
    /* Main Connection Struct */
    socketWrapper sockWrapper;

    /* fail check setup */
    int failCheck = 0;
    
    /* Main setup */
    failCheck = setupSocketWrapper(&sockWrapper, 1337);

    /* Error checking */
    if (failCheck == -1) {
        printf("There was an error with socket creation");
        return 0;
    }


    failCheck = 0;

    /* bind -> listen -> accept -> drop back into main */
    failCheck = bringOnline(&sockWrapper, 1, (struct sockaddr*)&(sockWrapper.server));

    /* Error checking */
    switch (failCheck)
    {
        case -1:
            printf("Error with binding socket\n");
            return 0;

        case -2:
            printf("Error with listening to incomming connections\n");
            return 0;

        case -3:
            printf("Error with accepting incoming connection\n");
            return 0;

        default:
            break;
    }

    /* Main File Sending Algorithm */
    failCheck = clientFileSend(&sockWrapper);

    /* Error checking */
    switch (failCheck)
    {
        case -1:
            printf("[-] Sending data failed\n");
            return 0;

        case -2:
            printf("[-] Filename Recv Failed\n");
            return 0;

        case -3:
            printf("[-] File Reading Failed - File does not seem to exist\n");
            return 0;

        case -4:
            printf("[-] File data sending failed\n");
            return 0;

        case -5:
            printf("[-] Closing socket failed\n");
            return 0;
        default:
            break;
    }

    /* Close main server socket */
    bringOffline(&sockWrapper);

    return 0;
}
