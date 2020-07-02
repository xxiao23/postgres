/*-------------------------------------------------------------------------
 *
 * lsm.h
 *    Log Merge Structure (LSM) data structures.
 *-------------------------------------------------------------------------
 */

#ifndef LSM_H
#define LSM_H

#include "c.h"

#include "storage/shmem.h"
#include "utils/relcache.h"

typedef int Memtable;

char *LsmtMemtableBlocks;

HTAB *SharedMemtableHash;

#define InvalidMemtable 0

extern PGDLLIMPORT int NBuffers;

/*
 * The log file contents are a sequence of 32KB blocks.
 * The only exception is that the tail of the file may contain a partial block.
 * Each block consists of a sequence of records:
 *
 *  block := record* trailer?
 *  record :=
 *    checksum: uint32     // crc32c of type and data[] ;
 *    length: uint16       // number of bytes in this record
 *    type: uint8          // One of FULL, FIRST, MIDDLE, LAST
 *    data: uint8[length]
 *
 * FULL == 1
 * FIRST == 2
 * MIDDLE == 3
 * LAST == 4
 */

#define LSM_MEMTABLE_BLOCKSZ 8192

typedef struct
{
  uint32 checksum; // crc32c of type and data[]
  uint16 length;
  uint8  type; // One of FULL, FIRST, MIDDLE, LAST
  char*  data;
} LsmMemtableRecord;

#endif                                                  /* LSM_H */

/*
 * prototypes for functions in lsm.c
 */
extern void InitLsmtMemtablePool(void);
