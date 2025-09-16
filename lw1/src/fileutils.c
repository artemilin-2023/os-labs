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

int ensure_directory_exists(const char *path) {
#ifdef _WIN32
	// Создаём директорию, игнорируя ошибку "уже существует"
	if (_mkdir(path) == 0) return 0;
	return 0; // упрощённо
#else
	if (mkdir(path, 0755) == 0) return 0;
	if (errno == EEXIST) return 0;
	return -1;
#endif
}

int counter_cb(const char *path, void *user_data);

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

char *get_relative_path(const char *root, const char *path)
{
	return path + strlen(root);
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