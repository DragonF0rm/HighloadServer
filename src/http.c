#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "../include/http.h"
#include "../include/log.h"

static void parse_http_req_method(char** req_str, struct http_request_t* req) {
    if (req_str == NULL || *req_str == NULL || req == NULL) {
        log(ERROR, "Invalid function arguments");
        return;
    }

    size_t get_len = strlen(STR_GET);
    if (strncmp(*req_str, STR_GET, get_len) == 0) {
        req->method = GET;
        *req_str += get_len + 1; // len of GET + single space
        return;
    }
    size_t head_len = strlen(STR_HEAD);
    if (strncmp(*req_str, STR_HEAD, head_len) == 0) {
        req->method = HEAD;
        *req_str += head_len + 1; //len of HEAD + single space
        return;
    }
    req->method = METHOD_UNDEFINED;
}

static void parse_http_req_uri(char** req_str, struct http_request_t* req) {
    //TODO parse percent-encoded uri
    //TODO parse URI correctly
    if (req_str == NULL || *req_str == NULL || req == NULL) {
        log(ERROR, "Invalid function arguments");
        return;
    }

    req->URI = *req_str;
    *req_str = strchr(*req_str, ' ');
    if (*req_str == NULL) {
        req->URI = NULL;
        return;
    }
    **req_str = '\0';
    *req_str += 1;
}

static void parse_http_req_proto_ver(char** req_str, struct http_request_t* req) {
    if (req_str == NULL || *req_str == NULL || req == NULL) {
        log(ERROR, "Invalid function arguments");
        return;
    }

    size_t http_v1_0_len = strlen(STR_HTTPv1_0);
    if (strncmp(*req_str, STR_HTTPv1_0, http_v1_0_len) == 0) {
        req->http_version=HTTPv1_0;
        *req_str += http_v1_0_len + 2; // len of HTTP/1.0 + \r\n
        return;
    }
    size_t http_v1_1_len = strlen(STR_HTTPv1_0);
    if (strncmp(*req_str, STR_HTTPv1_1, http_v1_1_len) == 0) {
        req->http_version=HTTPv1_1;
        *req_str += http_v1_1_len + 2; // len of HTTP/1.1 + \r\n
        return;
    }
    req->http_version = VERSION_UNDEFINED;
}

static void parse_http_req_headers(char** req_str, struct http_request_t* req) {
    if (req_str == NULL || *req_str == NULL || req == NULL) {
        log(ERROR, "Invalid function arguments");
        return;
    }

    struct http_header_t** header_ptr = &req->headers;
    const char* delim = "\r\n";
    const int delim_len = 2;
    char** cursor = req_str;
    char** prev_cursor = NULL;
    size_t header_len = 0;

    while(1) {
        struct http_header_t header = HTTP_HEADER_INITIALIZER;
        *header_ptr = &header;
        (*header_ptr)->text = *cursor;
        prev_cursor = cursor;
        *cursor = strstr(*cursor, delim);
        header_len = *prev_cursor - *cursor;
        if(header_len == delim_len) {
            *cursor += delim_len;
            break;
        }
        if (*cursor == NULL) {
            log(ERROR, "Can't parse headers: empty line does not reached");
            req->headers = NULL;
            return;
        }
        (*header_ptr)->len = header_len + delim_len; //CRLF is a part of a header too
        *cursor += delim_len;
        header_ptr = &(*header_ptr)->next;
    }
}

/*static void parse_http_req_body(char** req_str, struct http_request_t* req) {
    if (req_str == NULL || *req_str == NULL || req == NULL || req->headers == NULL) {
        log(ERROR, "Invalid function arguments");
        return;
    }
    //TODO develop?
}*/

static char* request_method_t_to_string(enum request_method_t method) {
    switch (method) {
        case HEAD: {
            return STR_HEAD;
        }
        case GET: {
            return STR_GET;
        }
        default: {
            return "METHOD_UNDEFINED\0";
        }
    }
}

static char* http_version_t_to_string(enum http_version_t version) {
    switch (version) {
        case HTTPv1_0: {
            return STR_HTTPv1_0;
        }
        case HTTPv1_1: {
            return STR_HTTPv1_1;
        }
        default: {
            return "VERSION_UNDEFINED";
        }
    }
}

