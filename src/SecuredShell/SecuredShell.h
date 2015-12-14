////////////////////////////////////////////////////////////////////////////
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

#ifndef Omn_SecuredShell_SecuredShell_h
#define Omn_SecuredShell_SecuredShell_h

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "Debug/Except.h"
#include "Debug/Rslt.h"
//#include "KernelInterface/CliSysCmd.h"
#include "Util/String.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util/RCObject.h"
#include "Util/ValList.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"




#include "SecuredShell/Ptrs.h"
#include "SecuredShell/ShellCommand.h"



class AosSecuredShell :  public OmnRCObject
{
	OmnDefineRCObject;
private:
	OmnVList <AosShellCommandPtr>	mCommands;	
	OmnString							mShellId;
	OmnString 						mPassword;

public:
	AosSecuredShell();
	~AosSecuredShell();

	bool 				setShellId(const OmnString &shellId);
	bool    			setPassword(const OmnString &password);
	bool				addCommand(const AosShellCommandPtr &cmd);
	bool				removeCommand(const AosShellCommandPtr &cmd);
	OmnString			getShellId(void);
	OmnString   		getPassword(void);
	bool    			setUpShell(const OmnString &path,const OmnString &username);
	bool  				clearShell(const OmnString &path);
	bool 				removeCommands();
	bool				restoreCommands();
};

#endif

