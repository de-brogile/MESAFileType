#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include<string.h>

void test_directory(const char* dirPath) {
    if (!dirPath) {
        printf("错误: 无效的目录路径\n");
        return;
    }

    MESAFileError* errors = NULL;
    MESAFileType* results = MESAFileIdentifyDirectory(dirPath, &errors);
    if (!results) {
        printf("错误: 无法识别目录\n");
        return;
    }

    DIR* dir = opendir(dirPath);
    if (!dir) {
        printf("错误: 无法打开目录\n");
        free(results);
        if (errors) {
            free(errors);
        }
        return;
    }

    struct dirent* entry;
    int file_count = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            file_count++;
        }
    }
    closedir(dir);

    DIR* dir2 = opendir(dirPath);
    if (!dir2) {
        printf("错误: 无法打开目录\n");
        free(results);
        if (errors) {
            free(errors);
        }
        return;
    }

    struct dirent* entry2;
    int i = 0;
    while ((entry2 = readdir(dir2)) != NULL) {
        if (strcmp(entry2->d_name, ".") == 0 || strcmp(entry2->d_name, "..") == 0) {
            continue;
        }

        if (i >= file_count) {
            break;
        }

        char filePath[1024];
        snprintf(filePath, sizeof(filePath), "%s/%s", dirPath, entry2->d_name);

        if (!errors || errors[i] != MESA_FILE_SUCCESS) {
            printf("文件: %s\n识别失败: %s\n",
                   filePath, MESAFileGetErrorDescription(errors[i]));
        } else {
            printf("文件: %s\n识别结果: %d\n",
                   filePath, results[i]);
        }

        i++;
    }

    closedir(dir2);
    free(results);
    if (errors) {
        free(errors);
    }
}