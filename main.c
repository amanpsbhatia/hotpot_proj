#include <stdio.h>
#include <stdlib.h>
#include "external.h"
int main()
{
	init_log();
	init_index();
	int a;
	while(1)
	{
		printf("1. Put values\n");
		printf("2. Get values\n");
		scanf("%d",&a);
		if(a == 1)
		{	
			struct put_op_s* op = (struct put_op_s*)malloc(sizeof(struct put_op_s));
			printf("Enter key\n");
			scanf("%d",&op->key);
			//op->key = 5;
			printf("Enter value\n");
			scanf("%d",&op->value);
			//op->value = 20;
			int status = do_put_txn(op);
			if (status != 0)
				printf("Put failed\n");
			else
				printf("Value put");
		}
		else
		{
			struct get_op_s* get_op = (struct get_op_s*)malloc(sizeof(struct get_op_s));
			printf("Enter key\n");
			scanf("%d",&get_op->key);
			
			get_op->value = 10;
			int a = do_get_txn(get_op);	
			//if (a != 0)
			//	printf("ey not found\n");
			if (a==0)
				printf("Get value : %d\n", get_op->value);
		}
		printf("\n");
}
}
/*	op->key = 9;
	op->value = 19;
	status = do_put_txn(op);
	
	if (status != 0)
		printf("There was some problem with put\n");
	else
		printf("Yay !!! Put works");
	//test get
	
	struct get_op_s* get_op = (struct get_op_s*)malloc(sizeof(struct get_op_s));
	get_op->key = 5;
	get_op->value = 10;
	int a = do_get_txn(get_op);	
	if (a != 0)
		printf("Get failed !!\n");
	else 
		printf("get passed : %d\n", get_op->value);
*/
/*	
	get_op->key = 8;
	get_op->value = 10;
	a = do_get_txn(get_op);	
	if (a != 0)
		printf("Get failed !!\n");
	else 
		printf("get passed : %d\n", get_op->value);

op->key = 8;
	op->value = 420;
	status = do_put_txn(op);
	if (status != 0)
		printf("There was some problem with put\n");
	else
		printf("Yay !!! Put works");
	get_op->key = 8;
	get_op->value = 10;
	a = do_get_txn(get_op);	
	if (a != 0)
		printf("Get failed !!\n");
	else 
		printf("get passed : %d\n", get_op->value);
*/

//}
