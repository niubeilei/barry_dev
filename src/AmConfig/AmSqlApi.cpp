////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AccessMgr.cpp
// Description:
//
// Modification History:
// 2007-05-24: Created by CHK
////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////
// Global functions for processing
/////////////////////////////////////////////////////////
#include "AmConfig/AmSqlApi.h"

#include "AmUtil/ReturnCode.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int aos_am_db_begin_transaction(sqlite3 *sqliteHandle)
{
	char *errMsg = 0;
	char sql[256];

	snprintf(sql, 256, "BEGIN TRANSACTION");
	int ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		return -(ret+eAosRc_AmDBStatusStart);
	}
	return 0;
}

int aos_am_db_commit_transaction(sqlite3 *sqliteHandle)
{
	char *errMsg = 0;
	char sql[256];

	snprintf(sql, 256, "COMMIT TRANSACTION");
	int ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		return -(ret+eAosRc_AmDBStatusStart);
	}
	return 0;
}

int aos_am_db_rollback_transaction(sqlite3 *sqliteHandle)
{
	char *errMsg = 0;
	char sql[256];

	snprintf(sql, 256, "ROLLBACK TRANSACTION");
	int ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		return -(ret+eAosRc_AmDBStatusStart);
	}
	return 0;
}


// insert_app
// input parameters:
// 	application:'tt'
// 	INSERT INTO "application" VALUES(1, 'tt');
int aos_am_insert_app(sqlite3 *sqliteHandle, 
					  const char *app_name, 
					  const char *desc)
{
	char *errMsg = 0;
	char sql[1024];

	snprintf(sql, 1024, "INSERT INTO application (name, desc) VALUES('%s', '%s')", app_name, desc);

	int ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		return -(ret+eAosRc_AmDBStatusStart);
	}
	return 0;
}

// insert_opr
// input parameters:
// 	operation:	'tt'
// 	INSERT INTO "operation" VALUES(1, 'tt');
int aos_am_insert_opr(sqlite3 *sqliteHandle, 
					  const char *opr_name, 
					  const char *desc )
{
	char *errMsg = 0;
	char sql[1024];

	snprintf(sql, 1024, "INSERT INTO operation (name, desc) VALUES('%s', '%s')", opr_name, desc);

	int ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		return -(ret+eAosRc_AmDBStatusStart);
	}
	return 0;
}


// insert_user
// input parameters:
// 	user_name: 	'tt'
// 	INSERT INTO "user" (user_id, user_name) VALUES(1, 'tt');
int aos_am_insert_user(sqlite3 *sqliteHandle, 
					  const char *user_name, 
					  const char *first_name, 
					  const char *last_name   , 
					  const char *email       , 
					  const char *office_phone, 
					  const char *cell_phone  , 
					  const char *home_phone  , 
					  const char *desc , 
					  const char status )
{
	char *errMsg = 0;
	char sql[1024];

	snprintf(sql, 1024, "INSERT INTO user "
						"(user_name, first_name, last_name, "
						"email, office_phone, cell_phone, "
						"home_phone, desc, status) "
						"VALUES('%s', '%s', '%s'"
						", '%s', '%s', '%s'"
						", '%s', '%s', '%c')", 
						user_name, first_name, last_name, 
						email, office_phone, cell_phone, 
					  	home_phone, desc, status);

	int ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		return -(ret+eAosRc_AmDBStatusStart);
	}
	return 0;
}


