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
// 09/28/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocSelector_DocSelectorType_h
#define Aos_DocSelector_DocSelectorType_h

#include "Thread/Mutex.h"
#include "Util/String.h" 
#include "Util/HashUtil.h"

#define AOSDOCSELTYPE_CREATED_DOC				"createddoc"
#define AOSDOCSELTYPE_BY_DOCID					"docid"
#define AOSDOCSELTYPE_BY_MYSQL					"mysql"
#define AOSDOCSELTYPE_LOCAL_VAR					"localvar"
#define AOSDOCSELTYPE_BY_OBJID					"objid"
#define AOSDOCSELTYPE_RETRIEVED_DOC				"retrieveddoc"
#define AOSDOCSELTYPE_RECEIVED_DOC				"receiveddoc"
#define AOSDOCSELTYPE_SOURCE_DOC				"sourcedoc"
#define AOSDOCSELTYPE_TARGET_DOC				"targetdoc"
#define AOSDOCSELTYPE_CLOUDID					"cloudid"
#define AOSDOCSELTYPE_WORKING_DOC				"workingdoc"
#define AOSDOCSELTYPE_OLD_DOC					"olddoc"
#define AOSDOCSELTYPE_STMC_BY_DOC				"stmcbydoc"
#define AOSDOCSELTYPE_DOC_BY_USER_GROUPS		"docbygrps"
#define AOSDOCSELTYPE_REQUESTER_ACCT			"requesterdoc"
#define AOSDOCSELTYPE_RDATA_DOC					"rdatadoc"


struct AosDocSelectorType 
{
private:
	static OmnMutex         smLock;
	static AosStr2U32_t     smNameMap;

public:
	enum E
	{
		eInvalid, 
		
		eCreatedDoc,
		eDocid,
		eLocalVar,
		eObjid,
		eRetrievedDoc,
		eReceivedDoc,
		eSourceDoc,
		eTargetDoc,
		eCloudid,
		eWorkingDoc,
		eOldDoc,
		eStmcByDoc,
		eDocByUserGroups,
		eRequesterAcct,
		eRdataDoc,
		eMysql,

		eMax
	};

	static E getFirst() {return (E)(eInvalid+1);}
	static E getLast() {return (E)(eMax-1);}

	static bool isValid(const E type)
	{
		 return (type > eInvalid && type < eMax);
	}

	static bool addName(const OmnString &name, const E type)
	{
OmnScreen << "DocSelector add: " << name << endl;
		smLock.lock();
		if (!smNameMap.empty())
		{
			AosStr2U32Itr_t itr = smNameMap.find(name);
			if (itr != smNameMap.end())
			{
				smLock.unlock();
				OmnAlarm << "Doc Selector name reused: " << name << ":" << type << enderr;
				return false;
			}
		}
		smNameMap[name] = type;
		smLock.unlock();
		return true;
	}

	static E toEnum(const OmnString &name)
	{
		if (name.length() < 1) return eInvalid;
		if (smNameMap.empty()) return eInvalid;
		AosStr2U32Itr_t itr = smNameMap.find(name);
		if (itr == smNameMap.end()) return eInvalid;
		return (E) itr->second;
	}
};

#endif

