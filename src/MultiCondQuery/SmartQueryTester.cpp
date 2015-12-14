////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2015/03/11 Created by liuwei	
////////////////////////////////////////////////////////////////////////////
#include "MultiCondQuery/SmartQueryTester.h"

#include "AppMgr/App.h"
#include "QueryRslt/QueryRslt.h"
#include "IILClient/IILClient.h"
#include "IILUtil/IILUtil.h"
#include "BitmapTrans/Ptrs.h"
#include "BitmapTrans/TransBitmapQuery.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include "Util/UtUtil.h"
#include "Util/Opr.h"
#include "MultiCondQuery/MCField.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "Util/File.h"
#include "NativeAPINew/zykie.h"

extern int sgMCQStep;
extern int sgQueryFlag;
extern u64 sgNumber;
extern u64 sgTime;

AosSmartQueryTester::AosSmartQueryTester()
{
	mNumRecords = 100;
	mTableName = "TEMP_ZW_0818_GJZ_4G1";
	mZeroStr = "00000000000000000000";
	//mOptHostName = "192.168.99.82";
	mOptHostName = "";
/*
	bool flag = true;
	while(flag)
	{}
*/
	system("mkdir /tmp/torturer/");
	OmnFile username_file("/tmp/torturer/username_file", OmnFile::eCreate AosMemoryCheckerArgs);
	system("whoami >> /tmp/torturer/username_file");
	//OmnFile username_file("username_file", OmnFile::eCreate AosMemoryCheckerArgs);
	//system("whoami >> username_file");
	username_file.openFile(OmnFile::eReadOnly);
	mName = "";
	username_file.readAll(mName);
	mName.setLength(mName.length()-1);
	username_file.closeFile();
	system("rm /tmp/torturer/username_file");
	//system("rm username_file");

	OmnFile ip_file("/tmp/torturer/ip_file", OmnFile::eCreate AosMemoryCheckerArgs);
	system("ifconfig eth0 | grep inet | awk '{ print $2}' | awk -F: '{print $2}' >> /tmp/torturer/ip_file");
	//OmnFile ip_file("ip_file", OmnFile::eCreate AosMemoryCheckerArgs);
	//system("ifconfig eth0 | grep inet | awk '{ print $2}' | awk -F: '{print $2}' >> ip_file");
	ip_file.openFile(OmnFile::eReadOnly);
	ip_file.readAll(mOptHostName);
	mOptHostName.setLength(mOptHostName.length()-2);
	ip_file.closeFile();
	system("rm /tmp/torturer/ip_file");
	//system("rm ip_file");
	
	OmnFile port_file("/tmp/torturer/port_file", OmnFile::eCreate AosMemoryCheckerArgs);
	//OmnFile port_file("port_file", OmnFile::eCreate AosMemoryCheckerArgs);
	OmnString port_str = "";
	port_str << "cat /home/" << mName << "/Jimo_Servers0/port | grep front_port | cut -c 12-16 >> /tmp/torturer/port_file";
	//port_str << "cat port | grep front_port | cut -c 12-16 >> port_file";
	system(port_str.data());
	port_file.openFile(OmnFile::eReadOnly);
	OmnString port = "";
	port_file.readAll(port);
	port.setLength(port.length()-1);
	port_file.closeFile();
	system("rm /tmp/torturer/port_file");
	//system("rm port_file");

	mUserName = "root";
	mPassWd = "12345";
	mRemotePort = port.toU64(0);
//mRemotePort = 30782;
	mNumber = sgNumber;
	mNumber = 10;
	mTime = sgTime;
	mSeed = 1000;
	config();
}


AosSmartQueryTester::~AosSmartQueryTester()
{
}