// insert_user_authority
// input parameters:
// 	user_name: 	'tt'
// 	user_app: 	'tt'
// 	user_opr: 	'tt'
// 	user_rsc: 	'tt'
// 		BEGIN TRANSACTION;
// 		select user_id from user where user_name='tt'
// 		select id from operation where opr='tt'
// 		select id from application where app='tt'
// 		// if no user_id or no opr_id or no app_id output: return false
// 		INSERT INTO "user_authority" (user_id, app_id, opr_id, resource) VALUES(1, 1, 1, 'tt');
// 		INSERT INTO "user_based_acc" (user_id, user_name, user_opr, user_rsc) VALUES(1, 'tt', 'tt', 'tt');
// 		COMMIT;
int aos_am_insert_user_authority(sqlite3 *sqliteHandle, 
					  			 const char *user_name, 
								 const char *user_app, 
								 const char *user_opr, 
								 const char *user_rsc)
{
	char *errMsg = 0;
	char *zTmpRslt = 0;
	char sql[1024];
	char **dbrslt = 0;
	int i = 1, ret;
	int numRows, numFields;
	int user_id, app_id, opr_id;

	aos_am_db_begin_transaction(sqliteHandle);

	snprintf(sql, 1024, "select user_id from user where user_name='%s'", user_name);
	ret = sqlite3_get_table(sqliteHandle, sql, &dbrslt, &numRows, &numFields, &errMsg);
	if(SQLITE_OK != ret)
	{
		sqlite3_free_table(dbrslt);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}
//	for(i = 1; i <= numRows; i++)
//	{
		if(!dbrslt[i*numFields + 0])
		{
			zTmpRslt = "";
		}
		else
		{
			zTmpRslt = dbrslt[i*numFields + 0];
		}
//	}
	user_id = atoi(zTmpRslt);
	sqlite3_free_table(dbrslt);

	snprintf(sql, 1024, "select id from operation where opr='%s'", user_opr);
	ret = sqlite3_get_table(sqliteHandle, sql, &dbrslt, &numRows, &numFields, &errMsg);
	if(SQLITE_OK != ret)
	{
		sqlite3_free_table(dbrslt);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}
//	for(i = 1; i <= numRows; i++)
//	{
		if(!dbrslt[i*numFields + 0])
		{
			zTmpRslt = "";
		}
		else
		{
			zTmpRslt = dbrslt[i*numFields + 0];
		}
//	}
	opr_id = atoi(zTmpRslt);
	sqlite3_free_table(dbrslt);

	snprintf(sql, 1024, "select id from application where app='%s'", user_opr);
	ret = sqlite3_get_table(sqliteHandle, sql, &dbrslt, &numRows, &numFields, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		sqlite3_free_table(dbrslt);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}
//	for(i = 1; i <= numRows; i++)
//	{
		if(!dbrslt[i*numFields + 0])
		{
			zTmpRslt = "";
		}
		else
		{
			zTmpRslt = dbrslt[i*numFields + 0];
		}
//	}
	app_id = atoi(zTmpRslt);
	sqlite3_free_table(dbrslt);

	if(!user_id || !app_id || !opr_id)
	{
		aos_am_db_rollback_transaction(sqliteHandle);
		return -eAosRc_AmDBUnknownErr;
	}

	snprintf(sql, 1024, 
		"INSERT INTO user_authority (user_id, app_id, opr_id, resource) VALUES(%d, %d, %d, '%s')", 
		user_id, app_id, opr_id, user_rsc);
	ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}
	snprintf(sql, 1024, 
		"INSERT INTO user_based_acc (user_id, user_name, app, opr, resource) VALUES(%d, '%s', '%s', '%s', '%s')", 
		user_id, user_name, user_app, user_opr, user_rsc);
	ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}

	aos_am_db_commit_transaction(sqliteHandle);
	return 0;
}


