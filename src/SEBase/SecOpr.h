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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef OMN_SEBase_SecOpr_h
#define OMN_SEBase_SecOpr_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

#define AOSSOPR_CREATEDOC				"zky_createdoc"
#define AOSSOPR_MODIFYACCT  			"zky_modacct"
#define AOSSOPR_MODIFYDOC				"zky_modifydoc"

#define AOSSOPR_ADDMEM					"zky_addmem"
#define AOSSOPR_ADDPUBLICOBJID			"zky_addobjid"
#define AOSSOPR_COPY					"zky_copy"
#define AOSSOPR_CREATE_CONTAINER		"zky_createctnr"
#define AOSSOPR_CREATEACCT				"zky_createacct"
#define AOSSOPR_CREATEACD				"zky_createacd"
#define AOSSOPR_CREATEURL				"zky_createurl"
#define AOSSOPR_DELETEDOC				"zky_deletedoc"
#define AOSSOPR_DELETEACCT				"zky_deleteacct"
#define AOSSOPR_DELMEM					"zky_delmem"
#define AOSSOPR_FEEL					"zky_feel"
#define AOSSOPR_LIST					"zky_list"
#define AOSSOPR_MODACD					"zky_modacd"
#define AOSSOPR_READ					"zky_read"
#define AOSSOPR_READACD					"zky_readacd"
#define AOSSOPR_REGHOOK					"zky_reghook"
#define AOSSOPR_REMOVE_URL				"zky_delurl"
#define AOSSOPR_SENDMSG					"zky_sendmsg"
#define AOSSOPR_SVRCMD					"zky_svrcmd"
#define AOSSOPR_UPLOADFILE				"zky_uploadfile"
#define AOSSOPR_VOTE					"zky_vote"
#define AOSSOPR_CREATE_STMC				"zky_crtstmc"
#define AOSSOPR_CREATE_DMOPRARD			"zky_crtdmoprard"
#define AOSSOPR_RESERVED_OBJID			"zky_resvedobjid"
#define AOSSOPR_CREATE_JOB				"zky_createjob"
#define AOSSOPR_RUN_JOB					"zky_runjob"
#define AOSSOPR_ABORT_JOB				"zky_abortjob"
#define AOSSOPR_PAUSE_JOB				"zky_pausejob"
#define AOSSOPR_RESUME_JOB				"zky_resumejob"
#define AOSSOPR_PAUSE_TASK				"zky_pausetask"
#define AOSSOPR_RESUME_TASK				"zky_resumetask"
#define AOSSOPR_USER_DEFINED			"zky_userdefined"
#define AOSSOPR_DELETEDOC				"zky_deletedoc"
#define AOSSOPR_CREATEDOC				"zky_createdoc"

#define AOSSOPR_INVALID				"invalid"


#endif