bool      	
AosSmartQueryTester::start()
{	
	OmnScreen << " SmartQuery Test start ..." << endl;
	switch(1)
	{
		case 1:
			cout << "Step 1:  Create Data." << endl;
			createFields();
//			break;
		case 2:
			cout << "Step 2:  Load Data." << endl;
			loadData();
			//break;
			cout << "=============sleep==============" << endl;
			OmnSleep(20);
			cout << "=============weak==============" << endl;
		case 3:
			cout << "Step 3:  Query." << endl;
			if(mNumber>0)
			{
				for(int k=0; k<mNumber; k++)
				{
					cout << "============================query==:" << k << endl;
					query();
				}
			}
			else
			{
				time_t start_time = time(NULL);
				time_t end_time = start_time;
				time_t time_dur = 0;
				while(time_dur < mTime)
				{
					cout << "============================query time :" << time_dur << endl;
					query();
					end_time = time(NULL);
					time_dur = (end_time-start_time);
				}
			}
			break;
		default:
			break;
	}
	return true;
}

bool
AosSmartQueryTester::config()
{
	//init config_xml for fields
	AosXmlTagPtr config_xml;
	mNumFields = 0;
	OmnString str;
	str << "<Fields>" 
				"<Field>"
					"<action type=\"eSwitch\" pos1=\"8\" pos2=\"0\"/>"
					"<action type=\"eRotate\" pos1=\"2\"/>"
					"<action type=\"eAdd\" delta=\"431\"/>"
				"</Field>"
				"<Field>"
					"<action type=\"eSwitch\" pos1=\"9\" pos2=\"1\"/>"
					"<action type=\"eRotate\" pos1=\"4\"/>"
					"<action type=\"eAdd\" delta=\"2148\"/>"
				"</Field>"
				"<Field>"
					"<action type=\"eSwitch\" pos1=\"9\" pos2=\"0\"/>"
					"<action type=\"eRotate\" pos1=\"3\"/>"
					"<action type=\"eAdd\" delta=\"1363\"/>"
				"</Field>"
				"<Field>"
					"<action type=\"eSwitch\" pos1=\"10\" pos2=\"1\"/>"
					"<action type=\"eRotate\" pos1=\"3\"/>"
					"<action type=\"eAdd\" delta=\"1875\"/>"
				"</Field>"
				"<Field>"
					"<action type=\"eSwitch\" pos1=\"5\" pos2=\"0\"/>"
					"<action type=\"eRotate\" pos1=\"2\"/>"
					"<action type=\"eAdd\" delta=\"3126\"/>"
				"</Field>"
			"</Fields>";
	config_xml = AosXmlParser::parse(str AosMemoryCheckerArgs);
	//create fields
	AosXmlTagPtr field = config_xml->getFirstChild("Field");
	while(field)
	{
		mNumFields++;
		//OmnNew AosMCField();
//		AosMCFieldPtr cur = OmnNew AosMCField();
		AosMCFieldPtr cur = OmnNew AosMCField(field);
		mMCFields.push_back(cur);
		field = config_xml->getNextChild("Field");
	}
	return true;
}

bool
AosSmartQueryTester::createFields()
{
	u64 tid = 0;

	OmnString source_dir = "";
	source_dir << "mkdir /home/" << mName << "/Jimo_Servers0/source";
	system(source_dir.data());

	OmnString file1_str = "";
	file1_str << "/home/" << mName << "/Jimo_Servers0/source/MCFields.txt";
	OmnFile file1(file1_str.data(), OmnFile::eAppend AosMemoryCheckerArgs);

if(file1.isGood())
{
	file1.deleteFile();
}
	OmnFile file(file1_str.data(), OmnFile::eCreate AosMemoryCheckerArgs);
	if (!file.isGood())
	{
		OmnAlarm << "Failed to open the file: MCFields.txt" << enderr;
		return false;
	}
	int num_fields =  mMCFields.size();
	u64 j = 0;
	u64 rslt;
	for(u64 i=0; i<mNumRecords; i++)
	{
		j=0;
		for(; j<num_fields-1; j++)
		{
			rslt = mMCFields[j]->getValueByTID(tid);
			file.append(OmnStrUtil::ulltoa(rslt), false);
//			file.append(toString(rslt), false);
			file.append(OmnString(","), false);
		}
		rslt = mMCFields[j]->getValueByTID(tid);
		file.append(OmnStrUtil::ulltoa(rslt), false);
//		file.append(toString(rslt), false);
		file.append(OmnString("\n"), false);

		tid++;
	}

	mMaxTid = tid-1;
	
	file.closeFile();
	return true;
//create directory   sourceData
//	sourceData/data.txt
}