// insert_role_authority
// input parameters:
// 	role_name: 	'tt'
// 	role_opr: 	'tt'
// 	role_rsc: 	'tt'
// 		BEGIN TRANSACTION;
// 		select id from role where name='tt'
// 		select id from operation where opr='tt'
// 		select id from application where app='tt'
// 		// if no role_id or no opr_id or no app_id output: return false
// 		INSERT INTO "role_authority" (role_id, app_id, opr_id, resource) VALUES(1, 1, 1, 'tt');
// 		select user_id from "r_user_role" where role_id={{role_id}};
// 		INSERT INTO "user_based_acc" (user_id, user_name, app, opr, resource) 
// 				select user.user_id, user.user_name, 'tt', 'tt', 'tt' from 
// 				r_user_role 
// 				join user on r_user_role.user_id=user.user_id 
// 				where role_id=1 ;
// 		COMMIT;
int aos_am_insert_role_authority(sqlite3 *sqliteHandle, 
								 const char *role_name, 
								 const char *role_app, 
								 const char *role_opr, 
								 const char *role_rsc)
{
	char *errMsg = 0;
	char *zTmpRslt = 0;
	char sql[1024];
	char **dbrslt = 0;
	int i = 1, ret;
	int numRows, numFields;
	int role_id, app_id, opr_id;

	aos_am_db_begin_transaction(sqliteHandle);

	snprintf(sql, 1024, "select id from role where name='%s'", role_name);
	ret = sqlite3_get_table(sqliteHandle, sql, &dbrslt, &numRows, &numFields, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		sqlite3_free_table(dbrslt);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}
//	for(i = 1; i <= numRows; i++)
//	{
		if(!dbrslt[i*numFields + 0])
		{
			zTmpRslt = "";
		}
		else
		{
			zTmpRslt = dbrslt[i*numFields + 0];
		}
//	}
	role_id = atoi(zTmpRslt);
	sqlite3_free_table(dbrslt);

	snprintf(sql, 1024, "select id from operation where opr='%s'", role_opr);
	ret = sqlite3_get_table(sqliteHandle, sql, &dbrslt, &numRows, &numFields, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		sqlite3_free_table(dbrslt);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}
//	for(i = 1; i <= numRows; i++)
//	{
		if(!dbrslt[i*numFields + 0])
		{
			zTmpRslt = "";
		}
		else
		{
			zTmpRslt = dbrslt[i*numFields + 0];
		}
//	}
	opr_id = atoi(zTmpRslt);
	sqlite3_free_table(dbrslt);

	snprintf(sql, 1024, "select id from application where app='%s'", role_app);
	ret = sqlite3_get_table(sqliteHandle, sql, &dbrslt, &numRows, &numFields, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		sqlite3_free_table(dbrslt);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}
//	for(i = 1; i <= numRows; i++)
//	{
		if(!dbrslt[i*numFields + 0])
		{
			zTmpRslt = "";
		}
		else
		{
			zTmpRslt = dbrslt[i*numFields + 0];
		}
//	}
	app_id = atoi(zTmpRslt);
	sqlite3_free_table(dbrslt);

	if(!role_id || !app_id || !opr_id)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -eAosRc_AmDBUnknownErr;
	}

	snprintf(sql, 1024, 
		"INSERT INTO role_authority (user_id, app_id, opr_id, resource) VALUES(%d, %d, %d, '%s')", 
		role_id, app_id, opr_id, role_rsc);
	ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}
	snprintf(sql, 1024, 
		"INSERT INTO user_based_acc (user_id, user_name, app, opr, resource) "
		" select user.user_id, user.user_name, '%s', '%s', '%s' from "
		" r_user_role join "
		" user on r_user_role.user_id=user.user_id "
		" where role_id=%d ", 
		role_app, role_opr, role_rsc, role_id);
	ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}

	aos_am_db_commit_transaction(sqliteHandle);
	return 0;
}


