//gcc client.c -o client
//./client 192.168.3.129

//用户名：员工姓名
//密码　：工号(唯一标识)

#include "0.h"

//main()
int main(int argc, const char *argv[])
{
  FILE *a=fopen("win10.txt","r+");//界面
  int sock,n,j;
  char buf[BUF_LENGH]={};//缓冲区
  //printf("%s","\033[1H\033[2J");//页面置顶
  for(int i=ONE;i<=WIN10_TXT_LINE;i++)
  {
    fgets(buf,BUF_LENGH,a);
    printf("%s",buf);
  }
  //sleep(2);
  printf("%s","\033[1H\033[2J");
  sqlite3 *p=NULL;
  sqlite3_open("p.db",&p);
  //创建套接字，返回文件描述符
  if((sock=socket(AF_INET,SOCK_STREAM,0))<ZERO)
  {
		perror("socket\n");
		return FAIL;
	}
	struct sockaddr_in dz;
	bzero(&dz,sizeof(dz));
	dz.sin_family=AF_INET;
	dz.sin_addr.s_addr=inet_addr(argv[1]);
	dz.sin_port=htons(atoi("8080"));
	//发送客户端的连接请求
	if(connect(sock,(struct sockaddr *)&dz,sizeof(dz))<ZERO)
	{
		perror("connect");
		return FAIL;
	}
  into_client(sock,p);
  sqlite3_close(p);
  return SUCCESS;
}

//进入客户端
int into_client(int sock,sqlite3 *p)
{
  if(sock==FAIL)
  {
    perror("socket\n");
    return FAIL;
  }
  if(p==NULL)
  {
    printf("数据库打开失败\n");
    return FAIL;
  }
  msg person;
  char num=ZERO;//功能标志
  char date[DATA_LENGH]={};//show_time
  while(TRUE)
  {
    printf("\n===================================================\n");
	  printf("\n\t\t  员工考勤系统\n");
	  printf("\t\t1.注册\n");
	  printf("\t\t2.登录\n");
	  printf("\t\t3.退出\n\n");	
	  printf("===================================================\n");
    printf("请输入序号:");
    if(scanf("%c",&num)!=TRUE)
    {
			puts("输入错误请重新输入:");
		}
    getchar();
    switch(num)
    {
      case CHAR_REGISTER:
        p_register(sock,&person,p);
        break;
      case CHAR_LOG_ON:
        p_into(sock,&person,p);
        break;
      case CHAR_LEAVE:
        close(sock);
        return SUCCESS;
        break;
      default:printf("非法命令\n");
    }
    show_time(date);
  }
  printf("\nHave a good day!\n");
  return SUCCESS;
}

//注册
int p_register(int sock,msg *person,sqlite3 *p)
{
  if(sock==FAIL)
  {
    perror("socket\n");
    return FAIL;
  }
  if(p==NULL)
  {
    printf("数据库打开失败\n");
    return FAIL;
  }
  int f=ZERO;//标志位
  person->type=ONE;
  while(f<THREE){
    printf("请输入用户名:");
    scanf("%s",person->name);
    getchar();
    printf("请输入密码:");
    scanf("%d",&person->pass);
    getchar();
	  f++;
	  if(send(sock,person,sizeof(msg),0)<ZERO)
	  {
		  printf("send\n");
		  return FAIL;
	  }
	  if(recv(sock,person,sizeof(msg),0)<ZERO)
	  {
		  printf("recv\n");
		  return FAIL;
	  }
	  printf("%s\n",person->data);
	  printf("是否退出(y/任意键)：");
		char a=getchar();
		getchar();
		if(a=='y')
		  return SUCCESS;
	}
	printf("失败次数过多！\n");
	return SUCCESS;
}

