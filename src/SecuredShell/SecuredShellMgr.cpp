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
#include "SecuredShell/SecuredShellMgr.h"


#include "Porting/TimeOfDay.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Tracer/Tracer.h"

#include <pthread.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>

OmnSingletonImpl(AosSecuredShellMgrSingleton,
		AosSecuredShellMgr,
		AosSecuredShellMgrSelf,
		"AosSecuredShellMgr");


const  u32 sgMaxUsers = 100;
static userInfo sgUsers[sgMaxUsers];
const  u32 sgMaxActiveShells = 100;
static AosActiveShellInfo sgActiveShells[sgMaxActiveShells];
static int sgNumActiveShells = 0;
static bool flagRemoveActiveShell = false;

const OmnString sgUserHome = "/home/";
const static OmnString sgSecuredShellConfigFileName = "config.txt";

AosSecuredShellMgr::AosSecuredShellMgr()
{
}


AosSecuredShellMgr::~AosSecuredShellMgr(){
}


bool        
AosSecuredShellMgr::start(){
	return true;
}


bool        
AosSecuredShellMgr::stop(){
	return true;
}


OmnRslt     
AosSecuredShellMgr::config(const OmnXmlParserPtr &parser){
	// 
	// Read the configuration and create the shells.
	//
	// mFileName = configFile;
	// OmnFile f(mFileName, OmnFile::eReadOnly);   
	// OmnString config;   
	// if (!f.readToString(config))    {       
	    // OmnAlarm << "Failed to read the config file: " << mFileName << enderr;      
	    // return false;   
	// }   
	// OmnXmlParser parser(config);
	OmnXmlItemPtr location = parser->getItem("CommandLoc");
	OmnString eCmdLoc = location->getStr();
	OmnXmlItemPtr shells = parser->getItem("Shells");
	//AosShellCommandMgr CmdMgr; 
	if (!shells)
	{
	//	OmnAlarm << "Missing Shells tag: " << config << enderr;
		OmnAlarm << "Missing Shells tag: "  << enderr;
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

		// 
		// Create an instance of AosSecuredShell
		//
		AosSecuredShellPtr theShell = OmnNew AosSecuredShell();
		addShell(theShell);

		shell->reset();
		OmnXmlItemPtr ShellId;
		ShellId = shell->next();
	//	cout << "Shell ID is: " << ShellId->getStr() << endl;
		theShell->setShellId(ShellId->getStr());
		OmnXmlItemPtr Password;
		Password = shell->next();
		theShell->setPassword(Password->getStr());
		OmnXmlItemPtr commands;
		while (shell->hasMore())
		{
			commands = shell->next();


			// 
			// AosShellCommandPtr cmd = OmnNew AosShellCommand(...);
			//
			//

			if(!commands)
			{
				OmnAlarm << "Program error. Shell has no command" << enderr;
				return false;
			}
		
			//cout<< "pointed to commands" << endl;
			OmnXmlItemPtr cmd ;
			
			while (commands->hasMore())
			{
				
				cmd = commands->next();
				if(!cmd)
				{
					OmnAlarm << "Program error.Shell has no command"<<enderr;
					return false;
				} 
				AosShellCommandPtr	theCmd = mCmdMgr.addCommand(cmd->getStr("Path"),cmd->getStr("OrigName"),cmd->getStr("EName"),eCmdLoc);
				//	cout <<ShellId->getStr() << " has command:" << cmd->getStr() << endl;
					theShell->addCommand(theCmd);
			
			}
		}
	}


	return true;
}


bool	
AosSecuredShellMgr::addShell(const AosSecuredShellPtr &shell){
	mShells.append(shell);
//	cout << shell->getShellId() << "has been added to the Manager. " << endl;
	return true;
}


bool	
AosSecuredShellMgr::removeShell(const AosSecuredShellPtr &shell){
	// 
	// Find the command in the list and then remove it.
	//
	mShells.reset();
	for (int i=0; i<mShells.entries(); i++)
	{
		AosSecuredShellPtr c = mShells.crtValue();
		if (c.getPtr() == shell.getPtr())
		{
			mShells.eraseCrt();
			return true;
		}
		mShells.next();
	}

	// 
	// Did not find it.
	//
	OmnWarn << "Item not found: " << shell->getShellId() << enderr;

	return false;
}
bool
AosSecuredShellMgr::checkShell(const OmnString &shellId, 
									const OmnString &passwd)
{
	mShells.reset();
	for (int i=0; i<mShells.entries(); i++)
	{
		AosSecuredShellPtr c = mShells.crtValue();
		if ((c->getShellId() == shellId)&&(c->getPassword() == passwd))
			return true;
		mShells.next();
	}
	return false;
}

