#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include<string.h>
void test_urls_file(const char* urlsFilePath) {
    FILE* urlsFile = fopen(urlsFilePath, "r");
    if (!urlsFile) {
        printf("错误: 无法打开URL列表文件\n");
        return;
    }

    char url[1024];
    while (fgets(url, sizeof(url), urlsFile)) {
        url[strcspn(url, "\n")] = 0;

        MESAFileError error = MESA_FILE_SUCCESS;
        MESAFileType fileType = MESAFileDownloadAndIdentify(url, &error);

        if (error != MESA_FILE_SUCCESS) {
            printf("URL: %s\n识别失败: %s\n", url, MESAFileGetErrorDescription(error));
        } else {
            printf("URL: %s\n识别结果: %d\n", url, fileType);
        }
    }

    fclose(urlsFile);
}