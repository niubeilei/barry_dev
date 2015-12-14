////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliDef.cpp
// Description:
//	A CLI is a series of words:
//		<word1> <word2> ... <wordn>
//
//  Each word has a helper, which explains the meaning of the word.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelInterface/CliDef.h"


bool
OmnCliDefMgr::help(const OmnString &cmd, OmnString &modifiedCmd)
{
	// 
	// The caller got a command 'cmd' and wants to complete the last word
	// in the command. If the words before the last word uniquely identifies
	// a command, it will expand the last word. The new command is stored
	// in modifiedCmd. True is returned. 
	//
	// Otherwise, false is returned.
	//
/*	OmnString str = cmd.trimLastWord();
	OmnCliDef *cmdFound = getCmd(str);
	if (!cmdFound)
	{
		// 
		// Either the string does not identify any command or it identifies
		// multiple commands. Return false.
		//
		return false;
	}

	// 
	// 'cmd' does uniquely identify a command. Use the command to expand the
	// command.
	//
	int numWords = str.numWords();
	if (numWords >= cmdFound->mNumWords)
	{
		// 
		// Can't expand it. It is an error.
		//
		return false;
	}

	cmdModified.set(' ', str.length());
	cmdModified << theCmd->mHelper[numWords]; */
	return true;
}

