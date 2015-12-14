////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 2007-05-27 Created by Frank Chen
////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>

#include "AmConfig/AmSqlApi.h"
#include "AmUtil/ReturnCode.h"

/* The names of functions that actually do the manipulation. */
int com_list (), com_view (), com_rename (), com_stat (), com_pwd ();
int com_delete (), com_help (), com_cd (), com_quit ();

/* A structure which contains information on the commands this program
   can understand. */
typedef struct {
  char *name;			/* User printable name of the function. */
  Function *func;		/* Function to call to do the job. */
  char *doc;			/* Documentation for this function.  */
} COMMAND;

COMMAND commands[] = {
//  { "cd", com_cd, "Change to directory DIR" },
//  { "delete", com_delete, "Delete FILE" },
//  { "help", com_help, "Display this text" },
//  { "?", com_help, "Synonym for `help'" },
//  { "list", com_list, "List files in DIR" },
//  { "ls", com_list, "Synonym for `list'" },
//  { "pwd", com_pwd, "Print the current working directory" },
//  { "quit", com_quit, "Quit using Fileman" },
//  { "rename", com_rename, "Rename FILE to NEWNAME" },
//  { "stat", com_stat, "Print out statistics on FILE" },
//  { "view", com_view, "View the contents of FILE" },
//  { (char *)NULL, (Function *)NULL, (char *)NULL }
};


int 
main(int argc, char **argv)
{
	char cmd[256];
	char *tmp;
	sqlite3 *sqliteHandle;
	int retVal = 0;

	retVal = sqlite3_open("../../Data/Database/AM_tmp.db", &sqliteHandle);
	if(SQLITE_OK != retVal)
	{
		printf("ptr[%d] Failed[%d]\n[%s]\n", (int)sqliteHandle, retVal, sqlite3_errmsg(sqliteHandle));
		return -(retVal + eAosRc_AmDBStatusStart);
	}
	while (1)
	{
		tmp = readline("Enter Command:$");
		strncpy(cmd, tmp, 256);
		free(tmp);

		if (0 == strcasecmp(cmd, "come on"))
		{
			std::cout << "Allowed" << std::endl;
		}
		else if (0 == strncasecmp(cmd, "add app", 7))
		{
			char name[30];
			char desc[256];
			sscanf(cmd, "add app %s %s", name, desc);
			retVal = aos_am_insert_app(sqliteHandle, name, desc);
			if(0 != retVal)
			{
				std::cout << "failed to insert into application! tt[" << retVal+eAosRc_AmDBStatusStart << "]" << std::endl;
			}
			else
			{
				std::cout << "successfully insert into application!" << std::endl;
			}
		}
		else if (0 == strncasecmp(cmd, "add opr", 7))
		{
			char name[30];
			char desc[256];
			sscanf(cmd, "add opr %s %s", name, desc);
			retVal = aos_am_insert_opr(sqliteHandle, name, desc);
			if(0 != retVal)
			{
				std::cout << "failed to insert! tt[" << retVal+eAosRc_AmDBStatusStart << "]" << std::endl;
			}
			else
			{
				std::cout << "successfully insert!" << std::endl;
			}
		}
		else if (0 == strncasecmp(cmd, "add usr", 7))
		{
			char user_name[30]; 
			char first_name[30]; 
			char last_name   [30]; 
			char email       [30]; 
			char office_phone[30]; 
			char cell_phone  [30]; 
			char home_phone  [30]; 
			char desc[256];
			char status;
			sscanf(cmd, "add usr %s %s %s %s %s %s %s %s %c", 
										user_name, 
										first_name, 
										last_name, 
										email , 
										office_phone, 
										cell_phone, 
										home_phone, 
										desc , 
										&status);
			retVal = aos_am_insert_user(sqliteHandle, 
										user_name, 
										first_name, 
										last_name   , 
										email       , 
										office_phone, 
										cell_phone  , 
										home_phone  , 
										desc , 
										status);
			if(0 != retVal)
			{
				std::cout << "failed to insert! tt[" << retVal+eAosRc_AmDBStatusStart << "]" << std::endl;
			}
			else
			{
				std::cout << "successfully insert!" << std::endl;
			}
		}
		else if (0 == strcasecmp(cmd, "add user"))
		{
			std::cout << "Allowed" << std::endl;
		}
		else if (0 == strcasecmp(cmd, "quit"))
		{
			std::cout << "Quit" << std::endl;
			break;
		}
		else if (0 == strcasecmp(cmd, "q"))
		{
			std::cout << "Quit" << std::endl;
			break;
		}
		else
		{
			std::cout << "Not allowed" << std::endl;
		}
	}

	sqlite3_close(sqliteHandle);

	return 0;
}

