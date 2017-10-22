#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<string.h>
int token;          		//当前的记号
char *src,*old_src; 		//指向源代码的char*
int poolsize;                   //空间
int line;                       //行号

void next(){                    //用于读取字符的函数
token=*src++;
return;
}
void expression(int level)      //用于解析表达式的函数
{
}
void program()
{   next(); 
   while(token>0){
	printf("token is :%c\n",token);
	next();
 }
}

int eval(){
	return 0;
}
int main(int argc,char **argv)
{
  int i,fd;


  argc--;
  argv++;  

  poolsize=256*1024;          //
  line=1;
  if((fd=open(*argv,0))<0)
  {
     printf("could not open(%s)\n,*argv");
     	return -1;
  }
  if(!(src=old_src=malloc(poolsize)))
  {
  	  printf("could not malloc(%d) for source area\n",poolsize);
  	  return -1;
  }

  if((i=read(fd,src,poolsize-1))<=0){
  	printf("read() returned %d\n",i);
  	return -1;
  }

  src[i]=0;
  
  close(fd);

  program();          //a blank function 

  return eval();
}