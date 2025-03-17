#include "MESA_file_type.h"
#include <string.h>
#include <curl/curl.h>

// 通过URL后缀名识别文件类型
MESAFileType MESAFileIdentifyByUrl(const char* url, MESAFileError* error) {
    if (!url) {
        if (error) {
            *error = MESA_FILE_ERR_INVALID_PARAM;
        }
        return MESA_FILE_TYPE_UNKNOWN;
    }

    const char* extension = strrchr(url, '.');
    if (extension) {
        if (strcmp(extension, ".png") == 0) {
            return MESA_FILE_TYPE_IMAGE_PNG;
        } else if (strcmp(extension, ".jpg") == 0 || strcmp(extension, ".jpeg") == 0) {
            return MESA_FILE_TYPE_IMAGE_JPG;
        } else if (strcmp(extension, ".gif") == 0) {
            return MESA_FILE_TYPE_IMAGE_GIF;
        } else if (strcmp(extension, ".txt") == 0) {
            return MESA_FILE_TYPE_TEXT;
        } else if (strcmp(extension, ".zip") == 0) {
            return MESA_FILE_TYPE_ARCHIVE;
        }
    }

    if (error) {
        *error = MESA_FILE_ERR_INTERNAL;
    }
    return MESA_FILE_TYPE_UNKNOWN;
}

// 回调函数：获取HTTP响应头中的Content-Type字段
static size_t ContentTypeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    char* content_type = (char*)userp;
    size_t len = size * nmemb;
    content_type[len] = '\0';
    return len;
}

// 通过HTTP响应的Content-Type字段识别文件类型
MESAFileType MESAFileIdentifyByHttpContentType(const char* url, MESAFileError* error) {
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

    char content_type[256] = {0};
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ContentTypeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, content_type);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L); // 只获取响应头

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        if (error) {
            *error = MESA_FILE_ERR_INTERNAL;
        }
        return MESA_FILE_TYPE_UNKNOWN;
    }

    if (strstr(content_type, "text/")) {
        return MESA_FILE_TYPE_TEXT;
    } else if (strstr(content_type, "image/png")) {
        return MESA_FILE_TYPE_IMAGE_PNG;
    } else if (strstr(content_type, "image/jpeg")) {
        return MESA_FILE_TYPE_IMAGE_JPG;
    } else if (strstr(content_type, "image/gif")) {
        return MESA_FILE_TYPE_IMAGE_GIF;
    } else if (strstr(content_type, "application/zip")) {
        return MESA_FILE_TYPE_ARCHIVE;
    }

    if (error) {
        *error = MESA_FILE_ERR_INTERNAL;
    }
    return MESA_FILE_TYPE_UNKNOWN;
}