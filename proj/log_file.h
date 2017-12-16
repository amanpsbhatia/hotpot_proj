#ifndef ADD_H
#define ADD_H

extern int* put_fetch_address(struct put_op_in* put_op);

extern void get_fetch_address(struct get_op_in* get_op);

// Validate apis

extern int validate_put(struct put_op_in* put_op);

extern int validate_get(struct get_op_in* get_op) ;

// Execute apis

extern int execute_put(struct put_op_in* put_op) ;

extern int execute_get(struct get_op_in* get_op) ;

extern int execute_del(struct delete_op_in* get_op) ;


#endif