bool
AosSmartQueryTester::loadData()
{
//	system call :   ./jimodb restart;
//  native api or system call :   ./jimodb jql -l source "script.txt"

	//source file
	if((mConn = zykie_init(NULL))== NULL)
	{
		OmnScreen << "zykie init failed." << endl;
		return false;
	}
	if(zykie_connect(mConn, mOptHostName.data(), mRemotePort, mUserName.data(), mPassWd.data()))
	{
		OmnScreen << "zykie_connect success." << endl;
	}
	else
	{
		OmnScreen << "zykie_connect failed." << endl;
		zykie_close(mConn);
		return false;
	}
/*
	if(zykie_query(mConn, "source /home/liuwei/Jimo_Servers0/script.txt"))
	{
		OmnScreen << "source file success." << endl;
	}
*/
/*
	bool flag = true;
	while(flag){}
*/
	OmnString file1_str = "/home/";
	file1_str << mName << "/Jimo_Servers0/load_scripts";
	OmnFile file1(file1_str.data(), OmnFile::eReadOnly AosMemoryCheckerArgs);
	if(!file1.isGood())
	{
		OmnAlarm << "open script file failed" << enderr;
	}
	OmnString contents;
	bool rslt = file1.readAll(contents);
	aos_assert_r(rslt,false);
	
//	rslt = zykie_query(mConn, "create database db;");
//	rslt = zykie_query(mConn, "use db;");

	
	if(zykie_query(mConn, contents.data()))
	{
		OmnScreen << "create database success." << endl;
	}
	else
	{
		OmnScreen << "source file failed." << endl;
		return false;
	}

	aos_assert_r(zykie_query(mConn, "create table TEMP_ZW_0818_GJZ_4G1 using TEMP_ZW_0818_GJZ_4G1_parser;"), false);
	aos_assert_r(zykie_query(mConn, "create index index0 on table TEMP_ZW_0818_GJZ_4G1(field0);"), false);
	aos_assert_r(zykie_query(mConn, "create index index1 on table TEMP_ZW_0818_GJZ_4G1(field1);"), false);
	aos_assert_r(zykie_query(mConn, "create index index2 on table TEMP_ZW_0818_GJZ_4G1(field2);"), false);
	aos_assert_r(zykie_query(mConn, "create index index3 on table TEMP_ZW_0818_GJZ_4G1(field3);"), false);
	aos_assert_r(zykie_query(mConn, "create index index4 on table TEMP_ZW_0818_GJZ_4G1(field4);"), false);
	OmnString job_str;
	job_str << "create job job2(job2) "
			<< "begin "
			<< "dataproc index TEMP_ZW1_idx"
			<< "("
			<< "	inputs: TEMP_ZW_0818_GJZ_4G1_dataset,"
			<< "	table: TEMP_ZW_0818_GJZ_4G1,"
			<< "	indexes: (index0,index1,index2,index3,index4)"
			<< ");"
			<< "dataproc doc doc_TEMP_ZW1"
			<< "("
			<< "	inputs: TEMP_ZW_0818_GJZ_4G1_dataset,"
			<< "	format:TEMP_ZW_0818_GJZ_4G1_parser,"
			<< "	table: TEMP_ZW_0818_GJZ_4G1"
			<< ");"
			<< "end;";
	aos_assert_r(zykie_query(mConn, job_str.data()), false);
	aos_assert_r(zykie_query(mConn, "run job job2 (threads:6);"), false);

	return true;
}

