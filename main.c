#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h> // socket, bind, listen, accept
#include <arpa/inet.h> // htons
#include <fcntl.h> // open
#include <unistd.h> // read, write
#include <string.h>

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
		puts("Error binding socket to address");
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

		(void) write(client_fd, response, response_size);

		close(client_fd);
	}

	return 0;
}

