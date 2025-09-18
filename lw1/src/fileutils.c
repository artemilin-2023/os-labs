#include "fileutils.h"

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#ifdef _WIN32
    #define PATH_SEP '\\'
#else
    #define PATH_SEP '/'
#endif

int counter_cb(const char *path, void *user_data);


int ensure_directory_exists(const char *path) {
#ifdef _WIN32
	if (_mkdir(path) == 0) return 0;
	return 0;
#else
	if (mkdir(path, 0755) == 0) return 0;
	if (errno == EEXIST) return 0;
	return -1;
#endif
}

int create_dirs(const char *path) {
    char tmp[1024];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);

    if(tmp[len - 1] == '\\' || tmp[len - 1] == '/')
        tmp[len - 1] = 0;

    for(p = tmp + 1; *p; p++) {
        if(*p == '\\' || *p == '/') {
            *p = 0;
            if(ensure_directory_exists(tmp) != 0) {
                return -1;
            }
            *p = PATH_SEP;
        }
    }

    return 0;
}

int get_file_count(const char *root) {
	int count = 0;
	walk_directory(root, counter_cb, &count);
	return count;
}

int counter_cb(const char *path, void *user_data) {
	int *cnt = (int *)user_data;
	(*cnt)++;
	return 0;
}

uint64_t get_file_size(FILE *file) {
	fseek(file, 0, SEEK_END);
	uint64_t size = ftell(file);
	fseek(file, 0, SEEK_SET);
	return size;
}

const char *get_relative_path(const char *root, const char *path)
{
	// пропускаем корень и убираем разделитель
	if (strlen(path) == strlen(root))
		return ".";
	return path + strlen(root) + 1;
}

const char *join_path(const char *p1, const char *p2) {
	size_t len1 = strlen(p1);
    size_t len2 = strlen(p2);

    while (len1 > 0 && (p1[len1 - 1] == '/' || p1[len1 - 1] == '\\')) {
        len1--;
    }
    while (*p2 == '/' || *p2 == '\\') {
        p2++;
        len2--;
    }

    char *res = malloc(len1 + 1 + len2 + 1);
    if (!res) return NULL;

    memcpy(res, p1, len1);
    res[len1] = PATH_SEP;
    memcpy(res + len1 + 1, p2, len2);
    res[len1 + 1 + len2] = '\0';

    return res;
}

int walk_directory(const char *root, file_visit_cb cb, void *user_data) {
#ifdef _WIN32
	WIN32_FIND_DATAA ffd;
	char pattern[MAX_PATH];
	snprintf(pattern, sizeof(pattern), "%s\\*", root);
	HANDLE hFind = FindFirstFileA(pattern, &ffd);
	if (hFind == INVALID_HANDLE_VALUE) return -1;
	do {
		if (strcmp(ffd.cFileName, ".") == 0 || strcmp(ffd.cFileName, "..") == 0) continue;
		char full[MAX_PATH];
		snprintf(full, sizeof(full), "%s\\%s", root, ffd.cFileName);
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			walk_directory(full, cb, user_data);
		} else {
			if (cb(full, user_data) != 0) break;
		}
	} while (FindNextFileA(hFind, &ffd));
	FindClose(hFind);
	return 0;
#else
	DIR *dir = opendir(root);
	if (!dir) return -1;
	struct dirent *ent;
	char pathbuf[4096];
	while ((ent = readdir(dir)) != NULL) {
		if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
		snprintf(pathbuf, sizeof(pathbuf), "%s/%s", root, ent->d_name);
		if (ent->d_type == DT_DIR) {
			walk_directory(pathbuf, cb, user_data);
		} else {
			if (cb(pathbuf, user_data) != 0) break;
		}
	}
	closedir(dir);
	return 0;
#endif
}