bool
AosSmartQueryTester::query()
{
	//1.set query conditions
	//2.decide rever or nor
	//3.choose sort field
	//4.decide query to the end or not
	//		if not,how many records we need to query
	//5.moveto
	//		do we need to moveto
	//		do we check moveto range
	//6.set query conditions into sql
	//7.query loop
	//8.check rslt
	//
	//rand select fields
	mQueryFlag = OmnNew bool[mNumFields];//表示是否添加query条件
	memset(mQueryFlag, 0, mNumFields);
	u64 pos;
	
	bool exist_cond = false;
	while(!exist_cond)
	{
		for(u64 i=0; i<mNumFields; i++)
		{
			if(percent(50))
			{
				continue;
			}
			srand(mSeed++);
			pos = rand()%(mNumFields-1);
			mQueryFlag[pos] = true;
			exist_cond = true;
		}
	}

	//getQueryOpr
	//getQueryValue
	//
	for(u64 i=0; i<mNumFields; i++)
	{
		if(mQueryFlag[i])
		{
			mMCFields[i]->setQueryConds();
		}
	}
	// decide reverse or not
	//bool reverse = rand()%2;

	// choose a field as sorting field
	srand(mSeed++);
	u64 sort_field = rand()%mNumFields;	//表示排序的域的序号
	// make desicion whether we query till the end or not
	// if not , how many records(roughly) we need to query
	bool query_toend = percent(5); //表示是否 query to end
	//u64 query_count = randQueryCount();
	// do we need to moveto?
	bool moveto = percent(20);
	u64 moveto_step = randMoveToStep();

	// do we check moveto range?
	bool moveto_check = percent(20);
	
	//set query conditions into a sql
	//
	OmnString* field_name = OmnNew OmnString[mNumFields];
	for(u64 i=0;i<mNumFields;i++)
	{
		field_name[i] << "field" << i;
	}
	OmnString sql_str;
	sql_str << "select ";
	for(u64 i=0; i<mNumFields-1; i++)
	{
		sql_str << field_name[i] << ",";
	}
	sql_str << field_name[mNumFields-1] << " from " << mTableName << " where ";

	bool first_time_flag = true;//表示是否第一次拼接条件字符串，若是第一次，则不加and
	for(u64 i=0; i<mNumFields; i++)
	{
		if(mQueryFlag[i])
		{
			if(first_time_flag)
			{
				first_time_flag = false;
			}
			else
			{
				sql_str << " and ";
			}
			sql_str << mMCFields[i]->getWhereStr(field_name[i]); 
		}
	}

	sql_str << " order by " << field_name[sort_field];
	u64 limit_start = 0;
	if(moveto)
	{
		limit_start = moveto_step;
	}

	u64 max_rslt = 5000;
	u64 page_size;
	bool finished = false;
	bool rslt;
	u64* field_rslt = OmnNew u64[mNumFields];
	u64 sort_field_rslt;//排序的域的值
	u64	sort_field_start;
	u64 sort_field_end;
	u64 tid;
	u64 tmp_tid;
	u64 num_check;
		
	if((mConn = zykie_init(NULL))== NULL)
	{
		OmnScreen << "zykie init failed." << endl;
	}
	if(zykie_connect(mConn, mOptHostName.data(),mRemotePort, mUserName.data(), mPassWd.data()))
	{
		OmnScreen << "zykie_connect success." << endl;
	}
	else
	{
		OmnScreen << "zykie_connect failed." << endl;
		zykie_close(mConn);
	}
	
	zykie_query(mConn, "use db;");

	mMaxTid = mNumRecords-1;
	mBitmap = OmnNew AosBitmap();
	while(!finished)
	{
		//random pageSize
		//
		// some chance to set pagesize to 0, if so , we know that we query until the end
		//
		OmnString tmp_str="";
		tmp_str = sql_str;
		
		page_size = randPageSize();
		
		if(!page_size)
		{
			query_toend = true;
			tmp_str << " limit " << mNumRecords; 
		}
		else
		{
			// set pagesize, start pos into sql
			tmp_str << " limit " <<" "<< limit_start <<", "<< page_size;
		}
		//
		//query
		//
		//jimo_real_connect();
		//jimo_init();
		//jimo_query(sql_str);
		//jimo_store_result();
		//
		tmp_str<<";";
		cout << tmp_str << endl;
/*
		if(zykie_query(mConn,"select * from TEMP_ZW_0818_GJZ_4G1;"))
		{
			mResSet = zykie_store_result(mConn);	
		}
		while((mRow = zykie_fetch_row(mResSet)) != NULL)
		{
			OmnScreen << "=================rslt=============" << endl;
		}
*/
		if(zykie_query(mConn, tmp_str.data()))
		{
			mResSet = zykie_store_result(mConn);
			if(mResSet == NULL)
			{	
				OmnAlarm << "zykie_store_result failed." << enderr;
			}
		}

		//rslt_size = jimo_num_rows();
		//
		//get fields value from rslt
		//check sorting field order ??
		//get tid by revAction
		//check tid in range
		//put tid into bitmap
		//check other field match tid
		//check all the fields are in the range or not
		bool first_row_flag = true;
		u64 rslt_size=0;

		while((mRow = zykie_fetch_row(mResSet)) != NULL)
		{
		//	get field rslt from rslt, put into field_rslt[];
			if(first_row_flag)
			{
				sort_field_rslt = charToU64(mRow[sort_field]);
				sort_field_start = charToU64(mRow[sort_field]);
				first_row_flag = false;
			}
			if(sort_field_rslt<=charToU64(mRow[sort_field]))
			{
				sort_field_rslt = charToU64(mRow[sort_field]);
				sort_field_end = charToU64(mRow[sort_field]);
			}
			else
			{
				OmnAlarm << "sort error" << enderr;
			}
		
			tid = mMCFields[0]->getTIDByValue(charToU64(mRow[0]));
			if(!inRange(tid))
			{
				OmnAlarm << "tid not in range" << enderr;
			}
			else
			{
				mBitmap->appendDocid(tid);
			}
			for(u64 j=0; j<mNumFields; j++)
			{
				if(mQueryFlag[j])
				{
					tmp_tid = mMCFields[j]->getTIDByValue(charToU64(mRow[j]));
					aos_assert_r(tid == tmp_tid,false);
					aos_assert_r(mMCFields[j]->matchCond(charToU64(mRow[j])),false);
				}
			}
			rslt_size++;
		}

		//judge whether we need finish
		if(first_row_flag)
		{
			query_toend = true;
			finished = true;
		}

		if(page_size)
		{
			aos_assert_r(rslt_size<=page_size,false);
		}
		if(rslt_size<page_size)
		{
			query_toend = true;
			finished = true;
		}
				
		limit_start += rslt_size;
		if(limit_start > max_rslt)
		{
			finished = true;
		}

		if(query_toend)
		{
			//1. check all
			//2. check some
			// in tid range pick one
			// if tid in bitmap, ignore
			// if not, judge if it match conds
			// not match, do nothing
			// match , alarm

			if(percent(20))
			{
				for(u64 i=0;i<mMaxTid;i++)
				{
					if(!mBitmap->checkDoc(i))
					{
						rslt = matchFieldConds(i);
						if(rslt)
						{
							aos_assert_r(moveto, false);
							aos_assert_r(page_size, false);
							u64 sort_field_value = mMCFields[sort_field]->getValueByTID(i);
							aos_assert_r(sort_field_value<=sort_field_start||sort_field_value>=sort_field_end,false);
						}
					}
				}
			}
			else
			{
				num_check = randCheckNum();
				for(u64 i=0;i<num_check;i++)
				{
					srand(mSeed++);
					tmp_tid = rand()%mMaxTid;
					if(!mBitmap->checkDoc(tmp_tid))
					{
						rslt = matchFieldConds(tmp_tid);
						if(rslt)
						{
							aos_assert_r(moveto, false);
							u64 sort_field_value = mMCFields[sort_field]->getValueByTID(i);
							aos_assert_r(sort_field_value<sort_field_start,false);
						}
					}
				}
			}

		}
		else// not all queried
		{
			// in tid range pick one
			// if tid in bitmap, ignore
			// if not, judge if it match conds
			// not match, do nothing
			// match , if sorting field in querystart-queryend, alarm
			// > queryend, do nothing
			// < querystart     if moveto, ok
			// if not moveto , alarm
			//

			num_check = randCheckNum();
			for(u64 i=0;i<num_check;i++)
			{
				srand(mSeed++);
				tmp_tid = rand()%mMaxTid;
				if(!mBitmap->checkDoc(tmp_tid))
				{
					rslt = matchFieldConds(tmp_tid);
					if(rslt)
					{
						u64 sort_field_value = mMCFields[sort_field]->getValueByTID(tmp_tid);
						if(sort_field_start<=sort_field_value&&sort_field_value<=sort_field_end)
						{
							OmnAlarm << "there is record match cond but not queried" << enderr;
						}
/*
						if(sort_field_value<sort_field_start || sort_field_value>sort_field_end)
						{
							if(!moveto)
							{
								OmnAlarm << "there is record match cond but not queried" << enderr;
							}
						}
*/
					}
				}
			}
		}

	}
	
	
//?	moveto check:
	// find all matched records witch not in bitmap, smaller than start, count, compare with moveto num
/*
	bool match_cond;
	if(moveto_check)
	{
		u64 num_before_start = 0;
		for(int i=0; i<mMaxTid; i++)
		{
			match_cond = matchFieldConds(i);
			if(!mBitmap->checkDoc(i) && match_cond && mMCFields[sort_field]->getValueByTID(i)<=sort_field_start)
			{
				num_before_start++;
			}
		}
		if(num_before_start>moveto_step)
		{
			OmnAlarm << "move to error" << enderr;
		}
	}
*/
	return true;
}


