////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: QuotaTester.cpp
// Description:
//   
//
// Modification History:
// Created by Frog 2007-04-04
////////////////////////////////////////////////////////////////////////////

#include "diskquota.h"
#include <stdio.h>
#include <string.h>

void printf_result(int expect, int ret, int line)
{
	if (expect == 1 && ret == 0)
		printf("%d, Expect: %s, Real: %s -------%s\n", line, "Correct", "true", "OK");
	else if (expect == 0 && ret < 0)
		printf("%d, Expect: %s, Real: %s -------%s\n", line, "Failed", "failed", "OK");
	else
		printf("%d, Expect: %s, Real: %s -------%s\n", line, (expect)?"Correct":"Failed", (ret<0)?"failed":"true", "FAILED");

	return;
}

int get_user(char *user)
{
	FILE *fp;
	int i;
	char read_buf[1024];
	char login[32];
	char user_name[32];

	memset(read_buf, 0, sizeof(read_buf));
	memset(login, 0, sizeof(login));
	memset(user_name, 0, sizeof(user_name));
	
	fp = fopen("/etc/passwd", "r");

	if (NULL == fp)
		return -1;

	while (fgets(read_buf, sizeof(read_buf), fp))
	{
		for (i=0; i<(int)sizeof(read_buf); i++)
		{ 
			if (read_buf[i] == ':')
				read_buf[i] = ' ';
		}

		
		sscanf(read_buf, "%s %*c %*d %*d %*s %*s %s", user_name, login);
		if (strcmp(login, "/bin/bash") || !strcmp(user_name, "root"))
		{
			memset(read_buf, 0, sizeof(read_buf));
			memset(login, 0, sizeof(login));
			memset(user_name, 0, sizeof(user_name));
			continue;
		}

		fclose(fp);
		strcpy(user, user_name);
		return 0;
	}

	fclose(fp);
	return -1;
}

int QuotaTester()
{
	int ret;
	char result[8196];
	int len;
	char username[32];
	memset(username, 0, sizeof(username));
	get_user(username);

	ret = aos_quota_set_status("/", "on");
	printf_result(1, ret, __LINE__);
	
	ret = aos_quota_set_status("/usr/include", "on");
	printf_result(0, ret, __LINE__);
	
	ret = aos_quota_set_status("/", "off");
	printf_result(1, ret, __LINE__);

	ret = aos_quota_set_status("/", "off");
	printf_result(0, ret, __LINE__);
	
	ret = aos_quota_set_status("/", "on");
	printf_result(1, ret, __LINE__);

	ret = aos_quota_get_status("/", result);
	printf_result(1, ret, __LINE__);
	
	len = sizeof(result);
	ret = aos_quota_retrieve_config(result, &len);
	printf_result(1, ret, __LINE__);

	len = sizeof(result);
	ret = aos_quota_list(result, &len);
	printf_result(1, ret, __LINE__);
	
	ret = aos_quota_add_user("/", username, "100M");
	printf_result(1, ret, __LINE__);
	
	len = sizeof(result);
	ret = aos_quota_retrieve_config(result, &len);
	printf_result(1, ret, __LINE__);

	ret = aos_quota_delete_user("/", username);
	printf_result(1, ret, __LINE__);

	ret = aos_quota_delete_user("/", username);
	printf_result(0, ret, __LINE__);
	
	ret = aos_quota_add_dir("/", "/lib", "100M");
	printf_result(1, ret, __LINE__);

	len = sizeof(result);
	ret = aos_quota_retrieve_config(result, &len);
	printf_result(1, ret, __LINE__);

	ret = aos_quota_add_dir("/", "/aaaaaa", "120M");
	printf_result(0, ret, __LINE__);
	
	len = sizeof(result);
	ret = aos_quota_retrieve_config(result, &len);
	printf_result(1, ret, __LINE__);

	len = sizeof(result);
	ret = aos_quota_retrieve_user(username, result, &len);
	printf_result(1, ret, __LINE__);
	
	len = sizeof(result);
	ret = aos_quota_retrieve_dir("/lib", result, &len);
	printf_result(1, ret, __LINE__);

	ret = aos_quota_delete_dir("/", "/lib");
	printf_result(1, ret, __LINE__);
	
	ret = aos_quota_get_status("/", result);
	printf_result(1, ret, __LINE__);

	ret = aos_quota_set_status("/", "off");
	printf_result(1, ret, __LINE__);

	ret = aos_quota_get_status("/", result);
	printf_result(1, ret, __LINE__);
	
	ret = aos_quota_clear_config();
	printf_result(1, ret, __LINE__);

	len = sizeof(result);
	ret = aos_quota_retrieve_config(result, &len);
	printf_result(1, ret, __LINE__);
	
	return 0;
}