//登录
int p_into(int sock,msg *person,sqlite3 *p)
{
  if(sock==FAIL)
  {
    perror("socket\n");
    return FAIL;
  }
  if(p==NULL)
  {
    printf("数据库打开失败\n");
    return FAIL;
  }
  int f=0;//标志位
  person->type=TWO;
  while(f<THREE){
    printf("请输入用户名:");
    scanf("%s",person->name);
    getchar();
    printf("请输入密码:");
    scanf("%d",&person->pass);
    getchar();
    if(send(sock,person,sizeof(msg),0)<ZERO)
    {
		  printf("send\n");
		  return FAIL;
	  }
		if(recv(sock,person,sizeof(msg),0)<ZERO)
		{
		  printf("recv\n");
		  return FAIL;
	  }
	  //admin
		if(!strcmp(person->name,"1"))
		{
		  printf("用户%s登陆成功\n\n",person->name);
		  msg ad;
		  ad.pass=ONE;
		  strcpy(ad.name,person->name);
		  admin_interface(sock,&ad,p);
		  return SUCCESS;
		}
		else if(!strcmp(person->data,"登录成功"))
		{
		  printf("用户%s登陆成功\n\n",person->name);
		  msg pp;
		  pp.pass=person->pass;
		  strcpy(pp.name,person->name);
		  printf("职工姓名：%s\n",pp.name);
		  printf("职工号：%d\n",pp.pass);
		  user_interface(sock,&pp,p);
		  return SUCCESS;
		}
		f++;
		printf("是否退出(y/任意键)：");
		char a=getchar();
		getchar();
		if(a=='y')
		{
		  return SUCCESS;
		}
  }
  printf("失败次数过多！\n");
	return SUCCESS;
}

//管理员界面
int admin_interface(int sock,msg *person,sqlite3 *p)
{
  if(sock==FAIL)
  {
    perror("socket\n");
    return FAIL;
  }
  if(p==NULL)
  {
    printf("数据库打开失败\n");
    return FAIL;
  }
  char num=ZERO;//功能标志
  msg person1;//账号
  msg_on msgon;//上班
  msg_down msgdown;//下班
  while(TRUE)
  {
    printf("\n===================================================\n");
	  printf("\n\t\t  功能选项\n");
	  printf("\t\t1.员工上班打卡信息\n");
	  printf("\t\t2.员工下班打卡信息\n");
	  printf("\t\t3.显示上班员工信息\n");
	  printf("\t\t4.显示下班员工信息\n");
	  printf("\t\t5.查询员工上班信息\n");
	  printf("\t\t6.查询员工下班信息\n");
	  printf("\t\t7.删除员工上班信息\n");
	  printf("\t\t8.删除员工下班信息\n");
	  printf("\t\t9.修改员工信息\n");
	  printf("\t\t0.退出\n\n");	
	  printf("===================================================\n");
	  printf("请输入序号:");
    scanf("%c",&num);
    getchar();
    person->type=num;
    if(send(sock,person,sizeof(msg),0)<ZERO)
    {
		  printf("send\n");
		  return FAIL;
	  }
    switch(num)
    {
      case WORK_START:
        input_on(sock,ADMIN,&msgon);
        break;
      case WORK_END:
        input_down(sock,ADMIN,&msgdown);
        break;
      case SHOW_WORK_START:
        show_on(sock,&msgon);
        break;
      case SHOW_WORK_END:
        show_down(sock,&msgdown);
        break;
      case SEARCH_WORK_START:
        search_on(sock,&msgon);
        break;
      case SEARCH_WORK_END:
        search_down(sock,&msgdown);
        break;
      case DELETE_WORK_START:
        delete_on(sock,&msgon);
        break;
      case DELETE_WORK_END:
        delete_down(sock,&msgdown);
        break;
      case REVISE_INFO:
        revise(sock,ADMIN,&person1);
        break;
      case QUIT:
        close(sock);
        exit(0);
        break;
      default:printf("非法命令\n");
    }
  }
  return SUCCESS;
}

