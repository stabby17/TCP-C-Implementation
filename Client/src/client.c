#include "TCP_protocol.h"
#include "validation.h"

// OS specific support
#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

// Standard libraries
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

void errorhandler(char* errorMessage) { printf("%s", errorMessage); }

void clearWinsock() {
#if defined WIN32
    WSACleanup();
#endif
}

void input_prompt(char* input_string) {
    puts(
        "\nInsert the string containing data (max 64 char) ['=' to exit] "
        "\n(Format: \"operator[space]num1[space]num2\"):");
    // Read the string from the console
    fgets(input_string, BUFFER_SIZE, stdin);
    // Remove the newline character from the string
    input_string[strcspn(input_string, "\n")] = 0; 
}

int main(void) {
#if defined WIN32
    WSADATA wsa_data;
    WORD version_requested;

    version_requested = MAKEWORD(2, 2);
    int result = WSAStartup(version_requested, &wsa_data);

    if (result != NO_ERROR) {
        printf("Error at WSAStartup() \n");
        printf("A usable WinSock DLL cannot be found\n");
        return -1;
    }
#endif

    // Create a socket
    int c_socket;
    c_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Check for errors to ensure that the socket is a valid socket
    if (c_socket < 0) {
        errorhandler("Socket creation failed. \n");
        clearWinsock();
        return -1;
    }

    struct sockaddr_in sad;                         // Structure to hold an IP address
    memset(&sad, 0, sizeof(sad));                   // Zero out the structure

    sad.sin_family = AF_INET;                       // Set the address family to AF_INET (IPv4)
    sad.sin_addr.s_addr = inet_addr("127.0.0.1");   // Set the remote IP address (defined in LOCALHOST)
    sad.sin_port = htons(PROTOPORT);                // Set the remote port (defined in PROTOPORT)

    // Connect to server
    if (connect(c_socket, (struct sockaddr*)&sad, sizeof(sad)) < 0) {
        errorhandler("Failed to connect. \n");
        closesocket(c_socket);
        clearWinsock();
        return -1;
    }

    int bytes_rcvd;                 // Number of bytes received  
    char buf[BUFFER_SIZE];          // Buffer for data from the server
    memset(buf, 0, BUFFER_SIZE);    // Clear the buffer
    printf("Waiting for connection from the server...\n");

    // Receives data from the server using the specified client socket
    if ((bytes_rcvd = recv(c_socket, buf, BUFFER_SIZE - 1, 0)) <= 0) {
        errorhandler("recv() failed or connection closed prematurely");
        closesocket(c_socket);
        clearWinsock();
        return -1;
    }

    // Prints a welcome message from the server
    buf[bytes_rcvd] = '\0';
    printf("%s", buf);

    // Input of the string to be sent to the server
    char input_string[BUFFER_SIZE];
    memset(input_string, '\0', BUFFER_SIZE);
    input_prompt(input_string);

    // String format validation
    while (lenght_check(strlen(input_string)) || string_check(input_string))
    {
        input_prompt(input_string);
    }

    // Sending the string to the server
    int string_len = strlen(input_string);
    if (send(c_socket, input_string, string_len, 0) != string_len) {
        errorhandler("send() sent a different number of bytes than expected");
        closesocket(c_socket);
        clearWinsock();
        return -1;
    }
    char result_in[BUFFER_SIZE]; //Buffer to store the result received from the server

    /**
     * This code snippet represents a loop that sends and receives data over a TCP socket.
     * It continues sending data until the string "input_string" is equal to "=".
     *
     * Inside the loop, it receives a result from the server using the recv() function,
     * and prints the result to the console.
     *
     * Then, it prompts the user for input, checks the format of the input string,
     * and sends the string over the TCP socket using the send() function.
     *
     * If any error occurs during the send or receive operations, the function returns -1.
     *
     * @param c_socket The socket descriptor.
     * @param input_string The string to be sent.
     * @param result_in The buffer to store the received result.
     * @param string_len The length of the string to be sent.
     * @param bytes_rcvd The number of bytes received from the server.
     * @return -1 if an error occurred, otherwise 0.
     */
    while (strcmp(input_string, "=") != 0) {
        memset(result_in, '\0', BUFFER_SIZE);
        printf("Result: ");
        if ((bytes_rcvd = recv(c_socket, result_in, BUFFER_SIZE - 1, 0)) <= 0) {
            errorhandler("recv() failed or connection closed prematurely");
            closesocket(c_socket);
            clearWinsock();
            return -1;
        }
        printf("%s\n", result_in);

        memset(input_string, '\0', BUFFER_SIZE);

        do {
            input_prompt(input_string);
        } while (string_check(input_string));

        string_len = strlen(input_string);
        if (send(c_socket, input_string, string_len, 0) != string_len) {
            errorhandler("send() sent a different number of bytes than expected");
            closesocket(c_socket);
            clearWinsock();
            return -1;
        }
    }

    // Closing connection
    closesocket(c_socket);
    clearWinsock();

    return 0;
}