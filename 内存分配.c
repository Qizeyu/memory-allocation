#include <stdio.h>
#include <stdlib.h>
#define FREE 0
#define BUSY 1
#define OK 1
#define ERROR 0
#define MIN 10  //不再分割的分区大小
#define MAXLENGTH 1024 //内存最大容量

//定义一个空闲表
typedef struct freearea
{
    int size;
    int address;
    int state;
}ElemType;

//双向链表存储结构
typedef struct DuLNode
{
    ElemType data;
    struct DuLNode *prior;
    struct DuLNode *next;
}DuLNode,*DuLinkList;

DuLinkList blockfirst;
DuLinkList blocklast;

//初始化内存空间
int Initblock()
{
    blockfirst = (DuLinkList)malloc(sizeof(DuLNode));
    blocklast = (DuLinkList)malloc(sizeof(DuLNode));
    blockfirst->prior = NULL;
    blockfirst->next = blocklast;
    blocklast->prior = blockfirst;
    blocklast->next = NULL;
    blocklast->data.size = MAXLENGTH;
    blocklast->data.address = 0;
    blocklast->data.state = FREE;
    return OK;
}

//首次适应算法
int firstfit(int request)
{
    int ch;  //记录最小剩余空间
    DuLinkList temp = (DuLinkList)malloc(sizeof(DuLNode));
    temp->data.size = request;
    temp->data.state = BUSY;
    DuLinkList p = blockfirst->next;
    DuLinkList q = NULL; //记录最佳插入位置
    while(p != NULL)
    {
        if(p->data.state == FREE && (p->data.size >= request))
        {
            if(q == NULL)
            {
                q = p;
                ch = p->data.size - request;
            }
            else if(q->data.address > p->data.address)
            {
                q = p;
                ch = p->data.size - request;
            }
        }
        p = p->next;
    }

    if(q == NULL) //没有找到空闲块
        return ERROR;
    else if(ch <= MIN)
    {
        q->data.state = BUSY;
        return OK;
    }else{
        temp->prior = q->prior;
        temp->next=q;
        temp->data.address=q->data.address;
        q->prior->next=temp;
        q->prior=temp;
        q->data.address+=request;
        q->data.size=ch;
        return OK;
    }
    return OK;
}

//最佳适应算法
int bestfit(int request)
{
    int ch;  //记录最小剩余空间
    DuLinkList temp = (DuLinkList)malloc(sizeof(DuLNode));
    temp->data.size = request;
    temp->data.state = BUSY;
    DuLinkList p = blockfirst->next;
    DuLinkList q = NULL; //记录最佳插入位置
    while(p != NULL)  //初始化最小空间和最佳位置
    {
        if(p->data.state == FREE && (p->data.size >= request))
        {
            if(q == NULL)
            {
                q = p;
                ch = p->data.size - request;
            }
            else if(q->data.size > p->data.size)
            {
                q = p;
                ch = p->data.size - request;
            }
        }
        p = p->next;
    }

    if(q == NULL) //没有找到空闲块
        return ERROR;
    else if(ch <= MIN)
    {
        q->data.state = BUSY;
        return OK;
    }else{
        temp->prior = q->prior;
        temp->next=q;
        temp->data.address=q->data.address;
        q->prior->next=temp;
        q->prior=temp;
        q->data.address+=request;
        q->data.size=ch;
        return OK;
    }
    return OK;
}

