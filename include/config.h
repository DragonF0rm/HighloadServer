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
inline const int _get_cpu_limit();
#define CPU_LIMIT _get_cpu_limit()

//Logger settings
#define LOG_LEVEL 1 //0 = DEBUG ... 5 = FATAL
#define DO_COLOR_LOG
//#define LOG_FULL_FILE_PATH

//FileSystem settings
inline const char* const _get_document_root();
#define DOCUMENT_ROOT _get_document_root()

#endif //HIGHLOADSERVER_CONFIG_H
