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
#include "SecuredShell/SecuredShell.h"

#include <sys/types.h>
#include <pwd.h>

AosSecuredShell::AosSecuredShell()
{
}


AosSecuredShell::~AosSecuredShell()
{
}


bool	
AosSecuredShell::addCommand(const AosShellCommandPtr &cmd)
{
	mCommands.append(cmd);
	//cout << cmd->getOrigName() << "has been added to the Shell. " << endl;
	return true;
}


bool	
AosSecuredShell::removeCommand(const AosShellCommandPtr &cmd)
{
	// 
	// Find the command in the list and then remove it.
	//
	mCommands.reset();
	for (int i=0; i<mCommands.entries(); i++)
	{
		AosShellCommandPtr c = mCommands.crtValue();
		if (c.getPtr() == cmd.getPtr())
		{
			mCommands.eraseCrt();
			return true;
		}
		mCommands.next();
	}

	// 
	// Did not find it.
	//
	OmnWarn << "Item not found: " << cmd->getOrigName() << enderr;

	return false;
}

OmnString
AosSecuredShell::getShellId(void){	
	return mShellId;	
}
OmnString
AosSecuredShell::getPassword(void){	
	return mPassword;	
}
bool        
AosSecuredShell::setShellId(const OmnString &shellId)
{
	mShellId = shellId;
	return true;
}
bool
AosSecuredShell::setPassword(const OmnString &password)
{
	mPassword = password;
	return true;
}

bool
AosSecuredShell::setUpShell(const OmnString &path, const OmnString &username)
{
	mCommands.reset();
	for (int i=0; i<mCommands.entries(); i++)
	{
		AosShellCommandPtr c = mCommands.crtValue();
	  	if(!c->checkCommand(1))
			return false;
		mCommands.next();
	}		
	mCommands.reset();
	for (int i=0; i<mCommands.entries(); i++)
	{
		AosShellCommandPtr c = mCommands.crtValue();
		OmnString oName = c->getOrigName();
		OmnString eName = c->getEName();
		OmnString ePath = c->getEPath();
		struct passwd* pw = getpwnam(username);
		if (pw == NULL)
		{
			return false;
		}
		int xPath = access(path,F_OK);
		if(xPath == -1)
		{
			int ret = mkdir(path,0100);
			chown(path,pw->pw_uid,pw->pw_gid);
			if(ret != 0)
				return false;
		}
		else
			if(xPath != 0)
				return false;
		ePath << eName;
		xPath = access(ePath,F_OK);
		OmnString cPath;
		cPath << path << "/" << oName;
		if(xPath == 0)
		{
			c->cp(ePath,cPath);
			c->decrypt(cPath);
			chown(path,pw->pw_uid,pw->pw_gid);
			chown(cPath,pw->pw_uid,pw->pw_gid);
			chmod(path,0100);
			chmod(cPath,0100);
		}
		else
			return false;
		mCommands.next();
	}
	return true;
}

bool
AosSecuredShell::clearShell(const OmnString &path)
{
	mCommands.reset();
	for (int i=0; i<mCommands.entries(); i++)
	{
		AosShellCommandPtr c = mCommands.crtValue();
		OmnString cName = c->getOrigName(); 
	    OmnString cPath;
		cPath << path << cName;
		unlink(cPath);	
		mCommands.next();
	}
	return true;
}


bool	
AosSecuredShell::removeCommands()
{
	mCommands.reset();
	for (int i=0; i<mCommands.entries(); i++)
	{
		AosShellCommandPtr c = mCommands.crtValue();
	  	if(!c->checkCommand(0))
			return false;
		mCommands.next();
	}		
	mCommands.reset();
	for (int i=0; i<mCommands.entries(); i++)
	{
		AosShellCommandPtr c = mCommands.crtValue();
	 	if(!c->removeEncrypted());
		mCommands.next();
	}
	return true;
}
bool	
AosSecuredShell::restoreCommands()
{
	mCommands.reset();
	for (int i=0; i<mCommands.entries(); i++)
	{
		AosShellCommandPtr c = mCommands.crtValue();
	  	if(!c->checkCommand(1) && !c->checkCommand(0))
			return false;
		mCommands.next();
	}		
	mCommands.reset();
	for (int i=0; i<mCommands.entries(); i++)
	{
		AosShellCommandPtr c = mCommands.crtValue();
	  	if(!c->restore())
			return false;
		mCommands.next();
	}		
	return true;
}
