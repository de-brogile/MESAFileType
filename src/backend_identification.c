#include "MESA_file_type.h"
#include <magic.h>
#include <pthread.h>
#include<stdlib.h>
// 内部结构体，隐藏实现细节
typedef struct {
    magic_t magic_cookie;
    pthread_mutex_t mutex;
} MESAFileIdentifierInternal;

// 内部函数：初始化文件识别组件
static MESAFileIdentifierInternal* MESAFileIdentifierInitInternal() {
    MESAFileIdentifierInternal* internal = (MESAFileIdentifierInternal*)malloc(sizeof(MESAFileIdentifierInternal));
    if (!internal) {
        return NULL;
    }

    internal->magic_cookie = magic_open(MAGIC_MIME_TYPE);
    if (!internal->magic_cookie) {
        free(internal);
        return NULL;
    }

    if (magic_load(internal->magic_cookie, NULL) != 0) {
        magic_close(internal->magic_cookie);
        free(internal);
        return NULL;
    }

    if (pthread_mutex_init(&internal->mutex, NULL) != 0) {
        magic_close(internal->magic_cookie);
        free(internal);
        return NULL;
    }

    return internal;
}

// 后端识别：基于libmagic识别单个文件类型
MESAFileType MESAFileIdentifyByLibmagic(const char* filePath, MESAFileError* error);

// 内部函数：销毁文件识别组件
static void MESAFileIdentifierDestroyInternal(MESAFileIdentifierInternal* internal) {
    if (internal) {
        pthread_mutex_destroy(&internal->mutex);
        magic_close(internal->magic_cookie);
        free(internal);
    }
}

// 内部函数：基于libmagic识别单个文件类型
static MESAFileType MESAFileIdentifyByLibmagicInternal(MESAFileIdentifierInternal* internal, const char* filePath, MESAFileError* error) {
    if (!internal || !filePath) {
        if (error) {
            *error = MESA_FILE_ERR_INVALID_PARAM;
        }
        return MESA_FILE_TYPE_UNKNOWN;
    }
    
   
    pthread_mutex_lock(&internal->mutex);

    const char* mime_type = magic_file(internal->magic_cookie, filePath);
    MESAFileType file_type = MESA_FILE_TYPE_UNKNOWN;

    if (!mime_type) {
        if (error) {
            *error = MESA_FILE_ERR_INTERNAL;
        }
    } else {
        if (strstr(mime_type, "text/")) {
            file_type = MESA_FILE_TYPE_TEXT;
        } else if (strstr(mime_type, "application/octet-stream")) {
            file_type = MESA_FILE_TYPE_BINARY;
        } else if (strstr(mime_type, "image/png")) {
            file_type = MESA_FILE_TYPE_IMAGE_PNG;
        } else if (strstr(mime_type, "image/jpeg")) {
            file_type = MESA_FILE_TYPE_IMAGE_JPG;
        } else if (strstr(mime_type, "image/gif")) {
            file_type = MESA_FILE_TYPE_IMAGE_GIF;
        } else if (strstr(mime_type, "image/")) {
            file_type = MESA_FILE_TYPE_IMAGE;
        } else if (strstr(mime_type, "audio/")) {
            file_type = MESA_FILE_TYPE_AUDIO;
        } else if (strstr(mime_type, "video/")) {
            file_type = MESA_FILE_TYPE_VIDEO;
        } else if (strstr(mime_type, "application/zip") || strstr(mime_type, "application/x-tar")) {
            file_type = MESA_FILE_TYPE_ARCHIVE;
        } else if (strstr(mime_type, "application/pdf") || strstr(mime_type, "application/msword")) {
            file_type = MESA_FILE_TYPE_DOCUMENT;
        } else if (strstr(mime_type, "application/x-executable")) {
            file_type = MESA_FILE_TYPE_EXECUTABLE;
        }
    }

    pthread_mutex_unlock(&internal->mutex);
    return file_type;
}

