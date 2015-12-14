////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SecuredShell.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "SecuredShell/ReadConfig.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Util/File.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/Ptrs.h"


AosReadConfig::AosReadConfig(const OmnString &filename)
{
	mFileName = filename;

	readConfig();
}


bool
AosReadConfig::readConfig()
{
	// <CommandLoc>the location of the commands</CommandLoc>
	// <Shells>
	//   <Shell>
	//        <ShellId>shell_id</ShellId>
	//	        <Commands>
	//		        <Cmd>command</Cmd>
	//		        <Cmd>command</Cmd>
	//		        ...
	//				<Cmd>command</Cmd>
	//			</Commands>
	//	</Shell>
	// <Shell>
	//	The definition of the second shell
	//	...
	// </Shell>
	// ...
	// </Shells>

	OmnFile f(mFileName, OmnFile::eReadOnly);
	OmnString config;
	if (!f.readToString(config))
	{
		OmnAlarm << "Failed to read the config file: " << mFileName << enderr;
		return false;
	}

	OmnXmlParser parser(config);
	OmnXmlItemPtr shells = parser.getItem("Shells");
	if (!shells)
	{
		OmnAlarm << "Missing Shells tag: " << config << enderr;
		return false;
	}

	shells->reset();
	OmnXmlItemPtr shell;
	while (shells->hasMore())
	{
		shell = shells->next();
		if (!shell)
		{
			OmnAlarm << "Program error. Shell is null" << enderr;
			return false;
		}

		cout << "Shell ID is: " << shell->getStr("ShellId") << endl;
	}

	return true;
}


AosReadConfig::~AosReadConfig()
{
}

