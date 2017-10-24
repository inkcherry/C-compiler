#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<string.h>
int token;          		//当前的记号
char *src,*old_src; 		//text的指针 token的下一个字符
int poolsize;                   //虚拟机内存开辟空间size的参数
int line;                       //行号
int *text,                 //代码段
 	*old_text,             
 	*stack;                //栈
char *data;                //数据段（只用与存放字符串）
int *pc,*bp,*sp,ax,cycle;  //虚拟机中的寄存器，类似于计算机中的汇编意义
int *symbols_tab,*cur_id;
int token_val;
int *idmain;  

enum {CHAR, INT, PTR };      // types of variable/function
                // the `main` function


//pc  存放吓一条计算指令
//sp  指向栈顶 栈是从高地址到低地址增长 入栈是sp减小
//bp  指向栈的某些特定位置
//ax  通用寄存器 用于存放指令结果
enum {
  Num = 128, Fun, Sys, Glo, Loc, Id,
  Char, Else, Enum, If, Int, Return, Sizeof, While,
  Assign, Cond, Lor, Lan, Or, Xor, And, Eq, Ne, Lt, Gt, Le, Ge, Shl, Shr, Add, Sub, Mul, Div, Mod, Inc, Dec, Brak
};

enum{LEA,IMM,JMP,CALL,JZ,JNZ,ENT,ADJ,LEV,LI,LC,SI,SC,PUSH,
OR,XOR,AND,EQ,NE,LT,GT,LE,GE,SHL,SHR,ADD,SUB,MUL,DIV,MOD,
OPEN,READ,CLOS,PRTF,MALC,MSET,MCMP,EXIT};
//指令集 在虚拟机中相当与计算机汇编的指令 意义与汇编相同



enum {Token, Hash, Name, Type, Class, Value, BType, BClass, BValue, IdSize}; //symbols_tab

