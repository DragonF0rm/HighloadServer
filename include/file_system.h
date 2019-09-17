#ifndef HIGHLOADSERVER_FILE_SYSTEM_H
#define HIGHLOADSERVER_FILE_SYSTEM_H

#include <stdint-gcc.h>

enum mime_t {
    MIME_TYPE_APPLICATION_OCTET_STREAM,
    MIME_TYPE_TEXT_HTML,
    MIME_TYPE_TEXT_CSS,
    MIME_TYPE_APPLICATION_JAVASCRIPT,
    MIME_TYPE_IMAGE_JPEG,
    MIME_TYPE_IMAGE_PNG,
    MIME_TYPE_IMAGE_GIF,
    MIME_TYPE_APPLICATION_X_SHOCKWAVE_FLASH
};

char* const STR_MIME_APPLICATION_OCTET_STREAM = "application/octet-stream\0";
char* const STR_MIME_TEXT_HTML = "text/html\0";
char* const STR_MIME_TEXT_CSS = "text/css\0";
char* const STR_MIME_APPLICATION_JAVASCRIPT = "application/javascript\0";
char* const STR_MIME_IMAGE_JPEG = "image/jpeg\0";
char* const STR_MIME_IMAGE_PNG = "image/png\0";
char* const STR_MIME_IMAGE_GIF = "image/gif\0";
char* const STR_MIME_APPLICATION_X_SHOCKWAVE_FLASH = "application/x-shockwave-flash\0";

char* mime_type_to_str(enum mime_t mime_type);

const char* const EXT_HTML = ".html\0";
const char* const EXT_CSS = ".css\0";
const char* const EXT_JS = ".js\0";
const char* const EXT_JPG = ".jpg\0";
const char* const EXT_JPEG = ".jpeg\0";
const char* const EXT_PNG = ".png\0";
const char* const EXT_GIF = ".gif\0";
const char* const EXT_SWF = ".swf\0";

struct file_t {
    char* path;
    int64_t len;
    int fd;
    enum mime_t mime_type;
};
#define FILE_INITIALIZER {NULL, -1, -1, MIME_TYPE_APPLICATION_OCTET_STREAM}

enum file_state_t {
    FILE_STATE_INTERNAL_ERROR,
    FILE_STATE_FORBIDDEN,
    FILE_STATE_NOT_FOUND,
    FILE_STATE_OK
};

enum file_state_t inspect_file(char* path, struct file_t* file, bool should_get_fd);

#endif //HIGHLOADSERVER_FILE_SYSTEM_H