//用户界面
int user_interface(int sock,msg *person,sqlite3 *p)
{
  if(sock==FAIL)
  {
    perror("socket\n");
    return FAIL;
  }
  if(p==NULL)
  {
    printf("数据库打开失败\n");
    return FAIL;
  }
  char num=ZERO;//功能标志
  msg person1;//账号
  msg_on msgon;//上班
  msg_down msgdown;//下班
  //传递职工姓名和职工号
  person1.pass=person->pass;
  msgon.id=person->pass;
  msgdown.id=person->pass;
  strcpy(person1.name,person->name);
  strcpy(msgon.name,person->name);
  strcpy(msgdown.name,person->name);
  while(TRUE)
  {
    printf("\n===================================================\n");
	  printf("\n\t\t  功能选项\n");
	  printf("\t\t1.上班打卡\n");
	  printf("\t\t2.下班打卡\n");
	  printf("\t\t3.上班打卡记录\n");
	  printf("\t\t4.下班打卡记录\n");
	  printf("\t\t5.修改员工信息\n");
	  printf("\t\t0.退出\n\n");	
	  printf("===================================================\n");
	  printf("请输入序号:");
    scanf("%c",&num);
    getchar();
    person->type=num;
    if(send(sock,person,sizeof(msg),0)<ZERO)
    {
		  printf("send\n");
		  return FAIL;
	  }
    switch(num)
    {
      case WORK_START:
        input_on(sock,USER,&msgon);
        break;
      case WORK_END:
        input_down(sock,USER,&msgdown);
        break;
      case SHOW_WORK_START:
        show_on(sock,&msgon);
        break;
      case SHOW_WORK_END:
        show_down(sock,&msgdown);
        break;
      case REVISE_INFO_U:
        revise(sock,USER,&person1);
        break;
      case QUIT:
        close(sock);
        exit(0);
        break;
      default:printf("非法命令\n");
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
  struct tm *tp=localtime(&t);//获取系统日期
  sprintf(date,"%d-%d-%d %d:%d:%d",tp->tm_year+1900,tp->tm_mon+1,tp->tm_mday,tp->tm_hour,tp->tm_min,tp->tm_sec);
	printf("当前日期：%s\n",date);
}
//员工上班打卡信息
int input_on(int sock,int aaa,msg_on *msgon)
{
  if(sock==FAIL)
  {
    perror("socket\n");
    return FAIL;
  }
  if((aaa!=ADMIN)&&(aaa!=USER))
  {
    perror("权限错误");
    return FAIL;
  }
  show_time(msgon->date);
  if(aaa==ADMIN)
  {
    printf("请输入员工工号：");
    scanf("%d",&msgon->id);
    getchar();
    printf("请输入员工姓名：");
    scanf("%s",msgon->name);
    getchar();
  }
  number_on[msgon->id]+=ONE;
  msgon->number=number_on[msgon->id];
  if(send(sock,msgon,sizeof(msg_on),0)<ZERO)
  {
		printf("send\n");
		return FAIL;
	}
	return SUCCESS;
}
//员工下班打卡信息
int input_down(int sock,int aaa,msg_down *msgdown)
{
  if(sock==FAIL)
  {
    perror("socket\n");
    return FAIL;
  }
  if((aaa!=ADMIN)&&(aaa!=USER))
  {
    perror("权限错误");
    return FAIL;
  }
  show_time(msgdown->date);
  if(aaa==ADMIN)
  {
    printf("请输入员工工号：");
    scanf("%d",&msgdown->id);
    getchar();
    printf("请输入员工姓名：");
    scanf("%s",msgdown->name);
    getchar();
  }
  number_down[msgdown->id]+=ONE;
  msgdown->number=number_down[msgdown->id];
  if(send(sock,msgdown,sizeof(msg_down),0)<ZERO)
  {
		printf("send\n");
		return FAIL;
	}
	return SUCCESS;
}

//显示上班员工信息
int show_on(int sock,msg_on *msgon)
{
  if(sock==FAIL)
  {
    perror("socket\n");
    return FAIL;
  }
  if(send(sock,msgon,sizeof(msg_on),0)<ZERO)
  {
		printf("send\n");
		return FAIL;
	}
	//获取文件数据并打印
  FILE *fp=fopen("on.txt","a+");
  char c;
  while((c=fgetc(fp))!=EOF)
  {
    printf("%c",c);
  }
  fclose(fp);
}

//显示下班员工信息
int show_down(int sock,msg_down *msgdown)
{
  if(sock==FAIL)
  {
    perror("socket\n");
    return FAIL;
  }
  if(send(sock,msgdown,sizeof(msg_down),0)<ZERO)
  {
		printf("send\n");
		return FAIL;
	}
	//获取文件数据并打印
  FILE *fp=fopen("down.txt","a+");
  char c;
  while((c=fgetc(fp))!=EOF)
  {
    printf("%c",c);
  }
  fclose(fp);
}

//查询员工上班信息
int search_on(int sock,msg_on *msgon)
{
  if(sock==FAIL)
  {
    perror("socket\n");
    return FAIL;
  }
  printf("请输入员工工号：");
  scanf("%d",&msgon->id);
  getchar();
  if(send(sock,msgon,sizeof(msg_on),0)<ZERO)
  {
		printf("send\n");
		return FAIL;
	}
	//获取文件数据并打印
  FILE *fp=fopen("on.txt","a+");
  char c;
  while((c=fgetc(fp))!=EOF)
  {
    printf("%c",c);
  }
  fclose(fp);
}

//查询员工下班信息
int search_down(int sock,msg_down *msgdown)
{
  if(sock==FAIL)
  {
    perror("socket\n");
    return FAIL;
  }
  printf("请输入员工工号：");
  scanf("%d",&msgdown->id);
  getchar();
  if(send(sock,msgdown,sizeof(msg_down),0)<ZERO)
  {
		printf("send\n");
		return FAIL;
	}
	//获取文件数据并打印
  FILE *fp=fopen("down.txt","a+");
  char c;
  while((c=fgetc(fp))!=EOF)
  {
    printf("%c",c);
  }
  fclose(fp);
}

//删除员工上班信息
int delete_on(int sock,msg_on *msgon)
{
  if(sock==FAIL)
  {
    perror("socket\n");
    return FAIL;
  }
  printf("请输入员工工号：");
  scanf("%d",&msgon->id);
  getchar();
  if(send(sock,msgon,sizeof(msg_on),0)<ZERO)
  {
		printf("send\n");
		return FAIL;
	}
  if(recv(sock,msgon,sizeof(msg_on),0)<ZERO)
  {
		printf("recv\n");
		return FAIL;
	}
	printf("%s\n",msgon->date);
	return SUCCESS;
}

//删除员工下班信息
int delete_down(int sock,msg_down *msgdown)
{
  if(sock==FAIL)
  {
    perror("socket\n");
    return FAIL;
  }
  printf("请输入员工工号：");
  scanf("%d",&msgdown->id);
  getchar();
  if(send(sock,msgdown,sizeof(msg_down),0)<ZERO)
  {
		printf("send\n");
		return FAIL;
	}
  if(recv(sock,msgdown,sizeof(msg_down),0)<ZERO)
  {
		printf("recv\n");
		return FAIL;
	}
	printf("%s\n",msgdown->date);
	return SUCCESS;
}

//修改员工信息
int revise(int sock,int aaa,msg *person)
{
  if(sock==FAIL)
  {
    perror("socket\n");
    return FAIL;
  }
  if((aaa!=ADMIN)&&(aaa!=USER))
  {
    perror("权限错误");
    return FAIL;
  }
  else if(aaa==USER)
  {
    printf("请输入工号对应的新职工姓名：");
    scanf("%s",person->name);
    getchar();
  }
  else if(aaa==ADMIN)
  {
    printf("请输入员工工号：");
    scanf("%d",&person->pass);
    getchar();
    printf("请输入工号对应的新职工姓名：");
    scanf("%s",person->name);
    getchar();
  }
  if(send(sock,person,sizeof(msg),0)<ZERO)
  {
		  printf("send\n");
		  return FAIL;
	}
  if(recv(sock,person,sizeof(msg),0)<ZERO)
  {
		printf("recv\n");
		return FAIL;
	}
	printf("%s\n",person->data);
	return SUCCESS;
}



