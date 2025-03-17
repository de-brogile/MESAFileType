#include "MESA_file_type.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <curl/curl.h>
#include <magic.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "utils/utils.h" 

// 综合三种识别方式的结果，得出最终的文件类型
MESAFileType MESAFileIdentifyCombined(const char* filePath, const char* url, MESAFileError* error) {
    MESAFileType libmagic_type = MESAFileIdentifyByLibmagic(filePath, error);
    MESAFileType url_type = MESAFileIdentifyByUrl(url, error);
    MESAFileType http_type = MESAFileIdentifyByHttpContentType(url, error);

    // 综合判断逻辑：优先使用libmagic的结果，其次使用HTTP Content-Type，最后使用URL后缀
    if (libmagic_type != MESA_FILE_TYPE_UNKNOWN) {
        return libmagic_type;
    } else if (http_type != MESA_FILE_TYPE_UNKNOWN) {
        return http_type;
    } else if (url_type != MESA_FILE_TYPE_UNKNOWN) {
        return url_type;
    } else {
        return MESA_FILE_TYPE_UNKNOWN;
    }
}

// 下载文件并保存到本地进行识别
// MESAFileType MESAFileDownloadAndIdentify(const char* url, MESAFileError* error) {
   
//     if (!url) {
//         if (error) {
//             *error = MESA_FILE_ERR_INVALID_PARAM;
//         }
//         return MESA_FILE_TYPE_UNKNOWN;
//     }

//     CURL* curl = curl_easy_init();
//     if (!curl) {
//         if (error) {
//             *error = MESA_FILE_ERR_INTERNAL;
//         }
//         return MESA_FILE_TYPE_UNKNOWN;
//     }

//     // 创建临时文件
//     FILE* fp = tmpfile();
//     if (!fp) {
//         curl_easy_cleanup(curl);
//         if (error) {
//             *error = MESA_FILE_ERR_INTERNAL;
//         }
//         return MESA_FILE_TYPE_UNKNOWN;
//     }

//     // 设置 CURL 选项
//     curl_easy_setopt(curl, CURLOPT_URL, url);
//     curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

//     CURLcode res = curl_easy_perform(curl);
//     curl_easy_cleanup(curl);
   

//     if (res != CURLE_OK) {
//         fclose(fp);
//         if (error) {
//             *error = MESA_FILE_ERR_INTERNAL;
//         }
//         return MESA_FILE_TYPE_UNKNOWN;
//     }

//     // 重新打开临时文件进行识别
//     rewind(fp);
   
//     MESAFileType file_type = MESAFileIdentifyByLibmagic(fp, error);
//     fclose(fp);

//     return file_type;
// }

MESAFileType MESAFileDownloadAndIdentify(const char* url, MESAFileError* error) {
    if (!url) {
        if (error) {
            *error = MESA_FILE_ERR_INVALID_PARAM;
        }
        return MESA_FILE_TYPE_UNKNOWN;
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        if (error) {
            *error = MESA_FILE_ERR_INTERNAL;
        }
        return MESA_FILE_TYPE_UNKNOWN;
    }

    // 创建临时文件用于下载
    FILE* temp_fp = tmpfile();
    if (!temp_fp) {
        curl_easy_cleanup(curl);
        if (error) {
            *error = MESA_FILE_ERR_INTERNAL;
        }
        return MESA_FILE_TYPE_UNKNOWN;
    }

    // 设置 CURL 选项
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, temp_fp);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        fclose(temp_fp);
        if (error) {
            *error = MESA_FILE_ERR_INTERNAL;
        }
        return MESA_FILE_TYPE_UNKNOWN;
    }

    // 提取文件名
    const char* filename = strrchr(url, '/');
    if (!filename) {
        filename = "downloaded_file"; // 默认文件名
    } else {
        filename++;
    }

    // 保存文件到当前目录
    FILE* save_fp = fopen(filename, "wb");
    if (!save_fp) {
        fclose(temp_fp);
        if (error) {
            *error = MESA_FILE_ERR_MAX;
        }
        return MESA_FILE_TYPE_UNKNOWN;
    }

    // 将临时文件内容复制到保存文件
    rewind(temp_fp);
    char buffer[4096];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), temp_fp)) > 0) {
        fwrite(buffer, 1, bytes_read, save_fp);
    }

    fclose(save_fp);
    fclose(temp_fp);

    MESAFileType file_type = MESAFileIdentifyByLibmagic(filename, error);



    
    return file_type;
}


