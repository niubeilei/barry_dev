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

#include "KernelInterface/CliCmd.h"

#include "Alarm/Alarm.h"
#include "alarm/Alarm.h"
#include "CliClient/ModuleCliServer.h"
#include "KernelInterface/ReturnCode.h"

#ifdef AOS_ENABLE_SYSTEM_EXCUTE_CLI
#include "KernelInterface/CliSysCmd.h"
#endif

#include "KernelInterface/Cli.h"
#include "Debug/Debug.h"
#include "KernelAPI/KernelAPI.h"
#include "KernelInterface/Ptrs.h"
#include "Util/StrParser.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/MacAddr.h"
#include "XmlParser/XmlItem.h"

#include "aos/aosReturnCode.h"

bool OmnCliCmd::mLoadConfig = false;
OmnString OmnCliParm::mBuff;
int		  OmnCliParm::mBuffCrt = 0;
bool	  OmnCliParm::mReadFromBuff = false;

static OmnString sgAosCertDir = "/usr/local/AOS/Data/Certificates/";

OmnCliCmd::~OmnCliCmd()
{
}

bool	
OmnCliParm::parseArg(OmnStrParser& arg, 
					 int64_t &intv, 
					 OmnString &strv, 
					 bool &stop, 
					 OmnString &rslt)
{
	ValueDef *v;
	stop = false;
	u32 tmp;
	bool ret = true;
	OmnString filename;

	if (arg.remainingAllSpaces() || !arg.hasMore())
	{
		if (mOptional)
		{
			if (mOccurances < mMin)
			{
				rslt = mHelp;
				rslt << " should appear at least: " << mMin 
					<< " times!";
				return false;
			}

			stop = true;
			return true;
		}
	}

	switch (mType)
	{
	case eAddr:
		 if (!arg.nextAddr(tmp, 0))
		 {
			 rslt = "Missing/Incorrect parameter: ";
			 rslt << mHelp;
			 ret = false;
			 break;
		 }
		 
		 intv = (int)tmp;
		 v = mValues;
		 while (v && v->mIntValue != intv) v = v->mNext;

		 if (v && v->mStop)
		 {
			 mFinished = true;
		 }
		 ret = true;
		 break;

	case eMacAddr:
		 if ((strv = arg.nextWord("", " ")).length() <= 0)
		 {
			 rslt = "Missing parameter: ";
			 rslt << mHelp;
			 ret = false;
			 break;
		 }

		 if (!OmnMacAddr::isValidAddr(strv))
		 {
			 rslt = "Error format of parameter: ";
			 rslt << mHelp;
			 ret = false;
			 break;
		 }

		 v = mValues;
		 if (v)
		 {
			// 
			// The value must match one of the values defined
			//
		 	while (v && v->mStrValue != strv) v = v->mNext;

			if (!v)
			{
				// 
				// The value does not match. It is an invalid value.
				//
				rslt = "Value incorrect. ";
				rslt << mHelp;
				ret = false;
				break;
			}
		 }

		 if (v && v->mStop)
		 {
			 mFinished = true;
		 }

		 ret = true;
		 break;

	case ePort:
		 if (!arg.nextIntEx(intv))
		 {
			 rslt = "Missing: ";
			 rslt << mHelp;
			 ret = false;
			 break;
		 }

		 if (intv < 0)
		 {
			 rslt = "Port cannot be negative: ";
			 rslt << mHelp;
			 ret = false;
			 break;
		 }

		 if (intv > 65535)
		 {
			 rslt = "Port is too big (maximum: 65535): ";
			 rslt << mHelp;
			 ret = false;
			 break;
		 }

		 v = mValues;
		 while (v && v->mIntValue != intv) v = v->mNext;

		 if (v && v->mStop)
		 {
			 mFinished = true;
		 }

		 ret = true;
		 break;

	case ePortRange:
		 if ((strv = arg.nextWord("", " ")).length() <= 0)
		 {
			 rslt = "Missing parameter: ";
			 rslt << mHelp;
			 ret = false;
			 break;
		 }

		 int len, i;
		 bool colon;
		 char* tmp;
		 
		 colon = false;
		 len = strv.length();
		 for (tmp = strv.getBuffer(), i = 0; i < len; i++)
		 {
		 	if ((tmp[i] >= '0') && (tmp[i] <= '9'))
				continue;

			if ((tmp[i] == ':') && !colon && i != len-1)
			{
				colon = true;
				continue;
			}

			rslt = "Error format parameter: ";
			rslt << mHelp;
			ret = false;
			break;
		 }
		 
		 if (!ret)
		 	break;

		 if (colon)
		 {
			int p1, p2;
			sscanf(strv.data(), "%d:%d", &p1, &p2);
			if (p1 >= p2 || p1 > 65535 || p1 < 0 ||
				p2 > 65535 || p2 < 0)
				ret = false;
		 }
		 else
		 {
			int p;
			sscanf(strv.data(), "%d", &p);
			if (p < 0 || p > 65535)
				ret = false;
		 }
		
		 if (!ret)
		 {
			rslt = "Error format parameter: ";
			rslt << mHelp;
			break;
		 }

		 v = mValues;
		 if (v)
		 {
			// 
			// The value must match one of the values defined
			//
		 	while (v && v->mStrValue != strv) v = v->mNext;

			if (!v)
			{
				// 
				// The value does not match. It is an invalid value.
				//
				rslt = "Value incorrect. ";
				rslt << mHelp;
				ret = false;
				break;
			}
		 }

		 if (v && v->mStop)
		 {
			 mFinished = true;
		 }

		 ret = true;
		 break;
	
	case eInt:
		 if (!arg.nextIntEx(intv))
		 {
			 rslt = "Missing/Incorrect parameter: ";
			 rslt << mHelp;
			 ret = false;
			 break;
		 }

		 v = mValues;
		 while (v && v->mIntValue != intv) v = v->mNext;

		 if (v && v->mStop)
		 {
			 mFinished = true;
		 }

		 ret = true;
		 break;

	case eString:
		 if ((strv = arg.nextWordEx("", " ")).length() <= 0)
		 {
			 rslt = "Missing parameter: ";
			 rslt << mHelp;
			 ret = false;
			 break;
		 }

		 v = mValues;
		 if (v)
		 {
			// 
			// The value must match one of the values defined
			//
		 	while (v && v->mStrValue != strv) v = v->mNext;

			if (!v)
			{
				// 
				// The value does not match. It is an invalid value.
				//
				rslt = "Value incorrect. ";
				rslt << mHelp;
				ret = false;
				break;
			}
		 }

		 if (v && v->mStop)
		 {
			 mFinished = true;
		 }
		
		 if (strv.length() < mMin)
		 {
		 	rslt = "Too short string, mininum length: ";
			rslt << mMin;
			ret = false;
			break;
		 }

		 if (mMax > 0 && strv.length() > mMax)
		 {
		 	rslt = "Too long string, maximum length: ";
			rslt << mMax;
			ret = false;
			break;
		 }

		 ret = true;
		 break;

	case eInteractive:
		 // 
		 // The contents of this parameter is collected interactively
		 // 
		 if (OmnCliCmd::isLoadConfig())
		 {
		 	if (!(ret = arg.nextBlock(strv)))
			{
			 	rslt = "Failed to retrieve the value: ";
			 	rslt << mHelp;
			 	break;
			}
		 }
		 else
		 {
		 	ret = getInteractive(strv);
		 }
		 break;

	case eFile:
		 // 
		 // The contents of this paramter is stored in a file.
		 //
		 if ((filename = arg.nextWord("", " ")).length() <= 0)
		 {
			 rslt = "Missing parameter: ";
			 rslt << mHelp;
			 ret = false;
			 break;
		 }

		 mFileName = filename;
		 ret = getFromFile(filename, strv);
		 break;

	default:
		 rslt = "Incorrect parameter type: ";
		 rslt << mType;
		 ret = false;
		 break;
	}

	mOccurances++;
	return ret;
}

