#include <stdlib.h>

enum RequestType {
	REQ_INVALID,
	REQ_GET
};

typedef struct {
	enum RequestType type;
	char *path;
	size_t max_size;
} Request;

int parse_request(const char* req, Request * dst);

int validate_path(Request *req);
