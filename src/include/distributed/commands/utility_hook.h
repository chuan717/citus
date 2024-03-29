/*-------------------------------------------------------------------------
 *
 * utility_hook.h
 *	  Citus utility hook and related functionality.
 *
 * Copyright (c) 2012-2016, Citus Data, Inc.
 *-------------------------------------------------------------------------
 */

#ifndef MULTI_UTILITY_H
#define MULTI_UTILITY_H

#include "postgres.h"

#include "distributed/version_compat.h"
#include "utils/relcache.h"
#include "tcop/utility.h"

typedef enum
{
	PROPSETCMD_INVALID = -1,
	PROPSETCMD_NONE, /* do not propagate SET commands */
	PROPSETCMD_LOCAL, /* propagate SET LOCAL commands */
	PROPSETCMD_SESSION, /* propagate SET commands, but not SET LOCAL ones */
	PROPSETCMD_ALL /* propagate all SET commands */
} PropSetCmdBehavior;
extern PropSetCmdBehavior PropagateSetCommands;
extern bool EnableDDLPropagation;

/*
 * A DDLJob encapsulates the remote tasks and commands needed to process all or
 * part of a distributed DDL command. It hold the distributed relation's oid,
 * the original DDL command string (for MX DDL propagation), and a task list of
 * DDL_TASK-type Tasks to be executed.
 */
typedef struct DDLJob
{
	Oid targetRelationId;      /* oid of the target distributed relation */
	bool concurrentIndexCmd;   /* related to a CONCURRENTLY index command? */
	bool executeSequentially;
	const char *commandString; /* initial (coordinator) DDL command string */
	List *taskList;            /* worker DDL tasks to execute */
} DDLJob;


extern void multi_ProcessUtility(PlannedStmt *pstmt, const char *queryString,
								 ProcessUtilityContext context, ParamListInfo params,
								 struct QueryEnvironment *queryEnv, DestReceiver *dest,
								 char *completionTag);
extern void CitusProcessUtility(Node *node, const char *queryString,
								ProcessUtilityContext context, ParamListInfo params,
								DestReceiver *dest, char *completionTag);
extern void MarkInvalidateForeignKeyGraph(void);
extern void InvalidateForeignKeyGraphForDDL(void);
extern List * DDLTaskList(Oid relationId, const char *commandString);
extern bool AlterTableInProgress(void);

#endif /* MULTI_UTILITY_H */
