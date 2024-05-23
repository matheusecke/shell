#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

void print_file_info(const char *name, const struct stat *sb) {
    char mode[11];
    snprintf(mode, sizeof(mode), (S_ISDIR(sb->st_mode)) ? "d" : "-");
    snprintf(mode + 1, sizeof(mode) - 1, (sb->st_mode & S_IRUSR) ? "r" : "-");
    snprintf(mode + 2, sizeof(mode) - 2, (sb->st_mode & S_IWUSR) ? "w" : "-");
    snprintf(mode + 3, sizeof(mode) - 3, (sb->st_mode & S_IXUSR) ? "x" : "-");
    snprintf(mode + 4, sizeof(mode) - 4, (sb->st_mode & S_IRGRP) ? "r" : "-");
    snprintf(mode + 5, sizeof(mode) - 5, (sb->st_mode & S_IWGRP) ? "w" : "-");
    snprintf(mode + 6, sizeof(mode) - 6, (sb->st_mode & S_IXGRP) ? "x" : "-");
    snprintf(mode + 7, sizeof(mode) - 7, (sb->st_mode & S_IROTH) ? "r" : "-");
    snprintf(mode + 8, sizeof(mode) - 8, (sb->st_mode & S_IWOTH) ? "w" : "-");
    snprintf(mode + 9, sizeof(mode) - 9, (sb->st_mode & S_IXOTH) ? "x" : "-");

    printf("%s %ld %s %s %ld %s %s\n",
           mode,
           (long) sb->st_nlink,
           getpwuid(sb->st_uid)->pw_name,
           getgrgid(sb->st_gid)->gr_name,
           (long) sb->st_size,
           strtok(ctime(&sb->st_mtime), "\n"), // Remove newline
           name);
}

int main(int argc, char *argv[]) {
    int lflag = 0, aflag = 0;
    char *path = ".";
    
    // Processar argumentos
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0) {
            lflag = 1;
        } else if (strcmp(argv[i], "-a") == 0) {
            aflag = 1;
        } else {
            path = argv[i];
        }
    }

    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (!aflag && entry->d_name[0] == '.') {
            continue;
        }

        if (lflag) {
            struct stat sb;
            if (stat(entry->d_name, &sb) == -1) {
                perror("stat");
                continue;
            }
            print_file_info(entry->d_name, &sb);
        } else {
            printf("%s\n", entry->d_name);
        }
    }

    closedir(dir);
    return 0;
}