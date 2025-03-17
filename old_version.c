#include "src/MESA_file_type.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <file|url|dir|urls_file>\n", argv[0]);
        return 1;
    }

    const char* target = argv[1];
    MESAFileError* errors = NULL;
    MESAFileType* results = NULL;

    if (strstr(target, "http://") || strstr(target, "https://")) {
        // 单个 URL
        MESAFileError error;
        MESAFileType fileType = MESAFileDownloadAndIdentify(target, &error);
        printf("URL: %s\n识别结果: %d\n", target, fileType);
    } else if (access(target, F_OK) == -1) {
        // 无效路径
        printf("错误: 无效的文件或目录路径\n");
    } else if (access(target, R_OK) == -1) {
        // 无读取权限
        printf("错误: 无读取权限\n");
    } else {
        struct stat buffer;
        stat(target, &buffer);

        if (S_ISDIR(buffer.st_mode)) {
            // 目录
            results = MESAFileIdentifyDirectory(target, &errors);
        } else if (S_ISREG(buffer.st_mode)) {
            // 文件
            FILE* file = fopen(target, "r");
            if (!file) {
                printf("错误: 无法打开文件\n");
                return 1;
            }

            char* line = NULL;
            size_t len = 0;
            ssize_t read;

            if ((read = getline(&line, &len, file)) != -1) {
                // 文件内容是 URL 列表
                fclose(file);
                free(line);
                results = MESAFileIdentifyMultipleUrls(target, &errors);
            } else {
                // 单个文件
                fclose(file);
                MESAFileError error;
                MESAFileType fileType = MESAFileIdentifyByLibmagic(target, &error);
                printf("文件: %s\n识别结果: %d\n", target, fileType);
                return 0;
            }
        }

        if (results) {
            int total = 0;
            if (errors) {
                total = sizeof(*errors) / sizeof(MESAFileError);
            } else {
                total = sizeof(*results) / sizeof(MESAFileType);
            }

            for (int i = 0; i < total; i++) {
                if (results[i] == MESA_FILE_TYPE_UNKNOWN && errors[i] != MESA_FILE_SUCCESS) {
                    printf("识别失败: %s\n", MESAFileGetErrorDescription(errors[i]));
                } else {
                    printf("识别结果: %d\n", results[i]);
                }
            }

            free(results);
            if (errors) {
                free(errors);
            }
        }
    }

    return 0;
}






// #include"src/MESA_file_type.h"
// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/stat.h>
// #include <unistd.h>
// #include <dirent.h>
// #include <errno.h>
// #include <string.h>
// const int default_error = MESA_FILE_SUCCESS;

// int main(int argc, char* argv[]) {
//     if (argc < 2) {
//         printf("Usage: %s <file|url|dir|urls_file>\n", argv[0]);
//         return 1;
//     }

//     const char* target = argv[1];
//     MESAFileError* errors = NULL;
//     MESAFileType* results = NULL;
//     MESAFileError error = default_error;
//     if (strstr(target, "http://") || strstr(target, "https://")) {
//         // 处理单个URL
//         printf("处理单个URL");
//         MESAFileType fileType = MESAFileDownloadAndIdentify(target, &error);
//         if (error != MESA_FILE_SUCCESS) {
//             printf("URL: %s\n识别失败: %s\n", target, MESAFileGetErrorDescription(error));
//         } else {
//             printf("URL: %s\n识别结果: %d\n", target, fileType);
//         }
//     } else {
//         struct stat buffer;
//         if (stat(target, &buffer) != 0) {
//             if (errno == ENOENT) {
//                 printf("错误: 无效的文件或目录路径\n");
//             } else if (errno == EACCES) {
//                 printf("错误: 无访问权限\n");
//             } else {
//                 printf("错误: 无法访问路径\n");
//             }
//             return 1;
//         }

//         if (S_ISDIR(buffer.st_mode)) {
//             // 处理目录
//             printf("处理目录");
            
//             results = MESAFileIdentifyDirectory(target, &errors);
            
//             DIR* dir = opendir(target);
//             if (!dir) {
//                 printf("错误: 无法打开目录\n");
//                 return 1;
//             }
//             int file_count = 0;
//             struct dirent* entry;
//             while ((entry = readdir(dir)) != NULL) {
//                 if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
//                     file_count++;
//                 }
//             }
//             closedir(dir);
          
            
//             if (results) {
//                 for (int i = 0; i < file_count; i++) {
//                     if (errors && errors[i] != MESA_FILE_SUCCESS) {
//                         printf("文件: %s/%s\n识别失败: %s\n",
//                                target, entry->d_name, MESAFileGetErrorDescription(errors[i]));
//                     } else {
//                         printf("文件: %s/%s\n识别结果: %d\n",
//                                target, entry->d_name, results[i]);
//                     }
//                 }
//                 free(results);
//                 if (errors) {
//                     free(errors);
//                 }
//             }
            
//         } else if (S_ISREG(buffer.st_mode)) {
//             // 处理文件
//             printf("处理文件\n");
//             FILE* file = fopen(target, "r");
//             if (!file) {
//                 printf("错误: 无法打开文件\n");
//                 return 1;
//             }

//             char* line = NULL;
//             size_t len = 0;
//             ssize_t read;

//             int is_url_list = 0;
//             if ((read = getline(&line, &len, file)) != -1) {
//                 // 检查第一行是否为URL
//                 if (strstr(line, "http://") || strstr(line, "https://")) {
//                     is_url_list = 1;
//                 }
//             }

//             fclose(file);
//             free(line);

//             if (is_url_list) {
//                 // 处理URL列表文件
//                 printf("处理URL列表文件");
//                 results = MESAFileIdentifyMultipleUrls(target, &errors);
//                 // 统计行数
//                 file = fopen(target, "r");
//                 int line_count = 0;
//                 while ((read = getline(&line, &len, file)) != -1) {
//                     line_count++;
//                 }
//                 fclose(file);
//                 free(line);

//                 if (results) {
//                     for (int i = 0; i < line_count; i++) {
//                         if (errors && errors[i] != MESA_FILE_SUCCESS) {
//                             printf("URL: %s\n识别失败: %s\n",
//                                    line, MESAFileGetErrorDescription(errors[i]));
//                         } else {
//                             printf("URL: %s\n识别结果: %d\n",
//                                    line, results[i]);
//                         }
//                     }
//                     free(results);
//                     if (errors) {
//                         free(errors);
//                     }
//                 }
//             } else {
//                 // 处理单个文件
//                 printf("处理单个文件\n");
//                 // MESAFileError error = 0;
//                 MESAFileType fileType = MESAFileIdentifyByLibmagic(target, &error);
//                 if (error != MESA_FILE_SUCCESS) {
//                     printf("文件: %s\n识别失败: %s\n", target, MESAFileGetErrorDescription(error));
//                 } else {
//                     printf("文件: %s\n识别结果: %d\n", target, fileType);
//                 }
//             }
//         } else {
//             printf("错误: 不支持的文件类型\n");
//             return 1;
//         }
//     }

//     return 0;
// }