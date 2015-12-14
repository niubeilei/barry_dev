////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// A Indexistics Task is defined as:
// 	Create Indexistics stat-name
// 	(
//     	Name="<stat-name>",
//      Inputs="<input-data>",
//      KeyFields=[<expression-list>],
//      Measures=[<measure-list>],
//      Filters=<condition>,
//      TimeUnit="<time_unit>"
//  );
//
// It will create the following:
// 	1. A dataset for the key field
// 	2. A group-by data proc
// 	3. A stat-join data proc
// 	4. A vector2D data proc
// 	5. A update key table data proc
// 	6. A update index data proc
// 	7. A task for the group-by data proc
// 	8. A task for join data proc
// 	9. A task for updating vector2D data proc
// 	10. A task for updating index data proc
// 	11. A task for updating key table


// Modification History:
// 2015/03/22 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicStartTorturer.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoParserAPI.h"
#include "Util/File.h"

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosJimoLogicStartTorturer_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicStartTorturer(version);
			aos_assert_r(jimo, 0);
			return jimo;
		}

		catch (...)
		{
			AosSetErrorU(rdata, "Failed creating jimo") << enderr;
			return 0;
		}

		OmnShouldNeverComeHere;
		return 0;
	}
}


AosJimoLogicStartTorturer::AosJimoLogicStartTorturer(const int version)
:
AosJimoLogicNew(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicStartTorturer::~AosJimoLogicStartTorturer()
{
}

AosJimoPtr
AosJimoLogicStartTorturer::cloneJimo() const
{
	return OmnNew AosJimoLogicStartTorturer(*this);
}


bool
AosJimoLogicStartTorturer::parseJQL(
		AosRundata *rdata,
		AosJimoParserObj *jimo_parser,
		AosJimoProgObj *prog,
		bool &parsed,
		bool dft)
{
	// The statement is:
	// 	Create Indexistics <stat-name>
	// 	(
	// 		name-value-list
	// 	);
	parsed = false;
	mIfNumber = true;
	aos_assert_rr(mKeywords[0] == "torturer", rdata, false);
	aos_assert_rr(mKeywords[1] == "start", rdata, false);

	mTorturerName = jimo_parser->nextObjName(rdata);
	if (mTorturerName == "")
	{
		// This is not what it expects.
		mErrmsg << "missing Torturer Name or unexpected Torturer Name!";		
		setErrMsg(rdata, eGenericError,"",mErrmsg);
		return false;
	}

	if(mTorturerName == "iiltorturer") mTorturerType = eIILTorturer;
	if(mTorturerName == "sorttorturer") mTorturerType = eSortTorturer;
	if(mTorturerName == "multicondquery") mTorturerType = eMultiCondTorturer;
	if(mTorturerName == "bitmaptorturer") mTorturerType = eBitmapTorturer;
	
	bool rslt = jimo_parser->getNameValueList(rdata, mNameValueList);
	if (!rslt)
	{
		// Not for this statement
		mErrmsg << mTorturerName;
		setErrMsg(rdata, eGenericError,"",mErrmsg);
		return false;
	}

	// Parse keys
    mNumber = jimo_parser->getParmInt(rdata, "number", mNameValueList);

	if (mNumber == -1)
	{
		mIfNumber = false;
		mTime = jimo_parser->getParmInt(rdata, "time", mNameValueList);
		if(mTime == -1)
		{
			mTime = mDftTime;
		}
		mStartMsg << "total time : " << mTime << "\n";
	}
	else
	{
		mStartMsg << "total round : " << mNumber << "\n";
	}
	
	mMaxRoundTime = jimo_parser->getParmInt(rdata, "maxroundtime", mNameValueList);
	if(mMaxRoundTime < 0)
	{
		mMaxRoundTime = 0;
	}
	aos_assert_r(mMaxRoundTime>=0, false);
	
	if(mMaxRoundTime==0)
	{
		mStartMsg << "limit round time : no \n"; 
	}
	else
	{
		mStartMsg << "limit round time : " << mMaxRoundTime << "\n";
	}
	
	mMaxMemory = jimo_parser->getParmInt(rdata, "maxmem", mNameValueList);
	if(mMaxMemory < 0)
	{
		mMaxMemory = 0;
	}
	aos_assert_r(mMaxMemory>=0, false);
	
	if(mMaxMemory==0)
	{
		mStartMsg << "limit memory : no \n"; 
	}
	else
	{
		mStartMsg << "limit memory : " << mMaxMemory << "\n";
	}
	
	mAlarmFlag = false;
	i64 alarm_flag = jimo_parser->getParmInt(rdata, "alarmflag", mNameValueList);

	if(alarm_flag >= 1)
		mAlarmFlag = true;
	
	mStartMsg << "Alarm Flag : " << mAlarmFlag << "\n";

	if(!checkNameValueList(rdata, mErrmsg, mNameValueList))
	{
		return false;
	}

	parsed = true;
	return true;
}


bool 
AosJimoLogicStartTorturer::run(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		OmnString &statements_str,
		bool inparser)
{
	OmnString msg;
	system("mkdir /tmp/torturer/");
	OmnFile username_file("/tmp/torturer/username_file", OmnFile::eCreate AosMemoryCheckerArgs);
	system("whoami >> /tmp/torturer/username_file");
	username_file.openFile(OmnFile::eReadOnly);
	OmnString username = "";
	username_file.readAll(username);
	username.setLength(username.length()-1);
	username_file.closeFile();

	OmnString torturerExe_str = "";
	OmnString torturerID_str = "";
	torturerExe_str << "/home/" << username << "/Jimo_Servers0/torturerExe_file";
	torturerID_str << "/home/" << username << "/Jimo_Servers0/torturerID_file";

	OmnFile exe_file(torturerExe_str.data(), OmnFile::eCreate AosMemoryCheckerArgs);
	//OmnFilePtr exe_file = OmnNew OmnFile("/home/liuwei/Jimo_Servers0/torturerExe_file", OmnFile::eCreate AosMemoryCheckerArgs);
	//OmnFile file("/home/liuwei/Jimo_Servers0/torturerID_file", OmnFile::eCreate AosMemoryCheckerArgs);
	OmnFile file(torturerID_str.data(), OmnFile::eCreate AosMemoryCheckerArgs);
	u64 tor_id;
	OmnString exe_file_str;
	
	switch(mTorturerType)
	{
		case eSortTorturer:
			system("find . -name sorttorturer.exe >> torturerExe_file");
			exe_file.openFile(OmnFile::eReadOnly);
			//bool finish = false;
			exe_file.readAll(exe_file_str);
			exe_file.closeFile();
			system("rm torturerExe_file");
			if(exe_file_str.isNull())
			{
				rdata->setJqlMsg("there is no sorttorturer.exe in current directory");
				return false;
			}
			system("pgrep sorttorturer >> torturerID_file");
			file.openFile(OmnFile::eReadOnly);
			tor_id = file.readU64(0,0);
			file.closeFile();
			system("rm torturerID_file");
			if(tor_id)
			{
				rdata->setJqlMsg("sorttorturer has been started already, do not start it again");
				return true;
			}
			else
			{
				OmnString start_str = "./sorttorturer.exe -config ~/Jimo_Servers0/tor_config.txt ";
				if(mIfNumber)
				{
					start_str << "-number " << mNumber;
				}
				else
				{
					start_str << "-time " << mTime;
				}
			
				if(mMaxRoundTime)
				{
					start_str << " -maxroundtime " << mMaxRoundTime;
				}
				
				if(mMaxMemory)
				{
					start_str << " -maxmemory " << mMaxMemory;
				}
				
				if(mAlarmFlag)
				{
					start_str << " -alarmflag " << mAlarmFlag;
				}
				start_str << "&";
				system(start_str.data());
				msg << "\ntorturer sorttorturer start \n";
				msg << mStartMsg;
			}
			break;
			
		case eIILTorturer:
			system("find . -name iiltorturer.exe >> torturerExe_file");
			exe_file.openFile(OmnFile::eReadOnly);
			//bool finish = false;
			exe_file.readAll(exe_file_str);
			exe_file.closeFile();
			system("rm torturerExe_file");
			if(exe_file_str.isNull())
			{
				rdata->setJqlMsg("there is no iiltorturer.exe in current directory");
				return false;
			}
			system("pgrep iiltorturer >> torturerID_file");
			file.openFile(OmnFile::eReadOnly);
			tor_id = file.readU64(0,0);
			file.closeFile();
			system("rm torturerID_file");
			if(tor_id)
			{
				rdata->setJqlMsg("iiltorturer has been started already, do not start it again");
				return true;
			}
			else
			{
				OmnString start_str = "./iiltorturer.exe -config ~/Jimo_Servers0/tor_config.txt";
				if(mIfNumber)
				{
					start_str << " -number " << mNumber;
				}
				else
				{
					start_str << " -time " << mTime;
				}
				
				if(mMaxRoundTime)
				{
					start_str << " -maxroundtime " << mMaxRoundTime;
				}
				
				if(mMaxMemory)
				{
					start_str << " -maxmemory " << mMaxMemory;
				}
				
				if(mAlarmFlag)
				{
					start_str << " -alarmflag " << mAlarmFlag;
				}
				start_str << "&";
				
				system(start_str.data());
				msg << "\ntorturer iiltorturer start \n";
				msg << mStartMsg;
			}
			break;
			
		case eMultiCondTorturer:
			system("find . -name multicondquery.exe >> torturerExe_file");
			exe_file.openFile(OmnFile::eReadOnly);
			//bool finish = false;
			exe_file.readAll(exe_file_str);
			exe_file.closeFile();
			system("rm torturerExe_file");
			if(exe_file_str.isNull())
			{
				rdata->setJqlMsg("there is no multicondquery.exe in current directory");
				return false;
			}
			system("pgrep multicondquery >> torturerID_file");
			file.openFile(OmnFile::eReadOnly);
			tor_id = file.readU64(0,0);
			file.closeFile();
			system("rm torturerID_file");
			if(tor_id)
			{
				rdata->setJqlMsg("multicondquery has been started already, do not start it again");
				return true;
			}
			else
			{
				OmnString start_str = "./multicondquery.exe -config ~/Jimo_Servers0/tor_config.txt";
				if(mIfNumber)
				{
					start_str << " -number " << mNumber;
				}
				else
				{
					start_str << " -time " << mTime;
				}
				
				if(mMaxRoundTime)
				{
					start_str << " -maxroundtime " << mMaxRoundTime;
				}
				
				if(mMaxMemory)
				{
					start_str << " -maxmemory " << mMaxMemory;
				}
				
				if(mAlarmFlag)
				{
					start_str << " -alarmflag " << mAlarmFlag;
				}
				start_str << "&";
				system(start_str.data());
				msg << "\ntorturer multicondquery start \n";
				msg << mStartMsg;
			}
			break;
			
		case eBitmapTorturer:
			system("find . -name bitmaptorturer.exe >> torturerExe_file");
			exe_file.openFile(OmnFile::eReadOnly);
			//bool finish = false;
			exe_file.readAll(exe_file_str);
			exe_file.closeFile();
			system("rm torturerExe_file");
			if(exe_file_str.isNull())
			{
				rdata->setJqlMsg("there is no bitmaptorturer.exe in current directory");
				return false;
			}
			system("pgrep bitmaptorturer >> torturerID_file");
			file.openFile(OmnFile::eReadOnly);
			tor_id = file.readU64(0,0);
			file.closeFile();
			system("rm torturerID_file");
			if(tor_id)
			{
				rdata->setJqlMsg("bitmaptorturer has been started already, do not start it again");
				return true;
			}
			else
			{
				OmnString start_str = "./bitmaptorturer.exe";
				if(mIfNumber)
				{
					start_str << " -number " << mNumber;
				}
				else
				{
					start_str << " -time " << mTime;
				}
				
				if(mMaxRoundTime)
				{
					start_str << " -maxroundtime " << mMaxRoundTime;
				}
				
				if(mMaxMemory)
				{
					start_str << " -maxmemory " << mMaxMemory;
				}
				
				if(mAlarmFlag)
				{
					start_str << " -alarmflag " << mAlarmFlag;
				}
				start_str << "&";
				system(start_str.data());
				msg << "\ntorturer bitmaptorturer start \n";
				msg << mStartMsg;
			}
			break;
			
		default:
			msg << "Wrong torturer name or order number\n";
			rdata->setJqlMsg(msg);
			return true;
	}

	rdata->setJqlMsg(msg);

	return true;
}

bool
AosJimoLogicStartTorturer::parseRun(
		AosRundata* rdata,
		OmnString &stmt,
		AosJimoProgObj *jimo_prog)
{
	AosJimoParserObjPtr jimo_parser = Jimo::jimoCreateJimoParser(rdata);
	aos_assert_rr(jimo_parser, rdata, false);

	vector<AosJqlStatementPtr> statements;
	bool rslt = jimo_parser->parse(rdata, jimo_prog, stmt, statements);
	aos_assert_rr(rslt, rdata, false);
	if (statements.size() <= 0)
	{
		AosLogError(rdata, true, "failed_parsing_statement")
			<< AosFN("Statement") << stmt << enderr;
		return false;
	}

	OmnString ss;
	for(size_t i = 0;i < statements.size();i++)
	{
		statements[i]->setContentFormat(OmnString("print"));
		statements[i]->run(rdata, jimo_prog, ss, true);
	}
	return true;
}


bool
AosJimoLogicStartTorturer::createIndexDataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &statements_str)
{
    mJobName = jimo_prog->getJobname();
    OmnString schemaname = jimo_prog->getEnv("schema_name");
	OmnString dataprocIndex_str = "create dataprocindex ";
	mDataprocIndex << mJobName << "_dp_index_" << mTableName << "_" << mIndexName;
	dataprocIndex_str << mDataprocIndex << " { "
		<< "\"type\":" << "\"" << "index" << "\"" << ","
        << "\"record_type\":" << "\"buff\"";
	if(mCond != "")
		dataprocIndex_str << "," << "\"condition\":" << "\"" << mCond << "\"";
	dataprocIndex_str << "," << "\"keys\":" << "[" ;
	for(u32 i=0; i< mKeyFields.size(); i++)
	{
		if(i>0)
			dataprocIndex_str << ",";
		dataprocIndex_str << "\"" << mKeyFields[i] << "\"";
	}
	dataprocIndex_str << "],";
	dataprocIndex_str << "\"max_keylen\":" << "100,"
        << "\"docid\":" << "\"" << "getDocid('" << schemaname << "')" << "\"";
	if(mCond != "")
		dataprocIndex_str << "," << "\"condition\":" << "\"" << mCond << "\"";
	dataprocIndex_str << "};";

	OmnScreen << dataprocIndex_str << endl;
	statements_str << "\n" << dataprocIndex_str;

    bool rslt = parseRun(rdata,dataprocIndex_str,jimo_prog);
    aos_assert_rr(rslt,rdata,false);
	return true;
}


