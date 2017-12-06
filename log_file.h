int* put_fetch_address(struct put_op_in* put_op);

void get_fetch_address(struct get_op_in* get_op);

// Validate apis

int validate_put(struct put_op_in* put_op);

int validate_get(struct get_op_in* get_op) ;

// Execute apis

int execute_put(struct put_op_in* put_op) ;

int execute_get(struct get_op_in* get_op) ;
