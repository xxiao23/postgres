/*-------------------------------------------------------------------------
 *
 * lsmtam_handler.c
 *	  Log Structure Merge (LSM) tree table access method code
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/access/lsmt/lsmtam_handler.c
 *
 *
 * NOTES
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "access/tableam.h"
#include "utils/builtins.h"

static const TableAmRoutine lsmtam_methods;

static const TableAmRoutine lsmtam_methods = {
  .type = T_TableAmRoutine,
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
