//gcc server.c -lsqlite3 -o server
//./server 192.168.3.129

//用户名：员工姓名
//密码　：工号(唯一标识)

#include "0.h"

int main(int argc, const char *argv[])
{
  int n;
  int jstfd;//接收套接字
  int sockfd;//套接字
  int reuse=0;//标志位
  sqlite3 *p=NULL;
  sqlite3_open("p.db",&p);//打开数据库
  pid_t pid;
  build_sql(p);//构建数据库
  //printf("%s","\033[1H\033[2J");//页面置顶
  //创建套接字，返回文件描述符
  if((sockfd=socket(AF_INET,SOCK_STREAM,0))<ZERO)
  {
		perror("sockfd\n");
		return FAIL;
	}
  //允许地址端口快速重用
  if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int))<ZERO)
  {
		perror("setsockopt");
		exit(1);
	}
  struct sockaddr_in addr;
	bzero(&addr,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=inet_addr(argv[1]);
	addr.sin_port=htons(atoi("8080"));
	//监听套接字
	if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr))<ZERO)
	{
		perror("bind\n");
		return FAIL;
	}
	//最多监听五个
  if(listen(sockfd,5)<ZERO)
  {
		perror("listen\n");
		return FAIL;
	}
  while(TRUE)
  {
    //接收用于通信的文件描述符
    if((jstfd=accept(sockfd,NULL,NULL))<ZERO)
    {
			perror("accept");
			return FAIL;
		}
		if((pid=fork())<ZERO)
		{
			perror("fork");
			return FAIL;
		}
		else if(pid==ZERO)
		{
			close(sockfd);
			into_server(jstfd,p);//进入服务器
		}
		else
		{
		  close(jstfd);
		}
  }
  if(sqlite3_close(p)!=SUCCESS)
  {
		fprintf(stderr,"数据库关闭失败\n");
		return FAIL;
	}
	printf("数据库关闭成功\n");
	printf("\nHave a good day!\n");
  return SUCCESS;
}

//构建数据库
int build_sql(sqlite3 *p)
{
  if(p==NULL)
  {
    printf("数据库打开失败\n");
    return FAIL;
  }
  else
  {
    printf("开启服务器\n");
    printf("数据库打开成功\n");
  }
  char buf[DATA_LENGH]={};
  char sql[SQL_LENGH]={};
  char *errmsg=NULL;
  bzero(sql,sizeof(sql));//数组置０
  if(sqlite3_exec(p,"create table if not exists card_on(id int,name char,number int,data TEXT )",NULL,NULL,&errmsg)!=SQLITE_OK)
  {
    printf("card_on表创建失败\n\n");
  }
  else 
  {
    printf("card_on表创建成功\n\n");
  }
  if(sqlite3_exec(p,"create table if not exists card_down(id int,name char,number int,data TEXT )",NULL,NULL,&errmsg)!=SQLITE_OK)
  {
    printf("card_down表创建失败\n\n");
  }
  else
  {
    printf("card_down表创建成功\n\n");
  }
  if(sqlite3_exec(p,"create table if not exists user(name text,pass int primary key);",NULL,NULL,&errmsg)!=SQLITE_OK)
  {
    printf("user表创建失败\n\n");
  }
  else
  {
    printf("user表创建成功\n\n");
  }
  return SUCCESS;
}

//进入服务器
int into_server(int jstfd,sqlite3 *p)
{
  if(p==NULL)
  {
    printf("数据库打开失败\n");
    return FAIL;
  }
  char date[DATA_LENGH]={};//show_time
  msg person;
  //printf("%s","\033[1H\033[2J");//页面置顶
  while(recv(jstfd,&person,sizeof(person),0)>ZERO)
  {
    printf("序号：%d\n",person.type);
    switch(person.type)
    {
      case REGISTER:
        s_register(jstfd,&person,p);
        break;
      case LOG_ON: 
        if(s_into(jstfd,&person,p)==INTO_ADMIN)
        {
          s_admin_interface(jstfd,p);
        }
        else if(s_into(jstfd,&person,p)==INTO_USER)
        {
          s_user_interface(jstfd,p);
        }
        break;
      case LEAVE: 
        return SUCCESS;
        break;
    }
    show_time(date);
  }
  printf("一名用户离开\n");
	close(jstfd);
	exit(0);//退出系统
  return SUCCESS;
}

