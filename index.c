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

typedef struct index_entry {
  int key;
  int used;
  int* offset;
}index_entry;

index_entry* index_start;

static void init_index(void)
{
    int mmap_len = 10000; // ARBIT

    /*
     * Open Hotpot dataset named abc
     * You should be able to see some outputs in CD side.
     */
    fd = open("/mnt/hotpot/index", O_RDWR | O_CREAT);
    if (fd < 0)
        die("Can not open file (1): %s", strerror(fd));

    /* mmap Hotpot regions */
    struct void* virt_hotpot = mmap(NULL, mmap_len, PROT_WRITE, MAP_SHARED, fd, 0);
    if (virt_hotpot == MAP_FAILED)
        die("Can not mmap (1): %s", strerror(errno));

    /* Direct memory load/store in DSPM address space */
    index_start = (index_entry*)virt_hotpot;
}


int* get_index_fetch_address(int key)
{
    index_entry* entry = index_start + hash_fn(key);
    while(entry && entry->used != -1) {
        if(key == entry->key){
            return entry;
        }
        entry += 1;
    }
    return NULL;
}


int* put_index_fetch_address(int key)
{
    index_entry* entry = index_start+hash_fn(key);
    while(entry) {
        if(entry->used == -1){
            return entry;
        }
        entry += 1;
    }
    return NULL;
}

bool validate_put_index_slot(int* index_entry_address)
{
    return (((index_entry*)index_entry_address)->used == -1);
}