// insert_group_authority
// input parameters:
// 	group_name: 	'tt'
// 	group_opr: 	'tt'
// 	group_rsc: 	'tt'
// 		BEGIN TRANSACTION;
// 		select id from group where name='tt'
// 		select id from operation where opr='tt'
// 		select id from application where app='tt'
// 		// if no group_id or no opr_id or no app_id output: return false
// 		INSERT INTO "group_authority" (group_id, app_id, opr_id, resource) VALUES(1, 1, 1, 'tt');
// 		select user_id from "r_user_group" where group_id={{group_id}};
// 		INSERT INTO "user_based_acc" (user_id, user_name, user_opr, user_rsc) VALUES({{user_id}}, 'tt', 'tt', 'tt');
// 		COMMIT;
int aos_am_insert_group_authority(sqlite3 *sqliteHandle, 
								  const char *group_name, 
								  const char *group_app, 
								  const char *group_opr, 
								  const char *group_rsc)
{
	char *errMsg = 0;
	char *zTmpRslt = 0;
	char sql[1024];
	char **dbrslt = 0;
	int i = 1, ret;
	int numRows, numFields;
	int group_id, app_id, opr_id;

	aos_am_db_begin_transaction(sqliteHandle);

	snprintf(sql, 1024, "select id from group where name='%s'", group_name);
	ret = sqlite3_get_table(sqliteHandle, sql, &dbrslt, &numRows, &numFields, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		sqlite3_free_table(dbrslt);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}
//	for(i = 1; i <= numRows; i++)
//	{
		if(!dbrslt[i*numFields + 0])
		{
			zTmpRslt = "";
		}
		else
		{
			zTmpRslt = dbrslt[i*numFields + 0];
		}
//	}
	group_id = atoi(zTmpRslt);
	sqlite3_free_table(dbrslt);

	snprintf(sql, 1024, "select id from operation where opr='%s'", group_opr);
	ret = sqlite3_get_table(sqliteHandle, sql, &dbrslt, &numRows, &numFields, &errMsg);
	if(SQLITE_OK != ret)
	{
		sqlite3_free_table(dbrslt);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}
//	for(i = 1; i <= numRows; i++)
//	{
		if(!dbrslt[i*numFields + 0])
		{
			zTmpRslt = "";
		}
		else
		{
			zTmpRslt = dbrslt[i*numFields + 0];
		}
//	}
	opr_id = atoi(zTmpRslt);
	sqlite3_free_table(dbrslt);

	snprintf(sql, 1024, "select id from application where app='%s'", group_app);
	ret = sqlite3_get_table(sqliteHandle, sql, &dbrslt, &numRows, &numFields, &errMsg);
	if(SQLITE_OK != ret)
	{
		sqlite3_free_table(dbrslt);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}
//	for(i = 1; i <= numRows; i++)
//	{
		if(!dbrslt[i*numFields + 0])
		{
			zTmpRslt = "";
		}
		else
		{
			zTmpRslt = dbrslt[i*numFields + 0];
		}
//	}
	app_id = atoi(zTmpRslt);
	sqlite3_free_table(dbrslt);

	if(!group_id || !app_id || !opr_id)
	{
		aos_am_db_rollback_transaction(sqliteHandle);
		return -eAosRc_AmDBUnknownErr;
	}

	snprintf(sql, 1024, 
		"INSERT INTO group_authority (user_id, app_id, opr_id, resource) VALUES(%d, %d, %d, '%s')", 
		group_id, app_id, opr_id, group_rsc);
	ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}
	snprintf(sql, 1024, 
		"INSERT INTO user_based_acc (user_id, user_name, app, opr, resource) "
		" select user.user_id, user.user_name, '%s', '%s', '%s' from "
		" r_user_group join "
		" user on r_user_group.user_id=user.user_id "
		" where group_id=%d ", 
		group_app, group_opr, group_rsc, group_id);
	ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}

	aos_am_db_commit_transaction(sqliteHandle);
	return 0;
}


// add_user_to_group
// input parameters:
// 	user_name: 	'tt'
// 	group_name: 'tt'
// 		BEGIN TRANSACTION;
// 		select user_id from user where user_name='tt'
// 		select id from group where name='tt'
// 		// if no group_id or no opr_id or no app_id output: return false
// 		// if (select user_id from r_user_group where user_id=1, group_id=1;)
// 			INSERT INTO r_user_group (group_id, user_id) VALUES(1, 1);
//			INSERT INTO user_based_acc (user_id, user_name, app, opr, resource) 
//				select user.user_id, user.user_name, application.app, operation.opr, group_authority.resource 
//				from r_user_group 
//				join user on r_user_group.user_id=user.user_id 
//				join group_authority on group_authority.group_id=r_user_group.group_id 
//				join application on group_authority.app_id=application.id 
//				join operation on group_authority.opr_id=operation.id 
//				where r_user_group.user_id=1 and r_user_group.group_id=1 ;
// 		COMMIT;
int aos_am_add_user_to_group(sqlite3 *sqliteHandle, 
							 const char *user_name, 
							 const char *group_name)
{
	char *errMsg = 0;
	char *zTmpRslt = 0;
	char sql[1024];
	char **dbrslt = 0;
	int i = 1, ret;
	int numRows, numFields;
	int group_id, user_id;

	aos_am_db_begin_transaction(sqliteHandle);

	snprintf(sql, 1024, "select id from group where name='%s'", group_name);
	ret = sqlite3_get_table(sqliteHandle, sql, &dbrslt, &numRows, &numFields, &errMsg);
	if(SQLITE_OK != ret)
	{
		sqlite3_free_table(dbrslt);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}
//	for(i = 1; i <= numRows; i++)
//	{
		if(!dbrslt[i*numFields + 0])
		{
			zTmpRslt = "";
		}
		else
		{
			zTmpRslt = dbrslt[i*numFields + 0];
		}
//	}
	group_id = atoi(zTmpRslt);
	sqlite3_free_table(dbrslt);

	snprintf(sql, 1024, "select user_id from user where user_name='%s'", user_name);
	ret = sqlite3_get_table(sqliteHandle, sql, &dbrslt, &numRows, &numFields, &errMsg);
	if(SQLITE_OK != ret)
	{
		sqlite3_free_table(dbrslt);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}
//	for(i = 1; i <= numRows; i++)
//	{
		if(!dbrslt[i*numFields + 0])
		{
			zTmpRslt = "";
		}
		else
		{
			zTmpRslt = dbrslt[i*numFields + 0];
		}
//	}
	user_id = atoi(zTmpRslt);
	sqlite3_free_table(dbrslt);

	if(!group_id || !user_id)
	{
		aos_am_db_rollback_transaction(sqliteHandle);
		return -eAosRc_AmDBUnknownErr;
	}

	snprintf(sql, 1024, 
			"select user_id from r_user_group where user_id=%d and group_id=%d", 
			user_id, group_id);
	ret = sqlite3_get_table(sqliteHandle, sql, &dbrslt, &numRows, &numFields, &errMsg);
	if(SQLITE_OK != ret)
	{
		sqlite3_free_table(dbrslt);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}
	if(numRows > 0)// for(i = 1; i <= numRows; i++)
	{
//		if(!dbrslt[i*numFields + 0])
//		{
//			zTmpRslt = "";
//		}
//		else
//		{
//			zTmpRslt = dbrslt[i*numFields + 0];
//		}
//		user_id = atoi(zTmpRslt);
		sqlite3_free_table(dbrslt);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -eAosRc_AmDBRcdFoundBfInsert;
	}
	sqlite3_free_table(dbrslt);

	snprintf(sql, 1024, 
			"INSERT INTO r_user_group (user_id, group_id) VALUES(%d, %d)", 
			user_id, group_id);
	ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}

	snprintf(sql, 1024, 
		"INSERT INTO user_based_acc (user_id, user_name, app, opr, resource) "
		" select user.user_id, user.user_name, application.app, operation.opr, group_authority.resource "
		" from r_user_group "
		" join user on r_user_group.user_id=user.user_id "
		" join group_authority on group_authority.group_id=r_user_group.group_id "
		" join application on group_authority.app_id=application.id "
		" join operation on group_authority.opr_id=operation.id "
		" where r_user_group.user_id=%d and r_user_group.group_id=%d ", 
		user_id, group_id);
	ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}

	aos_am_db_commit_transaction(sqliteHandle);
	return 0;
}