//最差适应算法
int worstfit(int request)
{
    int ch; //记录最大剩余空间
    DuLinkList temp = (DuLinkList)malloc(sizeof(DuLNode));
    temp->data.size = request;
    temp->data.state = BUSY;
    DuLinkList p = blockfirst->next;
    DuLinkList q = NULL; //记录最佳插入位置
    while(p != NULL) //初始化最大空间和最佳位置
    {
        if(p->data.state == FREE && (p->data.size >= request))
        {
            if(q == NULL)
            {
                q = p;
                ch = p->data.size - request;
            }
            else if(q->data.size < p->data.size)
            {
                q = p;
                ch = p->data.size - request;
            }
        }
        p = p->next;
    }

    if(q == NULL) //没有找到空闲块
        return ERROR;
    else if(ch <= MIN)
    {
        q->data.state = BUSY;
        return OK;
    }else{
        temp->prior = q->prior;
        temp->next=q;
        temp->data.address=q->data.address;
        q->prior->next=temp;
        q->prior=temp;
        q->data.address+=request;
        q->data.size=ch;
        return OK;
    }
    return OK;
}

//分配内存
int mallc(int ch)
{
    int request=0;
    printf("请输入需要分配的内存大小：");
    scanf("%d",&request);
    if(request < 0 || 0 == request)
    {
        printf("分配的大小不合适！！请重试.\n");
        return ERROR;
    }
    if(ch == 1)
    {
        if(firstfit(request) == OK)
            printf("分配成功\n");
        else
            printf("内存不足，分配失败\n");
        return OK;
    }
    if(ch == 2)
    {
        if(bestfit(request) == OK)
            printf("分配成功\n");
        else
            printf("内存不足，分配失败\n");
        return OK;
    }
    if(ch == 3)
    {
        if(worstfit(request) == OK)
            printf("分配成功\n");
        else
            printf("内存不足，分配失败\n");
        return OK;
    }
}

//内存回收
int recycle(int flag)
{
    DuLinkList p=blockfirst;
    for(int i=0;i<=flag;i++)
    {
        if(p != NULL)
            p = p->next;
        else
            return ERROR;
    }
    p->data.state = FREE;
    if(p->prior != blockfirst && (p->prior->data.state == FREE)) //与前面的空闲块相连
    {
        p->prior->data.size += p->data.size;
        p->prior->next = p->next;
        p->next->prior = p->prior;
        p = p->prior;
    }
    if(p->next != blocklast && (p->next->data.state == FREE)) //与后面的空闲块相连
    {
        p->data.size += p->next->data.size;
        p->next->next->prior = p;
        p->next = p->next->next;
    }
    if(p->next == blocklast && (p->next->data.state == FREE)) //与最后的空闲块相连
    {
        p->data.size += p->next->data.size;
        p->next = NULL;
    }
    return OK;
}

//显示内存分配情况
void show()
{
    int flag = 0;
    printf("----------------内存分配情况表---------------\n");
    printf("|分区号\t起始地址\t分区大小\t状态|\n");
    DuLinkList p=blockfirst->next;
    while(p != NULL)
    {
        printf("|  %d\t",flag++);
        printf("   %d\t\t",p->data.address);
        printf("  %d\t\t",p->data.size);
        if(p->data.state == FREE)
            printf("空闲|\n");
        else
            printf("分配|\n");
        p = p->next;
    }
    printf("---------------------------------------------\n");
}

//主函数
int main()
{
    int choice; //算法选择标记
    index:printf("1)首次适应算法\n2)最佳适应算法\n3)最坏适应算法\n");
    printf("请输入所使用的内存分配算法：");
    scanf("%d",&choice);
    while(choice<1 || choice>2)
    {
        printf("输入错误！！请重新输入所使用的内存分配算法：\n");
        scanf("%d",&choice);
    }
    Initblock();
    int ch;
    while(1)
    {
        show();
        printf("\n1: 分配内存\n2: 回收内存\n3: 返回上一层\n0: 退出\n\n请输入您的操作：");
        scanf("%d",&ch);
        if(ch == 1)
            mallc(choice);
        else if(ch == 2){
            int flag;
            printf("请输入您要回收的内存的分区号：");
            scanf("%d",&flag);
            recycle(flag);
        }else if(ch == 3){
            goto index;
        }else if(ch == 0){
            break;
        }else{
            printf("输入有误，请重试！！\n");
            continue;
        }
    }
    return 0;
}
