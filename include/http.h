#ifndef HIGHLOADSERVER_HTTP_H
#define HIGHLOADSERVER_HTTP_H

#include "config.h"

enum request_method_t {
    METHOD_UNDEFINED,
    HEAD,
    GET
};
const char* const STR_HEAD = "HEAD\0";
const char* const STR_GET = "GET\0";

enum http_version_t {
    VERSION_UNDEFINED,
    HTTPv1_0,
    HTTPv1_1
};
const char* const STR_HTTPv1_0 = "HTTP/1.0\0";
const char* const STR_HTTPv1_1 = "HTTP/1.1\0";

struct http_request_t {
    enum request_method_t method;
    char* URI;
    enum http_version_t http_version;
};
#define HTTP_REQUEST_INITIALIZER {METHOD_UNDEFINED, NULL, VERSION_UNDEFINED}

enum http_header_t {
    HEADER_UNDEFINED
};
const char* const STR_HEADER_SERVER = "Server: "APP_NAME"/"VERSION"\r\n\0";

size_t get_date_header(char* buffer, size_t len);

enum http_state_t {
    STATE_UNDEFINED = 0,
    OK = 200,
    BAD_REQUEST = 400,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    INTERNAL_SERVER_ERROR = 500
};
const char* const STR_200_OK = "200 OK\0";
const char* const STR_400_BAD_REQUEST = "400 Bad Request\0";
const char* const STR_403_FORBIDDEN = "403 Forbidden\0";
const char* const STR_404_NOT_FOUND = "404 Not Found\0";
const char* const STR_405_METHOD_NOT_ALLOWED = "405 Method Not Allowed\0";
const char* const STR_500_INTERNAL_SERVER_ERROR = "500 Internal Server Error\0";

int parse_http_request(char* req_str, struct http_request_t* req); /*
 * Returning values:
 * 0   - Request was parsed
 * -1  - Request was not parsed: req_str is empty
 * 400 - Request was partly parsed: should respond with BAD_REQUEST
 * 405 - Request was partly parsed: should respond with METHOD_NOT_ALLOWED
 * 500 - Request was not parsed: should respond with INTERNAL_SERVER_ERROR
 */

struct http_response_t {
    enum http_state_t code;
    enum http_version_t http_version;
};
#define HTTP_RESPONSE_INITIALIZER {STATE_UNDEFINED, VERSION_UNDEFINED};


#endif //HIGHLOADSERVER_HTTP_H
