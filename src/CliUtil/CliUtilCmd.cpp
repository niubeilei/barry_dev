////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliCmd.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "CliUtil/CliUtilCmd.h"

#include "Alarm/Alarm.h"
#include "CliClient/ModuleCliServer.h"
#include "KernelInterface/ReturnCode.h"
#include "KernelInterface/Cli.h"
#include "Debug/Debug.h"
#include "KernelAPI/KernelAPI.h"
#include "CliUtil/Ptrs.h"
#include "Util/StrParser.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "XmlParser/XmlItem.h"

#include "aos/aosReturnCode.h"

bool OmnCliUtilCmd::mLoadConfig = false;
static OmnString sgAosCertDir = "/usr/local/AOS/Data/Certificates/";

OmnCliUtilCmd::~OmnCliUtilCmd()
{
}

OmnCliUtilCmd::OmnCliUtilCmd()
{
	mFunc = NULL;
}

AosCliLvl::CliLevel		
OmnCliUtilCmd::getLevel()const
{
	return mLevel;
}

OmnCliUtilCmd::OmnCliUtilCmd(const OmnString &def)
{
	OmnXmlItem theDef(def);

	mPrefix = theDef.getStr("Prefix", "");
	mUsage = theDef.getStr("Usage", "No usage");
	mBufsize = theDef.getInt("bufsize", -1);
	mOprId = theDef.getStr("OprID", "");
	mModId = theDef.getStr("ModId", "");

	// get level
	OmnString strLevel = theDef.getStr("Level", "");
	if(strLevel == "engineering")
	{
		mLevel = AosCliLvl::eEngineering;
	}
	else if(strLevel  == "config")
	{
		mLevel = AosCliLvl::eConfig;
	}
	else if(strLevel  == "enable")
	{
		mLevel = AosCliLvl::eEnable;
	}
	else if(strLevel  == "user")
	{
		mLevel = AosCliLvl::eUser;
	}
	else
	{
		// default value is eConfig
		mLevel = AosCliLvl::eConfig;
	}

	OmnXmlItemPtr parms = theDef.tryItem("Parms");
	if (!parms)
	{
		OmnAlarm << "Missing parms: " << def << enderr;
		return;
	}

	parms->reset();
	while (parms->hasMore())
	{
		OmnCliParmPtr theParm = OmnNew OmnCliParm();
		OmnXmlItemPtr parm = parms->next();
		theParm->setHelp(parm->getStr("help", "No help"));
		theParm->setOptional(parm->getBool("optional", false));
		theParm->setMin(parm->getInt("min", 0));
		theParm->setMax(parm->getInt("max", -1));
		theParm->setSaveFlag(parm->getBool("saveflag", false));
		OmnString filename = parm->getStr("filename", "");
		theParm->setFileExt(parm->getStr("file_ext", ""));
		if (filename.length() > 0)
		{
			if (filename == "__prev__")
			{
				theParm->filenameFromPrevParm();
			}
			else
			{
				theParm->setFilename(filename);
			}
		}

		// 
		// Should be in the form: 
		// type=xxx: value:0, value:1, value, ...
		//
		OmnString type = parm->getStr("type", "");
		if (type == "string")
		{
			theParm->setType(OmnCliParm::eString);
		}
		else if (type == "int")
		{
			theParm->setType(OmnCliParm::eInt);
		}
		else if (type == "addr")
		{
			theParm->setType(OmnCliParm::eAddr);
		}
		else if (type == "port")
		{
			theParm->setType(OmnCliParm::ePort);
		}
		else if (type == "interactive")
		{
			theParm->setType(OmnCliParm::eInteractive);
		}
		else if (type == "file")
		{
			theParm->setType(OmnCliParm::eFile);
		}
		else if (type == "macaddr")
		{
			theParm->setType(OmnCliParm::eMacAddr);
		}
		else if (type == "portrange")
		{
			theParm->setType(OmnCliParm::ePortRange);
		}
		else
		{
			OmnAlarm << "Value type incorrect: " 
				<< type << " in: " 
				<< parm->toString() << ". " << def << enderr;
			return;
		}

		OmnStrParser sp(parm->getStr("values", ""));
		bool stop = false;
		while (!stop)
		{
			OmnString v = sp.nextEngWord();
			if (v.length() <= 0)
			{
				break;
			}

			char c = sp.nextChar();
			switch (c)
			{
			case ':':
				 //
				 // value:1, or value:0,
				 //
				 theParm->addValue(v, sp.nextBool());
				 sp.nextChar();
				 break;

			case ',':
				 theParm->addValue(v, false);
				 break;

			case 0:
				 // 
				 // This is the last one.
				 //
				 theParm->addValue(v, false);
				 stop = true;
				 break;

			default: 
				 OmnAlarm << "Incorrect parm definition: " 
					<< parm->toString() << enderr;
				 return;
			}
		}

		mParms.append(theParm);
	}
		
	return;
}

