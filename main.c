#include <stdio.h>
#include <stdlib.h>
#include "external.h"
int main()
{
	init_log();
	init_index();
	printf("Hi\n");
	struct put_op_s* op = (struct put_op_s*)malloc(sizeof(struct put_op_s));
	op->key = 5;
	op->value = 20;
	int status = do_put_txn(op);
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
}
