#ifndef _INCLUDE_DISKQUOTA_H
#define _INCLUDE_DISKQUOTA_H

#include "aosReturnCode.h"

enum
{
	eAosRc_FailedModifyFstab = eAosRc_DiskQuotaStart+1,
	eAosRc_StatusAlready,
	eAosRc_InvaldStatus,
	eAosRc_InvalidPartition,
	eAosRc_FailedOpenConfigFile,
	eAosRc_StatusNoOn,
	eAosRc_SizeError,
	eAosRc_UserNoExist,
	eAosRc_UserNoBeLimit,
	eAosRc_InvalidDir,
	eAosRc_DirNoBeLimit,
	eAosRc_DirNoExist,
	eAosRc_FailedOpenFile,
	eAosRc_FailedClearStatus,
};

int aos_quota_set_status(char * partition, char* status);
int aos_quota_get_status(char * partition, char* status);
int aos_quota_list(char * result, int* len);
int aos_quota_add_user(char * partition, char * user, char *size);
int aos_quota_delete_user(char * partition, char * user);
int aos_quota_add_dir(char * partition, char * dir, char *size);
int aos_quota_delete_dir(char * partition, char * dir);
int aos_quota_retrieve_user(char * user, char * result, int* len);
int aos_quota_retrieve_dir(char * dir, char * result, int* len);
int aos_quota_clear_config();
int aos_quota_retrieve_config(char *result, int* len);

#endif