bool
OmnCliUtilCmd::checkDef(OmnString &rslt)
{
	if (mBufsize < 0)
	{
		rslt = "Bufsize is negative: ";
		rslt << mBufsize;
		return false;
	}

	mParms.reset();
	while (mParms.hasMore())
	{
		OmnCliParmPtr parm = mParms.next();
		if (!parm->checkDef(rslt))
		{
			return false;
		}
	}

	return true;
}


bool 
OmnCliUtilCmd::run(const OmnString &cmd,const int parmPos, OmnString &rslt)
{
	rslt = "";
	if(cmd.length() < parmPos)
	{
		rslt = "Unknown CLI module name: ";
		rslt << mModId;
		return false;
	}

	OmnString parms(cmd.data()+parmPos,cmd.length()-parmPos);
	return runUserLand(parms, rslt);	
}

bool
OmnCliUtilCmd::runUserLand(const OmnString &parms, OmnString &rslt)
{
	OmnStrParser parser(parms);
	int intIndex = 0;
	int strIndex = 0;
	int64_t intv; 
	OmnString strv;
	int intValues[eMaxIntArgs];
	OmnString strValues[eMaxStrArgs];
	bool stop;

	mParms.reset();
	OmnCliParmPtr parm;
	while (1)
	{
		if (!parm)
		{
			if (!mParms.hasMore())
			{
				break;
			}
			parm = mParms.next();
		}

		if (parm->needToSave() && parm->nameFromPrevParm())
		{
			OmnString fn = sgAosCertDir;
			fn << strv;

			if ((parm->getFileExt()).length() > 0)
			{
				fn << "." << parm->getFileExt();
			}

			parm->setFilename(fn);
		}

		if (!parm->parseArg(parser, intv, strv, stop, rslt))
		{
			appendIntToStr(rslt, -eAosRc_CliCmdParmError);
			return false;
		}

		if (stop)
		{
			break;
		}

		if (parm->getType() == OmnCliParm::eFile)
		{
			if (strIndex >= eMaxStrArgs - 1)
			{
				rslt = "Too many string arguments";

				appendIntToStr(rslt, -eAosRc_CliCmdParmError);
				return false;
			}

			strValues[strIndex++] = parm->getFileName();
			strValues[strIndex++] = strv;
		}
		else if (parm->isInteger())
		{
			if (intIndex >= eMaxIntArgs)
			{
				rslt = "Too many arguments";

				appendIntToStr(rslt, -eAosRc_CliCmdParmError);
				return false;
			}

			intValues[intIndex++] = intv;
		}
		else
		{
			if (strIndex >= eMaxStrArgs)
			{
				rslt = "Too many string arguments";

				appendIntToStr(rslt, -eAosRc_CliCmdParmError);
				return false;
			}

			strValues[strIndex++] = strv;
		}

		if (parm->finished())
		{
			parm = 0;
		}
	}
	
	// 
	// Chen Ding, 10/13/2005, Bug #20
	// If it did not consume all of the arguments, it is an error.
	// The user has entered too many. 
	//
	if (parser.hasMore())
	{
		rslt = "Too many parameters";

		appendIntToStr(rslt, -eAosRc_CliCmdParmError);
		return false;
	}

	// 
	// Send the data to kernel.
	//
	
	// 
	// Calculate the buffer total length
	//
	uint totalLen = (intIndex + 2) * sizeof(int) + mBufsize;
	int i;
	for (i=0; i<strIndex; i++)
	{
		totalLen += strValues[i].length() + 1;
	}

	if (totalLen >= eMaxDataSize)
	{
		rslt = "Data too big to pass to kernel: ";
		rslt << totalLen;

		appendIntToStr(rslt, -eAosRc_NameTooLong);
		return false;
	}

	// 
	// oprId
	// number of integers (integer)
	// number of strings (integer)
	// integer parm
	// integer parm
	// ...
	// string parm
	// string parm
	// ...
	// 
	totalLen += mOprId.length();
	char *data = OmnNew char[totalLen+10];
	strcpy(data, mOprId.data());
	data[mOprId.length()] = 0;

	int *iv = (int*)&data[mOprId.length()+1];
	iv[0] = intIndex;
	iv[1] = strIndex;
	for (i=2; i<=intIndex+1; i++)
	{
		iv[i] = intValues[i-2];
	}

	char *svhead = &data[mOprId.length() + 1 + (intIndex + 2) * sizeof(int)];
	for (i=0; i<strIndex; i++)
	{
		strcpy(svhead, strValues[i].data());
		svhead += strValues[i].length() + 1;
	}

	// 
	// Now, we have put all data into 'data'. It is the time to 
	// send it.
	//
    int ret = sendToUserLand(data, totalLen+1);

	rslt = "";
  //  if (ret)
   // {
    //    rslt = OmnKernelApi::getErr(ret) << "\n";
    //}
	
	unsigned int length = ((int *)data)[0];
	if (length >= totalLen)
	{
		rslt << ". Returned data too long: " << length
			<< ". Max: " << totalLen << ". Truncated!";
		length = totalLen;
	}

	data[4 + length] = 0;
	rslt << &data[4];

	appendIntToStr(rslt, ret);
	return !ret;
}

