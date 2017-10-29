
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
		start_pos += 2;
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
		}
		*pos = key;
		*(pos+1) = value;
		
}