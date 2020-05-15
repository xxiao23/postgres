/*-------------------------------------------------------------------------
 *
 * lsm.c
 *	 Log structure merge (LSM) tree methods
 *
 * IDENTIFICATION
 *	  src/backend/storage/buffer/lsm.c
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "storage/buf_internals.h"
#include "storage/lsm.h"
#include "storage/shmem.h"

char *LsmMemtableBlocks;

typedef struct
{
  BufferTag key;  // Tag of a memtable. Just use a BufferTag key for now.
  int id;  // Memtable ID.
} MemtableLookupEnt;

static HTAB *SharedMemtableHash;

/*
 * Initialize a pool of LSM memtables and a memtable lookup table.
 */
void
InitLsmMemtablePool(void)
{
  HASHCTL info;
  int size = NBuffers + NUM_BUFFER_PARTITIONS;
  bool foundBufs;

  printf("Starting InitLsmMemtablePool\n");
  printf("Creating LsmMemtableBlocks\n");
  LsmMemtableBlocks = (char *)
      ShmemInitStruct("LSM Memtable Blocks",
                      NBuffers / 10 * (Size) LSM_MEMTABLE_BLOCKSZ, &foundBufs);

  info.keysize = sizeof(BufferTag);
  info.entrysize = sizeof(MemtableLookupEnt);
  info.num_partitions = NUM_BUFFER_PARTITIONS;

  printf("Creating SharedMemtableHash\n");
  SharedMemtableHash = ShmemInitHash("Shared Memtable  Lookup Table",
                                     size, size,
                                     &info,
                                     HASH_ELEM | HASH_BLOBS | HASH_PARTITION);
  printf("Done with InitLsmMemtablePool\n");
}