/*
bool
OmnCliParm::parseArgEx(OmnStrParser& arg, 
					 int &intv, 
					 OmnString &strv, 
					 bool &stop, 
					 OmnString &rslt, 
					 int& errno)
{
	ValueDef *v;
	stop = false;
	uint tmp;
	bool ret = true;
	OmnString filename;
	errno = 0;

	if (arg.remainingAllSpaces() || !arg.hasMore())
	{
		if (mOptional)
		{
			if (mOccurances < mMin)
			{
				rslt = mHelp;
				rslt << " should appear at least: " << mMin 
					<< " times!";
				return false;
			}

			stop = true;
			return true;
		}
	}

	switch (mType)
	{
	case eAddr:
		 if (!arg.nextAddr(tmp, 0))
		 {
			 rslt = "Missing/Incorrect parameter: ";
			 rslt << mHelp;
			 ret = false;
			 break;
		 }
		 
		 intv = (int)tmp;
		 v = mValues;
		 while (v && v->mIntValue != intv) v = v->mNext;

		 if (v && v->mStop)
		 {
			 mFinished = true;
		 }
		 ret = true;
		 break;

	case eMacAddr:
		 if ((strv = arg.nextWord("", " ")).length() <= 0)
		 {
			 rslt = "Missing parameter: ";
			 rslt << mHelp;
			 ret = false;
			 break;
		 }

		 if (!OmnMacAddr::isValidAddr(strv))
		 {
			 rslt = "Error format of parameter: ";
			 rslt << mHelp;
			 ret = false;
			 break;
		 }

		 v = mValues;
		 if (v)
		 {
			// 
			// The value must match one of the values defined
			//
		 	while (v && v->mStrValue != strv) v = v->mNext;

			if (!v)
			{
				// 
				// The value does not match. It is an invalid value.
				//
				rslt = "Value incorrect. ";
				rslt << mHelp;
				ret = false;
				break;
			}
		 }

		 if (v && v->mStop)
		 {
			 mFinished = true;
		 }

		 ret = true;
		 break;

	case ePort:
		 if (!arg.nextIntEx(intv))
		 {
			 rslt = "Missing: ";
			 rslt << mHelp;
			 ret = false;
			 break;
		 }

		 if (intv < 0)
		 {
			 rslt = "Port cannot be negative: ";
			 rslt << mHelp;
			 ret = false;
			 break;
		 }

		 if (intv > 65535)
		 {
			 rslt = "Port is too big (maximum: 65535): ";
			 rslt << mHelp;
			 ret = false;
			 break;
		 }

		 v = mValues;
		 while (v && v->mIntValue != intv) v = v->mNext;

		 if (v && v->mStop)
		 {
			 mFinished = true;
		 }

		 ret = true;
		 break;

	case ePortRange:
		 if ((strv = arg.nextWord("", " ")).length() <= 0)
		 {
			 rslt = "Missing parameter: ";
			 rslt << mHelp;
			 ret = false;
			 break;
		 }

		 int len, i;
		 bool colon;
		 char* tmp;
		 
		 colon = false;
		 len = strv.length();
		 for (tmp = strv.getBuffer(), i = 0; i < len; i++)
		 {
		 	if ((tmp[i] >= '0') && (tmp[i] <= '9'))
				continue;

			if ((tmp[i] == ':') && !colon && i != len-1)
			{
				colon = true;
				continue;
			}

			rslt = "Error format parameter: ";
			rslt << mHelp;
			ret = false;
			break;
		 }
		 
		 if (!ret)
		 	break;

		 if (colon)
		 {
			int p1, p2;
			sscanf(strv.data(), "%d:%d", &p1, &p2);
			if (p1 >= p2 || p1 > 65535 || p1 < 0 ||
				p2 > 65535 || p2 < 0)
				ret = false;
		 }
		 else
		 {
			int p;
			sscanf(strv.data(), "%d", &p);
			if (p < 0 || p > 65535)
				ret = false;
		 }
		
		 if (!ret)
		 {
			rslt = "Error format parameter: ";
			rslt << mHelp;
			break;
		 }

		 v = mValues;
		 if (v)
		 {
			// 
			// The value must match one of the values defined
			//
		 	while (v && v->mStrValue != strv) v = v->mNext;

			if (!v)
			{
				// 
				// The value does not match. It is an invalid value.
				//
				rslt = "Value incorrect. ";
				rslt << mHelp;
				ret = false;
				break;
			}
		 }

		 if (v && v->mStop)
		 {
			 mFinished = true;
		 }

		 ret = true;
		 break;
	
	case eInt:
		 if (!arg.nextIntEx(intv))
		 {
			 rslt = "Missing/Incorrect parameter: ";
			 rslt << mHelp;
			 ret = false;
			 break;
		 }

		 v = mValues;
		 while (v && v->mIntValue != intv) v = v->mNext;

		 if (v && v->mStop)
		 {
			 mFinished = true;
		 }

		 ret = true;
		 break;

	case eString:
		 if ((strv = arg.nextWordEx("", " ")).length() <= 0)
		 {
			 rslt = "Missing parameter: ";
			 rslt << mHelp;
			 ret = false;
			 break;
		 }

		 v = mValues;
		 if (v)
		 {
			// 
			// The value must match one of the values defined
			//
		 	while (v && v->mStrValue != strv) v = v->mNext;

			if (!v)
			{
				// 
				// The value does not match. It is an invalid value.
				//
				rslt = "Value incorrect. ";
				rslt << mHelp;
				ret = false;
				break;
			}
		 }

		 if (v && v->mStop)
		 {
			 mFinished = true;
		 }

		 ret = true;
		 break;

	case eInteractive:
		 // 
		 // The contents of this parameter is collected interactively
		 // 
		 if (OmnCliCmd::isLoadConfig())
		 {
		 	if (!(ret = arg.nextBlock(strv)))
			{
			 	rslt = "Failed to retrieve the value: ";
			 	rslt << mHelp;
			 	break;
			}
		 }
		 else
		 {
		 	ret = getInteractive(strv);
		 }
		 break;

	case eFile:
		 // 
		 // The contents of this paramter is stored in a file.
		 //
		 if ((filename = arg.nextWord("", " ")).length() <= 0)
		 {
			 rslt = "Missing parameter: ";
			 rslt << mHelp;
			 ret = false;
			 break;
		 }

		 mFileName = filename;
		 ret = getFromFile(filename, strv);
		 break;

	default:
		 rslt = "Incorrect parameter type: ";
		 rslt << mType;
		 ret = false;
		 break;
	}

	mOccurances++;
	return ret;
}
*/

