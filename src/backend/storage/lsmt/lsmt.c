/*-------------------------------------------------------------------------
 *
 * lsmt.c
 *    Log structure merge tree methods
 *
 * IDENTIFICATION
 *    src/backend/storage/buffer/lsm.c
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "storage/buf_internals.h"
#include "storage/lsmt.h"
#include "storage/relfilenode.h"
#include "storage/shmem.h"

/*
 * Initialize a pool of LSM memtables and a memtable lookup table.
 */
void
InitLsmtMemtablePool(void)
{
  HASHCTL info;
  int size = NBuffers + NUM_BUFFER_PARTITIONS;
  bool foundBufs;

  ereport(DEBUG5, (errmsg("XX== InitLsmMemtablePool\n")));
  // Create one 32K buffer to be shared among all DBs.
  // This means when a different DB is processed,
  // the buffer is replaced.
  // This is just for the current development stage,
  // not intended for the long term.
  LsmtMemtableBlocks = (char *)
      ShmemInitStruct("Shared LSMT Memtable Blocks",
                      1 * (Size) LSMT_MEMTABLE_BLOCKSZ, &foundBufs);

  info.keysize = sizeof(LsmtMemtableTag);
  info.entrysize = sizeof(LsmtMemtableLookupEnt);
  info.num_partitions = NUM_BUFFER_PARTITIONS;

  SharedMemtableHash = ShmemInitHash("Shared LSMT Memtable Lookup Table",
                                     size, size,
                                     &info,
                                     HASH_ELEM | HASH_BLOBS | HASH_PARTITION);
}

uint32
LsmtMemtableHashCode(LsmtMemtableTag *tag_ptr)
{
  return get_hash_value(SharedMemtableHash, (void *) tag_ptr);
}

int
LsmtMemtableLookUp(LsmtMemtableTag *tag_ptr, uint32 hashcode)
{
  LsmtMemtableLookupEnt *entry;

  entry = (LsmtMemtableLookupEnt *)
      hash_search_with_hash_value(SharedMemtableHash,
                                  (void *) tag_ptr,
                                  hashcode,
                                  HASH_FIND,
                                  NULL);

  if (!entry)
    return -1;

  return entry->memtable_id;
}