void  lexical_analyzer (){                    //词法分析
char *st_pos,*en_pos;
int hash;
    while(token=*src)
    {   src++;

    	if(token=='\n')  //换行;
    	{
    	   ++line;
    	}


    	else if(token =="#") //跳过
    	{
    	        while (*src != 0 && *src != '\n') 
    	        src++;
    	}    


    	else if((token>='a'&&token<='z')||(token>='A'&&token<="Z")||token=='_') //标识符开头;
    	{  
    	   st_pos=src-1;
    	   while((*src>='a'&&*src<='z')||(*src>='A'&&*src<="Z")||*src=='_'||(*src>= '0' && *src<= '9'))
    			{hash=token*147+*src;//hash映射
    	         src++; 
    			}
    	   en_pos=src-1;

    	   cur_id=symbols_tab;              
    	    while (cur_id[Token]) {  //遍历符号表
    	        if (cur_id[Hash] == hash && !memcmp((char *)cur_id[Name], st_pos, en_pos-st_pos+1)) {
    	                   
    	            token = cur_id[Token];  //如果是已经存在的字符
    	            return;
    	    	}
    	        else cur_id=cur_id+IdSize;   //++
    	    }
    		//如果没有return（跳出） ，下面这段相当于push			    
    		cur_id[Name]=(int)st_pos;
    		cur_id[Hash]=hash;
    		token=cur_id[Token]=Id;

    			return;
    	}


        else if (token >= '0' && token <= '9') {   //数字
            // parse number, three kinds: dec(123) hex(0x123) oct(017)
            token_val = token - '0';
            if (token_val > 0) {                    //10 default
                // dec, starts with [1-9]
                while (*src >= '0' && *src <= '9') {
                    token_val = token_val*10 + *src++ - '0';
                }
            } 

            else  if (*src == 'x' || *src == 'X')   //16
            {
                //hex
                token = *++src;
                while ((token >= '0' && token <= '9') || (token >= 'a' && token <= 'f') || (token >= 'A' && token <= 'F')) {
                    token_val = token_val * 16 + (token & 15) + (token >= 'A' ? 9 : 0);
                    token = *++src;
                }
            }

            else if (*src == 'o' || *src == 'O')
            {                                //8
                // oct
                while (*src >= '0' && *src <= '7') {
                    token_val = token_val*8 + *src++ - '0';
                }
            }
        
            token = Num;
            return;
        }

        else if (token == '"' || token == '\'') {
            // parse string literal, currently, the only supported escape
            // character is '\n', store the string literal into data.
            st_pos = data;                      //用data存放字符串
            while (*src != 0 && *src != token) {
                token_val = *src++;
                if (token_val == '\\') {
                    // escape character
                    token_val = *src++;
                    if (token_val == 'n') {
                        token_val = '\n';
                    }
                }
                if (token == '"') {
                    *data++ = token_val;
                }
            }
            src++;
            // if it is a single character, return Num token
            if (token == '"') {
                token_val = (int)st_pos;
            } else {
                token = Num;
            }
            return;
        }

        else if (token == '/') {               //注释 仅仅//类型
        if (*src == '/') {
            // skip comments
            while (*src != 0 && *src != '\n') {
                ++src;
            }
        } else {
            // divide operator
            token = Div;
            return;
        }
        }


        else if (token == '=') {
            // parse '==' and '='
            if (*src == '=') {
                src ++;
                token = Eq;
            } else {
                token = Assign;
            }
            return;
        }
        else if (token == '+') {
            // parse '+' and '++'
            if (*src == '+') {
                src ++;
                token = Inc;
            } else {
                token = Add;
            }
            return;
        }
        else if (token == '-') {
            // parse '-' and '--'
            if (*src == '-') {
                src ++;
                token = Dec;
            } else {
                token = Sub;
            }
            return;
        }
        else if (token == '!') {
            // parse '!='
            if (*src == '=') {
                src++;
                token = Ne;
            }
            return;
        }
        else if (token == '<') {
            // parse '<=', '<<' or '<'
            if (*src == '=') {
                src ++;
                token = Le;
            } else if (*src == '<') {
                src ++;
                token = Shl;
            } else {
                token = Lt;
            }
            return;
        }
        else if (token == '>') {
            // parse '>=', '>>' or '>'
            if (*src == '=') {
                src ++;
                token = Ge;
            } else if (*src == '>') {
                src ++;
                token = Shr;
            } else {
                token = Gt;
            }
            return;
        }
        else if (token == '|') {
            // parse '|' or '||'
            if (*src == '|') {
                src ++;
                token = Lor;
            } else {
                token = Or;
            }
            return;
        }
        else if (token == '&') {
            // parse '&' and '&&'
            if (*src == '&') {
                src ++;
                token = Lan;
            } else {
                token = And;
            }
            return;
        }
        else if (token == '^') {
            token = Xor;
            return;
        }
        else if (token == '%') {
            token = Mod;
            return;
        }
        else if (token == '*') {
            token = Mul;
            return;
        }
        else if (token == '[') {
            token = Brak;
            return;
        }
        else if (token == '?') {
            token = Cond;
            return;
        }
        else if (token == '~' || token == ';' || token == '{' || token == '}' || token == '(' || token == ')' || token == ']' || token == ',' || token == ':') {
            // directly return the character as token;
            return;
        }


    }
return;
}


void expression(int level)      //用于解析表达式的函数
{
}

void program()
{   
	lexical_analyzer(); 

   while(token>0){
	printf("token is :%c\n",token);
	lexical_analyzer();
 }
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
int main(int argc,char **argv)
{

     int i, fd;

    argc--;
    argv++;

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
    int *temp=symbols_tab;
 
    src="";
    while(*src){lexical_analyzer();src++;}
       while(*temp)
    {
        printf("%d   ",temp[0]);
        temp=temp+IdSize;
    }

    if ((fd = open(*argv, 0)) < 0) {
        printf("could not open(%s)\n", *argv);
        return -1;
    }

    if (!(src = old_src = malloc(poolsize))) {
        printf("could not malloc(%d) for source area\n", poolsize);
        return -1;
    }
    // read the source file
    if ((i = read(fd, src, poolsize-1)) <= 0) {
        printf("read() returned %d\n", i);
        return -1;
    }


 

    src[i] = 0; // add EOF character
    close(fd);

    program();

    return eval();

}