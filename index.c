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
#include "index.h"
#include "declaration.h"

index_entry* index_start;


 void init_index()
{
    int mmap_len = 10000; // ARBIT

    /*
     * Open Hotpot dataset named abc
     * You should be able to see some outputs in CD side.
     */
    int fd = open("/mnt/hotpot/index", O_RDWR | O_CREAT);
    if (fd < 0);
    //    die("Can not open file (1): %s", strerror(fd));

    /* mmap Hotpot regions */
    void* virt_hotpot = mmap(NULL, mmap_len, PROT_WRITE, MAP_SHARED, fd, 0);
    if (virt_hotpot == MAP_FAILED);
    //    die("Can not mmap (1): %s", strerror(errno));

    /* Direct memory load/store in DSPM address space */
    index_start = (index_entry*)virt_hotpot;

    int i;
    for(i=0;i<100;i++){
	((index_entry*)(index_start+i))->used = -1;
    }
}


int* get_index_fetch_address(int key)
{
    index_entry* entry = index_start;// + hash_fn(key);
    while(entry && entry->used != -1) {
	//printf("in loop : %d, and %d, value = %d\n", entry->key, key, entry->offset);
        if(key == entry->key){
            return entry;
        }
        entry += 1;
    }
    return NULL;
}


int* put_index_fetch_address(int key)
{
    index_entry* entry = index_start; //+hash_fn(key);
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
