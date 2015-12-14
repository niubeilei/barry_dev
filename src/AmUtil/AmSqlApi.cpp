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
#include "AmUtil/AmSqlApi.h"

#include "AmUtil/ReturnCode.h"
#include <stdio.h>


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
					  const char *home_phone  , 
					  const char *desc , 
					  const char status )
{
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
int aos_am_insert_user_authority(sqlite3 *sqliteHandle, 
					  			 const char *user_name, 
								 const char *user_opr, 
								 const char *user_rsc)
{
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
// 		INSERT INTO "user_based_acc" (user_id, user_name, user_opr, user_rsc) VALUES({{user_id}}, 'tt', 'tt', 'tt');
// 		COMMIT;
int aos_am_insert_role_authority(sqlite3 *sqliteHandle, 
								 const char *role_name, 
								 const char *role_opr, 
								 const char *role_rsc)
{
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
								  const char *group_opr, 
								  const char *group_rsc)
{
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
// 			INSERT INTO "r_user_group" (group_id, user_id) VALUES(1, 1);
// 			select opr, app, resource from "group_authority", operation, application 
// 						where group_id=1, operation.id=opr_id, application.id=app_id;
// 			INSERT INTO "user_based_acc" (user_id, user_name, user_opr, user_rsc) 
// 						VALUES({{user_id}}, 'tt', {{opr}}, {{app}}, {{resource}});
// 		COMMIT;
int aos_am_add_user_to_group(sqlite3 *sqliteHandle, 
							 const char *user_name, 
							 const char *group_name)
{
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
// 			INSERT INTO "r_user_role" (role_id, user_id) VALUES(1, 1);
// 			select opr, app, resource from "role_authority", operation, application 
// 						where role_id=1, operation.id=opr_id, application.id=app_id;
// 			INSERT INTO "user_based_acc" (user_id, user_name, user_opr, user_rsc) 
// 						VALUES({{user_id}}, 'tt', {{opr}}, {{app}}, {{resource}});
// 		COMMIT;
int aos_am_add_user_to_role(sqlite3 *sqliteHandle, 
							const char *user_name, 
							const char *role_name)
{
	return 0;
}

