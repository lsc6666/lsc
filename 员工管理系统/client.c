//gcc client.c -o client
//./client 192.168.3.129

//用户名：员工姓名
//密码　：工号

#include "0.h"

//main()
int main(int argc, const char *argv[]){
  FILE *a=fopen("win10.txt","r+");//界面
  int sock,n,i,j,iii;
  char num=0;//功能标志
  char date[128]={};//show_time
  char buf[255]={};//缓冲区
  //printf("%s","\033[1H\033[2J");//页面置顶
  for(int i=1;i<=14;i++){
    fgets(buf,255,a);
    printf("%s",buf);
  }
  sleep(2);
  printf("%s","\033[1H\033[2J");
  msg person;
  sqlite3 *p=NULL;
  sqlite3_open("p.db",&p);
  if((sock=socket(AF_INET,SOCK_STREAM,0))<0){
		perror("socket\n");
		return -1;
	}
	struct sockaddr_in dz;
	bzero(&dz,sizeof(dz));
	dz.sin_family=AF_INET;
	dz.sin_addr.s_addr=inet_addr(argv[1]);
	dz.sin_port=htons(atoi("8080"));
	if(connect(sock,(struct sockaddr *)&dz,sizeof(dz))<0){
		perror("connect");
		return -1;
	}
  while(1){
    printf("\n===================================================\n");
	  printf("\n\t\t  员工考勤系统\n");
	  printf("\t\t1.注册\n");
	  printf("\t\t2.登录\n");
	  printf("\t\t3.退出\n\n");	
	  printf("===================================================\n");
    printf("请输入序号:");
    if(scanf("%c",&num)!=1)
			puts("输入错误请重新输入:");
    while(getchar()!='\n'); 
    switch(num){
      case '1': p_register(sock,&person,p);break;
      case '2': p_into(sock,&person,p);break;
      case '3': close(sock);return 0;break;
      default:printf("非法命令\n");
    }
    show_time(date);
  }
  sqlite3_close(p);
  printf("\nHave a good day!\n");
  return 0;
}

