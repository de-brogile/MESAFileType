#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <file|url|dir|urls_file>\n", argv[0]);
        return 1;
    }

    const char* target = argv[1];

    if (strstr(target, "http://") || strstr(target, "https://")) {
        printf("single_URL\n");
        test_single_url(target);
    } else {
        struct stat buffer;
        if (stat(target, &buffer) != 0) {
            if (errno == ENOENT) {
                printf("错误: 无效的文件或目录路径\n");
            } else if (errno == EACCES) {
                printf("错误: 无访问权限\n");
            } else {
                printf("错误: 无法访问路径\n");
            }
            return 1;
        }

        if (S_ISDIR(buffer.st_mode)) {
            test_directory(target);
        } else if (S_ISREG(buffer.st_mode)) {
            FILE* file = fopen(target, "r");
            if (!file) {
                printf("错误: 无法打开文件\n");
                return 1;
            }

            char* line = NULL;
            size_t len = 0;
            ssize_t read;

            int is_url_list = 0;
            if ((read = getline(&line, &len, file)) != -1) {
                if (strstr(line, "http://") || strstr(line, "https://")) {
                    is_url_list = 1;
                }
            }

            fclose(file);
            free(line);

            if (is_url_list) {
                test_urls_file(target);
            } else {
               
                test_single_file(target);
            }
        } else {
            printf("错误: 不支持的文件类型\n");
            return 1;
        }
    }

    return 0;
}