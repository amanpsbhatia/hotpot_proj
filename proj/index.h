#include <stdbool.h>

extern void init_index(void);
extern int* get_index_fetch_address(int key);
extern int* put_index_fetch_address(int key);
extern bool validate_put_index_slot(int* index_entry_address);