int 
AosSecuredShellMgr::startShell(
				const OmnString &shellId, 
				const OmnString &username, 
				u32 duration)
{
	int state = addActiveShell(shellId, username ,duration);
	if(state > 0)
	{
		return state;
	}
	OmnString path;
	path << sgUserHome << username << "/" << shellId << "_bin";

	//restore shell's commands.
	//
	//if user's home dir already exists.
	//
	AosSecuredShellPtr theShell = getShellPtr(shellId);
	
	if(!theShell->setUpShell(path,username))
	{	
		return 4;
	}
	OmnString filename;
	filename << sgUserHome << username <<"/.bash_profile";
	struct passwd* pw = getpwnam(username);
	chown(filename,pw->pw_uid,pw->pw_gid);
	//delete [] pw;	
	OmnString mode = "w";	
	FILE* stream;	
	stream = fopen(filename,mode);	
	OmnString s;	
	int r = 255;	
	s << "PATH=" << path << "/";	
	r = fputs(s,stream);	
	fclose(stream);
	chmod(filename,0644);
	sgActiveShells[sgNumActiveShells].shellId = shellId;
	sgActiveShells[sgNumActiveShells].username = username;
	sgActiveShells[sgNumActiveShells].duration = duration;
	sgActiveShells[sgNumActiveShells].startTime = OmnSystemRelativeSec1();
	sgNumActiveShells++;

	//start remove_active_shell thread ;
	//	
	if( flagRemoveActiveShell == false )
	{
		flagRemoveActiveShell = true;
		pthread_t remove_active_shell;
		//NULL
		pthread_create(&remove_active_shell,
						NULL,
						removeActiveShell,
						NULL);

	}
	return state;
}

bool 
AosSecuredShellMgr::stopShell(
				const OmnString &shellId, 
				const OmnString &username )
{
	for(int i=0;i<sgNumActiveShells;i++)
	{
		if( (sgActiveShells[i].shellId == shellId) && (sgActiveShells[i].username == username) )
		{	OmnString path;
			path << sgUserHome << username << "/" << shellId << "_bin/";
			AosSecuredShellPtr theShell = getShellPtr(shellId);
			theShell->clearShell(path);	
			sgActiveShells[i] = sgActiveShells[sgNumActiveShells-1];
			sgNumActiveShells--;
			return true ;
		}					
	}
	return false;
}


bool 
AosSecuredShellMgr::stopAllShell(OmnString &rslt)
{
	OmnFile file(sgSecuredShellConfigFileName, OmnFile::eReadOnly);
	if (!file.isGood())
	{
		OmnAlarm << "Failed to read config file: " 
			<< sgSecuredShellConfigFileName << enderr;
		rslt = "Failed to read config file: ";
		rslt << sgSecuredShellConfigFileName;
		return false;
	}

	OmnString data;
	file.readToString(data);
	OmnXmlParserPtr parser = OmnNew OmnXmlParser(data);
	config(parser);
	int userNum = setUsers();	
	while(userNum > 0)	
	{		
		mShells.reset();
		for (int i=0; i<mShells.entries(); i++)
		{
			AosSecuredShellPtr c = mShells.crtValue();
			OmnString path;
			path << sgUserHome << sgUsers[userNum-1].name << "/" << c->getShellId() << "_bin/";
			c->clearShell(path);
			mShells.next();
		}
		userNum--;	
	}
	sgNumActiveShells = 0;
	rslt << "Active shells have been cleared";
	return true ;
}

int
AosSecuredShellMgr::addActiveShell(const OmnString &shellId, 
									const OmnString &username, 
									u32 duration)
{
	for(int i=0;i<sgNumActiveShells;i++)
	{
		if( (sgActiveShells[i].shellId == shellId) && (sgActiveShells[i].username == username) )
		{
			sgActiveShells[i].duration += duration;
			return 1 ;
		}
		
	}
	for(int i=0;i<sgNumActiveShells;i++)
	{
	if(sgActiveShells[i].username == username )
		{
			OmnAlarm  <<"Sorry.you have started a shell. If you want to start another shell,please logout first. " << enderr;
			return 2 ;
		}
	}
	if (sgNumActiveShells >= sgMaxActiveShells)
	{
		return 3;
	}

	return 0;
}


