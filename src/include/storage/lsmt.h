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

/* One memtable per DB.
 * Thus, the memtable hastable is keyed on tablespace+database.
 */
typedef struct
{
  Oid spc_node; /* tablespace */
  Oid db_node; /* database */
} LsmtMemtableTag;

#define INIT_LSMT_MEMTABLE_TAG(a,xx_spc_node,xx_db_node) \
( \
	(a).spc_node = (xx_spc_node), \
	(a).db_node = (xx_db_node) \
)

typedef struct
{
  LsmtMemtableTag key;
  int memtable_id;
} LsmtMemtableLookupEnt;

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
 *
 * The first 2 bytes indicates the tail of this log file.
 */

#define LSMT_MEMTABLE_BLOCKSZ 32768

typedef struct
{
  uint32 checksum; // crc32c of type and data[]
  uint16 length;
  uint8  type; // One of FULL, FIRST, MIDDLE, LAST
  char*  data;
} LsmtMemtableRecordData;

typedef LsmtMemtableRecordData *LsmtMemtableRecord;

#endif                                                  /* LSM_H */

/*
 * prototypes for functions in lsm.c
 */
extern void InitLsmtMemtablePool(void);
extern uint32 LsmtMemtableHashCode(LsmtMemtableTag*);
extern int LsmtMemtableLookUp(LsmtMemtableTag*, uint32);
