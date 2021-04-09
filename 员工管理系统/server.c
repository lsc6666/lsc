//gcc server.c -lsqlite3 -o server
//./server 192.168.3.129

//用户名：员工姓名
//密码　：工号

#include "0.h"

int main(int argc, const char *argv[]){
  int n,jstfd,sockfd;
  int reuse=1;//标志位
  char *errmsg;
  sqlite3 *p=NULL;
  sqlite3_open("p.db",&p);
  pid_t pid;
  build_sql(p);//构建数据库
  //printf("%s","\033[1H\033[2J");//页面置顶
  //创建socket套接字
  if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0){
		perror("sockfd\n");
		return -1;
	}
  //允许地址端口快速重用
  if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int))<0){
		perror("setsockopt");
		exit(1);
	}
  struct sockaddr_in addr;
	bzero(&addr,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=inet_addr(argv[1]);
	addr.sin_port=htons(atoi("8080"));
	if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr))<0){
		perror("bind\n");
		return -1;
	}
  if(listen(sockfd,5)<0){
		perror("listen\n");
		return -1;
	}
  while(1){
    if((jstfd=accept(sockfd,NULL,NULL))<0){
			perror("accept");
			return -1;
		}
		if((pid=fork())<0){
			perror("fork");
			return -1;
		}
		else if(pid==0){
			close(sockfd);
			into_client(jstfd,p);//进入客户端
		}
		else
			close(jstfd);
  }
  
  if(sqlite3_close(p)!=0){
		fprintf(stderr,"数据库关闭失败\n");
		return -1;
	}
	printf("数据库关闭成功\n");
	printf("\nHave a good day!\n");
  return 0;
}
//构建数据库
int build_sql(sqlite3 *p){
  int i=1;
  char buf[128]={},r1[10]={},r2[20]={};
  char sql[256]={},*errmsg=NULL;
  printf("开启服务器\n");
  printf("数据库打开成功\n");
  bzero(sql,sizeof(sql));//数组置０
  sqlite3_exec(p,"create table if not exists card_on(id int,name char,number int,data TEXT )",NULL,NULL,&errmsg);
  sqlite3_exec(p,"create table if not exists card_down(id int,name char,number int,data TEXT )",NULL,NULL,&errmsg);
  sqlite3_exec(p,"create table if not exists user(name text,pass int primary key);",NULL,NULL,&errmsg);
  printf("表格创建成功\n");
  return 0;
}
//客户端
int into_client(int jstfd,sqlite3 *p){
  char date[128]={};//show_time
  msg person;
  //printf("%s","\033[1H\033[2J");//页面置顶
  while(recv(jstfd,&person,sizeof(person),0)>0){
    printf("序号：%d\n",person.type);
    switch(person.type){
      case 1: s_register(jstfd,&person,p);break;
      case 2: 
        if(s_into(jstfd,&person,p)==1)
          s_admin_interface(jstfd,p);
        else if(s_into(jstfd,&person,p)==2)
          s_user_interface(jstfd,p);
        break;
      case 3: return 0;break;
    }
    show_time(date);
  }
  printf("一名用户离开\n");
	close(jstfd);
	exit(0);
  return 0;
}
//注册
int s_register(int jstfd,msg *person,sqlite3 *p){
  int ret=-1;//标志位
  char *errmsg=NULL,sql[256]={};
  sprintf(sql,"insert into user values('%s',%d);",person->name,person->pass);
  printf("%s\n",sql);
  if(sqlite3_exec(p,sql,NULL,NULL,&errmsg)!=SQLITE_OK){
	  printf("%s\n",errmsg);
	  strcpy(person->data,"注册失败\n");
	  printf("%s\n",person->data);
	 }
	else{
		printf("%s注册成功\n\n",person->name);
		strcpy(person->data,"注册成功!");
		printf("%s\n",person->data);
	}
	do{
		ret=send(jstfd,person,sizeof(msg),0);
	}while(ret<0&&EINTR==errno);
	if(ret<0){
	  perror("发送失败\n");
	  return 0;
  }
	return 0;
}
//登录
int s_into(int jstfd,msg *person,sqlite3 *p){
  int nrow,ncloumn;
  char sql[256]="",*cwmsg,**resultp;
  char *errmsg=NULL;
   sprintf(sql,"select*from user where name='%s' and pass=%d;",person->name,person->pass);
    printf("%s\n",sql);
    if(sqlite3_get_table(p,sql,&resultp,&nrow,&ncloumn,&errmsg)!=SQLITE_OK){
		  printf("获取失败\n");
		  return 0;
	  }
	  else printf("成功获取数据表\n");
	  if(nrow==1){
		  strcpy(person->data,"登录成功");
		  printf("%s\n",person->data);
		  send(jstfd,person,sizeof(msg),0);
		  if(!strcmp(person->name,"1")){//admin
		    printf("用户%s登陆成功\n\n",person->name);
		    return 1;
		  }
		  else if(!strcmp(person->data,"登录成功")){
		    return 2;
		  }
	  }
	  if(nrow==0){
		  strcpy(person->data,"用户名或密码错误\n");
		  printf("%s\n",person->data);
		  send(jstfd,person,sizeof(msg),0);
	  }
  return 0;
}
//管理员界面
int s_admin_interface(int jstfd,sqlite3 *p){
  msg person;
  printf("\nadmin\n");
  while(recv(jstfd,&person,sizeof(person),0)>0){
    printf("序号：%d\n",person.type);
    switch(person.type){
      case '1': s_input_on(jstfd,p);break;
      case '2': s_input_down(jstfd,p);break;
      case '3': s_show_on(jstfd,1,p);break;
      case '4': s_show_down(jstfd,1,p);break;
      case '5': s_search_on(jstfd,p);break;
      case '6': s_search_down(jstfd,p);break;
      case '7': s_delete_on(jstfd,p);break;
      case '8': s_delete_down(jstfd,p);break;
      case '9': s_revise(jstfd,1,p);break;
      case '0': printf("退出界面\n");return 0;break;
     }
  }
  return 0;
}
//用户界面
int s_user_interface(int jstfd,sqlite3 *p){
  msg person;
  printf("\nuser\n");
  while(recv(jstfd,&person,sizeof(person),0)>0){
    printf("序号：%d\n",person.type);
    switch(person.type){
      case '1': s_input_on(jstfd,p);break;
      case '2': s_input_down(jstfd,p);break;
      case '3': s_show_on(jstfd,0,p);break;
      case '4': s_show_down(jstfd,0,p);break;
      case '5': s_revise(jstfd,0,p);break;
      case '0': printf("退出界面\n");return 0;break;
    }
  }
  return 0;
}
//显示时间
void show_time(char *date){
  time_t t;
  time(&t);
  struct tm *tp=localtime(&t);
  sprintf(date,"%d-%d-%d %d:%d:%d",tp->tm_year+1900,tp->tm_mon+1,tp->tm_mday,tp->tm_hour,tp->tm_min,tp->tm_sec);
	printf("当前日期：%s\n",date);
}
//员工上班打卡信息
void s_input_on(int jstfd,sqlite3 *p){
  int ret=-1;//标志位
  char *errmsg=NULL,sql[256]={};//数据库缓冲区
  msg_on msgon;
  recv(jstfd,&msgon,sizeof(msg_on),0);
  sprintf(sql,"insert into card_on values(%d,'%s',%d,'%s');",msgon.id,msgon.name,msgon.number,msgon.date);
  printf("%s\n",sql);
  if(sqlite3_exec(p,sql,NULL,NULL,&errmsg)==SQLITE_OK)
    printf("上午打卡成功\n\n");
}
//员工下班打卡信息
void s_input_down(int jstfd,sqlite3 *p){
  int ret=-1;//标志位
  char *errmsg=NULL,sql[256]={};//数据库缓冲区
  msg_down msgdown;
  recv(jstfd,&msgdown,sizeof(msg_down),0);
  sprintf(sql,"insert into card_down values(%d,'%s',%d,'%s');",msgdown.id,msgdown.name,msgdown.number,msgdown.date);
  printf("%s\n",sql);
  if(sqlite3_exec(p,sql,NULL,NULL,&errmsg)==SQLITE_OK)
    printf("下午打卡成功\n\n");
}
//显示上班员工信息
void s_show_on(int jstfd,int aaa,sqlite3 *p){
  char **result,*errmsg=NULL;
  char sql[512]={};//数据库缓冲区
  int nrow,ncolumn,i,j,index;
  msg_on msgon;
  recv(jstfd,&msgon,sizeof(msg_on),0);
   printf("id     %d\n",msgon.id);
  if(aaa==0){
    sprintf(sql,"select*from card_on where id=%d and number=1;",msgon.id);
    sqlite3_get_table(p,sql,&result,&nrow,&ncolumn,&errmsg);
  }
  else if(aaa==1)
    sqlite3_get_table(p,"select*from card_on where number=1",&result,&nrow,&ncolumn,&errmsg);
  index=ncolumn;
  FILE *fp=fopen("on.txt","w+");
  for(i=0;i<nrow;i++){
    for(j=0;j<ncolumn;j++){
      fprintf(fp,"%-8s:%-8s\n",result[j],result[index++]);
    }
    fprintf(fp,"============================\n");
  }
  fclose(fp);
  sqlite3_free_table(result);
}
//显示下班员工信息
void s_show_down(int jstfd,int aaa,sqlite3 *p){
  char **result,*errmsg=NULL;
  char sql[512]={};//数据库缓冲区
  int nrow,ncolumn,i,j,index;
  msg_down msgdown;
  recv(jstfd,&msgdown,sizeof(msg_down),0);
   printf("id     %d\n",msgdown.id);
  if(aaa==0){
    sprintf(sql,"select*from card_down where id=%d and number=1;",msgdown.id);
    sqlite3_get_table(p,sql,&result,&nrow,&ncolumn,&errmsg);
  }
  else if(aaa==1)
    sqlite3_get_table(p,"select*from card_down where number=1",&result,&nrow,&ncolumn,&errmsg);
  index=ncolumn;
  FILE *fp=fopen("down.txt","w+");
  for(i=0;i<nrow;i++){
    for(j=0;j<ncolumn;j++){
      fprintf(fp,"%-8s:%-8s\n",result[j],result[index++]);
    }
    fprintf(fp,"============================\n");
  }
  fclose(fp);
  sqlite3_free_table(result);
}
//查询员工上班信息
void s_search_on(int jstfd,sqlite3 *p){
  char **result,*errmsg=NULL;
  char sql[512]={};//数据库缓冲区
  int nrow,ncolumn,i,j,index;
  msg_on msgon;
  recv(jstfd,&msgon,sizeof(msg_on),0);
  sprintf(sql,"select*from card_on where id=%d and number=1;",msgon.id);
  if(sqlite3_get_table(p,sql,&result,&nrow,&ncolumn,&errmsg)!=0)
    printf("查询失败\n\n");
  index=ncolumn;
  FILE *fp=fopen("on.txt","w+");
  for(i=0;i<nrow;i++){
    for(j=0;j<ncolumn;j++){
        fprintf(fp,"%-8s:%-8s\n",result[j],result[index++]);
    }
    fprintf(fp,"============================\n");
  }
  fclose(fp);
  sqlite3_free_table(result);
}
//查询员工下班信息
void s_search_down(int jstfd,sqlite3 *p){
  char **result,*errmsg=NULL;
  char sql[512]={};//数据库缓冲区
  int nrow,ncolumn,i,j,index;
  msg_down msgdown;
  recv(jstfd,&msgdown,sizeof(msg_down),0);
  sprintf(sql,"select*from card_down where id=%d and number=1;",msgdown.id);
  if(sqlite3_get_table(p,sql,&result,&nrow,&ncolumn,&errmsg)!=0)
    printf("查询失败\n\n");
  index=ncolumn;
  FILE *fp=fopen("down.txt","w+");
  for(i=0;i<nrow;i++){
    for(j=0;j<ncolumn;j++){
        fprintf(fp,"%-8s:%-8s\n",result[j],result[index++]);
    }
    fprintf(fp,"============================\n");
  }
  fclose(fp);
  sqlite3_free_table(result);
}
//删除员工上班信息
int s_delete_on(int jstfd,sqlite3 *p){
  char *errmsg=NULL,ret=-1;
  char sql[512]={};//数据库缓冲区
  msg_on msgon;
  recv(jstfd,&msgon,sizeof(msg_on),0);
  sprintf(sql,"delete from card_on where id=%d;",msgon.id);
  printf("%s\n",sql);
  if(sqlite3_exec(p,sql,NULL,NULL,&errmsg)!=SQLITE_OK){
    strcpy(msgon.date,"删除记录失败\n");
    printf("删除记录失败\n\n");
  }
  else{
    strcpy(msgon.date,"删除记录成功\n");
    printf("删除记录成功\n\n");
  }
  do{
		ret=send(jstfd,&msgon,sizeof(msg_on),0);
	}while(ret<0&&EINTR==errno);
	return 0;
}
//删除员工下班信息
int s_delete_down(int jstfd,sqlite3 *p){
  char *errmsg=NULL,ret=-1;
  char sql[512]={};//数据库缓冲区
  msg_down msgdown;
  if(recv(jstfd,&msgdown,sizeof(msg_down),0)<0){
		printf("recv\n");
		return -1;
	}
  sprintf(sql,"delete from card_down where id=%d;",msgdown.id);
  printf("%s\n",sql);
  if(sqlite3_exec(p,sql,NULL,NULL,&errmsg)!=SQLITE_OK){
    strcpy(msgdown.date,"删除记录失败\n");
    printf("删除记录失败\n\n");
  }
  else{
    strcpy(msgdown.date,"删除记录成功\n");
    printf("删除记录成功\n\n");
  }
  do{
		ret=send(jstfd,&msgdown,sizeof(msg_down),0);
	}while(ret<0&&EINTR==errno);
	return 0;
}
//修改员工信息
int s_revise(int jstfd,int aaa,sqlite3 *p){
  int ret=-1;//标志位
  char *errmsg=NULL,sql[256]={};//数据库缓冲区
  msg person;
  if(recv(jstfd,&person,sizeof(msg),0)<0){
		printf("recv\n");
		return -1;
	}
  sprintf(sql,"update user set name='%s' where pass=%d;",person.name,person.pass);
  printf("%s\n",sql);
  if(sqlite3_exec(p,sql,NULL,NULL,&errmsg)!=SQLITE_OK){
    strcpy(person.data,"修改信息失败\n");
    printf("修改信息失败\n\n");
  }
  else{
    strcpy(person.data,"修改信息成功\n");
    printf("修改信息成功\n\n");
  }
  do{
		ret=send(jstfd,&person,sizeof(msg),0);
	}while(ret<0&&EINTR==errno);
  return 0;
}







