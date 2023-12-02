#include "TCP_protocol.h"
#include "calculation.h"

// OS specific support
#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

// Standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void errorhandler(char* errorMessage) { printf("%s", errorMessage); }

void clearWinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

int main(int argc, char* argv[]) {
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

	// Sets the port number based on the command line arguments or a default value.
	int port;
	if (argc > 1)
		port = atoi(argv[1]);
	else
		port = PROTOPORT;

	// Create a socket
	int my_socket;
	my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Check for errors to ensure that the socket is a valid socket
	if (my_socket < 0) {
		errorhandler("Socket creation failed. \n");
		clearWinsock();
		return -1;
	}

	struct sockaddr_in sad;                         // Structure to hold an IP address
    memset(&sad, 0, sizeof(sad));                   // Zero out the structure

	sad.sin_family = AF_INET;                       // Set the address family to AF_INET (IPv4)
    sad.sin_addr.s_addr = inet_addr("127.0.0.1");   // Set the remote IP address (defined in LOCALHOST)
    sad.sin_port = htons(port);                		// Set the remote port

	// Bind the socket to the local address
	if (bind(my_socket, (struct sockaddr*)&sad, sizeof(sad)) < 0) {
		errorhandler("bind() failed. \n");
		closesocket(my_socket);
		return -1;
	}

	// Specify size of request queue
	if (listen(my_socket, QLEN) < 0) {
		errorhandler("listen() failed. \n");
		closesocket(my_socket);
		clearWinsock();
		return -1;
	}

	/**
	 * @brief This code represents the server-side implementation of a TCP connection.
	 *        It waits for a client to connect and establishes a connection with the client.
	 *        It then sends a "Connection established" message to the client.
	 *        After that, it receives strings from the client, performs calculations based on the received strings,
	 *        and sends the results back to the client until the client sends the "=" string.
	 *        The server continues to wait for new clients and repeat the process indefinitely.
	 *
	 * @return Returns 0 on success, -1 on failure.
	 */

	struct sockaddr_in cad;
	int client_socket;
	int client_len;
	printf("Waiting for a client to connect... \n");
	while (1) {
		client_len = sizeof(cad);

		if ((client_socket = accept(my_socket, (struct sockaddr*)&cad, &client_len)) < 0) {
			errorhandler("accept() failed. \n");
			closesocket(my_socket);
			clearWinsock();
			return -1;
		}

		printf("\nConnection established with %s:%d\n", inet_ntoa(cad.sin_addr), ntohs(cad.sin_port));
		char* input_string = " Connection established\n";
		int string_len = strlen(input_string);
		if (send(client_socket, input_string, string_len, 0) != string_len)
		{
			errorhandler("send() sent a different number of bytes than expected\n");
			closesocket(client_socket);
			clearWinsock();
			return -1;
		}

		int bytes_rcvd;
		char str_in[BUFFER_SIZE]; 				// Buffer for data from the server
		memset(str_in, '\0', BUFFER_SIZE); 		// Ensures extra bytes contain '\0'
		printf("Received: "); 					// Setup to print the echoed string

		//String reception from server
		if ((bytes_rcvd = recv(client_socket, str_in, BUFFER_SIZE - 1, 0)) <= 0)
		{
			errorhandler("recv() failed or connection closed prematurely\n");
			closesocket(my_socket);
			clearWinsock();
			return -1;
		}
		str_in[bytes_rcvd] = '\0';
		printf("%s\n", str_in);

		/**
		 * This code snippet represents a loop that performs calculations based on the input received from a client.
		 * It receives a string from the client, parses it to extract two numbers and an operator, performs the calculation,
		 * and sends the result back to the client. The loop continues until the received string is equal to "=".
		 * 
		 * The loop performs the following steps:
		 * 1. Receives a string from the client.
		 * 2. Parses the string to extract two numbers and an operator.
		 * 3. Performs the calculation based on the extracted numbers and operator.
		 * 4. Converts the result to a string and sends it back to the client.
		 * 5. Receives a new string from the client.
		 * 6. Prints the received string.
		 * 7. Repeats the above steps until the received string is equal to "=".
		 * 
		 * @param client_socket The socket descriptor for the client connection.
		 * @param str_in The buffer to store the received string from the client.
		 * @param BUFFER_SIZE The size of the buffer.
		 * @return Returns 0 on success, -1 on failure.
		 */
		while (strcmp(str_in, "=") != 0) {
			int num1 = 0, num2 = 0; 
			char op, result_out[100];
			float result; 

			set_operation(str_in, &num1, &num2, &op);
			result = calculation(num1, num2, op);

			if (num2 == 0 && op == '/') {
				strcpy(result_out, "inf");
			} else {
				sprintf(result_out, "%.3f", result);
			}
			
			string_len = strlen(result_out);
			
			if (send(client_socket, result_out, string_len, 0) != string_len)
			{
				errorhandler("\nsend() sent a different number of bytes than expected\n");
				closesocket(client_socket);
				clearWinsock();
				return -1;
			}

			printf("Received: "); 
			if ((bytes_rcvd = recv(client_socket, str_in, BUFFER_SIZE - 1, 0)) <= 0)
			{
				errorhandler("\nrecv() failed or connection closed prematurely\n");
				closesocket(client_socket);
				clearWinsock();
				return -1;
			}
			str_in[bytes_rcvd] = '\0';
			printf("%s\n", str_in);
		}

	}
}