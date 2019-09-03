#ifndef HIGHLOADSERVER_CONFIG_H
#define HIGHLOADSERVER_CONFIG_H

//Ручки, заданные через булевы переменные проверяются через #ifdef
//Для отключения просто закомментировать

//General settings
#define VERSION "1.0"
#define APP_NAME "HighloadServer"
#define DEBUG_MODE
#define DEFAULT_PORT 8000

//Logger settings
#define LOG_LEVEL 1 //0 = DEBUG ... 5 = FATAL
#define DO_COLOR_LOG
//#define LOG_FULL_FILE_PATH

#endif //HIGHLOADSERVER_CONFIG_H
