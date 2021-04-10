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

#define  DATABASE  "p.db"  //数据库名称

#define  SUCCESS     0  //返回真
#define  FAIL        -1 //返回假
#define  TRUE        1  //真
#define  FALSE       -1 //假
#define  ADMIN       1  //管理员权限
#define  USER        0  //用户权限
#define  INTO_ADMIN  1  //进入管理员界面
#define  INTO_USER   2  //进入用户界面

#define  REGISTER       1  //注册
#define  LOG_ON         2  //登录
#define  LEAVE          3  //离开
#define  CHAR_REGISTER  49  //注册
#define  CHAR_LOG_ON    50  //登录
#define  CHAR_LEAVE     51  //离开

#define  REVISE_INFO_U   5
#define  WIN10_TXT_LINE  14    //win10.txt文件行数
#define  NAME_LENGH     32    //用户名长度
#define  HUNDRED        100   //100
#define  DATA_LENGH     128   //数据长度
#define  BUF_LENGH      255   //缓冲区大小
#define  SQL_LENGH      256   //SQL语句长度大小

//员工上班打卡信息
typedef struct MSG_on{
  int id;               //职工号
  char name[NAME_LENGH];//职工姓名
  int number;           //打卡次数
  char date[DATA_LENGH];//打卡日期
}msg_on;

//员工上班打卡信息
typedef struct MSG_down{
  int id;               //职工号
  char name[NAME_LENGH];//职工姓名
  int number;           //打卡次数
  char date[DATA_LENGH];//打卡日期
}msg_down;

//员工下班打卡信息
typedef struct MSG{
  int type;             //功能标志位
  char name[NAME_LENGH];//用户名/职工姓名
  int pass;             //密码/职工号
  char data[DATA_LENGH];//数据
}msg;

//switch()函数中的功能选项,char类型
enum NUMBER_CHAR{
  QUIT=48,          //离开系统
  WORK_START,       //上班打卡
  WORK_END,         //下班打卡
  SHOW_WORK_START,  //上班人员信息
  SHOW_WORK_END,    //下班人员信息
  SEARCH_WORK_START,//查询指定上班人员信息
  SEARCH_WORK_END,  //查询指定下班人员信息
  DELETE_WORK_START,//删除上班员工信息
  DELETE_WORK_END,  //删除下班员工信息
  REVISE_INFO       //修改员工信息
};
//数字标记
enum NUMBER{
  ZERO=0,ONE,TWO,THREE,FOUR,
  FIVE,SIX,SEVENT,EIGHT,NINE
};

int on;  //员工上班人数
int down;//员工下班人数
int number_on[HUNDRED]={ZERO};  //上班打卡计数
int number_down[HUNDRED]={ZERO};//下班打卡计数

/*=====================server.c==================*/

/*功能：构建数据库
　参数：p:指定数据库
　 返回值：成功返回０，失败-1*/
int build_sql(sqlite3 *p);

/*功能：进入服务器
　参数：
　   jstfd：用于通信的文件描述符
　   p:指定数据库
　 返回值：成功返回０，失败-1
　 注意：选择LEAVE会直接退出用户端*/
int into_server(int jstfd,sqlite3 *p);

/*功能：注册信息插入数据库
　参数：
　   jstfd：用于通信的文件描述符
　   person：员工信息结构体
　   p:指定数据库
　 返回值：成功返回０，失败-1*/
int s_register(int jstfd,msg *person,sqlite3 *p);

/*功能：查询登录信息
　参数：
　   jstfd：用于通信的文件描述符
　   person：员工信息结构体
　   p:指定数据库
　 返回值：成功返回０，失败-1
　 注意：拥有管理员权限的职工号是：１*/
int s_into(int jstfd,msg *person,sqlite3 *p);

/*功能：打印管理员页面
　参数：
　   jstfd：用于通信的文件描述符
　   p:指定数据库
　 返回值：成功返回０，失败-1*/
int s_admin_interface(int jstfd,sqlite3 *p);

/*功能：打印普通用户页面
　参数：
　   jstfd：用于通信的文件描述符
　   p:指定数据库
　 返回值：成功返回０，失败-1*/
int s_user_interface(int jstfd,sqlite3 *p);

/*功能：显示当前时间
　参数：date:日期结构体
　 返回值：无*/
void show_time(char *date);

/*功能：员工上班打卡信息插入数据库
　参数：
　   jstfd：用于通信的文件描述符
　   p:指定数据库
　 返回值：无*/
void s_input_on(int jstfd,sqlite3 *p);

/*功能：员工下班打卡信息插入数据库
　参数：
　   jstfd：用于通信的文件描述符
　   p:指定数据库
　 返回值：无*/
void s_input_down(int jstfd,sqlite3 *p);

/*功能：显示上班员工信息并插入on.txt文件
　参数：
　   jstfd：用于通信的文件描述符
　   aaa：权限标记
　   p:指定数据库
　 返回值：无*/
void s_show_on(int jstfd,int aaa,sqlite3 *p);

/*功能：显示下班员工并插入down.txt文件
　参数：
　   jstfd：用于通信的文件描述符
　   aaa：权限标记
　   p:指定数据库
　 返回值：无*/
void s_show_down(int jstfd,int aaa,sqlite3 *p);