// 基于libmagic识别单个文件类型
MESAFileType MESAFileIdentifyByLibmagic(const char* filePath, MESAFileError* error) {
    if (!filePath) {
        if (error) {
            *error = MESA_FILE_ERR_INVALID_PARAM;
        }
        return MESA_FILE_TYPE_UNKNOWN;
    }
    printf("have路径\n");
    printf("%s\n",filePath);
    MESAFileIdentifierInternal* internal = MESAFileIdentifierInitInternal();
    if (!internal) {
        if (error) {
            *error = MESA_FILE_ERR_INIT;
        }
        return MESA_FILE_TYPE_UNKNOWN;
    }

    MESAFileType file_type = MESAFileIdentifyByLibmagicInternal(internal, filePath, error);
    MESAFileIdentifierDestroyInternal(internal);
    return file_type;
}

// 线程函数：识别单个文件类型
void* identify_file_type_thread(void* arg) {
    IdentifyThreadData* data = (IdentifyThreadData*)arg;
    // MESAFileIdentifierInternal* internal = (MESAFileIdentifierInternal*)data->internal;
    MESAFileIdentifierInternal* internal = MESAFileIdentifierInitInternal();
    if (data->filePath) {
        *data->result = MESAFileIdentifyByLibmagicInternal(internal, data->filePath, data->error);
    } else if (data->url) {
        *data->result = MESAFileIdentifyByHttpContentType(data->url, data->error);
    }

    free(data);
    return NULL;
}




// 基于libmagic识别多个文件类型（多线程）
// MESAFileType* MESAFileIdentifyMultipleByLibmagic(const char** filePaths, int fileCount, MESAFileError** errors) {
//     if (!filePaths || fileCount <= 0) {
//         if (errors) {
//             *errors = NULL;
//         }
//         return NULL;
//     }

//     MESAFileIdentifierInternal* internal = MESAFileIdentifierInitInternal();
//     if (!internal) {
//         if (errors) {
//             *errors = NULL;
//         }
//         return NULL;
//     }

//     MESAFileType* results = (MESAFileType*)malloc(fileCount * sizeof(MESAFileType));
//     if (!results) {
//         MESAFileIdentifierDestroyInternal(internal);
//         if (errors) {
//             *errors = NULL;
//         }
//         return NULL;
//     }

//     if (errors) {
//         *errors = (MESAFileError*)malloc(fileCount * sizeof(MESAFileError));
//         if (!*errors) {
//             free(results);
//             MESAFileIdentifierDestroyInternal(internal);
//             return NULL;
//         }
//     }

//     pthread_t* threads = (pthread_t*)malloc(fileCount * sizeof(pthread_t));
//     if (!threads) {
//         free(results);
//         if (errors) {
//             free(*errors);
//             *errors = NULL;
//         }
//         MESAFileIdentifierDestroyInternal(internal);
//         return NULL;
//     }

//     for (int i = 0; i < fileCount; i++) {
//         IdentifyThreadData* data = (IdentifyThreadData*)malloc(sizeof(IdentifyThreadData));
//         data->filePath = filePaths[i];
//         data->url = NULL;
//         data->result = &results[i];
//         data->error = (errors) ? &(*errors)[i] : NULL;
//         data->internal = internal;

//         if (pthread_create(&threads[i], NULL, identify_file_type_thread, data) != 0) {
//             for (int j = 0; j < i; j++) {
//                 pthread_join(threads[j], NULL);
//             }
//             free(threads);
//             free(results);
//             if (errors) {
//                 free(*errors);
//                 *errors = NULL;
//             }
//             MESAFileIdentifierDestroyInternal(internal);
//             return NULL;
//         }
//     }

//     for (int i = 0; i < fileCount; i++) {
//         pthread_join(threads[i], NULL);
//     }

//     free(threads);
//     MESAFileIdentifierDestroyInternal(internal);
//     return results;
// }