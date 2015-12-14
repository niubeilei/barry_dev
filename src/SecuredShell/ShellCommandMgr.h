
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ShellCommandMgr.h
// Description:
//   
//
// Modification History:
// 11/16/2006: Created by JZZ
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_SecuredShell_ShellCommandMgr_h
#define Omn_SecuredShell_ShellCommandMgr_h

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "Debug/Except.h"
#include "Debug/Rslt.h"
#include "SingletonClass/SingletonTplt.h"
#include "SingletonClass/SingletonObj.h"
#include "SingletonClass/SingletonObjId.h"
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




OmnDefineSingletonClass(AosShellCommandMgrSingleton,
		AosShellCommandMgr,
		AosShellCommandMgrSelf,
		OmnSingletonObjId::eShellCommandMgr,
		"ShellCommandMgr");

class AosShellCommandMgr
{
private:
	OmnVList<AosShellCommandPtr>	mCommands;
	//AosShellCommandMgr						mCmdMgr;
	OmnString						mFileName;

public:
	AosShellCommandMgr();

	~AosShellCommandMgr();
	bool        start();
	bool        stop();
	OmnRslt     config(const OmnXmlParserPtr &conf);
	static AosShellCommandMgr* getSelf();

	AosShellCommandPtr addCommand(const OmnString &origPath,
													     const OmnString &origName,
													     const OmnString &encryptedName,
													     const OmnString &encryptedPath);
	bool		removeCommand(const AosShellCommandPtr &cmd);
	bool		encryptCommands();
	bool		createCommands();
	bool		removeCommands();
	bool		restoreCommands();
	OmnString	list();
};

#endif

