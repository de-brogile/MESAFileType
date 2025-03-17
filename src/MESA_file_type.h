#ifndef MESA_FILE_TYPE_H
#define MESA_FILE_TYPE_H

#include <stdio.h>

// 定义文件类型枚举
typedef enum {
    MESA_FILE_TYPE_UNKNOWN = 0,
    MESA_FILE_TYPE_TEXT,
    MESA_FILE_TYPE_BINARY,
    MESA_FILE_TYPE_IMAGE,
    MESA_FILE_TYPE_IMAGE_PNG,
    MESA_FILE_TYPE_IMAGE_JPG,
    MESA_FILE_TYPE_IMAGE_GIF,
    MESA_FILE_TYPE_AUDIO,
    MESA_FILE_TYPE_VIDEO,
    MESA_FILE_TYPE_ARCHIVE,
    MESA_FILE_TYPE_DOCUMENT,
    MESA_FILE_TYPE_EXECUTABLE,
    MESA_FILE_TYPE_DIRECTORY,
    MESA_FILE_TYPE_SYMLINK,
    MESA_FILE_TYPE_MAX
} MESAFileType;

// 定义错误码
typedef enum {
    MESA_FILE_SUCCESS = 0,
    MESA_FILE_ERR_INIT,
    MESA_FILE_ERR_INVALID_PARAM,
    MESA_FILE_ERR_FILE_NOT_FOUND,
    MESA_FILE_ERR_MAGIC_LOAD,
    MESA_FILE_ERR_INTERNAL,
    MESA_FILE_ERR_MAX
} MESAFileError;

// 线程数据结构
typedef struct {
    const char* filePath;
    const char* url;
    MESAFileType* result;
    MESAFileError* error;
    void* internal;
} IdentifyThreadData;



// 获取错误描述
const char* MESAFileGetErrorDescription(MESAFileError error);

// 前端识别：通过URL后缀名识别
MESAFileType MESAFileIdentifyByUrl(const char* url, MESAFileError* error);

// 前端识别：通过HTTP响应的Content-Type字段识别
MESAFileType MESAFileIdentifyByHttpContentType(const char* url, MESAFileError* error);


// 后端识别：基于libmagic识别单个文件类型
MESAFileType MESAFileIdentifyByLibmagic(const char* filePath, MESAFileError* error);

// // 后端识别：基于libmagic识别多个文件类型（多线程）
// MESAFileType* MESAFileIdentifyMultipleByLibmagic(const char** filePaths, int fileCount, MESAFileError** errors);




// 综合识别：结合前端和后端识别方式
MESAFileType MESAFileIdentifyCombined(const char* filePath, const char* url, MESAFileError* error);



//功能性函数
// 下载并识别URL指向的文件
MESAFileType MESAFileDownloadAndIdentify(const char* url, MESAFileError* error);

// 识别目录中的所有文件
MESAFileType* MESAFileIdentifyDirectory(const char* dirPath, MESAFileError** errors);

// 识别URL列表文件中的所有URL
MESAFileType* MESAFileIdentifyMultipleUrls(const char* urlsFilePath, MESAFileError** errors);

// 线程函数：识别单个文件类型
void* identify_file_type_thread(void* arg);


#endif // MESA_FILE_TYPE_H