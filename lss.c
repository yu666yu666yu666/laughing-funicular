#include <stdio.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define a 0b1000000
#define l 0b0100000
#define R 0b0010000
#define t 0b0001000
#define r 0b0000100
#define I 0b0000010
#define s 0b0000001

#define NONE "\033[m"
#define GREEN "\033[0;32;32m"
#define BLUE "\033[0;32;34m"

void get_(int argc, char* argv[]);
void restored_ls(struct dirent* cur_item);
void sort(char** filenames, int start, int end);
void l_r(char** filenames, int file_cnt);
int partition(char** filenames, int start, int end);
void swap(char** s1, char** s2);
int compare(char* s1, char* s2);
char* uid_to_name(uid_t);
char* gid_to_name(gid_t);
void mode_to_letters(int, char[]);
char* uid_to_name(uid_t);

void l_i(char filename[]);
void l_s(char filename[]);
int l_name(char dirname[]);
void l_myls();
void l_t(char** filenames);
int i_f = 0;
char* dirname[4096 * 128];
int dirlen = 0;
char* filenames[4096 * 128];
int file_cnt = 0;
int main(int argc, char* argv[]) {
    get_(argc, argv);
    l_myls();
    return 0;
}
void l_myls() {
    for (int i = 0; i < dirlen; i++) {
        if (l_name(dirname[i]) == -1) {
            continue;
        }
        
        if ((i_f & t) == t) {  // 时间排序
            l_t(filenames);
        }
        if ((i_f & r) == r) {  // 逆序
            l_r(filenames, file_cnt);
        }
        printf("当前路径:\"%s\"\n", dirname[i]);
        int tag = 0;  
        for (int j = 0; j < file_cnt; j++) {
            
            char path[4096] = {0};
            strcpy(path, dirname[i]);
            int len = strlen(dirname[i]);
            strcpy(&path[len], "/");
            strcpy(&path[len + 1], filenames[j]);
            tag++;
            if ((i_f & a) == 0) {
                if ((strcmp(filenames[j], ".") == 0 ||
                     strcmp(filenames[j], "..") == 0) ||
                    filenames[j][0] == '.') {
                    continue;
                }
            }
            struct stat info;
            stat(path, &info);  
            if (S_ISDIR(info.st_mode) && ((i_f & R) == R)) {
                
                char* tempdirname = (char*)malloc(sizeof(char) * 4096);
                strcpy(tempdirname, dirname[i]);
                int len = strlen(tempdirname);
                strcpy(&tempdirname[len], "/");
                strcpy(&tempdirname[len + 1], filenames[j]);
                dirname[dirlen++] = tempdirname;
            }
            if ((i_f & I) == I) {
                l_i(path);
            }
            if ((i_f & s) == s) {
                l_s(path);
            }
            if ((i_f & l) == 0) {
                if (S_ISDIR(info.st_mode))  // 判断是否为目录
                {
                    printf(GREEN "%s\t" NONE, filenames[j]);
                } else {
                    printf(BLUE "%s\t" NONE, filenames[j]);
                }
            }
            if ((i_f & l) == l) {
                void mode_to_letters();
                char modestr[11];
                mode_to_letters(info.st_mode, modestr);
                printf("%s ", modestr);
                printf("%4d ", (int)info.st_nlink);
                printf("%-8s ", uid_to_name(info.st_uid));
                printf("%-8s ", gid_to_name(info.st_gid));
                printf("%8ld ", (long)info.st_size);
                printf("%.12s ", ctime(&info.st_mtime));
                if (S_ISDIR(info.st_mode))  // 判断是否为目录
                {
                    printf(GREEN "%s\t" NONE, filenames[j]);
                } else {
                    printf(BLUE "%s\t" NONE, filenames[j]);
                }
                printf("\n");
            }
            if ((tag % 5 == 0) && ((i_f & l) == 0)) {
                printf("\n");
            }
        }
        // 清空
        for (int k = 0; k < file_cnt; k++) {
            memset(filenames[k], 4096, '\0');
        }
        file_cnt = 0;
    }
}
void get_(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] !=
            '-') {  // 只接受以'-'开头的参数
            char* tempdirname = (char*)malloc(sizeof(char) * 4096);
            strcpy(tempdirname, argv[i]);
            dirname[dirlen++] = tempdirname;
        } else {
            int len = strlen(argv[i]);
            for (int j = 1; j < len; j++) {
                switch (argv[i][j]) {
                    case 'a':
                        i_f |= a;
                        break;
                    case 'l':
                        i_f |= l;
                        break;
                    case 'R':
                        i_f |= R;
                        break;
                    case 't':
                        i_f |= t;
                        break;
                    case 'r':
                        i_f |= r;
                        break;
                    case 'i':
                        i_f |= I;
                        break;
                    case 's':
                        i_f |= s;
                        break;
                    default:
                        fprintf(stderr, "%c参数错误!\n", argv[i][j]);
                        break;
                }
            }
        }
    }
    if (dirlen == 0) {
        dirlen = 1;
        char* tempdirname = (char*)malloc(sizeof(char) * 2048);
        strcpy(tempdirname, ".");
        dirname[0] = tempdirname;
    }
}
void l_i(char filename[]) {
    struct stat info;
    if (stat(filename, &info) == -1)
        perror(filename);
    printf("%llu\t", info.st_ino);
}
void l_s(char filename[]) {
    struct stat info;
    if (stat(filename, &info) == -1)
        perror(filename);
    printf("%4llu\t", info.st_size / 4096 * 4 + (info.st_size % 4096 ? 4 : 0));
}
int l_name(char dirname[]) {
    int i = 0;
    int len = 0;
    DIR* dir_ptr;
    struct dirent* direntp;
    if ((dir_ptr = opendir(dirname)) == NULL) {
        fprintf(stderr, "权限不够,cannot open: %s\n", dirname);
        return -1;
    } else {
        while ((direntp = readdir(dir_ptr))) {
            restored_ls(direntp);
        }
        sort(filenames, 0, file_cnt - 1);
    }
    printf("\n");
    closedir(dir_ptr);
    return 1;
}
void sort(char** filenames, int start, int end) {
    if (start < end) {
        int position = partition(filenames, start, end);
        sort(filenames, start, position - 1);
        sort(filenames, position + 1, end);
    }
}
int partition(char** filenames, int start, int end) {
    if (!filenames)
        return -1;
    char* privot = filenames[start];
    while (start < end) {
        while (start < end && compare(privot, filenames[end]) < 0)
            --end;
        swap(&filenames[start], &filenames[end]);
        while (start < end && compare(privot, filenames[start]) >= 0)
            ++start;
        swap(&filenames[start], &filenames[end]);
    }
    return start;
}
void swap(char** s1, char** s2) {
    char* tmp = *s1;
    *s1 = *s2;
    *s2 = tmp;
}
int compare(char* s1, char* s2) {
    if (*s1 == '.')
        s1++;
    if (*s2 == '.')
        s2++;
    while (*s1 && *s2 && *s1 == *s2) {
        ++s1;
        ++s2;
        if (*s1 == '.')
            s1++;
        if (*s2 == '.')
            s2++;
    }
    return *s1 - *s2;
}
void restored_ls(struct dirent* cur_item) {
    char* result = (char*)malloc(sizeof(char) * 4096);
    strcpy(result, cur_item->d_name);
    filenames[file_cnt++] = result;
}
void mode_to_letters(int mode, char str[]) {
    strcpy(str, "----------");
    if (S_ISDIR(mode))
        str[0] = 'd';
    if (S_ISCHR(mode))
        str[0] = 'c';
    if (S_ISBLK(mode))
        str[0] = 'b';
    if (mode & S_IRUSR)
        str[1] = 'r';
    if (mode & S_IWUSR)
        str[2] = 'w';
    if (mode & S_IXUSR)
        str[3] = 'x';

    if (mode & S_IRGRP)
        str[4] = 'r';
    if (mode & S_IWGRP)
        str[5] = 'w';
    if (mode & S_IXGRP)
        str[6] = 'x';

    if (mode & S_IROTH)
        str[7] = 'r';
    if (mode & S_IWOTH)
        str[8] = 'w';
    if (mode & S_IXOTH)
        str[9] = 'x';
}
char* gid_to_name(gid_t gid) {
    struct group *getgrgid(), *grp_ptr;
    static char numstr[10];
    if ((grp_ptr = getgrgid(gid)) == NULL) {
        sprintf(numstr, "%d", gid);
        return numstr;
    } else {
        return grp_ptr->gr_name;
    }
}
char* uid_to_name(gid_t uid) {
    struct passwd* getpwuid();
    struct passwd* pw_ptr;
    static char numstr[10];
    if ((pw_ptr = getpwuid(uid)) == NULL) {
        sprintf(numstr, "%d", uid);
        return numstr;
    } else {
        return pw_ptr->pw_name;
    }
}
void l_t(char** filenames) {
    char temp[2048] = {0};
    struct stat info1;
    struct stat info2;
    for (int i = 0; i < file_cnt - 1; i++) {
        for (int j = i + 1; j < file_cnt; j++) {
            stat(filenames[i], &info1);
            stat(filenames[j], &info2);
            if (info1.st_mtime < info2.st_mtime) {
                strcpy(temp, filenames[i]);
                strcpy(filenames[i], filenames[j]);
                strcpy(filenames[j], temp);
            }
        }
    }
}
void l_r(char** arr, int file_cnt) {
    char left = 0;              
    char right = file_cnt - 1;  
    char* temp;
    while (left < right) {
        char* temp = arr[left];
        arr[left] = arr[right];
        arr[right] = temp;
        left++;   
        right--; 
    }
}
