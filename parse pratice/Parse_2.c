#include<stdio.h>
//<expr> ::= <term> <expr_tail>
//<expr_addsub> ::= + <term> <expr_tail>
//              | - <term> <expr_tail>
//              | <empty>
//<term> ::= <factor> <term_tail>
//<term_muldiv> ::= * <factor> <term_tail>
//              | / <factor> <term_tail>
//              | <empty>
//<factor> ::= ( <expr> )
//           | Num
char *src;
char *cursrc;
int   cursrc_val=0;
int deal_expr();       //1
int expr_addsub();     //2
int deal_term();       //3
int term_muldiv();     //4
int deal_factor();     //5
void  lexical_analysiser()   //½«Ê¶±ðNUM
{

	cursrc = src++;
	if (*cursrc >= '0'&&*cursrc <= '9') {

		cursrc_val = *cursrc - '0';

		while (*src >= '0'&&*src <= '9')
		{

			cursrc_val = cursrc_val * 10 + (*src - '0');

			src++;
		}
		return;
	}
}
int expr_addsub(int val)        //exprtail
{
	if (*cursrc == '+') {
		lexical_analysiser();
		 val = val + deal_term();
		return expr_addsub(val);
	}
	else if (*cursrc == '-') {
		lexical_analysiser();
		 val = val -deal_term();
		return expr_addsub(val);
	}
	else return val;
}
int deal_expr()           //¼Ó¼õ
{
	int val = deal_term();
	return expr_addsub( val);
}
int term_muldiv(int val)
{
	if (*cursrc == '*')
	{
		lexical_analysiser();
		val = val*deal_factor();
		return term_muldiv(val);
	}
	else if (*cursrc == '/')
	{
		lexical_analysiser();
		val = val/deal_factor();
		return term_muldiv(val);
	}
	else return val;                        //empty;
}
int deal_term()			//³Ë³ý
{
	int val = deal_factor();
	return term_muldiv( val);
}
int deal_factor()
{
	int val = 0;
	if (*cursrc == '(')
	{
		lexical_analysiser();
		val= deal_expr();
		(*cursrc == ')');
		lexical_analysiser();
	}
	else
	{
		val = cursrc_val;
		lexical_analysiser();

	}

	return val;


}
int main()
{
	src="(12+7)*2-(11-10)";
	lexical_analysiser();
	printf("%d\n", deal_expr());
	return 0;
}