// add_user_to_role
// input parameters:
// 	user_name: 	'tt'
// 	role_name: 'tt'
// 		BEGIN TRANSACTION;
// 		select user_id from user where user_name='tt'
// 		select id from role where name='tt'
// 		// if no role_id or no opr_id or no app_id output: return false
// 		// if (select user_id from r_user_role where user_id=1, role_id=1;)
// 			INSERT INTO r_user_role (role_id, user_id) VALUES(1, 1);
//			INSERT INTO user_based_acc (user_id, user_name, app, opr, resource) 
//				select user.user_id, user.user_name, application.app, operation.opr, role_authority.resource 
//				from r_user_role 
//				join user on r_user_role.user_id=user.user_id 
//				join role_authority on role_authority.role_id=r_user_role.role_id 
//				join application on role_authority.app_id=application.id 
//				join operation on role_authority.opr_id=operation.id 
//				where r_user_role.user_id=1 and r_user_role.role_id=1 ;
// 		COMMIT;
int aos_am_add_user_to_role(sqlite3 *sqliteHandle, 
							const char *user_name, 
							const char *role_name)
{
	char *errMsg = 0;
	char *zTmpRslt = 0;
	char sql[1024];
	char **dbrslt = 0;
	int i = 1, ret;
	int numRows, numFields;
	int role_id, user_id;

	aos_am_db_begin_transaction(sqliteHandle);

	snprintf(sql, 1024, "select id from role where name='%s'", role_name);
	ret = sqlite3_get_table(sqliteHandle, sql, &dbrslt, &numRows, &numFields, &errMsg);
	if(SQLITE_OK != ret)
	{
		sqlite3_free_table(dbrslt);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}
//	for(i = 1; i <= numRows; i++)
//	{
		if(!dbrslt[i*numFields + 0])
		{
			zTmpRslt = "";
		}
		else
		{
			zTmpRslt = dbrslt[i*numFields + 0];
		}
//	}
	role_id = atoi(zTmpRslt);
	sqlite3_free_table(dbrslt);

	snprintf(sql, 1024, "select user_id from user where user_name='%s'", user_name);
	ret = sqlite3_get_table(sqliteHandle, sql, &dbrslt, &numRows, &numFields, &errMsg);
	if(SQLITE_OK != ret)
	{
		sqlite3_free_table(dbrslt);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}
//	for(i = 1; i <= numRows; i++)
//	{
		if(!dbrslt[i*numFields + 0])
		{
			zTmpRslt = "";
		}
		else
		{
			zTmpRslt = dbrslt[i*numFields + 0];
		}
//	}
	user_id = atoi(zTmpRslt);
	sqlite3_free_table(dbrslt);

	if(!role_id || !user_id)
	{
		aos_am_db_rollback_transaction(sqliteHandle);
		return -eAosRc_AmDBUnknownErr;
	}

	snprintf(sql, 1024, 
			"select user_id from r_user_role where user_id=%d and role_id=%d", 
			user_id, role_id);
	ret = sqlite3_get_table(sqliteHandle, sql, &dbrslt, &numRows, &numFields, &errMsg);
	if(SQLITE_OK != ret)
	{
		sqlite3_free_table(dbrslt);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}
	if(numRows > 0)// for(i = 1; i <= numRows; i++)
	{
//		if(!dbrslt[i*numFields + 0])
//		{
//			zTmpRslt = "";
//		}
//		else
//		{
//			zTmpRslt = dbrslt[i*numFields + 0];
//		}
//		user_id = atoi(zTmpRslt);
		sqlite3_free_table(dbrslt);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -eAosRc_AmDBRcdFoundBfInsert;
	}
	sqlite3_free_table(dbrslt);

	snprintf(sql, 1024, 
			"INSERT INTO r_user_role (user_id, role_id) VALUES(%d, %d)", 
			user_id, role_id);
	ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}

	snprintf(sql, 1024, 
		"INSERT INTO user_based_acc (user_id, user_name, app, opr, resource) "
		" select user.user_id, user.user_name, application.app, operation.opr, role_authority.resource "
		" from r_user_role "
		" join user on r_user_role.user_id=user.user_id "
		" join role_authority on role_authority.role_id=r_user_role.role_id "
		" join application on role_authority.app_id=application.id "
		" join operation on role_authority.opr_id=operation.id "
		" where r_user_role.user_id=%d and r_user_role.role_id=%d ", 
		user_id, role_id);
	ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}

	aos_am_db_commit_transaction(sqliteHandle);
	return 0;
}

