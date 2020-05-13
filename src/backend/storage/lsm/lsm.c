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

#include "storage/lsm.h"
#include "storage/shmem.h"

char *LsmLogBlocks;

/*
 * Initialize LSM log blocks
 */
void
InitLsmLogBlocks(void)
{
  bool foundBufs;
  LsmLogBlocks = (char *)
      ShmemInitStruct("LSM Log Blocks",
                      NBuffers * (Size) LSM_LOG_BLOCKSZ, &foundBufs);
}