//注册
int s_register(int jstfd,msg *person,sqlite3 *p)
{
  if(p==NULL)
  {
    printf("数据库打开失败\n");
    return FAIL;
  }
  int ret=0;//标志位
  char *errmsg=NULL;//错误信息
  char sql[SQL_LENGH]={};//保存ｓql功能语句
  sprintf(sql,"insert into user values('%s',%d);",person->name,person->pass);
  printf("%s\n",sql);//打印SQL语句
  if(sqlite3_exec(p,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
  {
	  printf("%s\n",errmsg);
	  strcpy(person->data,"注册失败\n");
	  printf("%s\n",person->data);
	 }
	else
	{
		printf("%s注册成功\n\n",person->name);
		strcpy(person->data,"注册成功!");
		printf("%s\n",person->data);
	}
	do{
		ret=send(jstfd,person,sizeof(msg),0);
	}while(ret<0&&EINTR==errno);
	if(ret<0)
	{
	  perror("发送失败\n");
	  return SUCCESS;
  }
	return SUCCESS;
}

//登录
int s_into(int jstfd,msg *person,sqlite3 *p)
{
  if(p==NULL)
  {
    printf("数据库打开失败\n");
    return FAIL;
  }
  int nrow,ncloumn;
  char sql[SQL_LENGH]="";
  char *cwmsg,**resultp;
  char *errmsg=NULL;
   sprintf(sql,"select*from user where name='%s' and pass=%d;",person->name,person->pass);
    printf("%s\n",sql);
    if(sqlite3_get_table(p,sql,&resultp,&nrow,&ncloumn,&errmsg)!=SQLITE_OK)
    {
		  printf("获取失败\n");
		  return SUCCESS;
	  }
	  else printf("成功获取数据表\n");
	  if(nrow==ONE){
		  strcpy(person->data,"登录成功");
		  printf("%s\n",person->data);
		  send(jstfd,person,sizeof(msg),0);
		  //admin
		  if(!strcmp(person->name,"1"))
		  {
		    printf("用户%s登陆成功\n\n",person->name);
		    return INTO_ADMIN;
		  }
		  else if(!strcmp(person->data,"登录成功"))
		  {
		    return INTO_USER;
		  }
	  }
	  if(nrow==0)
	  {
		  strcpy(person->data,"用户名或密码错误\n");
		  printf("%s\n",person->data);
		  send(jstfd,person,sizeof(msg),0);
	  }
  return SUCCESS;
}
//管理员界面
int s_admin_interface(int jstfd,sqlite3 *p){
  if(p==NULL)
  {
    printf("数据库打开失败\n");
    return FAIL;
  }
  msg person;
  printf("\nadmin\n");
  while(recv(jstfd,&person,sizeof(person),0)>ZERO)
  {
    printf("序号：%d\n",person.type);
    switch(person.type)
    {
      case WORK_START:
        s_input_on(jstfd,p);
        break;
      case WORK_END:
        s_input_down(jstfd,p);
        break;
      case SHOW_WORK_START:
        s_show_on(jstfd,ADMIN,p);
        break;
      case SHOW_WORK_END:
        s_show_down(jstfd,ADMIN,p);
        break;
      case SEARCH_WORK_START:
        s_search_on(jstfd,p);
        break;
      case SEARCH_WORK_END:
        s_search_down(jstfd,p);
        break;
      case DELETE_WORK_START:
        s_delete_on(jstfd,p);
        break;
      case DELETE_WORK_END:
        s_delete_down(jstfd,p);
        break;
      case REVISE_INFO:
        s_revise(jstfd,ADMIN,p);
        break;
      case QUIT:
        printf("退出界面\n");
        return SUCCESS;
        break;
     }
  }
  return SUCCESS;
}

//用户界面
int s_user_interface(int jstfd,sqlite3 *p)
{
  if(p==NULL)
  {
    printf("数据库打开失败\n");
    return FAIL;
  }
  msg person;
  printf("\nuser\n");
  while(recv(jstfd,&person,sizeof(person),0)>ZERO)
  {
    printf("序号：%d\n",person.type);
    switch(person.type)
    {
      case WORK_START:
        s_input_on(jstfd,p);
        break;
      case WORK_END:
        s_input_down(jstfd,p);
        break;
      case SHOW_WORK_START:
        s_show_on(jstfd,USER,p);
        break;
      case SHOW_WORK_END:
        s_show_down(jstfd,USER,p);
        break;
      case REVISE_INFO_U:
        s_revise(jstfd,USER,p);
        break;
      case QUIT:
        printf("退出界面\n");
        return SUCCESS;
        break;
    }
  }
  return SUCCESS;
}

//显示时间
void show_time(char *date)
{
  if(date==NULL){
    perror("date");
  }
  time_t t;
  time(&t);
  struct tm *tp=localtime(&t);
  sprintf(date,"%d-%d-%d %d:%d:%d",tp->tm_year+1900,tp->tm_mon+1,tp->tm_mday,tp->tm_hour,tp->tm_min,tp->tm_sec);
	printf("当前日期：%s\n",date);
}

//员工上班打卡信息
void s_input_on(int jstfd,sqlite3 *p)
{
  if(p==NULL)
  {
    printf("数据库打开失败\n");
    return;
  }
  int ret=-1;//标志位
  char *errmsg=NULL;
  char sql[SQL_LENGH]={};//数据库缓冲区
  msg_on msgon;
  if(recv(jstfd,&msgon,sizeof(msg_on),0)<ZERO)
  {
		printf("recv\n");
		return;
	}
  sprintf(sql,"insert into card_on values(%d,'%s',%d,'%s');",msgon.id,msgon.name,msgon.number,msgon.date);
  printf("%s\n",sql);
  if(sqlite3_exec(p,sql,NULL,NULL,&errmsg)==SQLITE_OK)
  {
    printf("上班打卡成功\n\n");
  }
}

//员工下班打卡信息
void s_input_down(int jstfd,sqlite3 *p)
{
  if(p==NULL)
  {
    printf("数据库打开失败\n");
    return;
  }
  int ret=-1;//标志位
  char *errmsg=NULL;
  char sql[SQL_LENGH]={};//数据库缓冲区
  msg_down msgdown;
  if(recv(jstfd,&msgdown,sizeof(msg_down),0)<ZERO)
  {
		printf("recv\n");
		return;
	}
  sprintf(sql,"insert into card_down values(%d,'%s',%d,'%s');",msgdown.id,msgdown.name,msgdown.number,msgdown.date);
  printf("%s\n",sql);
  if(sqlite3_exec(p,sql,NULL,NULL,&errmsg)==SQLITE_OK)
  {
    printf("下班打卡成功\n\n");
  }
}

//显示上班员工信息
void s_show_on(int jstfd,int aaa,sqlite3 *p)
{
  if(p==NULL)
  {
    printf("数据库打开失败\n");
    return;
  }
  if((aaa!=ADMIN)&&(aaa!=USER)){
    perror("权限错误");
    return;
  }
  char **result,*errmsg=NULL;
  char sql[SQL_LENGH]={};//数据库缓冲区
  int nrow,ncolumn,i,j,index;
  msg_on msgon;
  if(recv(jstfd,&msgon,sizeof(msg_on),0)<ZERO)
  {
		printf("recv\n");
		return;
	}
   printf("id\t%d\n",msgon.id);
  if(aaa==USER)
  {
    sprintf(sql,"select*from card_on where id=%d and number=1;",msgon.id);
    sqlite3_get_table(p,sql,&result,&nrow,&ncolumn,&errmsg);
  }
  else if(aaa==ADMIN)
  {
    sqlite3_get_table(p,"select*from card_on where number=1",&result,&nrow,&ncolumn,&errmsg);
  }
  index=ncolumn;
  FILE *fp=fopen("on.txt","w+");
  for(i=ZERO;i<nrow;i++)
  {
    for(j=ZERO;j<ncolumn;j++)
    {
      fprintf(fp,"%-8s:%-8s\n",result[j],result[index++]);
    }
    fprintf(fp,"============================\n");
  }
  fclose(fp);
  sqlite3_free_table(result);
}

//显示下班员工信息
void s_show_down(int jstfd,int aaa,sqlite3 *p)
{
  if(p==NULL)
  {
    printf("数据库打开失败\n");
    return;
  }
  if((aaa!=ADMIN)&&(aaa!=USER))
  {
    perror("权限错误");
    return;
  }
  char **result,*errmsg=NULL;
  char sql[SQL_LENGH]={};//数据库缓冲区
  int nrow,ncolumn,i,j,index;
  msg_down msgdown;
  if(recv(jstfd,&msgdown,sizeof(msg_down),0)<ZERO)
  {
		printf("recv\n");
		return;
	}
  printf("id\t%d\n",msgdown.id);
  if(aaa==USER)
  {
    sprintf(sql,"select*from card_down where id=%d and number=1;",msgdown.id);
    sqlite3_get_table(p,sql,&result,&nrow,&ncolumn,&errmsg);
  }
  else if(aaa==ADMIN)
  {
    sqlite3_get_table(p,"select*from card_down where number=1",&result,&nrow,&ncolumn,&errmsg);
  }
  index=ncolumn;
  FILE *fp=fopen("down.txt","w+");
  for(i=ZERO;i<nrow;i++)
  {
    for(j=ZERO;j<ncolumn;j++)
    {
      fprintf(fp,"%-8s:%-8s\n",result[j],result[index++]);
    }
    fprintf(fp,"============================\n");
  }
  fclose(fp);
  sqlite3_free_table(result);
}

//查询员工上班信息
void s_search_on(int jstfd,sqlite3 *p)
{
  if(p==NULL)
  {
    printf("数据库打开失败\n");
    return;
  }
  char **result,*errmsg=NULL;
  char sql[SQL_LENGH]={};//数据库缓冲区
  int nrow,ncolumn,i,j,index;
  msg_on msgon;
  if(recv(jstfd,&msgon,sizeof(msg_on),0)<ZERO)
  {
		printf("recv\n");
		return;
	}
  sprintf(sql,"select*from card_on where id=%d and number=1;",msgon.id);
  if(sqlite3_get_table(p,sql,&result,&nrow,&ncolumn,&errmsg)!=SUCCESS)
  {
    printf("查询失败\n\n");
  }
  index=ncolumn;
  //将获取的信息写入到文件中
  FILE *fp=fopen("on.txt","w+");
  for(i=ZERO;i<nrow;i++)
  {
    for(j=ZERO;j<ncolumn;j++)
    {
        fprintf(fp,"%-8s:%-8s\n",result[j],result[index++]);
    }
    fprintf(fp,"============================\n");
  }
  fclose(fp);
  sqlite3_free_table(result);
}

//查询员工下班信息
void s_search_down(int jstfd,sqlite3 *p)
{
  if(p==NULL)
  {
    printf("数据库打开失败\n");
    return;
  }
  char **result,*errmsg=NULL;
  char sql[SQL_LENGH]={};//数据库缓冲区
  int nrow,ncolumn,i,j,index;
  msg_down msgdown;
  if(recv(jstfd,&msgdown,sizeof(msg_down),0)<ZERO)
  {
		printf("recv\n");
		return;
	}
  sprintf(sql,"select*from card_down where id=%d and number=1;",msgdown.id);
  if(sqlite3_get_table(p,sql,&result,&nrow,&ncolumn,&errmsg)!=SUCCESS)
  {
    printf("查询失败\n\n");
  }
  index=ncolumn;
   //将获取的信息写入到文件中
  FILE *fp=fopen("down.txt","w+");
  for(i=ZERO;i<nrow;i++)
  {
    for(j=ZERO;j<ncolumn;j++)
    {
        fprintf(fp,"%-8s:%-8s\n",result[j],result[index++]);
    }
    fprintf(fp,"============================\n");
  }
  fclose(fp);
  sqlite3_free_table(result);
}

//删除员工上班信息
int s_delete_on(int jstfd,sqlite3 *p)
{
  if(p==NULL)
  {
    printf("数据库打开失败\n");
    return FAIL;
  }
  char *errmsg=NULL;
  char ret=-1;//标志位
  char sql[SQL_LENGH]={};//数据库缓冲区
  msg_on msgon;
  if(recv(jstfd,&msgon,sizeof(msg_on),0)<ZERO)
  {
		printf("recv\n");
		return FAIL;
	}
  sprintf(sql,"delete from card_on where id=%d;",msgon.id);
  printf("%s\n",sql);
  if(sqlite3_exec(p,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
  {
    strcpy(msgon.date,"删除记录失败\n");
    printf("删除记录失败\n\n");
  }
  else
  {
    strcpy(msgon.date,"删除记录成功\n");
    printf("删除记录成功\n\n");
  }
  do{
		ret=send(jstfd,&msgon,sizeof(msg_on),0);
	}while(ret<0&&EINTR==errno);
	return SUCCESS;
}

//删除员工下班信息
int s_delete_down(int jstfd,sqlite3 *p)
{
  if(p==NULL)
  {
    printf("数据库打开失败\n");
    return FAIL;
  }
  char *errmsg=NULL;
  char ret=-1;//标志位
  char sql[SQL_LENGH]={};//数据库缓冲区
  msg_down msgdown;
  if(recv(jstfd,&msgdown,sizeof(msg_down),0)<ZERO)
  {
		printf("recv\n");
		return FAIL;
	}
  sprintf(sql,"delete from card_down where id=%d;",msgdown.id);
  printf("%s\n",sql);
  if(sqlite3_exec(p,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
  {
    strcpy(msgdown.date,"删除记录失败\n");
    printf("删除记录失败\n\n");
  }
  else{
    strcpy(msgdown.date,"删除记录成功\n");
    printf("删除记录成功\n\n");
  }
  do
  {
		ret=send(jstfd,&msgdown,sizeof(msg_down),0);
	}while(ret<0&&EINTR==errno);
	return 0;
}

//修改员工信息
int s_revise(int jstfd,int aaa,sqlite3 *p)
{
  if(p==NULL)
  {
    printf("数据库打开失败\n");
    return FAIL;
  }
  int ret=-1;
  char *errmsg=NULL;
  char sql[SQL_LENGH]={};//数据库缓冲区
  msg person;
  if(recv(jstfd,&person,sizeof(msg),0)<ZERO)
  {
		printf("recv\n");
		return FAIL;
	}
  sprintf(sql,"update user set name='%s' where pass=%d;",person.name,person.pass);
  printf("%s\n",sql);
  if(sqlite3_exec(p,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
  {
    strcpy(person.data,"修改信息失败\n");
    printf("修改信息失败\n\n");
  }
  else
  {
    strcpy(person.data,"修改信息成功\n");
    printf("修改信息成功\n\n");
  }
  do{
		ret=send(jstfd,&person,sizeof(msg),0);
	}while(ret<0&&EINTR==errno);
  return SUCCESS;
}



