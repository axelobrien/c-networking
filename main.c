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
#define MAX_PATH 100

int main(void) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (socket_fd < 0) {
		(void) puts("Error creating socket");
		return EXIT_FAILURE;
	}

	struct sockaddr_in socket_address = {0};
	socklen_t addrlen = sizeof(socket_address);

	socket_address.sin_family = AF_INET;
	socket_address.sin_port = htons(PORT);
	socket_address.sin_addr.s_addr = INADDR_ANY;

	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
		(void) puts("error setting socket options");
		return EXIT_FAILURE;
	}

	if (bind(socket_fd, (struct sockaddr *)&socket_address, sizeof(socket_address)) < 0) {
		printf("Error binding socket to address: %d\n", errno);
		if (errno == EADDRINUSE) {
			(void) puts("addr in use");
		}
		return EXIT_FAILURE;
	}

	if (listen(socket_fd, 1) < 0) {
		(void) puts("Error listening on socket");
		return EXIT_FAILURE;
	}

	while (1) {
		int client_fd = accept(socket_fd, (struct sockaddr *)&socket_address, &addrlen);

		if (client_fd < 0) {
			(void) puts("Error accepting client");
			return EXIT_FAILURE;
		}

		int child_pid = fork();

		if (child_pid == 0) {
			char read_buf[0x1001];
			ssize_t request_len = read(client_fd, read_buf, 0x1000);

			if (request_len > 0) {
				(void) printf("--- REQUEST ---\n%s\n", read_buf);
			}

			Request req_type = {0};
			char path_buf[MAX_PATH];
			req_type.path = path_buf;
			memcpy(req_type.path, "./public/", strlen("./public/"));
			req_type.max_size = MAX_PATH - strlen("./public/");

			int parse_rc = parse_request((const char *)read_buf, &req_type);
		
			if (parse_rc != 0) {
				printf("Error parsing request: %d\n", parse_rc);
				return EXIT_FAILURE;
			}

			if (req_type.type == REQ_GET) {
				printf("PATH: %s\n", req_type.path);

				if (strcmp(req_type.path + 9, "") == 0) {
					strcat(req_type.path, "index.html");
				}

				if (validate_path(&req_type) != 0) {
					write(client_fd, "404", 3);
					return EXIT_FAILURE;
				}

				int page_fd = open(req_type.path, O_RDONLY);

				if (page_fd < 0) {
					(void) puts("Error opening file");
					(void) write(client_fd, "404", 3);
					return EXIT_FAILURE;
				}

				char buf[0x1000];

				ssize_t bytes_read = read(page_fd, buf, 0x1000);

				if (bytes_read < 0) {
					(void) puts("Error reading html");
					return EXIT_FAILURE;
				}

				char *header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n";
				int header_size = snprintf(NULL, 0, header, bytes_read);

				if (header_size < 0) {
					(void) puts("error getting size of header");
					return EXIT_FAILURE;
				}

				size_t response_size = (size_t)header_size + (size_t)bytes_read + 1;
				char *response = malloc(response_size);

				snprintf(response, (size_t)header_size + 1, header, (size_t)bytes_read);
				memcpy(response + header_size, buf, (size_t)bytes_read);


				(void) write(client_fd, response, response_size);
				(void) free(response);
			} else {
				(void) write(client_fd, "wtf", 3);
			}

			(void) close(socket_fd);
			(void) close(client_fd);
			(void) exit(0);
		} else if (child_pid < 0) {
			(void) puts("Error creating child process to handle request");
			return EXIT_FAILURE;
		}

		(void) close(client_fd);
	}

	(void) close(socket_fd);
	return 0;
}

