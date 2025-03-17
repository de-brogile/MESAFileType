#ifndef TEST_H
#define TEST_H

#include "src/MESA_file_type.h"

void test_single_file(const char* filePath);
void test_single_url(const char* url);
void test_directory(const char* dirPath);
void test_urls_file(const char* urlsFilePath);

#endif // TEST_H