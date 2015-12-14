////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ShellCommandMgr.cpp
// Description:
//   
//
// Modification History:
// 11/16/2006: Created by JZZ
// 
////////////////////////////////////////////////////////////////////////////
#include "SecuredShell/ShellCommandMgr.h"

#include "SingletonClass/SingletonImpl.cpp"


OmnSingletonImpl(AosShellCommandMgrSingleton,
		AosShellCommandMgr,
		AosShellCommandMgrSelf,
		"AosShellCommandMgr");

AosShellCommandMgr::AosShellCommandMgr(){
		
}


AosShellCommandMgr::~AosShellCommandMgr(){
}


bool        
AosShellCommandMgr::start(){
	return true;
}


bool        
AosShellCommandMgr::stop(){
	return true;
}


OmnRslt     
AosShellCommandMgr::config(const OmnXmlParserPtr &parser){

}
AosShellCommandPtr	
AosShellCommandMgr::addCommand(const OmnString &origPath,
							const OmnString &origName,
							const OmnString &encryptedName,
							const OmnString &encryptedPath){
		
		AosShellCommandPtr cmd = OmnNew AosShellCommand(origPath,origName,encryptedName,encryptedPath);
		//cmd->mEncrypted = false ;
		//cmd->mRemoved = false ;
		mCommands.reset();
		for (int i=0; i<mCommands.entries(); i++)
		{
			AosShellCommandPtr c = mCommands.crtValue();
			if (c->getOrigName() == cmd->getOrigName())
			{
				//mCommands.eraseCrt();
				return c;
			}
			mCommands.next();
		}
	
		// 
		// Did not find it.
		//

		mCommands.append(cmd);
		return cmd;		
		}
		
bool	
AosShellCommandMgr::removeCommand(const AosShellCommandPtr &cmd)
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

bool	
AosShellCommandMgr::encryptCommands(){
		//cout << "The commands have been encrypted . "<< endl;
		return true;
}


bool
AosShellCommandMgr::createCommands()
{
	mCommands.reset();
	for (int i=0; i<mCommands.entries(); i++)
	{
	    AosShellCommandPtr c = mCommands.crtValue();
		if(!c->checkCommand(0) && !c->checkCommand(1))
			return false;
		mCommands.next();
	}
	mCommands.reset();
	for (int i=0; i<mCommands.entries(); i++)
	{
	    AosShellCommandPtr c = mCommands.crtValue();
		if(!c->create())
			return false;
		mCommands.next();
	}
	return true;	
}


bool	
AosShellCommandMgr::removeCommands()
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
		if(!c->removeEncrypted())
			return false;
		mCommands.next();
	}
	return true;
}


bool	
AosShellCommandMgr::restoreCommands()
{
	mCommands.reset();
	for (int i=0; i<mCommands.entries(); i++)
	{
	    AosShellCommandPtr c = mCommands.crtValue();
		if(!c->checkCommand(0) && !c->checkCommand(1))
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


OmnString 
AosShellCommandMgr::list()
{
	mCommands.reset();
	for (int i=0; i<mCommands.entries(); i++)
	{
		AosShellCommandPtr c = mCommands.crtValue();
		cout << c->getOrigName() << '\n' << endl;
		mCommands.next();
	}
}
