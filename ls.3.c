#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/stat.h>
#include<string.h>
#include<time.h>
#include<pwd.h>
#include<grp.h>

#define MAX_PATH 1024

void print_file_info(const char *path, const struct stat *file_stat, int show_inode, int show_size);/*打印指定文件的详细信息*/
void print_dir(const char *dir_path, int show_hidden, int show_details, int recursive, int sort_by_time, int reverse_sort, int show_inode, int show_size);/*打印指定目录下的所有文件和子目录的信息*/
int cmp_file_time(const void *a, const void *b);/*比较两个文件的修改时间*/
int cmp_file_name(const void *a, const void *b);/*比较两个文件的名称*/
int cmp_file_size(const void *a, const void *b);/*比较两个文件的大小*/

void print_file_info(const char *path, const struct stat *file_stat, int show_inode, int show_size) 
{
    char mode_str[11];
    mode_t mode = file_stat->st_mode;
    struct passwd *user_info = getpwuid(file_stat->st_uid);
    struct group *group_info = getgrgid(file_stat->st_gid);
    strftime(mode_str, 11, "%b %d %H:%M", localtime(&file_stat->st_mtime));
    if (S_ISDIR(mode)) 
    {
        printf("d");
    } else if (S_ISLNK(mode)) 
    {
        printf("l");
    } else 
    {
        printf("-");
    }
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
    printf(" %ld", (long)file_stat->st_nlink);
    printf(" %s", user_info->pw_name);
    printf(" %s", group_info->gr_name);
    if (show_size) 
    {
        printf(" %7ld", (long)file_stat->st_size);
    }
    printf(" %s", mode_str);
    if (show_inode) 
    {
        printf(" %ld", (long)file_stat->st_ino);
    }
    printf(" %s", path);
    if (S_ISLNK(mode)) 
    {
        char link_target[MAX_PATH];
        ssize_t len = readlink(path, link_target, MAX_PATH);
        if (len != -1) 
        {
            link_target[len] = '\0';
            printf(" -> %s", link_target);
        }
    }
    printf("\n");
}

void print_dir(const char *dir_path, int show_hidden, int show_details, int recursive, int sort_by_time, int reverse_sort, int show_inode, int show_size) 
{
    DIR *dir = opendir(dir_path);
    if (!dir) 
    {
        printf("Error: cannot open directory %s\n", dir_path);
        return;
    }
    struct dirent *entry;
    struct stat file_stat;
    char path[MAX_PATH];
    while ((entry = readdir(dir)) != NULL) 
    {
        if (!show_hidden && entry->d_name[0] == '.') 
        {
            continue;
        }
        snprintf(path, MAX_PATH, "%s/%s", dir_path, entry->d_name);
        if (lstat(path, &file_stat) == -1) 
        {
            printf("Error: cannot get file status for %s\n", path);
            continue;
        }
        if (show_details) 
        {
            print_file_info(entry->d_name, &file_stat, show_inode, show_size);
        } else 
        {
            if (S_ISDIR(file_stat.st_mode)) 
            {
                printf("\033[1;34m%s/\033[0m", entry->d_name);
            } else if (S_ISLNK(file_stat.st_mode)) 
            {
                printf("\033[1;36m%s@\033[0m", entry->d_name);
            } else 
            {
                printf("%s", entry->d_name);
            }
            printf(" ");
        }
        if (recursive && S_ISDIR(file_stat.st_mode) && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) 
        {
            print_dir(path, show_hidden, show_details, recursive, sort_by_time, reverse_sort, show_inode, show_size);
        }
    }
    closedir(dir);
}

int cmp_file_time(const void *a, const void *b)
{
    return ((struct stat *)b)->st_mtime - ((struct stat *)a)->st_mtime;
}

int cmp_file_name(const void *a, const void *b) 
{
    return strcmp(((struct dirent *)a)->d_name, ((struct dirent *)b)->d_name);
}

int cmp_file_size(const void *a, const void *b) 
{
    return ((struct stat *)b)->st_size - ((struct stat *)a)->st_size;
}

