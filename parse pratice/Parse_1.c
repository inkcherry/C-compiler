
#include<cstdio>
//A��B(,)��C(,))            ��,�� ��(�� ��)����ʡ��
char *src ="A(B(,C(,)),D(,))";


//int index=0; src=b,
struct node;
struct node
{
	node *left;
	node *right;
	char content;
};
void show_tree(node *tree)
{
	if(tree)
	{
		printf("%c", (tree->content));
		show_tree(tree->left);
		show_tree(tree->right);
	}

}

node *deal_node()
{
	char *thissrc = src;

	if (*thissrc >= 'A'&&*thissrc <= 'Z')
	{
		char tempchar = *thissrc;


		src+=2;//�����ַ���������

		node *left = deal_node();

		src++;//��������

		node *right = deal_node();

		src++;

		node *tree = new node;

		*tree = { left,right,tempchar};

		return tree;
	}

	else if (*thissrc== ','|| *thissrc == ')') //,�ǿ��ַ������ǽ�����
	{
		return nullptr;

	}

	else {
			printf("error [%c]",*src);
			return nullptr;
	}
}

int main()
{

	node *tree=deal_node();

	show_tree(tree);
	return 0;
}