void*
AosSecuredShellMgr::removeActiveShell(void* arg)
{
	while(true)
		{
		//	cout << "Here is the pthread call back function."<< endl;
			int crtSec = OmnSystemRelativeSec1();

			for (int i=0; i<sgNumActiveShells; i++)
			{
				if (crtSec - sgActiveShells[i].startTime > sgActiveShells[i].duration)
				{
					OmnString path;
	    			path << sgUserHome << sgActiveShells[i].username << "/" << sgActiveShells[i].shellId << "_bin" ;
					DIR * dir;
					struct dirent * ptr;
					dir =opendir(path);
					while((ptr = readdir(dir))!=NULL)
					{
						OmnString filename;
						filename << path << "/" << ptr->d_name; 
						unlink(filename);
					}
					closedir(dir);
					sgActiveShells[i] = sgActiveShells[sgNumActiveShells-1];
					sgNumActiveShells--;
				}
			}
			sleep(1);
		}
}

AosSecuredShellPtr
AosSecuredShellMgr::getShellPtr(const OmnString &shellId)
{
	mShells.reset();
	for (int i=0; i<mShells.entries(); i++)
	{
		AosSecuredShellPtr c = mShells.crtValue();
		if ((c->getShellId() == shellId))
			return c;
		mShells.next();
	}
	return 0;
}

bool 
AosSecuredShellMgr::create()
{
	if(mCmdMgr.createCommands())
		return true;
	return false;
}

bool 
AosSecuredShellMgr::removeAll()
{
	if(mCmdMgr.removeCommands())
		return true;
	return false;
}

bool 
AosSecuredShellMgr::removeShell(const OmnString &shellId)
{
	mShells.reset();
	for (int i=0; i<mShells.entries(); i++)
	{
		AosSecuredShellPtr c = mShells.crtValue();
		if ((c->getShellId() == shellId))
			{
				if(c->removeCommands())
					return true;
			}
		mShells.next();
	}
	return false;
}

bool 
AosSecuredShellMgr::restoreAll()
{
	if(mCmdMgr.restoreCommands())
		return true;
	return false;
}

bool 
AosSecuredShellMgr::restoreShell(const OmnString &shellId)
{
	mShells.reset();
	for (int i=0; i<mShells.entries(); i++)
	{
		AosSecuredShellPtr c = mShells.crtValue();
		if ((c->getShellId() == shellId))
			{
				if(c->restoreCommands())
					return true;
			}
		mShells.next();
	}
	return false;
}
bool 
AosSecuredShellMgr::startSecuredShell(const OmnString &username,
				const OmnString &shellId,
				const OmnString &password,
				int duration,
				OmnString &rslt)
{
	// 1. Read the configuration file
	OmnFile file(sgSecuredShellConfigFileName, OmnFile::eReadOnly);
	if (!file.isGood())
	{
		OmnAlarm << "Failed to read config file: " 
			<< sgSecuredShellConfigFileName << enderr;
		rslt = "Failed to read config file: ";
		rslt << sgSecuredShellConfigFileName;
		return false;
	}

	OmnString data;
	file.readToString(data);
	OmnXmlParserPtr parser = OmnNew OmnXmlParser(data);
	config(parser);

	// 2. Check whether the shell is defined
   	if (!checkShell(shellId, password))
	{
		rslt = "Shell not found or password does not match";
		return false;
	}

	// 3. Start the shell

	int length = 0;
	if(duration < 0)
	{
		OmnAlarm << " it is not a positive number;" << enderr;
		rslt = "You shoule give a positive number as duration!";
	}

	
	switch(startShell(shellId,username,duration))
	{
		case 0:
			rslt = "Your Shell -- ";
			rslt << shellId << " has been started.";
			return true;
		case 1:
			rslt = "duration added";
			return true;
		case 2:
			rslt = "Sorry.you have started a shell. If you want to start another shell,please logout first. " ;
			return false;
		case 3:
			rslt = "Too many active shells.";
			return false;
		default:
			rslt = "Can't set up your shell!";
			return false;
	}
}

