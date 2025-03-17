#include "utils.h"
#include <stdio.h>

// 显示进度条
void show_progress(int current, int total) {
    float progress = (float)current / total;
    int barWidth = 50;

    printf("[");
    int pos;
    for (pos = 0; pos < barWidth * progress; pos++) {
        printf("=");
    }
    for (; pos < barWidth; pos++) {
        printf(" ");
    }
    printf("] %.2f%%\r", progress * 100);
    fflush(stdout);
}