bool
OmnCliParm::getInteractive(OmnString &strv)
{
	// 
	// This function gets the parameter value interactively.
	// The value is entered in form of multiple lines. Each line
	// should not exceed eMaxLineLength. The input is ended with
	// a line "...".
	//
    // If 'save' flag is set, this function also saves the contents
    // into a file. The file name is either 'mFilename' or from
    // its previous parameter.
    //

	char local[eMaxLineLength+1];
	cout << mHelp << endl;
	
	strv = "";
	while (1)
	{
		int index = 0;
		char c;
		ssize_t ret;
		while (index < eMaxLineLength)
		{
			// char c = getchar();
			if (mReadFromBuff)
			{
				if (mBuffCrt >= mBuff.length())
				{
					ret = 0;
				}
				else
				{
					ret = 1;
					c = (mBuff.data())[mBuffCrt++];
				}
			}
			else
			{
				ret = read(0, &c, 1);
			}

			if (ret == 0) 
			{
				local[index++] = 0;
				break;
			}
		
			if (c == '\n')
			{
				local[index++] = c;
				local[index++] = 0;
				break;
			}

			local[index++] = c;
		}
			
		if (strncmp(local, "...", 3) == 0)
		{
			// 
			// This is the end of the value.
			//
			if (mSaveToFile)
			{
				// 
				// Need to save the contents into a file. 
				// 
				if (mFileName.length() < 0)
				{
					aos_alarm("File name is null");
					return false;
				}

				OmnFile file(mFileName, OmnFile::eCreate);
				if (!file.isGood())
				{
					aos_alarm("Failed to open the file: %s", 
						mFileName.data());
					return false;
				}

				if (!file.append(strv))
				{
					aos_alarm("Failed to write contents to file: %s", mFileName.data());
					return false;
				}
			}

			return true;
		}

		strv << local;
	}

	return false;
}
	
OmnCliCmd::OmnCliCmd()
{

}

AosCliLvl::CliLevel		
OmnCliCmd::getLevel()const
{
	return mLevel;
}

