/*-------------------------------------------------------------------------
 *
 * lsmtam_handler.c
 *  Log Structure Merge (LSM) tree table access method code
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *  src/backend/access/lsmt/lsmtam_handler.c
 *
 *
 * NOTES
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "access/hio.h"
#include "access/tableam.h"
#include "access/skey.h"
#include "storage/lsmt.h"
#include "utils/builtins.h"

typedef struct LsmtScanDescData
{
  TableScanDescData rs_base;  /* AM independent part of the descriptor */
} LsmtScanDescData;
typedef struct LsmtScanDescData* LsmtScanDesc;

static const TableAmRoutine lsmtam_methods;

static TableScanDesc
lsmt_beginscan(Relation relation, Snapshot snapshot,
               int nkeys, ScanKey key,
               ParallelTableScanDesc parallel_scan,
               uint32 flags)
{
  LsmtScanDesc scan;

  return (TableScanDesc) scan;
}

static void
lsmt_tuple_insert(Relation relation, TupleTableSlot *slot, CommandId cid,
                  int options, BulkInsertStateData* bistate)
{
  // relation->rd_id is the relation's OID.
  // slot->tts_tupleDescriptor->attrs is an array that contains information of
  // all the columns (attributes) touched in this insert.
  // slot->tts_values is an array that contains the column values to be
  // inserted. Each element is either the actual value or a pointer to the
  // memory location that has the value.
  //
  // What needs to be done is
  // 1. Create a key for each column value to be inserted. This key consists of
  //    the relation's OID, this row's primary key values, and the column name.
  // 2. Call some method in lsmt.h to perform the insertion to a memtable for
  //    this relation.
  char key[1024];
  int key_size;
  int i;
  int tuple_data_len;
  bytea *tuple_data_bytea;
  Datum row_data;

  Bitmapset* pkindexattrs =
      RelationGetIndexAttrBitmap(relation, INDEX_ATTR_BITMAP_PRIMARY_KEY);

  bool		shouldFree = true;
  HeapTuple tuple = ExecFetchSlotHeapTuple(slot, true, &shouldFree);

  // Copy the inserted row data into bytea attribute.
  tuple_data_len = tuple->t_len - tuple->t_data->t_hoff;
  tuple_data_bytea = (bytea *) palloc(tuple_data_len + VARHDRSZ);
  SET_VARSIZE(tuple_data_bytea, tuple_data_len + VARHDRSZ);
  memcpy(VARDATA(tuple_data_bytea),
         (char *)tuple->t_data + tuple->t_data->t_hoff, tuple_data_len);
  row_data = PointerGetDatum(tuple_data_bytea);

  // Find all the primary key (pk) columns.
  for (i = 0; i < slot->tts_tupleDescriptor->natts; ++i) {
    if (bms_is_member(slot->tts_tupleDescriptor->attrs[i].attnum,
                      pkindexattrs)) {
    }
  }

  key_size += sizeof(relation->rd_id);
}

static const TableAmRoutine lsmtam_methods = {
  .type = T_TableAmRoutine,

  .scan_begin = lsmt_beginscan,

  .tuple_insert = lsmt_tuple_insert,
};

const TableAmRoutine *
GetLsmtamTableAmRoutine(void)
{
  return &lsmtam_methods;
}

Datum
lsmt_tableam_handler(PG_FUNCTION_ARGS)
{
  PG_RETURN_POINTER(&lsmtam_methods);
}