u64
AosSmartQueryTester::getValue()
{
	d64 aa;
	srand(mSeed++);
	aa = ((((u64)rand())<<33) |(((u64)rand()) << 2) | (((u64)rand()) >> 29));
	return aa;
}

OmnString
AosSmartQueryTester::toString(u64 orig_val)
{
	OmnString orig_str;
	orig_str << orig_val;
	OmnString rslt_str;
	rslt_str.setLength(20);
	u64 n = orig_str.length();
	rslt_str.replace(0, 20, mZeroStr);
	rslt_str.replace(20-n, n, orig_str);

	//strcpy(rslt_str, mZeroStr);
	//strcpy(rslt_str+(20-n), orig_str);
	
	return rslt_str;
}

u64
AosSmartQueryTester::randPageSize()
{
	srand(mSeed++);
	u64 s = rand()%100;
	if(s>=90) return 0;
	if(s>=80) return mNumRecords/10;
	if(s>=75) 
	{
		srand(mSeed++);
		return mNumRecords+rand()%mNumRecords; 
	}
	srand(mSeed++);
	return rand()%mNumRecords;
}

bool
AosSmartQueryTester::percent(const u64 percent)
{
	srand(mSeed++);
	u64 i = rand()%100;
	return i < percent;
}

u64
AosSmartQueryTester::randQueryCount()
{
	u64 rslt;
	if(percent(80))
	{
		srand(mSeed++);
		rslt = rand()%(mNumRecords/2);
	}
	else
	{
		srand(mSeed++);
		rslt = mNumRecords/2 + rand()%(mNumRecords/2);
	}
	return rslt;
}	