enum http_state_t parse_http_request(char* req_str, struct http_request_t* req) {
    if (req_str == NULL || req ==NULL) {
        log(ERROR, "Invalid function arguments");
        return INTERNAL_SERVER_ERROR;
    }

    char* cursor = req_str;
    while (*cursor == '\r' || *cursor == '\n') {
        cursor += 1;
    }
    if (*cursor == '\0') {
        log(INFO, "Skipped empty req_str while parsing request");
        return BAD_REQUEST;
    }

    parse_http_req_method(&cursor, req);
    log(DEBUG, "HTTP request method parsed: %s", request_method_t_to_string(req->method));
    if (req->method == METHOD_UNDEFINED) {
        return METHOD_NOT_ALLOWED;
    }

    parse_http_req_uri(&cursor, req);
    log(DEBUG, "HTTP request URI parsed: %s", req->URI);
    if (req->URI == NULL) {
        log(DEBUG, "parse_http_request returning BAD_REQUEST, URI==NULL");
        return BAD_REQUEST;
    }

    parse_http_req_proto_ver(&cursor, req);
    log(DEBUG, "HTTP request protocol version parsed: %s", http_version_t_to_string(req->http_version));
    if (req->http_version == VERSION_UNDEFINED) {
        log(DEBUG, "parse_http_request returning BAD_REQUEST, HTTP_VERSION==VERSION_UNDEFINED");
        return BAD_REQUEST;
    }

    parse_http_req_headers(&cursor, req);
    log(DEBUG, "HTTP headers parsed:");
#ifdef DEBUG_MODE
    struct http_header_t* header_cursor = req->headers;
    while (header_cursor != NULL) {
        log(DEBUG, "%.*s", header_cursor->len, header_cursor->text);
        header_cursor = header_cursor->next;
    }
#endif
    if (req->headers == NULL) {
        log(DEBUG, "parse_http_request returning BAD_REQUEST, unable to parse headers");
        return BAD_REQUEST;
    }

    return OK;
}

static int build_date_header(struct http_header_t* header) {
    if (header == NULL) {
        log(ERROR, "Invalid function arguments");
        return -1;
    }

    const size_t buffer_len = 64; //TODO put in config??
    char buffer[buffer_len];

    time_t raw_time; //timestamp
    if (time(&raw_time) < 0) {
        log(ERROR, "Unable to get calendar time");
        return -1;
    }

    struct tm* time_info = localtime(&raw_time); //datetime
    if (time_info == NULL) {
        log(ERROR, "Unable to parse raw_time into time_info");
        return -1;
    }

    size_t header_len = strftime(buffer, buffer_len, "Date: %a, %d %b %Y %X %Z\r\n", time_info);
    if (header_len == 0) {
        log(ERROR, "Can not build Date header: buffer is too small");
        return -1;
    }

    header->text = buffer;
    header->len = strlen(buffer);
    header->next = NULL;
    return 0;
}

static int build_content_length_header(struct http_header_t* header, int64_t content_len) {
    if (header == NULL) {
        log(ERROR, "Invalid function arguments");
        return -1;
    }
    const size_t buffer_len = 64; //TODO put in config??
    char buffer[buffer_len];
    strcpy(buffer, STR_CONTENT_LENGTH_HEADER);
    sprintf(buffer + strlen(buffer), "%lu\r\n", content_len);
    header->text = buffer;
    header->len = strlen(buffer);
    header->next = NULL;
    return 0;
}

static int build_content_type_header(struct http_header_t* header, enum mime_t mime_type) {
    if (header == NULL) {
        log(ERROR, "Invalid function arguments");
        return -1;
    }
    const size_t buffer_len = 64; //TODO put in config??
    char buffer[buffer_len];
    strcpy(buffer, STR_CONTENT_TYPE_HEADER);
    strcat(buffer, mime_type_to_str(mime_type));
    strcat(buffer, "\r\n\0");
    header->text = buffer;
    header->len = strlen(buffer);
    header->next = NULL;
    return 0;
}

