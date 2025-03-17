#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

void test_single_file(const char* filePath) {
    MESAFileError error = MESA_FILE_SUCCESS;
    MESAFileType fileType = MESAFileIdentifyByLibmagic(filePath, &error);

    if (error != MESA_FILE_SUCCESS) {
        printf("文件: %s\n识别失败: %s\n", filePath, MESAFileGetErrorDescription(error));
    } else {
        printf("文件: %s\n识别结果: %d\n", filePath, fileType);
    }
}