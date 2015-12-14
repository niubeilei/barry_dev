////////////////////////////////////////////////////////////////////////////
////
//// Copyright (C) 2005
//// Packet Engineering, Inc. All rights reserved.
////
//// Redistribution and use in source and binary forms, with or without
//// modification is not permitted unless authorized in writing by a duly
//// appointed officer of Packet Engineering, Inc. or its derivatives
////
//// File Name: ShellCommand.cpp
//// Description:
////   
////
//// Modification History:
//// 11/14/2006: Created by JZZ
//// 
//////////////////////////////////////////////////////////////////////////////

#include "Util/File.h"
#include "SecuredShell/ShellCommand.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h> 


AosShellCommand::AosShellCommand(const OmnString &cmdPath,
								 const OmnString &origName,
							     const OmnString &encryptedName,
							     const OmnString &encryptedPath){
	mOrigCmdPath 		= cmdPath;
	mOrigCmdName 		= origName;
	mEncryptedCmdName 	= encryptedName;
	mEncryptedCmdPath 	= encryptedPath;
//	mEncrypted 			= false;
//	mRemoved			= false; 			
}
AosShellCommand::~AosShellCommand(){
}

	
OmnString 
AosShellCommand::getOrigName(void)
{
	return mOrigCmdName;
}

OmnString 
AosShellCommand::getOrigPath(void)
{
	return mOrigCmdPath;
}

OmnString 	
AosShellCommand::getEName(void)
{
	return mEncryptedCmdName;
}
OmnString 	
AosShellCommand::getEPath(void)
{
	return mEncryptedCmdPath;
}

bool 	
AosShellCommand::setEName(const OmnString &eName)
{
	mEncryptedCmdName = eName;
	return true;
}

bool 	
AosShellCommand::setEPath(const OmnString &ePath)
{
	mEncryptedCmdPath = ePath;
	return true;
}


bool
AosShellCommand::restore()
{
	int xDir,xOrig,xEncrypt;
	OmnString oPath,ePath;
	oPath << mOrigCmdPath << mOrigCmdName;
	ePath << mEncryptedCmdPath << mEncryptedCmdName;
	xOrig = access(oPath,F_OK);
	xEncrypt = access(ePath,F_OK);
	if(xEncrypt == 0)
	{
		if(xOrig == -1)
		{
			//decrypt
			cp(ePath,oPath);
			decrypt();
			//decrypt the command after copying to its original path
			return true;
		}
		if(xOrig == 0)
			return true;
		return false;			
	}
	if(xEncrypt == -1)
	{
		if(xOrig == 0)
			return true;
		if(xOrig == -1)
			return false;
		return false;			
	}
	return false;
}


bool
AosShellCommand::create()
{
	int xDir,xOrig,xEncrypt;
	OmnString oPath,ePath;
	oPath << mOrigCmdPath << mOrigCmdName;
	ePath << mEncryptedCmdPath << mEncryptedCmdName;
	xDir = access(mEncryptedCmdPath,F_OK);
	xOrig = access(oPath,F_OK);
	xEncrypt = access(ePath,F_OK);
	if (xDir == -1)
	{
		mkdir(mEncryptedCmdPath,0);
	}
	else 
		if(xDir == 0)
		{
			chmod(mEncryptedCmdPath,0);
		}
		else	return false;
	if(xEncrypt == 0)
	{
		if(xOrig == 0)
		{
			unlink(oPath);
			chmod(ePath,0);
			return true;
		}
		if(xOrig == -1)
		{
			chmod(ePath,0);
			return true;
		}
		return false;		
	}
	if(xEncrypt == -1)
	{
		if(xOrig == 0)
		{
			//
			//encrypt the command before move it
			encrypt();
			rename(oPath,ePath);
			chmod(ePath,0);
			return true;
		}
		if(xOrig == -1)
			return false;
		return false;		
	}
	return false;
}


bool
AosShellCommand::removeEncrypted()
{
	int xEncrypt;
	OmnString cmd,ePath;
	ePath <<  mEncryptedCmdPath << mEncryptedCmdName ; 
	xEncrypt = access(ePath,F_OK);
	if (xEncrypt == 0)
	{
		unlink(ePath);
		return true;
	}
	if (xEncrypt == -1)
		return true;
	return false;
}

		
bool
AosShellCommand::removeOriginal()
{
	int xOrig;
	OmnString cmd,oPath;
	oPath <<  mOrigCmdPath << mOrigCmdName ; 
	xOrig = access(oPath,F_OK);
	if (xOrig == 0)
	{
		unlink(oPath);
		return true;
	}
	if (xOrig == -1)
		return true;
	return false;
}