struct http_response_t build_default_http_response(enum http_state_t code) {
    switch (code) {
        // NOTE: there are no default response for 200 OK
        case BAD_REQUEST: {
            break;
        }
        case FORBIDDEN: {
            break;
        }
        case NOT_FOUND: {
            break;
        }
        case METHOD_NOT_ALLOWED: {
            break;
        }
        case INTERNAL_SERVER_ERROR: {
            break;
        }
        default: {
            code = INTERNAL_SERVER_ERROR;
            break;
        }
    }

    struct http_header_t header_list = HTTP_HEADER_INITIALIZER;
    struct http_header_t* header_ptr = &header_list;

    *header_ptr = (struct http_header_t){
            STR_CONNECTION_CLOSE_HEADER,
            strlen(STR_CONNECTION_CLOSE_HEADER),
            NULL
    };
    header_ptr = header_ptr->next;

    struct http_header_t date_header = HTTP_HEADER_INITIALIZER;
    int build_result = build_date_header(&date_header);
    if (build_result < 0) {
        log(ERROR, "Unable to build Date header");
        *header_ptr = (struct http_header_t){
                STR_DEFAULT_DATE_HEADER,
                strlen(STR_DEFAULT_DATE_HEADER),
                NULL
        };
    } else {
        *header_ptr = date_header;
    }
    header_ptr = header_ptr->next;

    *header_ptr = (struct http_header_t){
            STR_SERVER_HEADER,
            strlen(STR_SERVER_HEADER),
            NULL
    };
    header_ptr = header_ptr->next;

    struct http_response_t resp = HTTP_RESPONSE_INITIALIZER;
    resp.code = code;
    resp.http_version = HTTPv1_1; //TODO put in the config???
    resp.headers = &header_list;

    return resp;
}

enum http_state_t build_http_response(struct http_request_t* req, struct http_response_t* resp) {
    struct http_header_t header_list = HTTP_HEADER_INITIALIZER;
    struct http_header_t* header_ptr = &header_list;

    *header_ptr = (struct http_header_t){
            STR_CONNECTION_CLOSE_HEADER,
            strlen(STR_CONNECTION_CLOSE_HEADER),
            NULL
    };
    header_ptr = header_ptr->next;

    struct http_header_t date_header = HTTP_HEADER_INITIALIZER;
    int build_result = build_date_header(&date_header);
    if (build_result < 0) {
        log(ERROR, "Unable to build Date header");
        *header_ptr = (struct http_header_t){
                STR_DEFAULT_DATE_HEADER,
                strlen(STR_DEFAULT_DATE_HEADER),
                NULL
        };
    } else {
        *header_ptr = date_header;
    }
    header_ptr = header_ptr->next;

    bool should_get_fd = req->method==GET;
    struct file_t file_to_send = FILE_INITIALIZER;
    enum file_state_t inspect_result = inspect_file(req->URI, &file_to_send, should_get_fd);
    switch (inspect_result) {
        case FILE_STATE_OK: {
            break;
        }
        case FILE_STATE_NOT_FOUND: {
            return NOT_FOUND;
        }
        case FILE_STATE_FORBIDDEN: {
            return FORBIDDEN;
        }
        case FILE_STATE_INTERNAL_ERROR: {
            return INTERNAL_SERVER_ERROR;
        }
        default: {
            log(ERROR, "Unknown return code: %d", inspect_result);
            return INTERNAL_SERVER_ERROR;
        }
    }

    struct http_header_t content_length_header = HTTP_HEADER_INITIALIZER;
    build_result = build_content_length_header(&content_length_header, file_to_send.len);
    if (build_result < 0) {
        log(WARNING, "Unable to build Content-Length header");
    } else {
        *header_ptr = content_length_header;
        header_ptr = header_ptr -> next;
    }

    struct http_header_t content_type_header = HTTP_HEADER_INITIALIZER;
    build_result = build_content_type_header(&content_type_header, file_to_send.mime_type);
    if (build_result) {
        log(WARNING, "Unable to build Content-Type header");
    } else {
        *header_ptr = content_type_header;
        header_ptr = header_ptr -> next;
    }

    *header_ptr = (struct http_header_t){
            STR_SERVER_HEADER,
            strlen(STR_SERVER_HEADER),
            NULL
    };
    header_ptr = header_ptr->next;

    resp->code = OK;
    resp->http_version = req->http_version;
    resp->headers = &header_list;
    resp->file_to_send = file_to_send;
    return OK;
}
