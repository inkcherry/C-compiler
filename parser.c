
#include "lexical_analyzer.c" 
int var_type;   //变量类型
int expr_type; //表达式类型
void deal_enmu_declaration();
void deal_function_declaration();
void deal_function_body();
void deal_global_declaration()    //识别全局变量 最前置的词法解析（不支持#)  
{
   int type;
   int i;
   var_type = INT;
   if(token==Enum)
   {
      token_match(Enum);  //枚举类型
   if(token=='{'){
       token_match('{');
       deal_enmu_declaration();
       token_match('}');
   }
   token_match(';');
   return;
   }

    if(token==Int){               //全局变量
        token_match(Int);
    }
    else if(token==Char){
        token_match(Char);
        var_type=CHAR;
    }

    while(token!=';'&&token!='}'){
        type=var_type;
        while(token==Mul){    //表示符号前面全部的* 地址  指针类型
            token_match(Mul);  
            type=type+PTR;
        }
        if(token!=Id){   //错误声明
            printf("%d:bad global declaration\n",line);
            exit(-1);
        }
        if(cur_id[Class]){  //重复声明
            printf("%d :duplicate global declaration\n",line);
            exit(-1);
        }
        token_match(Id);   //变量/函数 名称
        cur_id[type]=type;
        
        if(token=='(')    //函数类型 
        {
            cur_id[Class] = Fun; 
            cur_id[Value]=(int)(text+1);
            deal_function_declaration();
        }
        else {
            cur_id[Class]=Glo;   //变量类型
            cur_id[Value]=(int)data;
            data=data+sizeof(int);
        
        }
        if(token==',')
        token_match(',');
        
    }
    lexical_analyzer();
}

void deal_enmu_declaration(){
   int enmu_index=0;
   while(token!='}'){
       if(token!=Id){
           printf("%d:bad enum identifier %d\n",line,token);
           exit(-1);
       }
       lexical_analyzer();   //向内存中push 进这个id
       if(token==Assign){      //enmu初始化自己定义的下标
           lexical_analyzer();
           if(token!=Num){
           printf("%d:bad enum identifier %d\n",line,token);
           exit(-1);
           }
           enmu_index=token_val;   //初始化下标
          lexical_analyzer();
        }
        cur_id[Class]=Num;
        cur_id[Type]=INT;
        cur_id[Value]=enmu_index++;
        if(token==",")   
        lexical_analyzer();
   }
}
void deal_function_declaration(){
    
    token_match('(');


    //解析参数
    int type, parameter_index=0;
    while(token!=')'){         //每次while匹配一个参数
        type=INT;
        if(token==Int)
        token_match(Int);
        else if(token==Char){
            type =CHAR;
            token_match(Char);
        }
        
        while（token==Mul){
            match(Mul);
            type=type+PTR;
        }
        if(token!=Id){
            printf("%d: bad parameter declaration\n", line);
            exit(-1);
        }
        if (current_id[Class] == Loc) {  
            printf("%d: duplicate parameter declaration\n", line);
            exit(-1);
        }
       match(Id);
        //将全局变量临时保存到临时的BCLass中。再把这个变量初始化 
        cur_id[BClass]=cur_id[Class];
        cur_id[Class]=Loc;
        cur_id[BType]  = cur_id[Type];  
        cur_id[Type]   = type;
        cur_id[BValue] = cur_id[Value]; 
        cur_id[Value]  = parameter_index++; 

        if(token==','){
            match(',')
        }
    }
    cur_bp=parameter_index+1; //ebp在最后一参数 下两个地址
    token_match(')');
    //参数解析结束
    token_match('{')
    //函数体解析
      //局部变量定义 代码跟全局变量定义基本一致
    int p_localvar=cur_ebp;
    while(token==Int||token==Char){   //变量定义
            var_type=(token == Int) ? INT : CHAR;
            token_match(token);
    }
    while(token!=';'){
        type=var_type;
        while(token==Mul){
            token_match(Mul);
            type=type+PTR;
        }
        if(token!=Id){   //错误声明
            printf("%d:bad local declaration\n",line);
            exit(-1);
        }
        if(cur_id[Class]==Loc){  //重复声明 这里必须加Loc
            printf("%d :duplicate local declaration\n",line);
            exit(-1);
        }
        token_match(Id);   //变量/函数 名称
        cur_id[type]=type;

    }
}