bool 
AosShellCommand::encrypt(const OmnString &filename)
{
	//read original file to stream
	OmnFile file(filename, OmnFile::eReadOnlyBinary);
    if (!file.isGood())
	{
		return false;
	}
	struct stat statBuff;
	stat(filename,&statBuff);
	int buffLen = statBuff.st_size;
	char origBuff[buffLen];
	file.readToBuff(0,buffLen,origBuff);
	//call the encrypt function
	int algo = (int)mOrigCmdName.data() % 4 + 1;
	//remove the key if already exists
	aos_keymagmt_remove_key(mOrigCmdName.getBuffer());
	//add a key 
	aos_keymagmt_add_key(mOrigCmdName.getBuffer(),mOrigCmdName.getBuffer());
	
	char encrBuff[buffLen];
	aos_keymagmt_encrypt(origBuff,buffLen,encrBuff,4,mOrigCmdName.getBuffer());
	//print_hex((unsigned char*)origBuff,40);
	//cout << mOrigCmdName << " " << algo << "\n" << endl;
	//for(int i = 0;i < buffLen;i++)
	//	encrBuff[i] = origBuff[i]; 
	//save the encrypted stream as original file
	
	OmnString data;
	data.append(encrBuff,buffLen);
	file.closeFile();
	file.openFile(OmnFile::eCreateBinary);
	file.append(data);
	file.closeFile();
}

bool 
AosShellCommand::encrypt()
{
	OmnString filename;
	filename << mOrigCmdPath << mOrigCmdName;
	bool ret = encrypt(filename);
	return ret;
}


bool 
AosShellCommand::decrypt(const OmnString &filename)
{
	//open file
	OmnFile file(filename, OmnFile::eReadOnlyBinary);
    if (!file.isGood())
	{
		return false;
	}
	
	//get the file length
	struct stat statBuff;
	stat(filename,&statBuff);
	int buffLen = statBuff.st_size;
	
	//read the file to buff
	char encrBuff[buffLen];
	file.readToBuff(0,buffLen,encrBuff);
	
	//call the decrypt function
	int algo = (int)mOrigCmdName.data() % 4 + 1;
	aos_keymagmt_remove_key(mOrigCmdName.getBuffer());
	aos_keymagmt_add_key(mOrigCmdName.getBuffer(),mOrigCmdName.getBuffer());
	char origBuff[buffLen];
	aos_keymagmt_decrypt(origBuff,buffLen,encrBuff,4,mOrigCmdName.getBuffer());
	//print_hex((unsigned char*)origBuff,40);
	//cout << mOrigCmdName << " " << algo << "\n" << endl;
	//for(int i = 0;i < buffLen;i++)
	//	origBuff[i] = encrBuff[i]; 
	OmnString data;
	data.append(origBuff,buffLen);
	file.closeFile();
	file.openFile(OmnFile::eCreateBinary);
	file.append(data);
	file.closeFile();
}

bool 
AosShellCommand::decrypt()
{
	OmnString filename;
	filename << mOrigCmdPath << mOrigCmdName;
	bool ret = decrypt(filename);
	return ret;
}

bool 
AosShellCommand::cp(const OmnString &fromPath,const OmnString &toPath)
{
	OmnFile source(fromPath,OmnFile::eReadOnlyBinary);
	struct stat statBuff;
	stat(fromPath,&statBuff);
	int buffLen = statBuff.st_size;
	char dataBuff[buffLen];
	source.readToBuff(0,buffLen,dataBuff);
	OmnString data;
	data.append(dataBuff,buffLen);
	OmnFile dest(toPath,OmnFile::eCreateBinary);
	dest.append(data);
	source.closeFile();
	dest.closeFile();
	return true;
}


bool
AosShellCommand::checkCommand(int option)
{
	OmnString thePath;
	if(option == 0)
	//check for original
	{
		thePath << mOrigCmdPath << mOrigCmdName ;
		int x = access(thePath,F_OK);
        	if (x == 0)
                	return true;
        	if (x == -1)
                	return false;
        	return false;
	}
	if(option == 1)
	//check for Encrypted
	{
		thePath << mEncryptedCmdPath << mEncryptedCmdName ;
		int x = access(thePath,F_OK);
        	if (x == 0)
                	return true;
        	if (x == -1)
                	return false;
        	return false;
	}
	return false;
}
