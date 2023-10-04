#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fs.h"

#define OPEN_FAILED -1
#define MAX_PATH_SIZE 512

// '.' Matches any single character.​​​​
// '*' Matches zero or more of the preceding element.
// aabb .a.b
int 
match_path(char * s, char * p) 
{
    if (*(p + 1) != '*')
        return *s == *p || (*p == '.' && *s != '\0') ? match_path(s+1, p+1) : 0;
    return *s == *p || (*p == '.' && *s != '\0') ? match_path(s, p + 2) || match_path(s + 1, p) : match_path(s, p + 2);
}

int
open_file_dir_for_scan(char* path, struct stat* st) 
{
    int fd;
    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        return OPEN_FAILED;
    }

    if(fstat(fd, st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        goto cleanup;
    }

    return fd;
cleanup:
    close(fd);
    return OPEN_FAILED;
}

void
compile_dir_path(char * path, char * buf, char * name) 
{
    memcpy(buf, path, strlen(path));
    char *p = buf + strlen(path);
    *p++ = '/';
    memcpy(p, name, strlen(name));
    p += strlen(name);
    *p++ = 0;
}

void
scan_file_dir_pattern(int fd, char * path, char * target, struct stat* st)
{
    struct dirent de;
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        if(de.inum == 0)
            continue;

        char buf[512];
        compile_dir_path(path, buf, de.name);        

        if(stat(buf, st) < 0){
            printf("find: cannot stat %s\n", buf);
            continue;
        }
        
        if (strcmp(target, de.name) == 0) {
            printf("%s\n", buf);
        }

        if (strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0)
        {
            find(buf, target);
        }
    }
}


void 
find(char *path, char * target) 
{    
    int fd;

    struct stat st;
    if ((fd = open_file_dir_for_scan(path, &st)) < 0) return;
    
    switch(st.type) {
    case T_FILE:
        if (match_path(path, target)) 
            printf("%s\n", path);
        break;

    case T_DIR:
        if (strlen(path) + 1 + DIRSIZ + 1 > MAX_PATH_SIZE) {
            printf("find: path too long\n");
            break;
        }
        scan_file_dir_pattern(fd, path, target, &st);
        break;
    }
    close(fd);
}

int
main(int argc, char *argv[]) 
{
    if (argc != 3)
    {
        printf("find: not enough args\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);    
}