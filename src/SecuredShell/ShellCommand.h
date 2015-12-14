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

#ifndef Omn_SecuredShell_ShellCommand_h
#define Omn_SecuredShell_ShellCommand_h

#include "Util/String.h"
#include "Util/RCObject.h"
#include "keyman.h"



class AosShellCommand : public OmnRCObject
{
	OmnDefineRCObject;

	OmnString 	mOrigCmdPath;
	OmnString 	mOrigCmdName;
	OmnString 	mEncryptedCmdName;
	OmnString 	mEncryptedCmdPath;
	bool		mEncrypted;		// True if the encrypted version of the command is created
	bool		mRemoved;		// True if the original command was removed from the system
	bool 		mRestored;
	bool		mCreated;

public:
	AosShellCommand(const OmnString &cmdPath, 
			const OmnString &origName, 
			const OmnString &encryptedName,
			const OmnString &encryptedPath);
	~AosShellCommand();
	//void 				SetEncd(bool bl);
	//void 				SetRmvd(bool bl);
	OmnString 	getOrigName(void);
	OmnString 	getOrigPath(void);
	OmnString 	getEName(void);
	OmnString 	getEPath(void);
	bool		setEName(const OmnString &eName);
	bool		setEPath(const OmnString &ePath);
	bool 		restore();
	bool		create();
	bool		removeOriginal();
	bool		removeEncrypted();
	bool 		encrypt(const OmnString &filename);
	bool 		decrypt(const OmnString &filename);
	bool		encrypt();
	bool		decrypt();
	bool		checkCommand(int option);
	bool 		cp(const OmnString &fromPath,const OmnString &toPath);
};

#endif