OmnCliCmd::OmnCliCmd(const OmnString &def)
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
		OmnCliParm* theParm = OmnNew OmnCliParm();
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
		else if (type == "portrange")
		{
			theParm->setType(OmnCliParm::ePortRange);
		}
		else if (type == "macaddr")
		{
			theParm->setType(OmnCliParm::eMacAddr);
		}
		else if (type == "interactive")
		{
			theParm->setType(OmnCliParm::eInteractive);
		}
		else if (type == "file")
		{
			theParm->setType(OmnCliParm::eFile);
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
OmnCliCmd::checkDef(OmnString &rslt)
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
OmnCliCmd::run(const OmnString &cmd,const int parmPos, OmnString &rslt,const AosModuleCliServerPtr moduleServer = 0)
{

	if(cmd.length() < parmPos)
	{
		rslt = "Unknown CLI module name: ";
		rslt << mModId;
		return false;
	}

	OmnString parms(cmd.data()+parmPos,cmd.length()-parmPos);
	
#ifdef AOS_ENABLE_SYSTEM_EXCUTE_CLI
	if (mModId == "system" && mOprId == "system_excute")
	{
		return systemExcute(parms,rslt);
	}	
#endif

	if (mModId == "local")
	{

#ifdef AOS_OLD_CLI
		if (mOprId == "save_config")
		{
			return saveConfig(parms, rslt);
		}

		if (mOprId == "load_config")
		{
			return loadConfig(parms, rslt);
		}

		if (mOprId == "clear_config")
		{
			AosCli cli;
			return cli.clearConfig(rslt) == 0;
		}
		
		// 
		// Kevin, 06/23/2006
		//
		if (mOprId == "firewall_load_config")
		{
			//printf("you enter the fw load config\n");
			return loadFwConfig(parms, rslt);
		}
		
		if (mOprId == "firewall_save_config")
		{
			return saveFwConfig(parms, rslt);
		}

		if (mOprId == "firewall_clear_config")
		{
			AosCli cli;
			return cli.clearFwConfig(rslt);
		}
		
		if (mOprId == "firewall_show_config")
		{
			AosCli cli;
			return cli.showFwConfig(rslt);
		}
			
		if (mOprId == "firewall_show_log")
		{
			AosCli cli;
			return cli.showFwLog(rslt);
		}
		
		if (mOprId == "firewall_init")
		{
			AosCli cli;
			return cli.initFwConfig(rslt);
		}

        if (mOprId == "firewall_maxsec_init")
		{
			AosCli cli;
			return cli.initFwConfig(rslt);
		}

        if (mOprId == "firewall_midsec_init")
		{
			AosCli cli;
			return cli.initFwConfig(rslt);
		}

        if (mOprId == "firewall_minsec_init")
		{
			AosCli cli;
			return cli.initFwConfig(rslt);
		}

		if (mOprId == "firewall_rule")
		{
			return fwRules(parms, rslt);
		}
		
		if (mOprId == "fw_timefil")
		{
			return fwTimefil(parms, rslt);
		}	
	
	    if (mOprId == "fw_del_timefil")
		{
			return fwDelTimefil(parms, rslt);
		}

		if (mOprId == "fw_ipfil_all")
		{
			return fwIpfilAll(parms, rslt);
		}
		
		if (mOprId == "fw_ipfil_tcp")
		{
			return fwIpfilTcp(parms, rslt);
		}
		
		if (mOprId == "fw_ipfil_udp")
		{
			return fwIpfilUdp(parms, rslt);
		}
		
		if (mOprId == "fw_ipfil_icmp")
		{
			return fwIpfilIcmp(parms, rslt);
		}
		
		if (mOprId == "fw_ipfil_state_new")
		{
			return fwIpfilStateNew(parms, rslt);
		}
		
		if (mOprId == "fw_ipfil_state_ack")
		{
			return fwIpfilStateAck(parms, rslt);
		}
		
		if (mOprId == "fw_macfil_all")
		{
			return fwMacfilAll(parms, rslt);
		}
		
		if (mOprId == "fw_macfil_tcp")
		{
			return fwMacfilTcp(parms, rslt);
		}
		
		if (mOprId == "fw_macfil_udp")
		{
			return fwMacfilUdp(parms, rslt);
		}
		
		if (mOprId == "fw_macfil_icmp")
		{
			return fwMacfilIcmp(parms, rslt);
		}
	
		if (mOprId == "fw_nat_snat")
		{
			return fwNatSnat(parms, rslt);
		}

		if (mOprId == "fw_nat_dnat_port")
		{
			return fwNatDnatPort(parms, rslt);
		}
	
		if (mOprId == "fw_nat_dnat_ip")
		{
			return fwNatDnatIP(parms, rslt);
		}

		if (mOprId == "fw_nat_masq")
		{
			return fwNatMasq(parms, rslt);
		}

		if (mOprId == "fw_nat_redi")
		{
			return fwNatRedi(parms, rslt);
		}

		if (mOprId == "fw_ipfil_del_all")
		{
			return fwIpfilDelAll(parms, rslt);
		}
		
		if (mOprId == "fw_ipfil_del_tcp")
		{
			return fwIpfilDelTcp(parms, rslt);
		}
		
		if (mOprId == "fw_ipfil_del_udp")
		{
			return fwIpfilDelUdp(parms, rslt);
		}
		
		if (mOprId == "fw_ipfil_del_icmp")
		{
			return fwIpfilDelIcmp(parms, rslt);
		}
		
		if (mOprId == "fw_ipfil_del_state_new")
		{
			return fwIpfilDelStateNew(parms, rslt);
		}
		
		if (mOprId == "fw_ipfil_del_state_ack")
		{
			return fwIpfilDelStateAck(parms, rslt);
		}
		
		if (mOprId == "fw_macfil_del_all")
		{
			return fwMacfilDelAll(parms, rslt);
		}
		
		if (mOprId == "fw_macfil_del_tcp")
		{
			return fwMacfilDelTcp(parms, rslt);
		}
		
		if (mOprId == "fw_macfil_del_udp")
		{
			return fwMacfilDelUdp(parms, rslt);
		}
		
		if (mOprId == "fw_macfil_del_icmp")
		{
			return fwMacfilDelIcmp(parms, rslt);
		}
	
		if (mOprId == "fw_nat_del_snat")
		{
			return fwNatDelSnat(parms, rslt);
		}

		if (mOprId == "fw_nat_del_dnat_port")
		{
			return fwNatDelDnatPort(parms, rslt);
		}
	
		if (mOprId == "fw_nat_del_dnat_ip")
		{
			return fwNatDelDnatIP(parms, rslt);
		}

		if (mOprId == "fw_nat_del_masq")
		{
			return fwNatDelMasq(parms, rslt);
		}

		if (mOprId == "fw_nat_del_redi")
		{
			return fwNatDelRedi(parms, rslt);
		}

		if (mOprId == "fw_antiattack_syn")
		{
			return fwAntiSyn(parms, rslt);
		}

		if (mOprId == "fw_antiattack_icmp")
		{
			return fwAntiIcmp(parms, rslt);
		}

		if (mOprId == "fw_antiattack_del_syn")
		{
			return fwAntiDelSyn(parms, rslt);
		}
		
		if (mOprId == "fw_antiattack_del_icmp")
		{
			return fwAntiDelIcmp(parms, rslt);
		}
		
		// 
		// Wang Wei, 11/13/2006
		//
		if (mOprId == "fw_blacklist_load_file")
		{
			return fwBlacklistLoadFile(parms, rslt);
		}
		
		
		// 
		// Bin Gong, 11/13/2006
		//
		if (mOprId == "fw_blacklist_load_url")
		{
			return fwBlacklistLoadUrl(parms, rslt);
		}
		
		
		// Bin Gong, 11/14/2006
		//
		if (mOprId == "fw_blacklist_remove_file")
		{
			return fwBlacklistRemoveFile(parms, rslt);
		}
		
		
		// Bin Gong, 11/14/2006
		//
		if (mOprId == "fw_blacklist_remove_url")
		{
			return fwBlacklistRemoveUrl(parms, rslt);
		}
		// 
		// Kevin, 07/10/2006
		//
		if (mOprId == "system_show_config")
		{
			AosCli cli;
			return cli.showSystemConfig(rslt);
		}	
		
		if (mOprId == "system_clear_config")
		{
			AosCli cli;
			return cli.clearSystemConfig(rslt);
		}	
			
		if (mOprId == "system_uname")
		{
			return systemUname(parms, rslt);
		}	
			
		if (mOprId == "system_arp_proxy_add")
		{
			return systemArpProxyAdd(parms, rslt);
		}	
			
		if (mOprId == "system_arp_proxy_del")
		{
			return systemArpProxyDel(parms, rslt);
		}	

		if (mOprId == "system_ip_address")
		{
			return systemIpAddress(parms, rslt);
		}	
		
		if (mOprId == "system_ip_dns")
		{
			return systemDns(parms, rslt);
		}	
		
		if (mOprId == "system_route_common")
		{
			return systemRouteCommon(parms, rslt);
		}	
		
		if (mOprId == "system_route_default")
		{
			return systemRouteDefault(parms, rslt);
		}	
			
		if (mOprId == "system_del_route_common")
		{
			return systemDelRouteCommon(parms, rslt);
		}	
		
		if (mOprId == "system_del_route_default")
		{
			return systemDelRouteDefault(parms, rslt);
		}	
		
		//dxr,11/27/2006
		if (mOprId == "system_route_show")
		{
			return systemRouteShow(parms, rslt);
		}	
		
		if (mOprId == "system_dev_route")
		{
			return systemDevRoute(parms, rslt);
		}	
		
		if (mOprId == "system_del_dev_route")
		{
			return systemDelDevRoute(parms, rslt);
		}	
		
		if (mOprId == "system_update")
		{
			return systemUpdate(parms, rslt);
		}
	
		// kevin, 11/09/2006	
		if (mOprId == "webwall_redi")
		{
			return webwallRedi(parms, rslt);
		}
		
		// kevin, 11/09/2006	
		if (mOprId == "webwall_del_redi")
		{
			return webwallDelRedi(parms, rslt);
		}

		// dxr, 11/08/2006
		if (mOprId == "dhcp_start")
		{
			return dhcpStart(parms, rslt);
		}
		
		if (mOprId == "dhcp_stop")
		{
			return dhcpStop(parms, rslt);
		}
                
		// dxr, 11/15/2006
	    if (mOprId == "dhcp_server_show_config")
		{
		    return dhcpServerShowConfig(parms, rslt);
	    }
		
		// dxr, 11/16/2006
	    if (mOprId == "dhcp_server_lease_time_set")
		{
		        return dhcpServerLeaseTimeSet(parms, rslt);
	    }
		
	    if (mOprId == "dhcp_server_router_set")
		{
		        return dhcpServerRouterSet(parms, rslt);
	    }
	    
		if (mOprId == "dhcp_server_dns_set")
		{
		        return dhcpServerDnsSet(parms, rslt);
	    }
	    
		if (mOprId == "dhcp_server_ip_block")
		{
		        return dhcpServerIpBlock(parms, rslt);
	    }
		
		// dxr, 11/16/2006
	    if (mOprId == "dhcp_server_add_bind")
		{
		        return dhcpServerAddBind(parms, rslt);
	    }
	    
		if (mOprId == "dhcp_server_del_bind")
		{
		        return dhcpServerDelBind(parms, rslt);
	    }
		
		// dxr, 11/20/2006
		if (mOprId == "dhcp_server_load_config")
		{
			return loadDhcpServerConfig(parms, rslt);
		}
		
		// dxr, 11/20/2006
		if (mOprId == "dhcp_server_save_config")
		{
			return saveDhcpServerConfig(parms, rslt);
		}
		
		// dxr, 11/23/2006
		if (mOprId == "dhcp_client_get")
		{
			return dhcpClientGetIp(parms, rslt);
		}
		
		// dxr, 11/20/2006
		if (mOprId == "dhcp_client_show")
		{
			return dhcpClientShowIp(parms, rslt);
		}
		// xyb, 12/07/2006
		if (mOprId == "cpu_mgr_set_record_time")
		{
			return cpuMgrSetRecordTime(parms, rslt);
		}
		if (mOprId == "cpu_mgr_set_record_stop")
		{
			return cpuMgrSetRecordStop(parms, rslt);
		}
		if (mOprId == "cpu_mgr_show_cpu")
		{
			return cpuMgrShowCpu(parms, rslt);
		}
		if (mOprId == "cpu_mgr_show_process")
		{
			return cpuMgrShowProcess(parms, rslt);
		}

		// xyb. 12/25/2006
		if (mOprId == "disk_mgr_set_record_time")
		{
			return diskMgrSetRecordTime(parms, rslt);
		}
		if (mOprId == "disk_mgr_set_record_stop")
		{
			return diskMgrSetRecordStop(parms, rslt);
		}
		if (mOprId == "disk_mgr_show_statistics")
		{
			return diskMgrShowStatistics(parms, rslt);
		}
		if (mOprId == "disk_mgr_file_type_add")
		{
			return diskMgrFileTypeAdd(parms, rslt);
		}
		if (mOprId == "disk_mgr_file_type_remove")
		{
			return diskMgrFileTypeRemove(parms, rslt);
		}
		if (mOprId == "disk_mgr_file_type_clear")
		{
			return diskMgrFileTypeClear(parms, rslt);
		}
		if (mOprId == "disk_mgr_file_type_show")
		{
			return diskMgrFileTypeShow(parms, rslt);
		}
		// CHK. 2007/01/04
		if (mOprId == "mem_mgr_set_record_time")
		{
			return memMgrSetRecordTime(parms, rslt);
		}
		if (mOprId == "mem_mgr_set_record_stop")
		{
			return memMgrSetRecordStop(parms, rslt);
		}
		if (mOprId == "mem_mgr_show_mem")
		{
			return memMgrShowMem(parms, rslt);
		}

		/*
		//xyb 12/26
		if (mOprId == "disk_mgr_quota_status")
		{
			return diskMgrQuotaStatus(parms, rslt);
		}
		if (mOprId == "disk_mgr_quota_status_show")
		{
			return diskMgrQuotaStatusShow(parms, rslt);
		}
		if (mOprId == "disk_mgr_quota_list")
		{
			return diskMgrQuotaList(parms, rslt);
		}
		if (mOprId == "disk_mgr_quota_set")
		{
			return diskMgrQuotaSet(parms, rslt);
		}
		if (mOprId == "disk_mgr_quota_delete")
		{
			return diskMgrQuotaDelete(parms, rslt);
		}
		if (mOprId == "disk_mgr_quota_show")
		{
			return diskMgrQuotaShow(parms, rslt);
		}
		*/

		if (mOprId == "watchdog_set_status")
		{
			return watchdogSetStatus(parms, rslt);
		}
		if (mOprId == "watchdog_stop")
		{
			return watchdogStop(parms, rslt);
		}
		//dxr ,12/06
        if (mOprId == "secure_authcmd_policy")
		{
			return secureAuthcmdPolicy(parms, rslt);
		}
        
		if (mOprId == "secure_authcmd_command_add")
		{
			return secureAuthcmdCommandAdd(parms, rslt);
		}
        
		if (mOprId == "secure_authcmd_command_del")
		{
			return secureAuthcmdCommandDel(parms, rslt);
		}
        
		if (mOprId == "secure_authcmd_command_show")
		{
			return secureAuthcmdCommandShow(parms, rslt);
		}
        
		if (mOprId == "secure_authcmd_command_reset")
		{
			return secureAuthcmdCommandReset(parms, rslt);
		}
		
		// GB, 11/15/2006
		if(mOprId == "dns_proxy_start")
		{
			return dnsproxyStart(parms, rslt);
		}
		
		if (mOprId == "dns_proxy_stop")
		{
			return dnsproxyStop(parms, rslt);
		}
                
                // GB, 11/15/2006
	        if (mOprId == "dns_proxy_set_name")
		{
		    return dnsproxySetName(parms, rslt);
	        }

		// GB, 11/15/2006
	        if (mOprId == "dns_proxy_set_ip")
		{
		    return dnsproxySetIp(parms, rslt);
	        }
	
                // GB, 11/24/2006
	        if (mOprId == "dns_proxy_set_lip")
		{
		    return dnsproxySetLIp(parms, rslt);
	        }

		// GB, 11/20/2006
	        if (mOprId == "dns_proxy_show_config")
		{
		    return dnsproxyShow(parms, rslt);
	        } 
     	
		// GB, 11/20/2006
		if (mOprId == "dns_proxy_load_config")
		{
			return loadDnsproxyConfig(parms, rslt);
		}
	
	    // GB, 11/28/2006
	        if (mOprId == "dns_proxy_add_global")
		{
		    return dnsproxyAddGlobal(parms, rslt);
	        } 
     	
		// GB, 11/20/2006
		if (mOprId == "dns_proxy_del_global")
		{
			return dnsproxyDelGlobal(parms, rslt);
		}

		// GB, 11/20/2006
		if (mOprId == "dns_proxy_save_config")
		{
			return saveDnsproxyConfig(parms, rslt);
		}
		
		// GB, 11/16/2006
		if (mOprId == "pppoe_start")
		{
			return pppoeStart(parms, rslt);
		}
		
		if (mOprId == "pppoe_stop")
		{
			return pppoeStop(parms, rslt);
		}
                
		// GB, 11/16/2006
	    if (mOprId == "pppoe_status")
		{
		    return pppoeStatus(parms, rslt);
	    }
		
		// GB, 11/20/2006
	    if (mOprId == "pppoe_show_config")
		{
		    return pppoeShow(parms, rslt);
	    }
		
		// GB, 11/20/2006
		if (mOprId == "pppoe_load_config")
		{
			return loadPppoeConfig(parms, rslt);
		}
		
		// GB, 11/20/2006
		if (mOprId == "pppoe_save_config")
		{
			return savePppoeConfig(parms, rslt);
		}

        // GB, 11/17/2006
	    if (mOprId == "pppoe_username_set")
		{
		    return pppoeUsernameSet(parms, rslt);
	    }
        
		//GBm 11/20/2006
		if (mOprId == "pppoe_password_set")
		{
		    return pppoePasswordSet(parms, rslt);
	    }
		
		if (mOprId == "pppoe_dns_set")
		{
		    return pppoeDnsSet(parms, rslt);
	    }
		// GB, 11/16/2006
		if (mOprId == "pptp_start")
		{
			return pptpStart(parms, rslt);
		}
		
		if (mOprId == "pptp_stop")
		{
			return pptpStop(parms, rslt);
		}
        
		//GB , 11/22/2006
        if (mOprId == "pptp_local_ip")
		{
			return pptpIpLocal(parms, rslt);
		}
		
		if (mOprId == "pptp_ip_range")
		{
			return pptpIpRange(parms, rslt);
		}
		
		if (mOprId == "pptp_add_user")
		{
			return pptpAddUser(parms, rslt);
		}
		
		if (mOprId == "pptp_del_user")
		{
			return pptpDelUser(parms, rslt);
		}

        if (mOprId == "pptp_show_config")
		{
			return pptpShowConfig(parms, rslt);
		}
	
	    if (mOprId == "pptp_save_config")
		{
			return savePptpConfig(parms, rslt);
		}

		if (mOprId == "pptp_load_config")
		{
			return loadPptpConfig(parms, rslt);
		}

        if (mOprId == "mac_set")
		{
			return macSet(parms, rslt);
		}

        if (mOprId == "mac_back_set")
		{
			return macBackSet(parms, rslt);
		}

        if (mOprId == "mac_save_config")
		{
			return saveMacConfig(parms, rslt);
		}
	
        if (mOprId == "mac_show_config")
		{
			return macShowConfig(parms, rslt);
		}

        if (mOprId == "mac_load_config")
		{
			return loadMacConfig(parms, rslt);
		}
        
		if (mOprId == "bridge_group_add")
		{
			return bridgeGroupAdd(parms, rslt);
		}

        if (mOprId == "bridge_group_del")
		{
			return bridgeGroupDel(parms, rslt);
		}
		
		if (mOprId == "bridge_group_member_add")
		{
			return bridgeGroupMemberAdd(parms, rslt);
		}

        if (mOprId == "bridge_group_member_del")
		{
			return bridgeGroupMemberDel(parms, rslt);
		}

        if (mOprId == "bridge_group_set_ip")
		{
			return bridgeGroupIp(parms, rslt);
		}

       if (mOprId == "bridge_group_member_set_ip")
		{
			return bridgeGroupMemberIp(parms, rslt);
		}

        if (mOprId == "bridge_show_config")
		{
			return bridgeShowConfig(parms, rslt);
		}
	
        if (mOprId == "bridge_clear_config")
		{
			return bridgeClearConfig(parms, rslt);
		}

        if (mOprId == "bridge_group_up")
		{
			return bridgeGroupUp(parms, rslt);
		}

        if (mOprId == "bridge_group_down")
		{
			return bridgeGroupDown(parms, rslt);
		}

		// CHK, 12/12/2006
		if (mOprId == "bridge_load_config")
		{
			return loadBridgeConfig(parms, rslt);
		}
	
		// CHK, 12/12/2006
		if (mOprId == "bridge_save_config")
		{
			return saveBridgeConfig(parms, rslt);
		}
	
		// Chen Ding, 05/01/2006
		//
		if (mOprId == "InMemRetrieve")
		{
			return retrieveInMemLog(parms, rslt);
		}
		
		//jzz , 11/20/2006
		if (mOprId == "create_secured_shell")
		{
			return createSecuredShell(parms, rslt);
		}
		//jzz , 11/20/2006
		if (mOprId == "remove_secured_shell_commands")
		{
			return removeShellCommands(parms, rslt);
		}
		//jzz , 11/20/2006
		if (mOprId == "restore_secured_shell_commands")
		{
			return restoreShellCommands(parms, rslt);
		}
		//jzz , 11/20/2006
		if (mOprId == "clear_secured_shell")
		{
			return clearActiveShells(parms, rslt);
		}
		//jzz , 11/20/2006
		if (mOprId == "start_secured_shell")
		{
			return startSecuredShell(parms, rslt);
		}
		
		//jzz , 11/20/2006
		if (mOprId == "stop_secured_shell")
		{
			return stopSecuredShell(parms, rslt);
		}
		//jzz , 11/20/2006
		if (mOprId == "username")
		{
			return username(parms, rslt);
		}
#endif
		rslt = "Unknown local operation: ";
		rslt << mOprId;
		return false;
	}

	if (mModId == "kernel")
	{
		return runKernel(cmd, parms, rslt);
	}
	
	

	// 
	// Li Xiaoxiang, 03/12/2006
	// 
	return (moduleServer && moduleServer->runCli(mModId, cmd,rslt));
}


bool
OmnCliCmd::runKernel(OmnString cmd, const OmnString &parms, OmnString &rslt)
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
	// uint totalLen = (intIndex + 2) * sizeof(int) + mBufsize;
	
	uint totalLen = (intIndex + 2) * sizeof(int) + mBufsize + (cmd.length()+1);
	
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

	// added by liqin send cmd to kernel 
	strcpy(svhead, cmd.data());
	svhead += cmd.length() + 1;
	
	// 
	// Now, we have put all data into 'data'. It is the time to 
	// send it.
	//
    int ret = OmnKernelApi::sendToKernel(data, totalLen+1);

	rslt = "";
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
OmnCliCmd::appendIntToStr(OmnString &rslt, int ret)
{
	int len = rslt.length();
	char *r;
	
	rslt.setSize(len+sizeof(int)+2);
	rslt.setLength(len+sizeof(int)+1);
	r = rslt.getBuffer() + len + 1;
	memcpy(r, &ret, sizeof(int));

	//
	// Very important
	//
	rslt.setChar(0, len);
	return true;
}

bool
OmnCliCmd::runAppPreparing(const OmnString &parms, 
				  char **data, 
				  unsigned int *totalLen,
				  OmnString &rslt)
{
	*data = new char[1000];
	*totalLen = 1000;

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
				return false;
			}

			intValues[intIndex++] = intv;
		}
		else
		{
			if (strIndex >= eMaxStrArgs)
			{
				rslt = "Too many string arguments";
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

		return false;
	}

	// 
	// Send the data to kernel.
	//
	
	// 
	// Calculate the buffer total length
	//
	*totalLen = (intIndex + 2) * sizeof(int) + mBufsize;
	int i;
	for (i=0; i<strIndex; i++)
	{
		*totalLen += strValues[i].length() + 1;
	}

	if (*totalLen >= eMaxDataSize)
	{
		rslt = "Data too big to pass to kernel: ";
		rslt << *totalLen;
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
	*totalLen += mOprId.length();
	*data = OmnNew char[*totalLen+10];
	memcpy(*data, mOprId.data(),mOprId.length());
	(*data)[mOprId.length()] = 0;

	int *iv = (int*)((*data) + mOprId.length()+1);
	iv[0] = intIndex;
	iv[1] = strIndex;
	for (i=2; i<=intIndex+1; i++)
	{
		iv[i] = intValues[i-2];
	}

	char *svhead = (*data) + mOprId.length() + 1 + (intIndex + 2) * sizeof(int);
	for (i=0; i<strIndex; i++)
	{
		strcpy(svhead, strValues[i].data());
		svhead += strValues[i].length() + 1;
	}
	return true;
}




bool
OmnCliParm::getFromFile(const OmnString &filename, OmnString &strv)
{
	//
	// The parameter value is stored in a file. The file
	// name is passed through 'filename'. 
	//
	OmnFile file(filename, OmnFile::eReadOnly);
	if (!file.isGood())
	{
		cout << "Failed to read the file: " 
			<< filename << endl;
		return false;
	}
	/*	
	if (file.getLength() >= eMaxFileLength)
	{
		cout << "File too big: " << file.getLength()
			<< ". Maximum allowed: " 
			<< eMaxFileLength << endl;
		return false;
	}*/

	strv = "";
	if (!file.readAll(strv))
	{
		cout << "Failed to read the file: " 
			<< filename << endl;
		return false;
	}

	return true;
}


// 
// Chen Ding, 05/01/2006
//
bool
OmnCliCmd::retrieveInMemLog(const OmnString &parms, OmnString &rslt)
{
	if (!runKernel(OmnString("Retrieve in memory log"), parms, rslt)) return false;

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

		if (!runKernel(OmnString("Retrieve in memory log"), parms, rslt)) return false;
	}

	return true;
}