int main(int argc, char **argv) 
{
    int show_hidden = 0;
    int show_details = 0;
    int recursive = 0;
    int sort_by_time = 0;
    int reverse_sort = 0;
    int show_inode = 0;
    int show_size = 0;
    char *dir_path;
    int i;
    for (i = 1; i < argc; i++) 
    {
        if (strcmp(argv[i], "-a") == 0) 
        {
            show_hidden = 1;
        } else if (strcmp(argv[i], "-l") == 0) 
        {
            show_details = 1;
        } else if (strcmp(argv[i], "-R") == 0) 
        {
            recursive = 1;
        } else if (strcmp(argv[i], "-t") == 0) 
        {
            sort_by_time = 1;
        } else if (strcmp(argv[i], "-r") == 0) 
        {
            reverse_sort = 1;
        } else if (strcmp(argv[i], "-i") == 0) 
        {
            show_inode = 1;
        } else if (strcmp(argv[i], "-s") == 0) 
        {
            show_size = 1;
        } else 
        {
            dir_path = argv[i];
        }
    }
    if (!dir_path) 
    {
        dir_path = ".";
    }
    struct dirent **namelist;
    int n = scandir(dir_path, &namelist, NULL, alphasort);
    if (n == -1) 
    {
        printf("Error: cannot open directory %s\n", dir_path);
        return 1;
    }
    if (sort_by_time) 
    {
        qsort(namelist, n, sizeof(struct dirent *), cmp_file_time);
    } else 
    {
        qsort(namelist, n, sizeof(struct dirent *), cmp_file_name);
    }
    if (reverse_sort) 
    {
        for (i = n - 1; i >= 0; i--) 
        {
            char path[MAX_PATH];
            snprintf(path, MAX_PATH, "%s/%s", dir_path, namelist[i]->d_name);
            struct stat file_stat;
            if (lstat(path, &file_stat) == -1) 
            {
                printf("Error: cannot get file status for %s\n", path);
                continue;
            }
            if (show_inode) 
            {
                printf("%ld ", (long)file_stat.st_ino);
            }
            if (show_size) 
            {
                printf("%ld ", (long)file_stat.st_blocks);
            }
            if (show_details) 
            {
                print_file_info(namelist[i]->d_name, &file_stat, show_inode, show_size);
            } else 
            {
                if (S_ISDIR(file_stat.st_mode)) 
                {
                    printf("\033[1;34m%s/\033[0m", namelist[i]->d_name);
                } else if (S_ISLNK(file_stat.st_mode)) 
                {
                    printf("\033[1;36m%s@\033[0m", namelist[i]->d_name);
                } else 
                {
                    printf("%s", namelist[i]->d_name);
                }
                printf(" ");
            }
        }
    } else 
    {
        for (i = 0; i < n; i++) 
        {
            char path[MAX_PATH];
            snprintf(path, MAX_PATH, "%s/%s", dir_path, namelist[i]->d_name);
            struct stat file_stat;
            if (lstat(path, &file_stat) == -1) 
            {
                printf("Error: cannot get file status for %s\n", path);
                continue;
            }
            if (show_inode) 
            {
                printf("%ld ", (long)file_stat.st_ino);
            }
            if (show_size) 
            {
                printf("%ld ", (long)file_stat.st_blocks);
            }
            if (show_details) 
            {
                print_file_info(namelist[i]->d_name, &file_stat, show_inode, show_size);
            } else 
            {
                if (S_ISDIR(file_stat.st_mode)) 
                {
                    printf("\033[1;34m%s/\033[0m", namelist[i]->d_name);
                } else if (S_ISLNK(file_stat.st_mode)) 
                {
                    printf("\033[1;36m%s@\033[0m", namelist[i]->d_name);
                } else 
                {
                    printf("%s", namelist[i]->d_name);
                }
                printf(" ");
            }
        }
    }
    printf("\n");
    
    if (recursive) 
    {
        print_dir(dir_path, show_hidden, show_details, recursive, sort_by_time, reverse_sort, show_inode, show_size);
    }
    return 0;
}
