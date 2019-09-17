#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <memory.h>
#include "../include/config.h"

int cpu_limit = 1;
int get_cpu_limit(void) {
    return cpu_limit;
}

char* document_root = "";
char* get_document_root(void) {
    return document_root;
}

bool init_func_called = false;
pthread_mutex_t init_func_mutex = PTHREAD_MUTEX_INITIALIZER;
int conf_init(const char *conf_path) {
    pthread_mutex_lock(&init_func_mutex);
    if (init_func_called) {
        pthread_mutex_unlock(&init_func_mutex);
        return 0;
    }
    init_func_called = true;
    pthread_mutex_unlock(&init_func_mutex);

    FILE* conf_file = fopen(conf_path, "r");
    if (conf_file == NULL) {
        return -1;
    }

    //Govnocod detected

    bool num_cpu_inited = false;
    bool document_root_inited = false;

    const char* const key_cpu_limit = "cpu_limit \0";
    const char* const key_document_root = "document_root \0";

    char buffer[64];
    char* cursor = NULL;

    while (1) {
        if (num_cpu_inited && document_root_inited) break;
        if (!fgets(buffer, 64, conf_file)) return -1;

        cursor = strstr(buffer, key_cpu_limit);
        if (cursor) {
            cursor += strlen(key_cpu_limit);
            sscanf(cursor, "%d", &cpu_limit);
            num_cpu_inited = true;
            continue;
        }

        cursor = strstr(buffer, key_document_root);
        if (cursor) {
            cursor += strlen(key_document_root);
            sscanf(cursor, "%s", document_root);
            document_root_inited = true;
            continue;
        }
    }

    return 0;
}
