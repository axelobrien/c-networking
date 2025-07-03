#include <stdio.h> // printf (REMOVE LATER!)
#include <string.h> // strlen, strcmp
#include <stdlib.h> // calloc, free
#include "requests.h" // RequestType

static int strcmp_between(const char *str1, const char *str2, size_t start, size_t end) {
	if (start > end) {
		size_t temp = start;
		start = end;
		end = temp;
	} else if (start == end) {
		end++;
	}

	size_t slice_size = end - start;

	char *slice_1 = calloc(slice_size, 1);
	char *slice_2 = calloc(slice_size, 1);

	if (strlen(str1) >= start) {
		memcpy(slice_1, str1, slice_size);
	}

	if (strlen(str2) >= start) {
		memcpy(slice_2, str2, slice_size);
	}

	printf("%s\n", slice_1);
	printf("%s\n", slice_2);

	int comparison = strcmp(slice_1, slice_2);

	free(slice_1);
	free(slice_2);
	return comparison;
}


/*
 * Determines what kind of request is passed in. 
 * req is the raw request as a string, must not be NULL
 */
enum RequestType parse_request(const char *req) {
	size_t len = strlen(req) + 1;

	if (len < 6) {
		return REQ_INVALID;
	}

	if (strcmp_between(req, "GET /", 0, 5) == 0) {
		return REQ_GET;
	}

	return REQ_INVALID;
}
