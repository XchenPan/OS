#include "fs.h"

int main(int argc, char const *argv[])
{
    ngx_queue_t list;
    ngx_queue_init(&list);

    MFD *u1 = create_root_node("root", "2002");
    MFD *u2 = create_root_node("bapie", "2002");

    create_folder(u1->user);
    create_folder(u2->user);

    insert_root_node(&list, u1);
    insert_root_node(&list, u2);

    // char op;
    // while (1) {
    //     printf("--------------------欢迎登陆bapie的文件系统--------------------\n");
    //     printf("L-登陆用户, C-创建用户, D-删除用户, P-打印用户, Q-关闭系统\n");
    //     scanf(" %c", &op);
    //     switch (op) {
    //     case 'L':
    //         printf("登陆用户\n");
    //         if (login(&list)) {
    //             printf("Access granted.\n");
    //             printf("C-创建文件/文件夹, D-删除文件/文件夹, E-编辑文件, P-打印目录, Q-注销用户\n");
    //             char Lop;
    //             scanf(" %c", &op);
    //             while (1) {
    //                 switch (Lop) {
    //                 case 'C':
    //                     char *name;
    //                     printf("请输入文件名/文件夹\n");
    //                     scanf("%s", name);
    //                     UFD *node = create_sub_node(u2, "bin", "d");
    //                     break;
                    
    //                 default:
    //                     break;
    //                 }
    //             }
                
    //         }
    //         else
    //             printf("Access denied.\n");
    //         break;
    //     case 'C':
    //         printf("创建用户\n");
    //         break;
    //     case 'D':
    //         printf("删除用户\n");
    //         break;
    //     case 'Q':
    //         printf("关闭系统\n");
    //         break;
    //     default:
    //         printf("未知操作\n");
    //         break;
    //     }
    // }

    UFD *sn1 = create_sub_node(u2, "bin", "d");
    UFD *sn2 = create_sub_node(u2, "etc", "d");
    insert_sub_node(&u2->root->children, sn1);
    insert_sub_node(&u2->root->children, sn2);
    

    ngx_queue_t *q;
    UFD *node;
    cd(u2->user);
    ngx_queue_foreach(q, &u2->root->children) {
        node = ngx_queue_data(q, UFD, children);
        create_folder(node->name);
        cd(node->name);
        printf("%s\n", node->name);
        // UFD *f1 = create_sub_node(u2, "hello", "txt");
        // insert_file_node(&node->children, f1);
    }
    return 0;
}