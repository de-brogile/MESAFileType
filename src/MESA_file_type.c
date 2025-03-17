#include "MESA_file_type.h"
#include <magic.h>
#include <pthread.h>
#include <curl/curl.h>
#include <dirent.h>
#include <sys/stat.h>

// 获取错误描述
const char* MESAFileGetErrorDescription(MESAFileError error) {
    switch (error) {
        case MESA_FILE_SUCCESS:
            return "Success";
        case MESA_FILE_ERR_INIT:
            return "Initialization failed";
        case MESA_FILE_ERR_INVALID_PARAM:
            return "Invalid parameter";
        case MESA_FILE_ERR_FILE_NOT_FOUND:
            return "File not found";
        case MESA_FILE_ERR_MAGIC_LOAD:
            return "Failed to load magic database";
        case MESA_FILE_ERR_INTERNAL:
            return "Internal error";
        default:
            return "Unknown error";
    }
}

