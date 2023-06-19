#ifndef __FS_H_
#define __FS_H_

#include <stdio.h>
#include <time.h>
#include <memory.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "queue.h"

typedef struct _fcb
{
    char *permission; // drwxrwx
    char *create_date;
    char *access_date;
    char *modify_date;
    char *user;
    int size;
} FCB;

typedef struct _ufd // 用户目录
{
    char *name;
    char *type;
    int length;
    FCB *fcb;
    ngx_queue_t children;
    struct _ufd *file;
} UFD;

typedef struct _mfd
{
    char *user;
    char *pswd;
    UFD *root;
    ngx_queue_t list;
} MFD;

char *get_time() {
    time_t t = time(NULL);
    struct tm *tm_ptr = localtime(&t);
    char *time_str = (char*)malloc(20 * sizeof(char));
    strftime(time_str, 20, "%Y-%m-%d %H:%M:%S", tm_ptr);
    return time_str;
}

MFD *create_root_node(char *user, char *pswd) {
    char *time_str = get_time();

    MFD *node = (MFD*)malloc(sizeof(MFD));
    node->root = (UFD*)malloc(sizeof(UFD));
    node->root->fcb = (FCB*)malloc(sizeof(FCB));

    node->user = user;
    node->pswd = pswd;

    node->root->name = user;
    node->root->type = "d";
    node->root->length = 0;

    node->root->fcb->permission = "drwxrwx";
    node->root->fcb->user = user;
    node->root->fcb->size = 0;
    node->root->fcb->create_date = time_str;
    node->root->fcb->access_date = time_str;
    node->root->fcb->modify_date = time_str;
    node->root->file = NULL;
    
    ngx_queue_init(&node->list);
    ngx_queue_init(&node->root->children);
    node->root->file = NULL;

    free(time_str);
    return node;
}

UFD *create_sub_node(MFD *root, char *name, char *type) {
    char *time_str = get_time();

    UFD *node = (UFD*)malloc(sizeof(UFD));
    node->fcb = (FCB*)malloc(sizeof(FCB));

    node->name = name;
    node->type = type;
    node->length = 0;

    node->fcb->permission = "drwxrwx";
    node->fcb->user = root->user;
    node->fcb->size = 0;
    node->fcb->create_date = time_str;
    node->fcb->access_date = time_str;
    node->fcb->modify_date = time_str;
    
    ngx_queue_init(&node->children);
    node->file = NULL;

    free(time_str);
    return node;
}

// UFD *create_file_node(UFD *dir, char *name, char *type) {
//     char *time_str = get_time();

//     UFD *node = (UFD*)malloc(sizeof(UFD));
//     node->fcb = (FCB*)malloc(sizeof(FCB));

//     node->name = name;
//     node->type = type;
//     node->length = 0;

//     node->fcb->permission = "drwxrwx";
//     node->fcb->user = dir->name;
//     node->fcb->size = 0;
//     node->fcb->create_date = time_str;
//     node->fcb->access_date = time_str;
//     node->fcb->modify_date = time_str;

//     ngx_queue_init(&node->children);
//     node->file = NULL;

//     free(time_str);
//     return node;
// }

int insert_root_node(ngx_queue_t *list, MFD *node) {
    if (node == NULL)
        return 0;
    ngx_queue_insert_tail(list, &node->list);
    return 1;
}

int insert_sub_node(ngx_queue_t *root, UFD *node) {
    if (node == NULL)
        return 0;
    ngx_queue_insert_tail(root, &node->children);
    return 1;
}

int insert_file_node(ngx_queue_t *dir, UFD *node) {
    if (node == NULL)
        return 0;
    ngx_queue_insert_tail(dir, &node->children);
    return 1;
}

int delete_root_node(ngx_queue_t *list, char *name) {
    ngx_queue_t *q;
    MFD *node;
    if (ngx_queue_empty(list))
        return -1;
    ngx_queue_foreach(q, list) {
        node = ngx_queue_data(q, MFD, list);
        if (node->user == name) {
            ngx_queue_remove(q);
            free(node); break;
        }
    }
    return 0;
}

int delete_sub_node(ngx_queue_t *root, char *name) {
    ngx_queue_t *q;
    UFD *node;
    if (ngx_queue_empty(root))
        return -1;
    ngx_queue_foreach(q, root) {
        node = ngx_queue_data(q, UFD, children);
        if (node->name == name) {
            ngx_queue_remove(q);
            free(node); break;
        }
    }
    return 0;
}

char *get_cwd() {
    char *cwd = (char*)malloc(1024 * sizeof(char));
    if (getcwd(cwd, 1024) == NULL) {
        perror("Failed to get current working directory");
        return NULL;
    }
    return cwd;
}

int create_folder(char *fdname) {
    char path[1024];
    sprintf(path, "%s/%s", get_cwd(), fdname);

    if (mkdir(path, 0777) != 0) {
        perror("Failed to create folder");
        return 1;
    }
    return 0;
}

void write_file(FILE *fp) {
    char str[100];
    printf("请输入写入的内容\n");
    fgets(str, sizeof(str), stdin);
    fputs(str, fp);
}

int open_file(char *fname, char *mode) {    //mode  'r', 'a'
    char path[1024];
    sprintf(path, "%s/%s", get_cwd(), fname);

    FILE *fp = fopen(path, mode);
    if (fp == NULL) {
        perror("Failed to open file");
        return 1;
    }
    if (strcmp(mode, "a") == 0)
        write_file(fp);
    fclose(fp);
    return 0;
}

int rm(char *name) {
    char path[1024];
    sprintf(path, "%s/%s", get_cwd(), name);

    int ret = remove(path);
    if (ret != 0) {
        perror("Failed to delete folder");
        return 1;
    }
    return 0;
}

int cd(char *path) {
    if (chdir(path) == -1) {
        printf("Failed to change directory.\n");
        return 1;
    }
    return 0;
}

int login(ngx_queue_t *list) {
    char username[20], password[20];
    
    printf("Enter your username: ");
    scanf("%s", username);
    printf("Enter your password: ");
    scanf("%s", password);
    
    ngx_queue_t *q;
    MFD *node;

    ngx_queue_foreach(q, list) {
        node = ngx_queue_data(q, MFD, list);
        if (strcmp(node->user, username) == 0 && strcmp(node->pswd, password) == 0) {
            printf("Welcome, %s!\n", username);
            cd(username);
            return 1;
        }
    }
    printf("Invalid username or password.\n");
    return 0;
}

#endif