//注册
int p_register(int sock,msg *person,sqlite3 *p){
  int f=0;//标志位
  char *errmsg=NULL,sql[256]={};
  person->type=1;
  while(f<3){
    printf("请输入用户名:");
    scanf("%s",person->name);
    getchar();
    printf("请输入密码:");
    scanf("%d",&person->pass);
    getchar();
	  f++;
	  if(send(sock,person,sizeof(msg),0)<0){
		  printf("send\n");
		  return -1;
	  }
	  if(recv(sock,person,sizeof(msg),0)<0){
		  printf("recv\n");
		  return -1;
	  }
	  printf("%s\n",person->data);
	  printf("是否退出(y/任意键)：");
		char a=getchar();
		getchar();
		if(a=='y')
		  return 0;
	}
	printf("失败次数过多！\n");
	return 0;
}
//登录
int p_into(int sock,msg *person,sqlite3 *p){
  int f=0;//标志位
  person->type=2;
  while(f<3){
    printf("请输入用户名:");
    scanf("%s",person->name);
    getchar();
    printf("请输入密码:");
    scanf("%d",&person->pass);
    getchar();
    if(send(sock,person,sizeof(msg),0)<0){
		  printf("send\n");
		  return -1;
	  }
		if(recv(sock,person,sizeof(msg),0)<0){
		  printf("recv\n");
		  return -1;
	  }
		if(!strcmp(person->name,"1")){//admin
		  printf("用户%s登陆成功\n\n",person->name);
		  msg ad;
		  ad.pass=1;
		  strcpy(ad.name,person->name);
		  admin_interface(sock,&ad,p);
		  return 0;
		}
		else if(!strcmp(person->data,"登录成功")){
		  printf("用户%s登陆成功\n\n",person->name);
		  msg pp;
		  pp.pass=person->pass;
		  strcpy(pp.name,person->name);
		  //printf("职工姓名：%s\n",pp.name);
		  //printf("职工号：%d\n",pp.pass);
		  user_interface(sock,&pp,p);
		  return 0;
		}
		f++;
		printf("是否退出(y/任意键)：");
		char a=getchar();
		getchar();
		if(a=='y')
		  return 0;
  }
  printf("失败次数过多！\n");
	return 0;
}
//管理员界面
int admin_interface(int sock,msg *person,sqlite3 *p){
  char num=0;//功能标志
  msg person1;//账号
  msg_on msgon;//上班
  msg_down msgdown;//下班
  while(1){
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
    if(send(sock,person,sizeof(msg),0)<0){
		  printf("send\n");
		  return -1;
	  }
    switch(num){
      case '1': input_on(sock,1,&msgon);break;
      case '2': input_down(sock,1,&msgdown);break;
      case '3': show_on(sock,&msgon);break;
      case '4': show_down(sock,&msgdown);break;
      case '5': search_on(sock,&msgon);break;
      case '6': search_down(sock,&msgdown);break;
      case '7': delete_on(sock,&msgon);break;
      case '8': delete_down(sock,&msgdown);break;
      case '9': revise(sock,1,&person1);break;
      case '0': close(sock);exit(0);break;
      default:printf("非法命令\n");
    }
  }
  return 0;
}
//用户界面
int user_interface(int sock,msg *person,sqlite3 *p){
  char num=0;//功能标志
  msg person1;//账号
  msg_on msgon;//上班
  msg_down msgdown;//下班
  person1.pass=person->pass;
  msgon.id=person->pass;
  msgdown.id=person->pass;
  strcpy(person1.name,person->name);
  strcpy(msgon.name,person->name);
  strcpy(msgdown.name,person->name);
  while(1){
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
    if(send(sock,person,sizeof(msg),0)<0){
		  printf("send\n");
		  return -1;
	  }
    switch(num){
      case '1': input_on(sock,0,&msgon);break;
      case '2': input_down(sock,0,&msgdown);break;
      case '3': show_on(sock,&msgon);break;
      case '4': show_down(sock,&msgdown);break;
      case '5': revise(sock,0,&person1);break;
      case '0': close(sock);exit(0);;break;
      default:printf("非法命令\n");
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
int input_on(int sock,int aaa,msg_on *msgon){
  show_time(msgon->date);
  if(aaa==1){
    printf("请输入员工工号：");
    scanf("%d",&msgon->id);
    getchar();
    printf("请输入员工姓名：");
    scanf("%s",msgon->name);
    getchar();
  }
  number_on[msgon->id]+=1;
  msgon->number=number_on[msgon->id];
  if(send(sock,msgon,sizeof(msg_on),0)<0){
		printf("send\n");
		return -1;
	}
	return 0;
}
//员工下班打卡信息
int input_down(int sock,int aaa,msg_down *msgdown){
  show_time(msgdown->date);
  if(aaa==1){
    printf("请输入员工工号：");
    scanf("%d",&msgdown->id);
    getchar();
    printf("请输入员工姓名：");
    scanf("%s",msgdown->name);
    getchar();
  }
  number_down[msgdown->id]+=1;
  msgdown->number=number_down[msgdown->id];
  if(send(sock,msgdown,sizeof(msg_down),0)<0){
		printf("send\n");
		return -1;
	}
	return 0;
}
//显示上班员工信息
int show_on(int sock,msg_on *msgon){
  if(send(sock,msgon,sizeof(msg_on),0)<0){
		printf("send\n");
		return -1;
	}
  FILE *fp=fopen("on.txt","a+");
  char c;
  while((c=fgetc(fp))!=EOF)
    printf("%c",c);
  fclose(fp);
}
//显示下班员工信息
int show_down(int sock,msg_down *msgdown){
  if(send(sock,msgdown,sizeof(msg_down),0)<0){
		printf("send\n");
		return -1;
	}
  FILE *fp=fopen("down.txt","a+");
  char c;
  while((c=fgetc(fp))!=EOF)
    printf("%c",c);
  fclose(fp);
}
//查询员工上班信息
int search_on(int sock,msg_on *msgon){
  printf("请输入员工工号：");
  scanf("%d",&msgon->id);
  getchar();
  if(send(sock,msgon,sizeof(msg_on),0)<0){
		printf("send\n");
		return -1;
	}
  FILE *fp=fopen("on.txt","a+");
  char c;
  while((c=fgetc(fp))!=EOF)
    printf("%c",c);
  fclose(fp);
}
//查询员工下班信息
int search_down(int sock,msg_down *msgdown){
  printf("请输入员工工号：");
  scanf("%d",&msgdown->id);
  getchar();
  if(send(sock,msgdown,sizeof(msg_down),0)<0){
		printf("send\n");
		return -1;
	}
  FILE *fp=fopen("down.txt","a+");
  char c;
  while((c=fgetc(fp))!=EOF)
    printf("%c",c);
  fclose(fp);
}
//删除员工上班信息
int delete_on(int sock,msg_on *msgon){
  printf("请输入员工工号：");
  scanf("%d",&msgon->id);
  getchar();
  if(send(sock,msgon,sizeof(msg_on),0)<0){
		printf("send\n");
		return -1;
	}
  if(recv(sock,msgon,sizeof(msg_on),0)<0){
		printf("recv\n");
		return -1;
	}
	printf("%s\n",msgon->date);
	return 0;
}
//删除员工下班信息
int delete_down(int sock,msg_down *msgdown){
  printf("请输入员工工号：");
  scanf("%d",&msgdown->id);
  getchar();
  if(send(sock,msgdown,sizeof(msg_down),0)<0){
		printf("send\n");
		return -1;
	}
  if(recv(sock,msgdown,sizeof(msg_down),0)<0){
		printf("recv\n");
		return -1;
	}
	printf("%s\n",msgdown->date);
	return 0;
}
//修改员工信息
int revise(int sock,int aaa,msg *person){
  if(aaa==0){
    printf("请输入工号对应的新职工姓名：");
    scanf("%s",person->name);
    getchar();
  }
  else if(aaa==1){
    printf("请输入员工工号：");
    scanf("%d",&person->pass);
    getchar();
    printf("请输入工号对应的新职工姓名：");
    scanf("%s",person->name);
    getchar();
  }
  if(send(sock,person,sizeof(msg),0)<0){
		  printf("send\n");
		  return -1;
	}
  if(recv(sock,person,sizeof(msg),0)<0){
		printf("recv\n");
		return -1;
	}
	printf("%s\n",person->data);
	return 0;
}



