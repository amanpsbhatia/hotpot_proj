#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/syscall.h>

#include "../dsnvm-helper.h"
#include "../dsnvm-common.h"
#include "dsnvm.h"
#include "declaration.h"
#include "log_file.h"
#include "index.h"

int *log_size;
struct kv_pair *log;

void init_log(void) {

	int fd = open("/mnt/hotpot/main_log", O_RDWR | O_CREAT);
	if (fd < 0)
		printf("failed to open file\n");
	void* base = mmap(0, 40960, PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (base == MAP_FAILED)
		printf("mmap failedi\n");
	log_size = base;
	log = log_size + sizeof(int);
	*log_size = 0;
	printf("log file is initialized\n");

}

// struct commit_area_struct[] fetch_addresses(struct put_op put_ops[], int n) {
//   struct commit_area_struct areas[2*n + 1];
//   int j = 1;
//   //Put the initial log_size entry
//   commit_area_struct[0].address = log_size;
//   commit_area_struct[0].length = sizeof(int);
//
//   for (i = 0 ; i < n; i++) {
//     commit_area_struct[j].address = &log[*log_size + i];
//     commit_area_struct[j].length = sizeof(int);
//     j++;
//     commit_area_struct[j].address = get_index_address(put_ops.key);
//     commit_area_struct[j].length = sizeof(int);
//     j++;
//   }
//
//
// }
//
// struct commit_area_struct[] fetch_addresses(struct get_op get_ops[], int n) {
//   struct commit_area_struct areas[2 * n];
//   int j = 0;
//   for (i = 0 ; i < n; i++) {
//     commit_area_struct[j].address = get_index_address(put_ops.key);
//     commit_area_struct[j].length = sizeof(int);
//
//     commit_area_struct[j].address = &log[*log_size + i];
//     commit_area_struct[j].length = sizeof(int);
//     j++;
//     commit_area_struct[j].address = get_index_address(put_ops.key);
//     commit_area_struct[j].length = sizeof(int);
//     j++;
//   }

//Fetch address apis

  int* put_fetch_address(struct put_op_in* put_op) {
    put_op->log_pt = &log[*log_size];
    put_op->log_entry_size = sizeof(struct kv_pair);
    put_op->index_pt = put_index_fetch_address(put_op->key);
/*
	if (put_op->index_pt == NULL)
		printf("index slot returned null\n");
	else
		printf("%x\n", put_op->index_pt);
*/  
  //printf("used var in put is : %d\n",((index_entry*)put_op->index_pt)->used);
    put_op->index_entry_size = sizeof(struct index_entry);
    return log_size;
  }

  void get_fetch_address(struct get_op_in* get_op) {
    get_op->index_pt = get_index_fetch_address(get_op->key);
	//printf("The address fetched is : %x\n", get_op->index_pt);
	//printf("used var in get is : %d\n",((index_entry*)get_op->index_pt)->used);
    get_op->index_entry_size = sizeof(struct index_entry);
    if (get_op->index_pt == NULL) {
      get_op->status = -1;
      return;
    }
    struct index_entry* entry = (struct index_entry*)get_op->index_pt;

    //TODO : Handle case when the key does not exist or is deleted
    if (entry->key != get_op->key)
      return;
    get_op->log_pt = entry->offset;
    get_op->log_entry_size = sizeof(struct kv_pair);
    get_op->status = 0;
  }

// Validate apis

  int validate_put(struct put_op_in* put_op) {
    int status = 0;
    if (&log[*log_size] != put_op->log_pt)
      status = 1;
    status = status & validate_put_index_slot(put_op->index_pt);
    return status;
  }

  int validate_get(struct get_op_in* get_op) {
    int status = 0;
    return status;
  }

// Execute apis

  int execute_put(struct put_op_in* put_op) {
    int status = 0;
    struct kv_pair* log_entry =(struct kv_pair*) put_op->log_pt;
    *log_size += 1;
    log_entry->key = put_op->key;
    log_entry->value = put_op->value;

    struct index_entry* entry = (struct index_entry*)put_op->index_pt;
    entry->used = 1;
    entry->key = put_op->key;
    entry->offset = put_op->log_pt;

   printf("Key Value inserted : %d, %d", entry->key, log_entry->value);

    return status;
  }

  int execute_get(struct get_op_in* get_op) {
    int status = 0;

    //TODO : handle case where the index entry does not exist

    struct kv_pair* log_entry =(struct kv_pair*) get_op->log_pt;
    if (log_entry == NULL){
	printf("Key has been deleted\n");
        return -1;
    }
	
    get_op->value = log_entry->value;
	printf("The pair in get is : %d, %d\n", log_entry->key, log_entry->value);
    return status;
  }


  int execute_del(struct delete_op_in* del_op) {
    int status = 0;

    //TODO : handle case where the index entry does not exist

    struct index_entry* entry =(struct index_entry*) del_op->index_pt;
    if (entry->offset == NULL) {
	printf("Key is already deleted.\n");
	return -1;
    }

    entry->offset = NULL;
    return status;
}
