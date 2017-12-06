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
#include "declaration.h"

#include "../dsnvm-helper.h"
#include "../dsnvm-common.h"
#include "dsnvm.h"


int update(int key, int value){
  int *pos = getPos(key);
  if (pos == -1)
    return -1;

  struct entry *e = pos;
  entry->key = key;
  entry->value = value;
  //commit


}

int get(int key) {
  int *pos = getPos(key);
  if (pos == -1)
    return -1;

    struct entry *e = pos;
    return e->value;
}

int put(int key, int value) {
  int *pos = insert(key, value);
  insertIndex(key, pos);
  return 1;
}

int delete(int key) {
  deleteIndex(key);
  return 1;
}


int main(int argc, char *argv[])
{
	int i, ret, *foo, bar;
	int args[NR_THREADS_PER_NODE];
	pthread_t tid[NR_THREADS_PER_NODE];

	if (LOOPS * NR_XACT_AREAS > NR_PAGES)
		die("Adjust NR_PAGES");

	srand(time(NULL) + getpid());
	open_and_mmap_files();
	init_xact();

	ret = pthread_barrier_init(&local_barrier, NULL, NR_THREADS_PER_NODE);
	if (ret)
		die("fail to init local barrier");

	dist_sync_barrier();

	for (i = 0; i < NR_THREADS_PER_NODE; i++) {
		args[i] = i;
		ret = pthread_create(&tid[i], NULL, thread_func, &args[i]);
		if (ret)
			die("fail to create thread");
	}

	for (i = 0; i < NR_THREADS_PER_NODE; i++) {
		ret = pthread_join(tid[i], NULL);
		if (ret)
			die("fail to join");
	}

	show_perf_num();

	/* sync between clients */
	/* clean up */
	close_files();
	if (!xact_struct)
		free(xact_struct);

	return 0;
}