bool
AosJimoLogicStartTorturer::createIndexU64(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &statements_str)
{
    mJobName = jimo_prog->getJobname();
    OmnString schemaname = jimo_prog->getEnv("schema_name");
	OmnString dataprocindex_str = "create dataprocindexu64 ";
	mDataprocIndex << mJobName << "_dp_index_" << mTableName << "_" << mIndexName;
	dataprocindex_str << mDataprocIndex << " { "
		<< "\"type\":" << "\"indexu64\"" << ","
		<< "\"key\":" << "\"" << mKeyFields[0] << "\", ";
	if(mCond != "")
	{
		dataprocindex_str << "," << "\"condition\":" << "\"" << mCond << "\"";
	}
	dataprocindex_str << "\"docid\":" << "\"" << "getDocid('" << schemaname << "')" << "\""
		<< "};";

	OmnScreen << dataprocindex_str << endl;
	statements_str << "\n" << dataprocindex_str;

	bool rslt = parseRun(rdata,dataprocindex_str,jimo_prog);
	aos_assert_rr(rslt,rdata,false);
	return true;

}

bool
AosJimoLogicStartTorturer::createIILDataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &statements_str)
{

	OmnString dataprociil_str = "create dataprociilbatchopr ";
	OmnString mIILName;
	mIndexIILName << mJobName << "_dp_index_iil_" << mTableName << "_" << mIndexName;
	mIILName << "_zt44_idx_" << mTableName << "_" << mIndexName;
	//for(u32 i=0; i<mKeyFields.size(); i++)
//	{
		//mIILName << "_" << mKeyFields[0];
//	}
	dataprociil_str << mIndexIILName << " { "
		<< "\"iilname\":" << "\"" << mIILName << "\"" << ",";
	if (mType == "indexu64")
		dataprociil_str << "\"opr\":" << "\"u64add\"" << "};";
	else
		dataprociil_str << "\"opr\":" << "\"stradd\"" << "};";

	OmnScreen << dataprociil_str << endl;
	statements_str << "\n" << dataprociil_str;

    bool rslt = parseRun(rdata,dataprociil_str,jimo_prog);
    aos_assert_rr(rslt,rdata,false);
	return true;
}

