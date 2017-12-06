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

#include <dsnvm-helper.h>
#include <dsnvm-common.h>

#include "declaration.h"
#include "external.h"
#include "log_file.h"
#include "dsnvm.h"

void acquire_lock(struct dsnvm_addr_len* areas, int n);
void release_lock(struct dsnvm_addr_len* areas, int n) ;
// void do_txn( struct put_op put_ops1[], struct get_op get_ops1[], struct update_op update_ops1[], struct delete_op delete_ops1[]) {
//   // Convert external struct to internal struct.
//   struct get_op_in get_ops[] = convert(get_ops1);
//   struct put_op_in put_ops[];
//   struct update_op_in update_ops[];
//   struct delete_op_in delete_ops[];
//
//   fetch_put_addresses(put_ops);
//   fetch_get_addresses(get_ops);
//   // fetch_update_addresses(update_ops);
//   // fetch_delete_addresses(delete_ops);
//
//
//
//   struct commit_area_struct areas[] = append(put_areas, get_areas, update_areas, delete_areas);
//
//   acquire_lock(areas);
//
//   validate(put_areas, put_ops);
//   validate(get_areas, get_ops);
//   validate(update_areas, update_ops);
//   validate(delete_areas, delete_ops);
//
//   execute(put_areas, put_ops);
//
// }
//
// void do_steps(struct put_op_in* put_op, struct get_op_in* get_op) {
//   int* log_address = put_fetch_address(put_op);
//   get_fetch_address(get_op);
//
//   struct commit_area_struct areas[3];
//   areas[0].address = put_op->log_pt;
//   areas[0].length = put_op->log_entry_size;
//
//   areas[1].address = put_op->index_pt;
//   areas[1].length = put_op->index_entry_size;
//
//   areas[2].address = log_address;
//   areas[2].length = sizeof(int)
//
//   acquire_lock(areas, 3);
//
//   validate_put(put_op);
//   validate_get(get_op);
//
//   //TODO : handle cases if validateion has failed
//
//   execute_put(put_op);
//   execute_get(get_op);
//
// }
//
// int do_txn(struct put_op_s* put_op, struct get_op_s* get_op ) {
//   struct put_op_in p1 = (struct put_op_in)malloc(sizeof(struct put_op_in));
//   p1.key = put_op->key;
//   p1.value = put_op->value;
//   p1.status = 0;
//
//   struct get_op_in p2 = (struct get_op_in)malloc(sizeof(struct get_op_in));
//   p2.key = get_op->key;
//   p2.status = 0;
//
//   do_steps(&p1, &p2);
//
//   get_op->status = p2.status;
//   get_op->value = p2.value;
//   return 0;
// }


int do_put_txn(struct put_op_s* put_op) {
  if(!put_op)
	return -1;
  struct put_op_in* p1 = (struct put_op_in*)malloc(sizeof(struct put_op_in));
  p1->key = put_op->key;
  p1->value = put_op->value;
  p1->status = 0;

  do_put_steps(p1);

  put_op->status = p1->status;

  return 0;
}

void do_put_steps(struct put_op_in* put_op) {
  if(!put_op)
	return;
  int* log_address = put_fetch_address(put_op);

  struct dsnvm_addr_len areas[3];
  areas[0].addr = put_op->log_pt;
  areas[0].len = put_op->log_entry_size;

  areas[1].addr = put_op->index_pt;
  areas[1].len = put_op->index_entry_size;

  areas[2].addr = log_address;
  areas[2].len = sizeof(int);

  //acquire_lock(areas, 3);

  validate_put(put_op);

  //TODO : handle cases if validateion has failed

  execute_put(put_op);

  //release_lock(areas, 3);

}

int do_get_txn(struct get_op_s* get_op) {
  if(!get_op)
	return -1;
  struct get_op_in* p2 = (struct get_op_in*)malloc(sizeof(struct get_op_in));
  p2->key = get_op->key;
  p2->status = 0;
  p2->log_entry_size = sizeof(struct kv_pair);
  p2->index_entry_size = sizeof(index_entry);
  int ret = do_get_steps(p2);

  get_op->status = p2->status;
  get_op->value = p2->value;

  return ret;
}

int do_get_steps(struct get_op_in* get_op) {
  if(!get_op)
	return -1;
  get_fetch_address(get_op);
  if (get_op->status == -1) {
    printf("Key not found.\n");
    return -1;
  }

 /* struct dsnvm_addr_len areas[2];
  areas[0].addr = get_op->log_pt;
  areas[0].len = get_op->log_entry_size;

  areas[1].addr = get_op->index_pt;
  areas[1].len = get_op->index_entry_size;
*/
//  acquire_lock(areas, 2);

  validate_get(get_op);

  //TODO : handle cases if validateion has failed

  execute_get(get_op);

 // release_lock(areas, 2);

  return 0;
}

void acquire_lock(struct dsnvm_addr_len* areas, int n){
  int ret = msync(areas, n, DSNVM_BEGIN_XACT_FLAG);
  ret = msync(areas, n, DSNVM_COMMIT_XACT_FLAG);
}

void release_lock(struct dsnvm_addr_len* areas, int n) {
  int ret = msync(areas, n, DSNVM_COMMIT_XACT_FLAG);
  acquire_lock(areas, n);
}