MESAFileType* MESAFileIdentifyDirectory(const char* dirPath, MESAFileError** errors) {
    if (!dirPath) {
        if (errors) {
            *errors = NULL;
        }
        return NULL;
    }

    DIR* dir = opendir(dirPath);
    if (!dir) {
        if (errors) {
            *errors = NULL;
        }
        printf("Error: Unable to open directory\n");
        return NULL;
    }

    printf("Directory opened successfully\n");

    int fileCount = 0;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            fileCount++;
        }
    }
    rewinddir(dir);

    printf("File count: %d\n", fileCount);

    if (fileCount == 0) {
        closedir(dir);
        if (errors) {
            *errors = NULL;
        }
        printf("Error: No regular files in directory\n");
        return NULL;
    }

    MESAFileType* results = (MESAFileType*)malloc(fileCount * sizeof(MESAFileType));
    if (!results) {
        closedir(dir);
        if (errors) {
            *errors = NULL;
        }
        printf("Error: Memory allocation failed for results\n");
        return NULL;
    }

    if (errors) {
        *errors = (MESAFileError*)malloc(fileCount * sizeof(MESAFileError));
        if (!*errors) {
            free(results);
            closedir(dir);
            printf("Error: Memory allocation failed for errors\n");
            return NULL;
        }
    }
    memset(*(errors),0,sizeof *(errors));


    pthread_t* threads = (pthread_t*)malloc(fileCount * sizeof(pthread_t));
    if (!threads) {
        free(results);
        if (errors) {
            free(*errors);
            *errors = NULL;
        }
        closedir(dir);
        printf("Error: Memory allocation failed for threads\n");
        return NULL;
    }

    int i = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            char filePath[1024];
            snprintf(filePath, sizeof(filePath), "%s/%s", dirPath, entry->d_name);

            IdentifyThreadData* data = (IdentifyThreadData*)malloc(sizeof(IdentifyThreadData));
            data->filePath = strdup(filePath);
            data->url = NULL;
            data->result = &results[i];
            data->error = (errors) ? &(*errors)[i] : NULL;
            data->internal = NULL;

            if (pthread_create(&threads[i], NULL, identify_file_type_thread, data) != 0) {
                for (int j = 0; j < i; j++) {
                    pthread_join(threads[j], NULL);
                }
                free(threads);
                free(results);
                if (errors) {
                    free(*errors);
                    *errors = NULL;
                }
                closedir(dir);
                return NULL;
            }
           
            i++;
        }
    }

    closedir(dir);

    for (i = 0; i < fileCount; i++) {
        pthread_join(threads[i], NULL);
        // show_progress(i + 1, fileCount); 进度条函数
    }




    free(threads);
    return results;
}


// 识别URL列表文件中的所有URL
MESAFileType* MESAFileIdentifyMultipleUrls(const char* urlsFilePath, MESAFileError** errors) {
    FILE* urlsFile = fopen(urlsFilePath, "r");
    if (!urlsFile) {
        if (errors) {
            *errors = NULL;
        }
        return NULL;
    }

    // 读取 URL 文件并计数
    int urlCount = 0;
    char url[1024];
    while (fgets(url, sizeof(url), urlsFile)) {
        urlCount++;
    }
    rewind(urlsFile);

    MESAFileType* results = (MESAFileType*)malloc(urlCount * sizeof(MESAFileType));
    if (!results) {
        fclose(urlsFile);
        if (errors) {
            *errors = NULL;
        }
        return NULL;
    }

    if (errors) {
        *errors = (MESAFileError*)malloc(urlCount * sizeof(MESAFileError));
        if (!*errors) {
            free(results);
            fclose(urlsFile);
            return NULL;
        }
    }

    pthread_t* threads = (pthread_t*)malloc(urlCount * sizeof(pthread_t));
    if (!threads) {
        free(results);
        if (errors) {
            free(*errors);
            *errors = NULL;
        }
        fclose(urlsFile);
        return NULL;
    }

    int i = 0;
    while (fgets(url, sizeof(url), urlsFile)) {
        url[strcspn(url, "\n")] = 0;

        IdentifyThreadData* data = (IdentifyThreadData*)malloc(sizeof(IdentifyThreadData));
        data->filePath = NULL;
        data->url = strdup(url);
        data->result = &results[i];
        data->error = (errors) ? &(*errors)[i] : NULL;
        data->internal = NULL;

        if (pthread_create(&threads[i], NULL, identify_file_type_thread, data) != 0) {
            for (int j = 0; j < i; j++) {
                pthread_join(threads[j], NULL);
            }
            free(threads);
            free(results);
            if (errors) {
                free(*errors);
                *errors = NULL;
            }
            fclose(urlsFile);
            return NULL;
        }

        i++;
    }

    fclose(urlsFile);

    for (i = 0; i < urlCount; i++) {
        pthread_join(threads[i], NULL);
        show_progress(i + 1, urlCount);
    }

    printf("\n");

    free(threads);
    return results;
}