//==================================================================================

// aos_am_del_app
// input parameters:
// 	application:'tt'
// 	delete from application where name='tt';
int aos_am_del_app(sqlite3 *sqliteHandle, 
					  const char *app_name)
{
	char *errMsg = 0;
	char sql[1024];
	int ret = 0;

	aos_am_db_begin_transaction(sqliteHandle);

	snprintf(sql, 1024, "delete from user_based_acc where app='%s'", app_name);
	ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}

	snprintf(sql, 1024, "delete from user_authority where app_id="
						"( select id from application where name='%s' )", app_name);
	ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}

	snprintf(sql, 1024, "delete from role_authority where app_id="
						"( select id from application where name='%s' )", app_name);
	ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}

	snprintf(sql, 1024, "delete from group_authority where app_id="
						"( select id from application where name='%s' )", app_name);
	ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}

	snprintf(sql, 1024, "delete from application where name='%s'", app_name);
	ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}

	aos_am_db_commit_transaction(sqliteHandle);
	return 0;
}


// insert_opr
// input parameters:
// 	operation:	'tt'
// 	delete from operation where name='tt';
int aos_am_del_opr(sqlite3 *sqliteHandle, 
					  const char *opr_name)
{
	char *errMsg = 0;
	char sql[1024];
	int ret = 0;

	aos_am_db_begin_transaction(sqliteHandle);

	snprintf(sql, 1024, "delete from user_based_acc where opr='%s'", opr_name);
	ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}

	snprintf(sql, 1024, "delete from user_authority where opr_id="
						"( select id from operation where name='%s' )", opr_name);
	ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}

	snprintf(sql, 1024, "delete from role_authority where opr_id="
						"( select id from operation where name='%s' )", opr_name);
	ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}

	snprintf(sql, 1024, "delete from group_authority where opr_id="
						"( select id from operation where name='%s' )", opr_name);
	ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}

	snprintf(sql, 1024, "delete from operation where name='%s'", opr_name);
	ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}

	aos_am_db_commit_transaction(sqliteHandle);
	return 0;
}