u64 
AosSmartQueryTester::randMoveToStep()
{
	u64 rslt;
	if(percent(80))
	{
		srand(mSeed++);
		rslt = rand()%(mNumRecords/3);
	}
	else
	{
		srand(mSeed++);
		rslt = mNumRecords/3 + rand()%(2*mNumRecords/3);
	}
	return rslt;
}

bool
AosSmartQueryTester::matchFieldConds(const u64 tid)
{
	bool rslt = true;
	for(u64 i=0;i<mNumFields;i++)
	{
		if(mQueryFlag[i])
		{
			rslt &= mMCFields[i]->matchCond(mMCFields[i]->getValueByTID(tid));
		}
	}
	return rslt;
}

u64
AosSmartQueryTester::randCheckNum()
{
	u64 rslt;
	if(percent(80))
	{
		srand(mSeed++);
		rslt = rand()%(mNumRecords/10);
	}
	else
	{
		srand(mSeed++);
		rslt = mNumRecords/10 + rand()%(1*mNumRecords/10);
	}
	return rslt;
}

bool
AosSmartQueryTester::inRange(const u64 tid)
{
	return tid>=0&&tid<=mMaxTid;
}

u64
AosSmartQueryTester::charToU64(char * c)
{
	return OmnString(c).toU64(0);
}

bool			
AosSmartQueryTester::resp(const AosXmlTagPtr &rslt)
{
	//1. clone rslt to local
	//2. signal
	mCondVar->signal();
	mLock->unlock();
	return true;
}

