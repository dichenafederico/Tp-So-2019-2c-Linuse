#define GFILEBYTABLE 1024
#define GFILEBYBLOCK 1
#define GFILENAMELENGTH 71
#define GHEADERBLOCKS 1
#define BLKINDIRECT 1000
#define GBLOCKSIZE 4096
#define PTRBYINDIRECT 1024

typedef uint32_t ptrGBloque;

typedef struct sac_header_t { // un bloque
	unsigned char sac[3];
	uint32_t version;
	uint32_t blk_bitmap;
	uint32_t size_bitmap; // en bloques
	unsigned char padding[4081];
} GHeader;

typedef struct sac_file_t { // un cuarto de bloque (256 bytes)
	uint8_t state; // 0: borrado, 1: archivo, 2: directorio
	unsigned char fname[GFILENAMELENGTH];
	uint32_t parent_dir_block;
	uint32_t file_size;
	uint64_t c_date;
	uint64_t m_date;
	ptrGBloque blk_indirect[BLKINDIRECT];
} GFile;

typedef struct sac_ptr_ind_simple_t { // un cuarto de bloque (256 bytes)
	ptrGBloque blk_direct[PTRBYINDIRECT];
} GPtrIndSimple;

typedef struct sac_data_block {
	unsigned char bytes[GBLOCKSIZE];
} GDataBlock;
