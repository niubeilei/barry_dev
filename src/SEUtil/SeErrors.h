////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 	Created: 08/22/2010	by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Omn_SEUtil_SeErrors_h
#define Omn_SEUtil_SeErrors_h

enum AosSeError
{
	eAosErrUndefined,
	eAosErrMissingCloudid,
	eAosErrMissingObjid,
	eAosErrMissingVersionNum,
	eAosErrFailedObjidProc,
	eAosErrFailedRetDelObj,
	eAosErrReqVersionFailed,
	eAosErrInternal,
	eAosErrFailedRetrieve
};


#define AOSERR_OBJID_TOOLONG		"(10001)Objid too long!"
#define AOSERR_OBJID_EMPTY			"(10002)Objid empty!"
#define AOSERR_OBJID_INVALID		"(10003)Objid contains invalid characters!"
#define AOSERR_FAILED_DECOBJID		"(10004)Failed to decompose objid!"
#define AOSERR_MISS_CID				"(10005)Missing Cloud ID!"
#define AOSERR_MISS_OWNER_CID		"(10006)Missing Onwer Cloud ID!"
#define AOSERR_ACCESSED_NOT_FOUND	"(10008)Accessed Not Found!"
#define AOSERR_NO_ARD				"(10009)No Access Record!"
#define AOSERR_MISS_CID_1			"(10010)Missing Cloud ID. Please ask your system administrator to assign a Cloud ID to you!"
#define AOSERR_OBJECT_NOT_FOUND		"(10011)Object not found!"
#define AOSERR_INVALID_VOTE_TYPE	"(10012)Invalid Vote Type!"
#define AOSERR_NOT_AUTHORIZED		"(10013)Not Authorized!"
#define AOSERR_NO_ACCESS_RCD		"(10014)No Access Record!"
#define AOSERR_USER_NOT_FOUND		"(10015)User Not Found!"
#define AOSERR_CTNR_NOT_FOUND		"(10016)Container Not Found!"
#define AOSERR_USER_ALREADY_DEF		"(10017)User Already Defined!"
#define AOSERR_MISSING_CONTAINER	"(10018)Missing Container!"
#define AOSERR_MISSING_USERINFO		"(10019)Missing User Object!"
#define AOSERR_UNREC_OPERATOR		"(10020)Unrecognized Operator!"
#define AOSERR_MISSING_OBJECT		"(10021)Missing Object!"

#define AOSERR_INTERNAL_0001		"(10001)Error!"
#define AOSERR_INTERNAL_0002		"(10002)Error!"
#define AOSERR_INTERNAL_0003		"(10003)Error: "
#define AOSERR_INTERNAL_0004		"(10004)Error: "
#define AOSERR_INTERNAL_0005		"(10005)Error: "
#define AOSERR_INTERNAL_0006		"(10006)Error: "
#define AOSERR_INTERNAL_0007		"(10007)Error: "
#define AOSERR_INTERNAL_0008		"(10008)Error: "
#define AOSERR_INTERNAL_0009		"(10009)Error: "
#define AOSERR_INTERNAL_0010		"(10010)Error: "
#define AOSERR_INTERNAL_0011		"(10011)Error: "
#define AOSERR_INTERNAL_0012		"(10012)Error"
#define AOSERR_INTERNAL_0013		"(10012)Error"

#endif
#endif
