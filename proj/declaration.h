struct _key {
  int key;
};

struct _value {
  int value;
};

struct kv_pair {
  int  key;
  int  value;
};

struct commit_area_struct {
  void *address;
  int length;
};

typedef struct index_entry {
  int used;
  int key;
  int* offset;
}index_entry;

//Client facing structs

struct put_op_in {
  int key;
  int value;
  int status;
  int* log_pt;
  int log_entry_size;
  int* index_pt;
  int index_entry_size;
};

struct get_op_in {
  int key;
  int value;
  int status;
  int* log_pt;
  int log_entry_size;
  int* index_pt;
  int index_entry_size;
};

struct delete_op_in {
  int key;
  int value;
  int status;
  int* log_pt;
  int log_entry_size;
  int* index_pt;
  int index_entry_size;
};