int aos_am_sql_append_cond(char * sql_cond, const int sql_cond_len, const char *column_name, const char *column_val)
{
	char tmp[256];
	if(column_val)
	{
		snprintf(tmp, 256, " %s='%s'", column_name, column_val);
		if(strlen(sql_cond) > 0)
		{
			snprintf(sql_cond, 1024, " where %s", tmp);
		}
		else
		{
			strncat(sql_cond, " and", 256);
			strncat(sql_cond, tmp, 256);
		}
	}
}

// aos_am_del_user
// input parameters:
// 	user_name: 	'tt'
// 	INSERT INTO "user" (user_id, user_name) VALUES(1, 'tt');
// Note:
// 		If you don't wanna use this condition, just set the condition parameter as NULL.
int aos_am_del_user(sqlite3 *sqliteHandle, 
					  const char *user_name, 
					  const char *first_name, 
					  const char *last_name   , 
					  const char *email       , 
					  const char *office_phone, 
					  const char *cell_phone  , 
					  const char *home_phone  , 
					  const char *desc , 
					  const char status )
{
	char *errMsg = 0;
	char sql[1024];
	char sql_cond[1024] = "", tmp[256] = "";
	int ret = 0;

	if(user_name && strlen(user_name) > 0)
	{
		aos_am_sql_append_cond(sql_cond, 1024, "user_name", user_name);
	}
	if(first_name && strlen(first_name) > 0)
	{
		aos_am_sql_append_cond(sql_cond, 1024, "first_name", first_name);
	}
	if(last_name && strlen(last_name) > 0)
	{
		aos_am_sql_append_cond(sql_cond, 1024, "last_name", last_name);
	}
	if(email && strlen(email) > 0)
	{
		aos_am_sql_append_cond(sql_cond, 1024, "email", email);
	}
	if(office_phone && strlen(office_phone) > 0)
	{
		aos_am_sql_append_cond(sql_cond, 1024, "office_phone", office_phone);
	}
	if(cell_phone && strlen(cell_phone) > 0)
	{
		aos_am_sql_append_cond(sql_cond, 1024, "cell_phone", cell_phone);
	}
	if(home_phone && strlen(home_phone) > 0)
	{
		aos_am_sql_append_cond(sql_cond, 1024, "home_phone", home_phone);
	}
	if(desc && strlen(desc) > 0)
	{
		aos_am_sql_append_cond(sql_cond, 1024, "desc", desc);
	}
	if(status)
	{
		snprintf(tmp, 256, " status='%c'", status);
		if(strlen(sql_cond) > 0)
		{
			snprintf(sql_cond, 1024, " where %s", tmp);
		}
		else
		{
			strncat(sql_cond, " and", 256);
			strncat(sql_cond, tmp, 256);
		}
	}

	aos_am_db_begin_transaction(sqliteHandle);

	snprintf(sql, 1024, "delete from user_based_acc where user_id=(select user_id from user %s)", sql_cond);
	ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}

	snprintf(sql, 1024, "delete from r_user_role where user_id=(select user_id from user %s)", sql_cond);
	ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}

	snprintf(sql, 1024, "delete from r_user_group where user_id=(select user_id from user %s)", sql_cond);
	ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}

	snprintf(sql, 1024, "delete from user_authority where user_id=(select user_id from user %s)", sql_cond);
	ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}

	snprintf(sql, 1024, "delete from user %s", sql_cond);
	ret = sqlite3_exec(sqliteHandle, sql, 0, 0, &errMsg);
	if(SQLITE_OK != ret)
	{
		printf("dbptr[%d] sql[%s] Failed\n", (int)sqliteHandle, sql);
		aos_am_db_rollback_transaction(sqliteHandle);
		return -(ret+eAosRc_AmDBStatusStart);
	}

	aos_am_db_commit_transaction(sqliteHandle);
	return 0;
}


