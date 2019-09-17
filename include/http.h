#ifndef HIGHLOADSERVER_HTTP_H
#define HIGHLOADSERVER_HTTP_H

#include "config.h"
#include "file_system.h"

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

char* const STR_CONNECTION_CLOSE_HEADER = "Connection: close\r\n\0";
char* const STR_DEFAULT_DATE_HEADER = "Date: Thu, 1 Jan 1970 00:00:00 GMT\r\n\0";
char* const STR_SERVER_HEADER = "Server: "APP_NAME"/"VERSION"\r\n\0";
char* const STR_CONTENT_TYPE_HEADER = "Content-Type: \0";
char* const STR_CONTENT_LENGTH_HEADER = "Content-Length: \0";

struct http_header_t {
    char* text;
    size_t len;
    struct http_header_t* next;
};
#define HTTP_HEADER_INITIALIZER {NULL, 0, NULL}

struct http_body_t {
    char* text;
    size_t len;
};
#define HTTP_BODY_INITIALIZER {NULL, 0}

struct http_request_t {
    enum request_method_t method;
    char* URI;
    enum http_version_t http_version;
    struct http_header_t* headers;
}; //TODO make constructor and destructor?
#define HTTP_REQUEST_INITIALIZER {METHOD_UNDEFINED, NULL, VERSION_UNDEFINED, NULL}

enum http_state_t parse_http_request(char* req_str, struct http_request_t* req);

struct http_response_t {
    enum http_state_t code;
    enum http_version_t http_version;
    struct http_header_t* headers;
    struct file_t file_to_send;
};
#define HTTP_RESPONSE_INITIALIZER {STATE_UNDEFINED, VERSION_UNDEFINED, NULL, FILE_INITIALIZER}

struct http_response_t build_default_http_response(enum http_state_t code);

enum http_state_t build_http_response(struct http_request_t* req, struct http_response_t* resp);

#endif //HIGHLOADSERVER_HTTP_H