bool
AosSecuredShellMgr::stopSecuredShell(const OmnString &username,
				const OmnString &shellId,
				const OmnString &password,
				OmnString &rslt)
{
	// 1. Read the configuration file
	OmnFile file(sgSecuredShellConfigFileName, OmnFile::eReadOnly);
	if (!file.isGood())
	{
		OmnAlarm << "Failed to read config file: " 
			<< sgSecuredShellConfigFileName << enderr;
		rslt = "Failed to read config file: ";
		rslt << sgSecuredShellConfigFileName;
		return false;
	}

	OmnString data;
	file.readToString(data);
	OmnXmlParserPtr parser = OmnNew OmnXmlParser(data);
	config(parser);

	// 2. Check whether the shell is defined
  	if (!checkShell(shellId, password))
	{
		rslt = "Shell not found or password does not match";
		return false;
	}

	// 3. Stop the shell
	//cout <<" shell checked" << endl;	
	if(stopShell(shellId,username))
	{
		rslt << username <<  "'s shell -- "<< shellId << " has been stoped.";
		return true;
	}
	rslt << "You haven't started this shell or shell has already been removed.";
	return false;
}

bool
AosSecuredShellMgr::createSecuredShell(OmnString &rslt)
{
		// 1. Read the configuration file
	OmnFile file(sgSecuredShellConfigFileName, OmnFile::eReadOnly);
	if (!file.isGood())
	{
		OmnAlarm << "Failed to read config file: " 
			<< sgSecuredShellConfigFileName << enderr;
		rslt = "Failed to read config file: ";
		rslt << sgSecuredShellConfigFileName;
		return false;
	}

	OmnString data;
	file.readToString(data);
	OmnXmlParserPtr parser = OmnNew OmnXmlParser(data);
	config(parser);

	if(!create())
	{
		rslt << "Failed to create secured shell!";
		return false;
	}
	rslt << "Secured shells have been created.";
	return true;
}

bool
AosSecuredShellMgr::restoreSecuredShell(const OmnString &shellId,OmnString &rslt)
{
		// 1. Read the configuration file
	OmnFile file(sgSecuredShellConfigFileName, OmnFile::eReadOnly);
	if (!file.isGood())
	{
		OmnAlarm << "Failed to read config file: " 
			<< sgSecuredShellConfigFileName << enderr;
		rslt = "Failed to read config file: ";
		rslt << sgSecuredShellConfigFileName;
		return false;
	}

	OmnString data;
	file.readToString(data);
	OmnXmlParserPtr parser = OmnNew OmnXmlParser(data);
	config(parser);
	
	if(shellId == "all")
	{
		if(!restoreAll())
		{
			rslt << "Failed to restore the commands!";
			return false;
		}
		rslt << "All commands have been restored!";
		return true;
	}
	if(restoreShell(shellId))
	{
		rslt << shellId << "'s commands have been restored!";
		return true;
	}
	rslt << "Wrong shell id or can't restore the shell's commands";
	return false;
}

bool 
AosSecuredShellMgr::removeSecuredShell(const OmnString &shellId,OmnString &rslt)
{
	// 1. Read the configuration file
	OmnFile file(sgSecuredShellConfigFileName, OmnFile::eReadOnly);
	if (!file.isGood())
	{
		OmnAlarm << "Failed to read config file: " 
			<< sgSecuredShellConfigFileName << enderr;
		rslt = "Failed to read config file: ";
		rslt << sgSecuredShellConfigFileName;
		return false;
	}

	OmnString data;
	file.readToString(data);
	OmnXmlParserPtr parser = OmnNew OmnXmlParser(data);
	config(parser);	
	if(shellId == "all")
	{
		if(!removeAll())
		{
			rslt << "Failed to remove the commands!";
			return false;
		}
		rslt << "All commands have been removed!";
		return true;
	}
	if(removeShell(shellId))
	{
		rslt << shellId << "'s commands have been removed!";
		return true;
	}
	rslt << "Wrong shell id or can't remove the shell's commands";
	return false;	
}


int AosSecuredShellMgr::setUsers()
{
	struct passwd *user;
	int i = 0;
	while((user = getpwent())!=0)
	{
		if( (user->pw_uid >= 500) && (user->pw_uid < 65534) )
		{
		sgUsers[i].name = user->pw_name;
		sgUsers[i].home = user->pw_dir;
		i++;
		}
	}
	delete [] user;
	endpwent();
	return i;
}
