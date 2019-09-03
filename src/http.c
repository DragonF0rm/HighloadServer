#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../include/http.h"
#include "../include/log.h"

/*ssize_t count_request_headers(const char* req_str) {
    if (req_str == NULL) {
        log(ERROR, "Invalid function arguments");
        return -1;
    }

    const char* delim = "\r\n";
    const int delim_len = 2;
    const char* cursor = req_str;
    const char* prev_cursor = NULL;
    ssize_t  headers_count = 0;

    while(1) {
        prev_cursor = cursor;
        cursor = strstr(cursor, delim);
        if(cursor == NULL || prev_cursor-cursor == delim_len) {
            break;
        }
        cursor += delim_len;
        headers_count++;
    }

    headers_count-=1; //There is a single CRLF between Request-Line and Headers
    return headers_count;
}*/

static void parse_http_req_method(char** req_str, struct http_request_t* req) {
    if (req_str == NULL || *req_str == NULL || req == NULL) {
        log(ERROR, "Invalid function arguments");
        return;
    }
    log(DEBUG, "parse_http_req_method: req_str <%s>", *req_str);

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
    //TODO parce percent-encoded uri
    if (req_str == NULL || *req_str == NULL || req == NULL) {
        log(ERROR, "Invalid function arguments");
        return;
    }
    log(DEBUG, "parse_http_req_uri: req_str <%s>", *req_str);

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
    log(DEBUG, "parse_http_req_proto_ver, req_str: <%s>", *req_str);

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

static void parse_http_req_headers(char* req_str, struct http_request_t* req) {
    //TODO develop?
}

int parse_http_request(char* req_str, struct http_request_t* req) {
    if (req_str == NULL || req ==NULL) {
        log(ERROR, "Invalid function arguments");
        return -1;
    }

    char* cursor = req_str;
    while (*cursor == '\r' || *cursor == '\n') {
        cursor += 1;
    }
    if (*cursor == '\0') {
        log(INFO, "Skipped empty req_str while parsing request");
        return -1;
    }

    parse_http_req_method(&cursor, req);
    if (req->method == METHOD_UNDEFINED) {
        return METHOD_NOT_ALLOWED;
    }

    parse_http_req_uri(&cursor, req);
    if (req->URI == NULL) {
        log(DEBUG, "parse_http_request returning BAD_REQUEST, URI==NULL");
        return BAD_REQUEST;
    }

    parse_http_req_proto_ver(&cursor, req);
    if (req->http_version == VERSION_UNDEFINED) {
        log(DEBUG, "parse_http_request returning BAD_REQUEST, HTTP_VERSION==VERSION_UNDEFINED");
        return BAD_REQUEST;
    }

    //parse_http_req_headers(&cursor, req);

    return 0;
}

size_t get_date_header(char* buffer, size_t len) {
    if (buffer == NULL) {
        log(ERROR, "Invalid function arguments");
        return 0;
    }
    time_t rawtime; //timestamp
    if (time(&rawtime) < 0) {
        log(ERROR, "Unable to get calendar time");
        return 0;
    }

    struct tm *timeinfo = localtime(&rawtime); //datetime
    if (timeinfo == NULL) {
        log(ERROR, "Unable to parse rowtime into timeinfo");
        return 0;
    }

    size_t header_len = strftime(buffer, len, "Date: %a, %d %b %Y %X %Z\r\n", timeinfo);
    if (header_len == 0) {
        log(ERROR, "Can not build Date header: buffer is too small");
        return 0;
    }

    return header_len;
}
