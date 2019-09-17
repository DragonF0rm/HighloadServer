#ifndef HIGHLOADSERVER_CONFIG_H
#define HIGHLOADSERVER_CONFIG_H

//Ручки, заданные через булевы переменные проверяются через #ifdef
//Для отключения просто закомментировать

int conf_init(const char *conf_path); //Call once

//General settings
#define VERSION "1.0"
#define APP_NAME "HighloadServer"
#define DEBUG_MODE
#define DEFAULT_PORT 8000
int get_cpu_limit(void);

//Logger settings
#define LOG_LEVEL 1 //0 = DEBUG ... 5 = FATAL
#define DO_COLOR_LOG
//#define LOG_FULL_FILE_PATH

//FileSystem settings
char* get_document_root(void);

#endif //HIGHLOADSERVER_CONFIG_H