bool
AosJimoLogicStartTorturer::addIndexTask(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog)
{

	OmnString addindex_str;
 	OmnString input = mInput->dumpByNoQuote();
	OmnString output;
	OmnString TaskName;
	TaskName << "task_index_" << mTableName << "_" << mIndexName;
	output << mDataprocIndex << "_output";
	//addgroupby_str << jobname;
	addindex_str << " {"
		<< "\"name\":" << "\"" << TaskName << "\"" <<","
		<< "\"dataengine_type\":" << "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" <<  "\"" << mDataprocIndex << "\","
		<< "\"inputs\":" << "[\"" << input << "\"],"
		<< "\"outputs\":" << "[\"" << output << "\"" << "]}]}";
	jimo_prog->addTask(rdata, addindex_str, "");
//	mTasks.push_back(addindex_str);
//	OmnScreen << addindex_str << endl;
//  bool rslt = parseRun(rdata,addindex_str,jimo_prog);
//  aos_assert_rr(rslt,rdata,false);
	return true;
}

/*
bool
AosJimoLogicStartTorturer::addIndexU64Task(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog)
{
	OmnString addindex_str;
 	OmnString input = mInput->dumpByNoQuote();
	OmnString output;
	OmnString TaskName;
	TaskName << "task_index_" << mTableName << "_" << mIndexName;
	output << mDataprocU64Index << "_output";
	//addgroupby_str << jobname;
	addindex_str << " {"
		<< "\"name\":" << "\"" << TaskName << "\"" <<","
		<< "\"dataengine_type\":" << "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" <<  "\"" << mDataprocU64Index << "\","
		<< "\"inputs\":" << "[\"" << input << "\"],"
		<< "\"outputs\":" << "[\"" << output << "\"" << "]}]}";
	jimo_prog->addTask(rdata, addindex_str, "");
//	mTasks.push_back(addindex_str);
//	OmnScreen << addindex_str << endl;
//  bool rslt = parseRun(rdata,addindex_str,jimo_prog);
//  aos_assert_rr(rslt,rdata,false);
	return true;
}
*/

bool
AosJimoLogicStartTorturer::addIILTask(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog)
{
	OmnString addiil_str;
 	OmnString input;
	OmnString output;
	OmnString TaskName;
	TaskName << "task_index_iil_" << mTableName << "_" << mIndexName;
	input << mDataprocIndex << "_output";
	//addgroupby_str << jobname;
	addiil_str << " {"
		<< "\"name\":" << "\"" << TaskName << "\"" <<","
		<< "\"dataengine_type\":" << "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" << "\"" << mIndexIILName << "\","
		<< "\"inputs\":" << "[\"" << input << "\"" << "]}]}";
	jimo_prog->addTask(rdata, addiil_str, "");
	//OmnScreen << addiil_str << endl;
	return true;
}


