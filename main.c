#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h> // socket, bind, listen, accept
#include <arpa/inet.h> // htons
#include <fcntl.h> // open
#include <unistd.h> // read, write
#include <string.h>
#include <errno.h> // errno
#include "requests.h" // RequestType

#define PORT 8080

int main(void) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (socket_fd < 0) {
		puts("Error creating socket");
		return 1;
	}

	struct sockaddr_in socket_address = {};
	socklen_t addrlen = sizeof(socket_address);

	socket_address.sin_family = AF_INET;
	socket_address.sin_port = htons(PORT);
	socket_address.sin_addr.s_addr = INADDR_ANY;

    int bind_rc = bind(socket_fd, (struct sockaddr *)&socket_address, sizeof(socket_address));

	if (bind_rc < 0) {
		printf("Error binding socket to address: %d\n", errno);
		if (errno == EADDRINUSE) {
			puts("addr in use");
		}
		return 1;
	}

	int listen_rc = listen(socket_fd, 1);

	if (listen_rc < 0) {
		puts("Error listening on socket");
		return 1;
	}
	
	int page_fd = open("index.html", O_RDONLY);

	if (page_fd < 0) {
		puts("Error opening html");
		return 1;
	}

	char buf[0x1000];

	ssize_t bytes_read = read(page_fd, buf, 0x1000);

	if (bytes_read < 0) {
		puts("Error reading html");
		return 1;
	}

	char *header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n";
	int header_size = snprintf(NULL, 0, header, bytes_read);
	int response_size = header_size + bytes_read + 1;
	char *response = malloc(response_size);

	snprintf(response, header_size + 1, header, bytes_read);
	memcpy(response + header_size, buf, bytes_read);

	while (1) {
		int client_fd = accept(socket_fd, (struct sockaddr *)&socket_address, &addrlen);

		if (client_fd < 0) {
			puts("Error accepting client");
			return 1;
		}

		int child_pid = fork();

		if (child_pid == 0) {
			char read_buf[0x1001];
			int request_len = read(client_fd, read_buf, 0x1000);

			if (request_len > 0) {
				printf("--- REQUEST ---\n%s\n", read_buf);
			}

			enum RequestType req_type = parse_request((const char *)response);
			if (req_type == REQ_GET) {
				(void) write(client_fd, response, response_size);
			} else {
				(void) write(client_fd, "wtf", 4);
			}

			close(socket_fd);
			close(client_fd);
			exit(0);
		} else if (child_pid < 0) {
			puts("Error creating child process to handle request");
			return 1;
		}

		close(client_fd);

	}

	close(socket_fd);
	
	free(response);
	return 0;
}

