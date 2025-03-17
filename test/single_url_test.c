#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

void test_single_url(const char* url) {
    MESAFileError error = MESA_FILE_SUCCESS;
    MESAFileType fileType = MESAFileDownloadAndIdentify(url, &error);

    if (error != MESA_FILE_SUCCESS) {
        printf("URL: %s\n识别失败: %s\n", url, MESAFileGetErrorDescription(error));
    } else {
        printf("URL: %s\n识别结果: %d\n", url, fileType);
    }
}