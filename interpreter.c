#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<string.h>
// #include"lexical_analyzer.c"   //语法解析(包含所有全局定义)
#include"parser.c"             //词法解析






void program()
{   
	lexical_analyzer(); 
    // if(token>0){
    // printf("token is :%d\n",token);
    // src++;
    while(token>0){
        deal_global_declaration();
        // deal_global_decalration();
    }
	// lexical_analyzer();
 
}

int eval(){
	int op,*tmp;
    while(1){       //MOV指令集
        op=*pc++;
       
		if(op==IMM)      {ax=*pc++;}
		else if(op==LC)  {ax=*(char*)ax;} 				//ax存放的地址，对其取值并存入ax 
		else if(op==LI)  {ax=*(int *)ax;}				// 同上
		else if(op==SC)  {ax=*(char*)*sp++=ax;}			// 栈顶存放地址，将ax赋给栈顶指针的内存
		else if(op==SI)  {*(int*)*sp++=ax;}    			//同上
//以上是实现mov的指令
		else if(op==PUSH){*--sp=ax;}    			    //将ax放入栈中，指针-- push
	    else if(op==JMP) {pc=(int*)*pc;} 				//pc指向的是下一条指令

	    else if(op==JZ)  {pc=ax?pc+1:(int*)*pc;}  		//为0跳转 pc+1继续执行，pc当前存放的是跳转指令的地址，取值就相当于跳转
	    else if(op==JNZ)  {pc=ax?(int*)*pc:pc+1;}  		//不为0跳转
	    else if(op==CALL){*--sp=(int)(pc+1);pc=(int *)*pc;}   //保存pc+1到栈，跳转到函数指令
	  
	    else if(op==ENT) {*--sp=(int)bp;bp=sp;sp=sp-*pc++;}	  		//为函数开辟栈空间，保存当前bp到栈，重新为bp赋值，使其指向上一个保存到栈中的值，形成链表
	    else if(op==ADJ) {sp=sp+*pc++;}                       		//在将调用子函数时压入栈中的数据清除
	    else if(op==LEV) {sp=bp;bp=(int*)*sp++;pc=(int*)*sp++;}     //将bp还原到callee的bp，同时pc还原到调用结束后的函数指令，即CALL执行保存到栈中的pc指令
	    else if(op==LEA) {ax=(int)(bp+*pc++);}               	    //用于将参数地址返回给ax bp高地址偏移为参数（逆序排列），低偏移为局部变量
//运算指令  同汇编
	    else if (op == OR)  ax = *sp++ | ax;
		else if (op == XOR) ax = *sp++ ^ ax;
		else if (op == AND) ax = *sp++ & ax;
		else if (op == EQ)  ax = *sp++ == ax;
		else if (op == NE)  ax = *sp++ != ax;
		else if (op == LT)  ax = *sp++ < ax;
		else if (op == LE)  ax = *sp++ <= ax;
		else if (op == GT)  ax = *sp++ >  ax;
		else if (op == GE)  ax = *sp++ >= ax;
		else if (op == SHL) ax = *sp++ << ax;
		else if (op == SHR) ax = *sp++ >> ax;
		else if (op == ADD) ax = *sp++ + ax;
		else if (op == SUB) ax = *sp++ - ax;
		else if (op == MUL) ax = *sp++ * ax;
		else if (op == DIV) ax = *sp++ / ax;
		else if (op == MOD) ax = *sp++ % ax;
//通过C函数实现指令，实际计算机中没有。  
		else if (op == EXIT) { printf("exit(%d)", *sp); return *sp;}
		else if (op == OPEN) { ax = open((char *)sp[1], sp[0]); }
		else if (op == CLOS) { ax = close(*sp);}
		else if (op == READ) { ax = read(sp[2], (char *)sp[1], *sp); }
		else if (op == PRTF) { tmp = sp + pc[1]; ax = printf((char *)tmp[-1], tmp[-2], tmp[-3], tmp[-4], tmp[-5], tmp[-6]); }
		else if (op == MALC) { ax = (int)malloc(*sp);}
		else if (op == MSET) { ax = (int)memset((char *)sp[2], sp[1], *sp);}
		else if (op == MCMP) { ax = memcmp((char *)sp[2], (char *)sp[1], *sp);}
		else {
			printf("unknown instruction:%d\n",op);
			return -1;
		}
	}
	return 0;
}
int main()
{
     
     int i, fd;

    // argc--;
    // argv++;

    poolsize = 256 * 1024; // arbitrary size
    line = 1;

    // allocate memory for virtual machine
    if (!(text = old_text = malloc(poolsize))) {
        printf("could not malloc(%d) for text area\n", poolsize);
        return -1;
    }
    if (!(data = malloc(poolsize))) {
        printf("could not malloc(%d) for data area\n", poolsize);
        return -1;
    }
    if (!(stack = malloc(poolsize))) {
        printf("could not malloc(%d) for stack area\n", poolsize);
        return -1;
    }
    if (!(symbols_tab = malloc(poolsize))) {
        printf("could not malloc(%d) for symbol table\n", poolsize);
        return -1;
    }
  

    memset(text, 0, poolsize);
    memset(data, 0, poolsize);
    memset(stack, 0, poolsize); 
    memset(symbols_tab,0,poolsize);


    src="char else enum if int return sizeof while "
        "open read close printf malloc memset memcmp exit void main";
    bp = sp = (int *)((int)stack + poolsize);  
    ax = 0;
    
    i = Char;   
    while (i <= While) {     //关键字
      lexical_analyzer();
        cur_id[Token] = i++;
    }

    // add library to symbol table
    i = OPEN;
    while (i <= EXIT) {      //内置函数
        lexical_analyzer();
        cur_id[Class] = Sys; 
        cur_id[Type] = INT;
        cur_id[Value] = i++;
    }

    lexical_analyzer(); cur_id[Token] = Char; // handle void type
    lexical_analyzer(); idmain = cur_id; // keep track of main
    
    //test 
    // int *temp=symbols_tab;
 
    
    // while(*src){lexical_analyzer();src++;} //test three
    //test
    //    while(*temp)
    // {
    //     printf("%d   ",temp[0]);
    //     temp=temp+IdSize;
    // }
    // src=malloc(20000);
    // src[0]='1';
    // src[1]='+';
    // src[2]='2';
    // src[3]=0;
    // *(src+1)=0;
    src=malloc(poolsize);
    char *m="int a;";
    i=0;
    for( i=0;i<6;i++)
    {src[i]=m[i];
     src[i+6]=m[i];    //声明两个int i
    }
    src[12]=0;
    
    program();



    // if ((fd = open(*argv, 0)) < 0) {
    //     printf("could not open(%s)\n", *argv);
    //     return -1;
    // }

    // if (!(src = old_src = malloc(poolsize))) {
    //     printf("could not malloc(%d) for source area\n", poolsize);
    //     return -1;
    // }
    // // read the source file
    // if ((i = read(fd, src, poolsize-1)) <= 0) {
    //     printf("read() returned %d\n", i);
    //     return -1;
    // }


 

    // src[i] = 0; // add EOF character
    // close(fd);
    // program();
    // return eval();
    return 0;

}