bool
OmnCliUtilCmd::appendIntToStr(OmnString &rslt, int ret)
{
	int len = rslt.length();
	char *r;
	
	rslt.setSize(len+sizeof(int)+2);
	rslt.setLength(len+sizeof(int)+1);
	r = rslt.getBuffer() + len + 1;
	memcpy(r, &ret, sizeof(int));

	rslt.setChar(0, len);
	return true;
}

#ifdef AOS_OLD_CLI
bool
OmnCliUtilCmd::saveConfig(const OmnString &parms, OmnString &rslt)
{
	OmnStrParser parser(parms);
	int intv;
	bool stop;

	mParms.reset();
	OmnString fn;
	while (mParms.hasMore())
	{
		OmnCliParmPtr parm = mParms.next();
		if (!parm->parseArg(parser, intv, fn, stop, rslt))
		{
			return false;
		}

		if (stop)
		{
			// No name provided. User the default config name.
			break;
		}

		if (parm->isInteger())
		{
			rslt = "Command Incorrect";
			return false;
		}

		break;
	}
							
	AosCli cli(fn);
	int ret = cli.saveConfig(rslt);
	return ret == 0;	
}

bool
OmnCliUtilCmd::loadConfig(const OmnString &parms, OmnString &rslt)
{
	OmnStrParser parser(parms);
	int intv;
	bool stop;

	mParms.reset();
	OmnString fn;
	while (mParms.hasMore())
	{
		OmnCliParmPtr parm = mParms.next();
		if (!parm->parseArg(parser, intv, fn, stop, rslt))
		{
			return false;
		}

		if (stop)
		{
			// No name provided. User the default config name.
			break;
		}

		if (parm->isInteger())
		{
			rslt = "Command Incorrect";
			return false;
		}

		break;
	}
							
	AosCli cli(fn);
	int ret = cli.loadConfig(rslt);
	return ret == 0;	
}
#endif

// 
// Chen Ding, 05/01/2006
//
bool
OmnCliUtilCmd::retrieveInMemLog(const OmnString &parms, OmnString &rslt)
{
	if (!runUserLand(parms, rslt)) return false;

	mParms.reset();
	OmnCliParmPtr parm = mParms.next();
	OmnString fn = parm->getFileName();

	OmnFile file(fn, OmnFile::eCreate);
	if (!file.isGood())
	{
		rslt = "Failed to create the file: ";
		rslt << fn;
		return false;
	}

	while (rslt.length() <= 0)
	{
		if (!file.append(rslt))
		{
			rslt = "Failed to add contents to the file: ";
			rslt << fn;
			return false;
		}

		if (!runUserLand(parms, rslt)) return false;
	}

	return true;
}

int
OmnCliUtilCmd::sendToUserLand(char *data, unsigned int size)
{
	int ret;
	struct aosUserLandApiParms parms;
	char errmsg[eMaxErrLen];
	
	if (mFunc == NULL)
	{
		strcpy(errmsg, "Not found registered function"); 
		OmnAlarm << errmsg << enderr;
		return -1;
	}

	memset(&parms, 0, sizeof(struct aosUserLandApiParms));
	ret = aosUserLandApi_convert(data, size, &parms);
	if (!ret)
	{
		strcpy(errmsg, "Failed to parse parameters");
		OmnAlarm << errmsg << enderr;
		return -1;
	}

	ret = mFunc(data, &size, &parms, errmsg, eMaxErrLen);
	if (ret != 0) 
	{
		*((int*)data) = 0;
		if (errmsg[0] != 0)
		{
			*((int*)data) = strlen(errmsg);
			strcpy((char*)(&((int*)data)[1]), errmsg);
		}
	} else
		*((int*)data) = size;
		
	return ret;
}

bool
OmnCliUtilCmd::aosUserLandApi_convert(char *data,
						 unsigned int datalen,
						 struct aosUserLandApiParms *parms)
{
	//
	// oprId
	// number of integers (integer)
	// number of strings (integer)
	// integer parm
	// integer parm
	// ...
	// string parm
	// string parm
	// ...
	//
	// Index counts from 1.
	//
	unsigned int i, ii, index;
	char *dd;
	int len = strlen(data);
	int *iv = (int *)&data[len+1];

	if ((ii = len + 1 + (iv[0] + 2) * 4) >datalen)
	{
		return false;
	}
	//
	// Retrieve the integers
	//
	parms->mNumIntegers = iv[0];
	parms->mNumStrings = iv[1];
	if (parms->mNumIntegers < 0 ||
		parms->mNumIntegers > eMaxIntArgs)
	{
		return false;
	}

	for (index = 0; index < parms->mNumIntegers; index++)
	{
		parms->mIntegers[index] = iv[index+2];
	}

	//
	// Retrieve the strings
	//
	dd = &data[ii];
	index = 0;
	for (i=0; i<parms->mNumStrings; i++)
	{
		parms->mStrings[index++] = &data[ii];

		ii += strlen(dd) + 1;
		if (ii > datalen)
		{
			return false;
		}
		dd = &data[ii];
	}

	return true;
}