/*功能：查询员工上班信息
　参数：
　   jstfd：用于通信的文件描述符
　   p:指定数据库
　 返回值：无*/
void s_search_on(int jstfd,sqlite3 *p);

/*功能：查询员工上班信息
　参数：
　   jstfd：用于通信的文件描述符
　   p:指定数据库
　 返回值：无*/
void s_search_down(int jstfd,sqlite3 *p);

/*功能：操作数据库删除员工上班
　参数：
　   jstfd：用于通信的文件描述符
　   p:指定数据库
　 返回值：成功返回０，失败-1*/
int s_delete_on(int jstfd,sqlite3 *p);

/*功能：操作数据库删除员工下班
　参数：
　   jstfd：用于通信的文件描述符
　   p:指定数据库
　 返回值：成功返回０，失败-1*/
int s_delete_down(int jstfd,sqlite3 *p);

/*功能：操作数据库修改员工信息
　参数：
　   jstfd：用于通信的文件描述符
　   aaa：权限标记
　   p:指定数据库
　 返回值：成功返回０，失败-1
　 注意：职工号具有唯一性，修改的是员工姓名*/
int s_revise(int jstfd,int aaa,sqlite3 *p);
/*================Server_end===================*/


/*=====================client.c==================*/

/*功能：打印客户端界面
　参数：
　   sock：文件描述符
　   p:指定数据库
　 返回值：成功返回０，失败-1*/
int into_client(int sock,sqlite3 *p);

/*功能：发送员工信息结构体，向服务器请求注册
　参数：
　   sock：文件描述符
　   person：员工信息结构体
　   p:指定数据库
　 返回值：成功返回０，失败-1
　 注意：有三次注册机会*/
int p_register(int sock,msg *person,sqlite3 *p);

/*功能：发送员工信息结构体，向服务器请求登录
　参数：
　   sock：文件描述符
　   person：员工信息结构体
　   p:指定数据库
　 返回值：成功返回０，失败-1
　 注意：有三次登录机会*/
int p_into(int sock,msg *person,sqlite3 *p);

/*功能：打印管理员界面
　参数：
　   sock：文件描述符
　   person：员工信息结构体
　   p:指定数据库
　 返回值：成功返回０，失败-1*/
int admin_interface(int sock,msg *person,sqlite3 *p);

/*功能：打印普通用户页面
　参数：
　   sock：文件描述符
　   person：员工信息结构体
　   p:指定数据库
　 返回值：成功返回０，失败-1*/
int user_interface(int sock,msg *person,sqlite3 *p);

/*功能：获取当前日期
　参数：date：日期指结构体
　 返回值：无*/
void show_time(char *date);

/*功能：向服务器发送套接字，请求上班打卡
　参数：
　   sock：文件描述符
　   aaa：权限标记
　   msgon：员工上班打卡信息结构体
　 返回值：成功返回０，失败-1*/
int input_on(int sock,int aaa,msg_on *msgon);

/*功能：向服务器发送套接字，请求下班打卡
　参数：
　   sock：文件描述符
　   aaa：权限标记
　   msgdown：员工下班打卡信息结构体
　 返回值：成功返回０，失败-1*/
int input_down(int sock,int aaa,msg_down *msgdown);

/*功能：向服务器发送套接字，读取on.txt中的上班打卡信息
　参数：
　   sock：文件描述符
　   msgon：员工上班打卡信息结构体
　 返回值：成功返回０，失败-1*/
int show_on(int sock,msg_on *msgon);

/*功能：向服务器发送套接字，读取ｄown.txt中的下班打卡信息
　参数：
　   sock：文件描述符
　   msgdown：员工下班打卡信息结构体
　 返回值：成功返回０，失败-1*/
int show_down(int sock,msg_down *msgdown);

/*功能：向服务器发送套接字，查询上班打卡员工信息
　参数：
　   sock：文件描述符
　   msgon：员工上班打卡信息结构体
　 返回值：成功返回０，失败-1*/
int search_on(int sock,msg_on *msgon);

/*功能：向服务器发送套接字，查询下班打卡员工信息
　参数：
　   sock：文件描述符
　   msgdown：员工下班打卡信息结构体
　 返回值：成功返回０，失败-1*/
int search_down(int sock,msg_down *msgdown);

/*功能：向服务器发送套接字，请求删除指定员工上班打卡记录
　参数：
　   sock：文件描述符
　   msgon：员工上班打卡信息结构体
　 返回值：成功返回０，失败-1*/
int delete_on(int sock,msg_on *msgon);

/*功能：向服务器发送套接字，请求删除指定员工下班打卡记录
　参数：
　   sock：文件描述符
　   msgdown：员工下班打卡信息结构体
　 返回值：成功返回０，失败-1*/
int delete_down(int sock,msg_down *msgdown);

/*功能：向服务器发送套接字，请求修改职工号对应的员工姓名
　参数：
　   sock：文件描述符
　   person：用户信息结构体
　 返回值：成功返回０，失败-1*/
int revise(int sock,int aaa,msg *person);
/*================Client_end=================*/

#endif