// insert_user_authority
// input parameters:
// 	user_name: 	'tt'
// 	user_opr: 	'tt'
// 	user_rsc: 	'tt'
// 		BEGIN TRANSACTION;
// 		select user_id from user where user_name='tt'
// 		select id from operation where opr='tt'
// 		select id from application where app='tt'
// 		// if no user_id or no opr_id or no app_id output: return false
// 		INSERT INTO "user_authority" (user_id, app_id, opr_id, resource) VALUES(1, 1, 1, 'tt');
// 		INSERT INTO "user_based_acc" (user_id, user_name, user_opr, user_rsc) VALUES(1, 'tt', 'tt', 'tt');
// 		COMMIT;   
int aos_am_del_user_authority(sqlite3 *sqliteHandle, 
					  			 const char *user_name, 
								 const char *user_app, 
								 const char *user_opr, 
								 const char *user_rsc);


// insert_role_authority
// input parameters:
// 	role_name: 	'tt'
// 	role_opr: 	'tt'
// 	role_rsc: 	'tt'
// 		BEGIN TRANSACTION;
// 		select id from role where name='tt'
// 		select id from operation where opr='tt'
// 		select id from application where app='tt'
// 		// if no role_id or no opr_id or no app_id output: return false
// 		INSERT INTO "role_authority" (role_id, app_id, opr_id, resource) VALUES(1, 1, 1, 'tt');
// 		select user_id from "r_user_role" where role_id={{role_id}};
// 		INSERT INTO "user_based_acc" (user_id, user_name, user_opr, user_rsc) VALUES({{user_id}}, 'tt', 'tt', 'tt');
// 		COMMIT;
int aos_am_del_role_authority(sqlite3 *sqliteHandle, 
								 const char *role_name, 
								 const char *role_app, 
								 const char *role_opr, 
								 const char *role_rsc);


// insert_group_authority
// input parameters:
// 	group_name: 	'tt'
// 	group_opr: 	'tt'
// 	group_rsc: 	'tt'
// 		BEGIN TRANSACTION;
// 		select id from group where name='tt'
// 		select id from operation where opr='tt'
// 		select id from application where app='tt'
// 		// if no group_id or no opr_id or no app_id output: return false
// 		INSERT INTO "group_authority" (group_id, app_id, opr_id, resource) VALUES(1, 1, 1, 'tt');
// 		select user_id from "r_user_group" where group_id={{group_id}};
// 		INSERT INTO "user_based_acc" (user_id, user_name, user_opr, user_rsc) VALUES({{user_id}}, 'tt', 'tt', 'tt');
// 		COMMIT;
int aos_am_del_group_authority(sqlite3 *sqliteHandle, 
								  const char *group_name, 
								  const char *group_app, 
								  const char *group_opr, 
								  const char *group_rsc);


// add_user_to_group
// input parameters:
// 	user_name: 	'tt'
// 	group_name: 'tt'
// 		BEGIN TRANSACTION;
// 		select user_id from user where user_name='tt'
// 		select id from group where name='tt'
// 		// if no group_id or no opr_id or no app_id output: return false
// 		// if (select user_id from r_user_group where user_id=1, group_id=1;)
// 			INSERT INTO "r_user_group" (group_id, user_id) VALUES(1, 1);
// 			select opr, app, resource from "group_authority", operation, application 
// 						where group_id=1, operation.id=opr_id, application.id=app_id;
// 			INSERT INTO "user_based_acc" (user_id, user_name, user_opr, user_rsc) 
// 						VALUES({{user_id}}, 'tt', {{opr}}, {{app}}, {{resource}});
// 		COMMIT;
int aos_am_remove_user_from_group(sqlite3 *sqliteHandle, 
							 const char *user_name, 
							 const char *group_name);


// add_user_to_role
// input parameters:
// 	user_name: 	'tt'
// 	role_name: 'tt'
// 		BEGIN TRANSACTION;
// 		select user_id from user where user_name='tt'
// 		select id from role where name='tt'
// 		// if no role_id or no opr_id or no app_id output: return false
// 		// if (select user_id from r_user_role where user_id=1, role_id=1;)
// 			INSERT INTO "r_user_role" (role_id, user_id) VALUES(1, 1);
// 			select opr, app, resource from "role_authority", operation, application 
// 						where role_id=1, operation.id=opr_id, application.id=app_id;
// 			INSERT INTO "user_based_acc" (user_id, user_name, user_opr, user_rsc) 
// 						VALUES({{user_id}}, 'tt', {{opr}}, {{app}}, {{resource}});
// 		COMMIT;
int aos_am_remove_user_from_role(sqlite3 *sqliteHandle, 
							const char *user_name, 
							const char *role_name);

//==================================================================================