#ifdef AOS_OLD_CLI
//kevin 07/28/06
bool
OmnCliCmd::systemUname(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.systemUname(rslt);
	return ret == 0;	
}


//kevin 07/28/06
bool
OmnCliCmd::systemArpProxyAdd(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.systemArpProxyAdd(rslt);
	return ret == 0;	
}


//kevin 07/28/06
bool
OmnCliCmd::systemArpProxyDel(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.systemArpProxyDel(rslt);
	return ret == 0;	
}


//kevin 07/10/06
bool
OmnCliCmd::systemIpAddress(const OmnString &parms, OmnString &rslt)
{
/*	OmnStrParser parser(parms);
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
*/							
	AosCli cli(parms);
	int ret = cli.systemIpAddress(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::systemDns(const OmnString &parms, OmnString &rslt)
{
/*	OmnStrParser parser(parms);
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
*/							
	AosCli cli(parms);
	int ret = cli.systemDns(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::systemRouteCommon(const OmnString &parms, OmnString &rslt)
{
/*	OmnStrParser parser(parms);
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
*/							
	AosCli cli(parms);
	int ret = cli.systemRouteCommon(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::systemRouteDefault(const OmnString &parms, OmnString &rslt)
{
/*	OmnStrParser parser(parms);
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
*/							
	AosCli cli(parms);
	int ret = cli.systemRouteDefault(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::systemDelRouteCommon(const OmnString &parms, OmnString &rslt)
{
/*	OmnStrParser parser(parms);
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
*/							
	AosCli cli(parms);
	int ret = cli.systemDelRouteCommon(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::systemDelRouteDefault(const OmnString &parms, OmnString &rslt)
{
/*	OmnStrParser parser(parms);
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
*/							
	AosCli cli(parms);
	int ret = cli.systemDelRouteDefault(rslt);
	return ret == 0;	
}

//dxr 11/27/2006
bool
OmnCliCmd::systemRouteShow(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.systemRouteShow(rslt);
	return ret == 0;	

}

bool
OmnCliCmd::systemDevRoute(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.systemDevRoute(rslt);
	return ret == 0;	

}

bool
OmnCliCmd::systemDelDevRoute(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.systemDelDevRoute(rslt);
	return ret == 0;	

}


bool
OmnCliCmd::systemUpdate(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.systemUpdate(rslt);
	return ret == 0;	
}

bool
OmnCliCmd::webwallRedi(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.webwallRedi(rslt);
	return ret == 0;	
}

bool
OmnCliCmd::webwallDelRedi(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.webwallDelRedi(rslt);
	return ret == 0;	
}

// dxr, 11/08/2006
bool
OmnCliCmd::dhcpStart(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.dhcpStart(rslt);
	return ret == 0;	
}

bool
OmnCliCmd::dhcpStop(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.dhcpStop(rslt);
	return ret == 0;	
}

// dxr, 11/15/2006
bool
OmnCliCmd::dhcpServerShowConfig(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.dhcpServerShowConfig(rslt);
	return ret == 0;	
}

// dxr, 11/16/2006
bool
OmnCliCmd::dhcpServerLeaseTimeSet(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.dhcpServerLeaseTimeSet(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::dhcpServerRouterSet(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.dhcpServerRouterSet(rslt);
	return ret == 0;	
}

bool
OmnCliCmd::dhcpServerDnsSet(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.dhcpServerDnsSet(rslt);
	return ret == 0;	
}

bool
OmnCliCmd::dhcpServerIpBlock(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.dhcpServerIpBlock(rslt);
	return ret == 0;	
}

// dxr, 11/17/2006
bool
OmnCliCmd::dhcpServerAddBind(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.dhcpServerAddBind(rslt);
	return ret == 0;	
}

// dxr, 11/17/2006
bool
OmnCliCmd::dhcpServerDelBind(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.dhcpServerDelBind(rslt);
	return ret == 0;	
}


//dxr, 11/20/2006
bool
OmnCliCmd::saveDhcpServerConfig(const OmnString &parms, OmnString &rslt)
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
	int ret = cli.saveOnlyDhcpServerConfig(rslt);
	return ret == 0;	
}


//dxr, 11/20/2006
bool
OmnCliCmd::loadDhcpServerConfig(const OmnString &parms, OmnString &rslt)
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
	int ret = cli.loadDhcpServerConfig(rslt);
	return ret == 0;	
}

bool
OmnCliCmd::dhcpClientGetIp(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.dhcpClientGetIp(rslt);
	return ret == 0;	
}

bool
OmnCliCmd::dhcpClientShowIp(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.dhcpClientShowIp(rslt);
	return ret == 0;	
}

//dxr 12/06
bool
OmnCliCmd::secureAuthcmdPolicy(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.secureAuthcmdPolicy(rslt);
	return ret == 0;	
}

bool
OmnCliCmd::secureAuthcmdCommandAdd(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.secureAuthcmdCommandAdd(rslt);
	return ret == 0;	
}

bool
OmnCliCmd::secureAuthcmdCommandDel(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.secureAuthcmdCommandDel(rslt);
	return ret == 0;	
}

bool
OmnCliCmd::secureAuthcmdCommandShow(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.secureAuthcmdCommandShow(rslt);
	return ret == 0;	
}

bool
OmnCliCmd::secureAuthcmdCommandReset(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.secureAuthcmdCommandReset(rslt);
	return ret == 0;	
}

// GB, 11/14/2006
bool
OmnCliCmd::dnsproxyStart(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.dnsproxyStart(rslt);
	return ret == 0;	
}

bool
OmnCliCmd::dnsproxyStop(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.dnsproxyStop(rslt);
	return ret == 0;	
}

bool
OmnCliCmd::dnsproxySetName(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.dnsproxySetName(rslt);
	return ret == 0;	
}

bool
OmnCliCmd::dnsproxySetIp(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.dnsproxySetIp(rslt);
	return ret == 0;	
}

bool
OmnCliCmd::dnsproxySetLIp(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.dnsproxySetLIp(rslt);
	return ret == 0;	
}

bool
OmnCliCmd::dnsproxyAddGlobal(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.dnsproxyAddGlobal(rslt);
	return ret == 0;	
}

bool
OmnCliCmd::dnsproxyDelGlobal(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.dnsproxyDelGlobal(rslt);
	return ret == 0;	
}

bool
OmnCliCmd::dnsproxyShow(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.dnsproxyShow(rslt);
	return ret == 0;	
}


//GB 11/20/2006
bool
OmnCliCmd::saveDnsproxyConfig(const OmnString &parms, OmnString &rslt)
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
	int ret = cli.saveOnlyDnsproxyConfig(rslt);
	return ret == 0;	
}


//GB 11/20/2006
bool
OmnCliCmd::loadDnsproxyConfig(const OmnString &parms, OmnString &rslt)
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
	int ret = cli.loadDnsproxyConfig(rslt);
	return ret == 0;	
}
//xyb. 12/07/2006
bool
OmnCliCmd::cpuMgrSetRecordTime(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.cpuMgrSetRecordTime(rslt);
	return ret == 0;	
}
bool
OmnCliCmd::cpuMgrSetRecordStop(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.cpuMgrSetRecordStop(rslt);
	return ret == 0;	
}
bool
OmnCliCmd::cpuMgrShowProcess(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.cpuMgrShowProcess(rslt);
	return ret == 0;	
}
bool
OmnCliCmd::cpuMgrShowCpu(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.cpuMgrShowCpu(rslt);
	return ret == 0;	
}
bool
OmnCliCmd::diskMgrSetRecordTime(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.diskMgrSetRecordTime(rslt);
	return ret == 0;	
}
bool
OmnCliCmd::diskMgrSetRecordStop(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.diskMgrSetRecordStop(rslt);
	return ret == 0;	
}
bool
OmnCliCmd::diskMgrShowStatistics(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.diskMgrShowStatistics(rslt);
	return ret == 0;	
}

bool
OmnCliCmd::diskMgrFileTypeAdd(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.diskMgrFileTypeAdd(rslt);
	return ret == 0;	
}
bool
OmnCliCmd::diskMgrFileTypeRemove(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.diskMgrFileTypeRemove(rslt);
	return ret == 0;	
}
bool
OmnCliCmd::diskMgrFileTypeClear(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.diskMgrFileTypeClear(rslt);
	return ret == 0;	
}
bool
OmnCliCmd::diskMgrFileTypeShow(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.diskMgrFileTypeShow(rslt);
	return ret == 0;	
}
// CHK 2006/01/04
bool
OmnCliCmd::memMgrSetRecordTime(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.memMgrSetRecordTime(rslt);
	return ret == 0;	
}
bool
OmnCliCmd::memMgrSetRecordStop(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.memMgrSetRecordStop(rslt);
	return ret == 0;	
}
bool
OmnCliCmd::memMgrShowMem(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.memMgrShowMem(rslt);
	return ret == 0;	
}
/*
//xyb 12/26
bool
OmnCliCmd::diskMgrQuotaStatus(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.diskMgrQuotaStatus(rslt);
	return ret == 0;	
}

bool
OmnCliCmd::diskMgrQuotaStatusShow(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.diskMgrQuotaStatusShow(rslt);
	return ret == 0;	
}

bool
OmnCliCmd::diskMgrQuotaList(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.diskMgrQuotaList(rslt);
	return ret == 0;	
}

bool
OmnCliCmd::diskMgrQuotaSet(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.diskMgrQuotaSet(rslt);
	return ret == 0;	
}

bool
OmnCliCmd::diskMgrQuotaDelete(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.diskMgrQuotaDelete(rslt);
	return ret == 0;	
}

bool
OmnCliCmd::diskMgrQuotaShow(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.diskMgrQuotaShow(rslt);
	return ret == 0;	
}
*/

bool
OmnCliCmd::watchdogSetStatus(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.watchdogSetStatus(rslt);
	return ret == 0;	
}
bool
OmnCliCmd::watchdogStop(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.watchdogStop(rslt);
	return ret == 0;	
}
// GB, 11/16/2006
bool
OmnCliCmd::pppoeStart(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.pppoeStart(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::pppoeStop(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.pppoeStop(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::pppoeStatus(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.pppoeStatus(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::pppoeShow(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.pppoeShow(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::pppoeUsernameSet(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.pppoeUsernameSet(rslt);
	return ret == 0;	
}

//Gb, 11/20/2006
bool
OmnCliCmd::pppoePasswordSet(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.pppoePasswordSet(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::pppoeDnsSet(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.pppoeDnsSet(rslt);
	return ret == 0;	
}


//GB 11/20/2006
bool
OmnCliCmd::savePppoeConfig(const OmnString &parms, OmnString &rslt)
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
	int ret = cli.saveOnlyPppoeConfig(rslt);
	return ret == 0;	
}


//GB 11/20/2006
bool
OmnCliCmd::loadPppoeConfig(const OmnString &parms, OmnString &rslt)
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
	int ret = cli.loadPppoeConfig(rslt);
	return ret == 0;	
}


// GB, 11/16/2006
bool
OmnCliCmd::pptpStart(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.pptpStart(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::pptpStop(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.pptpStop(rslt);
	return ret == 0;	
}


// GB, 11/22/2006
bool
OmnCliCmd::pptpIpLocal(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.pptpIpLocal(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::pptpIpRange(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.pptpIpRange(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::pptpAddUser(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.pptpAddUser(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::pptpDelUser(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.pptpDelUser(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::pptpShowConfig(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.pptpShowConfig(rslt);
	return ret == 0;	
}


//GB 11/22/2006
bool
OmnCliCmd::savePptpConfig(const OmnString &parms, OmnString &rslt)
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
	int ret = cli.saveOnlyPptpConfig(rslt);
	return ret == 0;	
}


//GB 11/22/2006
bool
OmnCliCmd::loadPptpConfig(const OmnString &parms, OmnString &rslt)
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
	int ret = cli.loadPptpConfig(rslt);
	return ret == 0;	
}


// GB, 11/30/2006
bool
OmnCliCmd::macSet(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.macSet(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::macBackSet(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.macBackSet(rslt);
	return ret == 0;	
}


//GB 12/04/2006
bool
OmnCliCmd::saveMacConfig(const OmnString &parms, OmnString &rslt)
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
	int ret = cli.saveOnlyMacConfig(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::macShowConfig(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.macShowConfig(rslt);
	return ret == 0;	
}


//GB 11/22/2006
bool
OmnCliCmd::loadMacConfig(const OmnString &parms, OmnString &rslt)
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
	int ret = cli.loadMacConfig(rslt);
	return ret == 0;	
}

///jzz add for secured shell

bool
OmnCliCmd::startSecuredShell(const OmnString &parms, OmnString &rslt)
	
{
	AosCli cli(parms);
	//bool ret = cli.startSecuredShell(rslt);
	//return ret;	
}

bool
OmnCliCmd::stopSecuredShell(const OmnString &parms, OmnString &rslt)
	
{
	AosCli cli(parms);
	//bool ret = cli.stopSecuredShell(rslt);
	//return ret ;	
}

bool
OmnCliCmd::username(const OmnString &parms, OmnString &rslt)
	
{
	AosCli cli(parms);
	bool ret = cli.username(rslt);
	return ret;	
}
bool
OmnCliCmd::createSecuredShell(const OmnString &parms, OmnString &rslt)
	
{
	AosCli cli(parms);
	//bool ret = cli.createSecuredShell(rslt);
	//return ret;	
}
bool
OmnCliCmd::removeShellCommands(const OmnString &parms, OmnString &rslt)
	
{
	AosCli cli(parms);
	//bool ret = cli.removeShellCommands(rslt);
	//return ret;	
}
bool
OmnCliCmd::restoreShellCommands(const OmnString &parms, OmnString &rslt)
	
{
	AosCli cli(parms);
	bool ret = cli.restoreShellCommands(rslt);
	return ret;	
}
bool
OmnCliCmd::clearActiveShells(const OmnString &parms, OmnString &rslt)
	
{
	AosCli cli(parms);
	bool ret = cli.clearActiveShells(rslt);
	return ret;	
}

bool
OmnCliCmd::bridgeGroupAdd(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.bridgeGroupAdd(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::bridgeGroupDel(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.bridgeGroupDel(rslt);
	return ret == 0;	
}



bool
OmnCliCmd::bridgeGroupMemberAdd(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.bridgeGroupMemberAdd(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::bridgeGroupMemberDel(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.bridgeGroupMemberDel(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::bridgeGroupIp(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.bridgeGroupIp(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::bridgeShowConfig(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.bridgeShowConfig(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::bridgeClearConfig(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.bridgeClearConfig(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::bridgeGroupMemberIp(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.bridgeGroupMemberIp(rslt);
	return ret == 0;	
}

// CHK 12/12/2006
bool
OmnCliCmd::saveBridgeConfig(const OmnString &parms, OmnString &rslt)
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
	int ret = cli.saveOnlyBridgeConfig(rslt);
	return ret == 0;	
}

// CHK 12/12/2006
bool
OmnCliCmd::loadBridgeConfig(const OmnString &parms, OmnString &rslt)
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
	int ret = cli.loadBridgeConfig(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::bridgeGroupDown(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.bridgeGroupDown(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::bridgeGroupUp(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.bridgeGroupUp(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::saveConfig(const OmnString &parms, OmnString &rslt)
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
OmnCliCmd::loadConfig(const OmnString &parms, OmnString &rslt)
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


#ifdef AOS_ENABLE_SYSTEM_EXCUTE_CLI
bool
OmnCliCmd::systemExcute(const OmnString &parms, OmnString &rslt)
{
	OmnCliSysCmd::doShell(parms,rslt);	
}
#endif
