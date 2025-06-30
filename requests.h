enum RequestType {
	REQ_INVALID,
	REQ_GET
};

enum RequestType parse_request(const char* req);
