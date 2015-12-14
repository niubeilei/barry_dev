
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ShellCommand.h
// Description:
//   
//
// Modification History:
// 11/14/2006: Created by JZZ
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_SecuredShell_SecuredShellMgr_h
#define Omn_SecuredShell_SecuredShellMgr_h

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "Debug/Except.h"
#include "keyman.h"
#include "Debug/Rslt.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/String.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util/RCObject.h"
#include "Util/ValList.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"




#include "SecuredShell/Ptrs.h"
#include "SecuredShell/SecuredShell.h"
#include "SecuredShell/ShellCommand.h"
#include "SecuredShell/ShellCommandMgr.h"




OmnDefineSingletonClass(AosSecuredShellMgrSingleton,
		AosSecuredShellMgr,
		AosSecuredShellMgrSelf,
		OmnSingletonObjId::eSecuredShellMgr,
		"SecuredShellMgr");

class AosSecuredShellMgr
{
private:
	OmnVList<AosSecuredShellPtr>	mShells;
	OmnString						mFileName;
	AosShellCommandMgr				mCmdMgr;

public:
	AosSecuredShellMgr();
	~AosSecuredShellMgr();

	bool        start();
	bool        stop();
	OmnRslt     config(const OmnXmlParserPtr &conf);
	static 			AosSecuredShellMgr* getSelf();

	
	bool		removeShell(const AosSecuredShellPtr &shell);
	bool		checkShell(const OmnString &shellId, const OmnString &passwd);

	int			startShell(const OmnString &shellId, 
							const OmnString &username, 
							u32 duration);
	bool		stopShell(const OmnString &shellId,const OmnString &username);

	bool		stopAllShell(OmnString &rslt);
	
	static void* removeActiveShell(void* arg);
	
	bool create();
	bool removeAll();
	bool removeShell(const OmnString &shellId);
	bool restoreAll();
	bool restoreShell(const OmnString &shellId);
	bool startSecuredShell(const OmnString &username,
							const OmnString &shellId,
							const OmnString &password,
							int duration,
							OmnString &rslt);
	bool stopSecuredShell(const OmnString &username,
							const OmnString &shellId,
							const OmnString &password,
							OmnString &rslt);
	bool createSecuredShell(OmnString &rslt);
	bool restoreSecuredShell(const OmnString &shellId,OmnString &rslt);
	bool removeSecuredShell(const OmnString &shellId,OmnString &rslt);
	int	 setUsers();
private:
	bool				addShell(const AosSecuredShellPtr &shell);
	AosSecuredShellPtr 	getShellPtr(const OmnString &shellId);
	int			 		addActiveShell(const OmnString &shellId, 
							const OmnString &username, 
							u32 duration);

};

struct AosActiveShellInfo
{
	OmnString shellId;
	OmnString username;
	u32		  	startTime;
	u32       duration;
};

struct userInfo
{
	OmnString name;
	OmnString home;
};

#endif

