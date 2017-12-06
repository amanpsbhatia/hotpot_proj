struct put_op_s {
  int key;
  int value;
  int status;
};

struct get_op_s {
  int key;
  int value;
  int status;
};

struct update_op_s {
  int key;
  int value;
  int status;
};

struct delete_op_s {
  int key;
  int status;
};
