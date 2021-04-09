#ifndef __0_H__
#define __0_H__

#include<stdio.h>
#include<string.h>
#include<sqlite3.h>
#include<signal.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<time.h>
#define DATABASE "p.db"
//员工打卡信息
typedef struct MSG_on{
  int id;
  char name[32];
  int number;
  char date[128];
}__attribute__((packed)) msg_on;
typedef struct MSG_down{
  int id;
  char name[32];
  int number;
  char date[128];
}__attribute__((packed)) msg_down;
typedef struct MSG{
  int type;
  char name[32];
  int pass;
  char data[128];
}__attribute__((packed)) msg;
int on;//员工上班人数
int down;//员工下班人数
int number_on[100]={0};//上班打卡计数
int number_down[100]={0};//下班打卡计数

//Server
int build_sql(sqlite3 *p);
int into_client(int jstfd,sqlite3 *p);
int s_register(int jstfd,msg *person,sqlite3 *p);
int s_into(int jstfd,msg *person,sqlite3 *p);
int s_admin_interface(int jstfd,sqlite3 *p);
int s_user_interface(int jstfd,sqlite3 *p);
void show_time(char *date);
void s_input_on(int jstfd,sqlite3 *p);
void s_input_down(int jstfd,sqlite3 *p);
void s_show_on(int jstfd,int aaa,sqlite3 *p);
void s_show_down(int jstfd,int aaa,sqlite3 *p);
void s_search_on(int jstfd,sqlite3 *p);
void s_search_down(int jstfd,sqlite3 *p);
int s_delete_on(int jstfd,sqlite3 *p);
int s_delete_down(int jstfd,sqlite3 *p);
int s_delete_down(int jstfd,sqlite3 *p);
int s_revise(int jstfd,int aaa,sqlite3 *p);

//Client
int p_register(int sock,msg *person,sqlite3 *p);
int p_into(int sock,msg *person,sqlite3 *p);
int admin_interface(int sock,msg *person,sqlite3 *p);
int user_interface(int sock,msg *person,sqlite3 *p);
void show_time(char *date);
int input_on(int sock,int aaa,msg_on *msgon);
int input_down(int sock,int aaa,msg_down *msgdown);
int show_on(int sock,msg_on *msgon);
int show_down(int sock,msg_down *msgdown);
int search_on(int sock,msg_on *msgon);
int search_down(int sock,msg_down *msgdown);
int delete_on(int sock,msg_on *msgon);
int delete_down(int sock,msg_down *msgdown);
int revise(int sock,int aaa,msg *person);

#endif

