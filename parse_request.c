#include <stdio.h> // printf (REMOVE LATER!)
#include <string.h> // strlen, strcmp
#include <stdlib.h> // calloc, free
#include <stdint.h> // uintptr_t
#include "requests.h" // RequestType
#include "helpers.h" // strcmp_between

static void handle_invalid(Request *dst) {
	dst->type = REQ_INVALID;
	dst->path = NULL;
	return;
}

/*
 * Determines what kind of request is passed in. 
 * req is the raw request as a string, must not be NULL
 */
int parse_request(const char *req, Request *dst) {
	if (dst == NULL || dst->path == NULL) {
		handle_invalid(dst);
		return -1;
	}

	size_t len = strlen(req) + 1;

	if (len < 6) {
		handle_invalid(dst);
		return 1;
	}

	if (strcmp_between(req, "GET /", 0, 5) == 0) {
		char *nextNewline = strstr(req + 5, " HTTP/1.1\r\n");
		if (nextNewline == NULL) {
			handle_invalid(dst);
			return 2;
		}
		
		uintptr_t pathLength = (uintptr_t)nextNewline - (uintptr_t)(req + 5);
		if (pathLength >= dst->max_size) {
			handle_invalid(dst);
			return 3;
		}

		size_t prefix_len = strlen(dst->path);

		(void) memcpy(dst->path + prefix_len, req + 5, pathLength);
		printf("ur mom %s\n", dst->path);
		dst->path[pathLength + prefix_len] = 0;
		dst->type = REQ_GET;
		return 0;
	}

	handle_invalid(dst);
	return 4;
}

int validate_path(Request *req) {
	char* current = req->path;
	while (*current != 0) {
		if (*current == '.' && current[1] == '.') {
				return -1;
		}
		current++;
	}
	return 0;
}

