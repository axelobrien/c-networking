#include <stdio.h> // printf (REMOVE LATER!)
#include <stdlib.h> // calloc, free
#include <string.h> // memcpy
#include "helpers.h" // strcmp_between

int strcmp_between(const char *str1, const char *str2, size_t start, size_t end) {
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

