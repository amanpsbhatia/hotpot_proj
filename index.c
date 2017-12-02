
int* index_start = NULL;

struct find
{
	int found;
	int *pos;
};

static void init_index(void)
{
	mmap_len = KEY_VALUE_PAIR_SIZE*NUM_PAIRS;

	/*
	 * Open Hotpot dataset named abc
	 * You should be able to see some outputs in CD side.
	 */
	fd = open("/mnt/hotpot/index", O_RDWR | O_CREAT);
	if (fd < 0)
		die("Can not open file (1): %s", strerror(fd));

	/* mmap Hotpot regions */
	virt_hotpot = mmap(NULL, mmap_len, PROT_WRITE, MAP_SHARED, fd, 0);
	if (virt_hotpot == MAP_FAILED)
		die("Can not mmap (1): %s", strerror(errno));

	/* Direct memory load/store in DSPM address space */
	index_start = (int*)virt_hotpot;
	foo = *(int *)virt_hotpot;
}

struct find* search(int key,int (*fn)(int),int max)
{
	int start_pos = index_start+fn(key);
	struct find* f = (struct find*)malloc(sizeof(struct find));
	while(*start_pos) {
		if(*start_pos == key){
			f->found = 1;
			f->pos = start_pos;
			return f;
		}
		start_pos += 2*(sizeof(int));
	}
	f->found = -1;
	f->pos = start_pos;
	return f;
}

void insert_into_index(int key,int value,int (*fn)(int))
{
		assert(index_start != NULL);
		int pos = -1;
		struct find* f = search(key,fn);
		if (f->found == -1){
			// not found
			pos = f->pos;
		} else {
			//already insert_into_index
			return;
		}
		*pos = key;
		*(pos+1) = value;
		
}



//lock index entries before statrting transaction


int* lock_index_entries(int* keys,int (*fn)(),int max)
{
		assert(index_start != NULL);
		int i;
		int num_keys = sizeof(keys)/sizeof(int);
		int* addresses = (int*)malloc(sizeof(int)*num_keys);
		for(i=0;i<num_keys;i++) {
			struct find* f = search(keys[i],fn,max);
			*(addresses+i) = f->pos; 
		}
		return addresses;
}

// init xact = init xact header, xact area, 
// assume one thread only
// xact_struct = xact_header + xact_areas;

static struct dsnvm_addr_len *xact_areas;
static void *xact_struct;

static void init_index_xact_areas(int* keys,int (*fn)(),int max)
{
	int* index_addr = lock_index_entries(keys,fn,max);
	malloc_size = sizeof(struct dsnvm_xact_header);
	malloc_size += NR_XACT_AREAS*sizeof(struct dsnvm_addr_len);

	//NR_XACT_AREAS is sizeof(index_addr)/sizeof(int) for index entries
	xact_struct = malloc(malloc_size);
	struct dsnvm_xact_header* header = (struct dsnvm_xact_header*)xact_struct;
	header->rep_degree = NR_REPLICA;
	header->xact_id = -1;

	xact_areas = xact_struct + sizeof(struct dsnvm_xact_header);
	for (i = 0; i < NR_XACT_AREAS; i++) {
		xact_areas[i].addr = *(index_addr+i)
		xact_areas[i].len = sizeof(int)*2;
	}
}


static int validate_for_put(int* offsets)
{
	int i;
	int size = sizeof(offsets)/sizeof(int);
	for(i=0;i<size;i++) {
		int value = *(index_start+offsets[i]+1);
		if(value != -1)
			return -1;
	}
	return 1;
}
