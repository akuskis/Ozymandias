#include "dir.h"

#include "core/string.h"
#include "io/config/config.h"
#include "io/file.h"
#include "platform/file_manager.h"

#include <stdlib.h>
#include <string.h>

#define BASE_MAX_FILES 100

struct dir_data_t {
    dir_listing listing;
    int max_files;
    char* cased_filename;
};

dir_data_t g_dir_data;

static void allocate_listing_files(int min, int max) {
    auto& data = g_dir_data;
    for (int i = min; i < max; i++) {
        data.listing.files[i] = (char*)malloc(MAX_FILE_NAME * sizeof(char));
        data.listing.files[i][0] = 0;
    }
}

static void clear_dir_listing(void) {
    auto& data = g_dir_data;
    data.listing.num_files = 0;
    if (data.max_files <= 0) {
        data.listing.files = (char**)malloc(BASE_MAX_FILES * sizeof(char*));
        allocate_listing_files(0, BASE_MAX_FILES);
        data.max_files = BASE_MAX_FILES;
    } else {
        for (int i = 0; i < data.max_files; i++) {
            data.listing.files[i][0] = 0;
        }
    }
}

static void expand_dir_listing(void) {
    auto& data = g_dir_data;
    int old_max_files = data.max_files;

    data.max_files = 2 * old_max_files;
    data.listing.files = (char**)realloc(data.listing.files, data.max_files * sizeof(char*));
    allocate_listing_files(old_max_files, data.max_files);
}

static int compare_lower(const void* va, const void* vb) {
    // arguments are pointers to char*
    return string_compare_case_insensitive(*(const char**)va, *(const char**)vb);
}

static int add_to_listing(const char* filename) {
    auto& data = g_dir_data;

    if (data.listing.num_files >= data.max_files)
        expand_dir_listing();

    strncpy(data.listing.files[data.listing.num_files], filename, MAX_FILE_NAME);
    data.listing.files[data.listing.num_files][MAX_FILE_NAME - 1] = 0;
    ++data.listing.num_files;
    return LIST_CONTINUE;
}

const dir_listing* dir_find_files_with_extension(const char* dir, const char* extension) {
    auto& data = g_dir_data;

    clear_dir_listing();
    platform_file_manager_list_directory_contents(dir, TYPE_FILE, extension, add_to_listing);
    qsort(data.listing.files, data.listing.num_files, sizeof(char*), compare_lower);
    return &data.listing;
}

const dir_listing* dir_find_all_subdirectories(const char* dir) {
    auto& data = g_dir_data;

    clear_dir_listing();
    platform_file_manager_list_directory_contents(dir, TYPE_DIR, 0, add_to_listing);
    qsort(data.listing.files, data.listing.num_files, sizeof(char*), compare_lower);
    return &data.listing;
}

static int compare_case(const char* filename) {
    auto& data = g_dir_data;

    if (string_compare_case_insensitive(filename, data.cased_filename) == 0) {
        strcpy(data.cased_filename, filename);
        return LIST_MATCH;
    }
    return LIST_NO_MATCH;
}

static int correct_case(const char* dir, char* filename, int type) {
    auto& data = g_dir_data;

    data.cased_filename = filename;
    return platform_file_manager_list_directory_contents(dir, type, 0, compare_case) == LIST_MATCH;
}

static void move_left(char* str) {
    while (*str) {
        str[0] = str[1];
        str++;
    }
    *str = 0;
}

const char* dir_get_case_corrected_file(const char* dir, const char* filepath) {
    static char corrected_filename[2 * MAX_FILE_NAME] = {0};

    size_t dir_len = 0;
    if (dir) {
        dir_len = strlen(dir) + 1;
        strncpy(corrected_filename, dir, 2 * MAX_FILE_NAME - 1);
        corrected_filename[dir_len - 1] = '/';
    } else {
        dir = ".";
    }

    strncpy(&corrected_filename[dir_len], filepath, 2 * MAX_FILE_NAME - dir_len - 1);

    FILE* fp = file_open(corrected_filename, "rb");
    if (fp) {
        file_close(fp);
        return corrected_filename;
    }

    if (!platform_file_manager_should_case_correct_file()) {
        return corrected_filename;
    }

    strncpy(&corrected_filename[dir_len], filepath, 2 * MAX_FILE_NAME - dir_len - 1);

    char* slash = strchr(&corrected_filename[dir_len], '/');
    if (!slash)
        slash = strchr(&corrected_filename[dir_len], '\\');

    if (slash) {
        *slash = 0;
        if (correct_case(dir, &corrected_filename[dir_len], TYPE_DIR)) {
            char* path = slash + 1;
            if (*path == '\\') {
                // double backslash: move everything to the left
                move_left(path);
            }
            if (correct_case(corrected_filename, path, TYPE_FILE)) {
                *slash = '/';
                return corrected_filename;
            }
        }
    } else {
        if (correct_case(dir, corrected_filename, TYPE_FILE))
            return corrected_filename;
    }
    return 0;
}

const dir_listing* dir_append_files_with_extension(const char* dir, const char* extension) {
    auto& data = g_dir_data;

    platform_file_manager_list_directory_contents(dir, TYPE_FILE, extension, add_to_listing);
    qsort(data.listing.files, data.listing.num_files, sizeof(char*), compare_lower);
    return &data.listing;
}

const char* dir_get_file(const char* filepath, int localizable) {
    return dir_get_case_corrected_file(platform_file_manager_get_base_path(), filepath);
}
