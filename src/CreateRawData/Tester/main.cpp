////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

          
#include "Alarm/AlarmMgr.h"
#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h" 
#include "NMS/HouseKp.h"
#include "NMS/Ptrs.h"
#include "Porting/GetTime.h"
#include "Rundata/Rundata.h"
#include "Random/RandomUtil.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Timer/Timer.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/CompareFun.h"
#include "Util/BuffArray.h"
#include "Util/Tester/ValueRsltTester.h"
#include "XmlParser/XmlItem.h"
#include <vector>
#include <algorithm>
#include <functional>
#include <iostream>
#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/date_time/local_time/local_time.hpp>
#include "boost/date_time/gregorian/gregorian.hpp"
#include <dirent.h>
#include "Porting/Sleep.h"
using namespace std;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::local_time;
  
int getDataLen(OmnString type);
bool filterData(char * data);
 


class DayFiles : virtual public OmnThreadedObj
{
	struct FieldData
	{
		int			year;
		int 		month;
		int 		day;
	};

	OmnDefineRCObject;
	OmnString 		mDirName;
	bool			mInc;
	int				mRate;
	AosBuffPtr 		mBuff;
	char* 			mCrtPtr;
	int 			mCrtSize;
	OmnFilePtr		mFile;
	int 			mNumFiles;
	struct FieldData mFieldData;

private:
	DayFiles *				mDayFiles;
	int						mId;
public:
	static queue<pair<OmnFilePtr, AosBuffPtr> > smQueue;
	static OmnMutexPtr  	smLock;
	static OmnCondVarPtr    smCondVar;
	static bool				smReadyClose;
	static bool				smFinished;
	// ThreadedObj Interface
	virtual bool        threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
	{
		while (state == OmnThrdStatus::eActive)
		{
			smLock->lock();

			if (smQueue.size() <= 0)
			{
				if(smReadyClose)
				{
					smFinished = true;
		OmnScreen << "----------- finished! ------------" << endl;
				}
				smCondVar->wait(smLock);
				smLock->unlock();
				continue;
			}

			pair<OmnFilePtr, AosBuffPtr> e = smQueue.front();
			smQueue.pop();
			smLock->unlock();
			e.first->append(e.second->data(), e.second->dataLen());
		}
		return true;
	}
	virtual bool        signal(const int threadLogicId)
	{
		smCondVar->signal();
		return true;
	}
	virtual bool        checkThread(OmnString &err, const int thrdLogicId) const
	{
		return true;
	}

public:
	DayFiles(const OmnString &dirname,  int id)
	:
	mDirName(dirname),
	mInc(rand()%2 == 1),
	mRate(rand()%20),
	mBuff(OmnNew AosBuff(5000000 AosMemoryCheckerArgs)),
	mCrtPtr(mBuff->data()),
	mCrtSize(0),
	mNumFiles(0),
	mId(id)
	{
		DIR *dir;
		struct dirent *file;

		if((dir = opendir(mDirName.data())) == NULL)
		{
		    mkdir(mDirName.data(), 00755);
		}

		OmnString fname = mDirName;
		fname << "/Data_";
		fname << mNumFiles++;
		mFile = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);
		closedir(dir);
		//get year month day
		std::string time(mDirName.data());
		ptime pt(not_a_date_time);
		std::stringstream ss;
		local_time_input_facet* input_facet = OmnNew local_time_input_facet();
		ss.imbue(locale(ss.getloc(), input_facet));
		OmnString formatStr;
		formatStr << "%Y%m%d%H%M%S";
		input_facet->format(formatStr.data());
		ss.str(time);
		ss >> pt;
		if (pt.is_not_a_date_time()) 
		{
			OmnAlarm << "The time format is wrong." << mDirName <<"  "<< formatStr<< enderr;
		}
		date day = pt.date();
		date::ymd_type timeYmd = day.year_month_day();
		mFieldData.year = timeYmd.year;
		mFieldData.month = timeYmd.month; 
		mFieldData.day = timeYmd.day;
		//new thread

	}

	static bool hasFinished()
	{
		return smFinished;
	}

	static void close()
	{
		smReadyClose = true;
	}

	bool addToQueue(AosBuffPtr &buff, OmnFilePtr &file)
	{
		smLock->lock();
		while(smQueue.size()> 500)
		{
			smLock->unlock();
			OmnSleep(1);
		}

		smQueue.push(make_pair(file, buff));
		smCondVar->signal();
		smLock->unlock();
	}

	void saveTailBuff()
	{
		mFile->append(mBuff->data(), mCrtPtr-mBuff->data());
	}

	bool appendData(char *data, int len)
	{
		
		if (mCrtPtr-mBuff->data() + 2*len > 5000000)
		{
			if (mCrtSize > 400000000)
			{
OmnScreen << "close file, file size : " << mCrtSize << ", id:" <<  mId << ", mDate:" << mDirName << endl;
				OmnString fname = mDirName;
				fname << "/Data_";
				fname << mNumFiles++;
				mFile = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);
				mCrtSize = 0;
			}
OmnScreen << "append to file: file size : " << mCrtPtr-mBuff->data() << ", id:" <<  mId << ", mDate:" << mDirName << endl;
			mBuff->setDataLen(mCrtPtr-mBuff->data());
			addToQueue(mBuff, mFile);
			mBuff->reset();
			mBuff = OmnNew AosBuff(5000000 AosMemoryCheckerArgs);
			mCrtPtr = mBuff->data();
		}
//add random record, modify the date of the record, and save the record into this buff, if this buff is >5000000,
//then save to one file, when the size of current file is >400000000, then close current file and new one file, 
//and save buff to ne file


		if (mInc)
		{
			int rdm = rand() % 100;
			if(rdm < mRate)
			{
				memcpy(mCrtPtr, data, len);
				bool rslt = modifyRecord(mCrtPtr, len);
				aos_assert_r(rslt, false);
				mCrtPtr += len;
			}
			memcpy(mCrtPtr, data, len);
			bool rslt = modifyRecord(mCrtPtr, len);
			aos_assert_r(rslt, false);
			mCrtPtr += len;
			if(rdm < mRate)
			{
				mCrtSize += len*2;
			}
			else
			{
				mCrtSize += len;
			}
		}
		else
		{
			int rdm = rand() % 100;
			if(rdm < 100-mRate)
			{
				memcpy(mCrtPtr, data, len);
				bool rslt = modifyRecord(mCrtPtr, len);
				aos_assert_r(rslt, false);
				mCrtPtr += len;
				mCrtSize += len;
			}
		}

		return true;
	}

	bool
	modifyRecord(char *data, int len)
	{
		return modifyStartTime(data);
	}

	bool isDayOverFlow(int year, int month, int day, int &lastday)
	{
		static int lastDayOfMonth [] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
		if (month == 2 && (((year % 4 == 0) && (year%100 !=0)) || (year%400 == 0)))
		{
			lastday = 28;
			return day >28;	
		}
		if (day > lastDayOfMonth[month-1])
		{
			lastday = lastDayOfMonth[month-1];
			return true;
		}
		return false;

	}

	bool
	modifyStartTime(char *record)
	{
		OmnString type(&record[0], 2);
		int startTimePosition;
		startTimePosition = getSTPosition(type);
		aos_assert_r(startTimePosition >=0, false);


		int m = rand() % 30;
		int s = rand() % 60;
		int alg = rand() % 2;

		char *year = record + startTimePosition;
		char *month = record + startTimePosition + 4;
		char *day = record + startTimePosition + 6;
		char *hour = record + startTimePosition + 8;
		char *minute = record + startTimePosition + 10;
		char *second = record + startTimePosition + 12;

alg = 0;
		if(alg == 1)
		{
			bool needInc = false;

			char c = second[2];
			second[2] = 0;
			int value = atoi(second);
			value += s;
			if (value >= 60)
			{
				needInc = true;
				value = value - 60;
			}
			sprintf(second, "%02d", value);
			second[2] = c;

			c = minute[2];
			minute[2] = 0;
			value = atoi(minute);
			if(needInc)
			{
				value += m + 1;
			}
			else
			{
				value += m;
			}
			if (value >= 60)
			{
				needInc = true;
				value = value - 60;
			}
			needInc = false;
			sprintf(minute, "%02d", value);
			minute[2] = c;

			c = hour[2];
			hour[2] = 0;
			value = atoi(hour);
			if(needInc)
			{
				value += 1;
			}
			if (value >= 24)
			{
				needInc = true;
				value = value - 24;
			}
			needInc = false;
			sprintf(hour, "%02d", value);
			hour[2] = c;

			c = day[2];
			day[2] = 0;
			value = mFieldData.day;
			if(needInc)
			{
				value += 1;
			}
			int lastday;
			if(isDayOverFlow(mFieldData.year, mFieldData.month, mFieldData.day, lastday))
			{
				needInc = true;
				value = value - lastday;
			}
			needInc = false;
			sprintf(day, "%02d", value);
			day[2] = c;

			c = month[2];
			month[2] = 0;
			value = mFieldData.month;
			if(needInc)
			{
				value += 1;
			}
			if (value >= 12)
			{
				needInc = true;
				value = value - 12;
			}
			needInc = false;
			sprintf(month, "%02d", value);
			month[2] = c;

			c = year[4];
			year[4] = 0;
			value = mFieldData.year;
			if(needInc)
			{
				value += 1;
			}
			needInc = false;
			sprintf(year, "%04d", value);
			year[4] = c;
		}
		else
		{
			bool needDesc = false;

			char c = second[2];
			second[2] = 0;
			int value = atoi(second);
			value -= s;
			if (value < 0)
			{
				needDesc = true;
				value = value + 60;
			}
			sprintf(second, "%02d", value);
			second[2] = c;

			c = minute[2];
			minute[2] = 0;
			value = atoi(minute);
			if(needDesc)
			{
				value -= m - 1;
			}
			else
			{
				value -= m;
			}
			if (value < 0)
			{
				needDesc = true;
				value = value + 60;
			}
			needDesc = false;
			sprintf(minute, "%02d", value);
			minute[2] = c;

			c = hour[2];
			hour[2] = 0;
			value = atoi(hour);
			if(needDesc)
			{
				value -= 1;
			}
			if (value < 0)
			{
				needDesc = true;
				value = value + 24;
			}
			needDesc = false;
			sprintf(hour, "%02d", value);
			hour[2] = c;

			c = day[2];
			day[2] = 0;
			value = mFieldData.day;
			if(needDesc)
			{
				value -= 1;
			}
			int lastday;
			if(isDayOverFlow(mFieldData.year, mFieldData.month, mFieldData.day, lastday))
			{
				needDesc = true;
				value = value + lastday;
			}
			needDesc = false;
			sprintf(day, "%02d", value);
			day[2] = c;

			c = month[2];
			month[2] = 0;
			value = mFieldData.month;
			if(needDesc)
			{
				value -= 1;
			}
			if (value < 0)
			{
				needDesc = true;
				value = value + 12;
			}
			needDesc = false;
			sprintf(month, "%02d", value);
			month[2] = c;

			c = year[4];
			year[4] = 0;
			value = mFieldData.year;
			if(needDesc)
			{
				value -= 1;
			}
			needDesc = false;
			sprintf(year, "%04d", value);
			year[4] = c;
		}

		return true;

	}

	int
	getSTPosition(OmnString type)
	{
		if(type == "ac") return 80;
		if(type == "br") return 46;
		if(type == "de") return 69;
		if(type == "hl") return 134;
		if(type == "ib") return 43;
		if(type == "id") return 58;
		if(type == "iy") return 45;
		if(type == "ml") return 108;
		if(type == "ph") return 80;
		if(type == "pt") return 58;
		if(type == "sg") return 108;
		if(type == "sr") return 108;
		if(type == "ss") return 93;
		if(type == "vc") return 80;
		if(type == "ve") return 69;
		if(type == "vf") return 80;
		if(type == "vh") return 80;
		if(type == "vn") return 80;
		if(type == "vr") return 80;
		if(type == "vs") return 80;
		if(type == "vt") return 80;
		if(type == "bj") return 46;
		if(type == "cr") return 66;
		if(type == "cw") return 66;
		if(type == "gg") return 55;
		if(type == "gn") return 55;
		if(type == "hp") return 134;
		if(type == "hm") return 136;
		if(type == "ia") return 85;
		if(type == "ig") return 37;
		if(type == "ij") return 37;
		if(type == "im") return 49;
		if(type == "ir") return 51;
		if(type == "mm") return 91;
		if(type == "nd") return 166;
		if(type == "on") return 39;
		if(type == "pg") return 80;
		if(type == "pr") return 50;
		if(type == "vb") return 80;
		if(type == "vd") return 80;
		if(type == "vi") return 69;
		if(type == "vv") return 45;
		if(type == "vw") return 80;
		if(type == "wa") return 39;
		if(type == "wn") return 39;
		if(type == "vj") return 80;
		return -1;
	}

};

queue<pair<OmnFilePtr, AosBuffPtr> >  DayFiles::smQueue;
OmnMutexPtr  	DayFiles::smLock = OmnNew OmnMutex();
OmnCondVarPtr    DayFiles::smCondVar = OmnNew OmnCondVar();
bool DayFiles::smReadyClose = false;
bool DayFiles::smFinished = false;

bool proc(const OmnString &dirname, vector<DayFiles*> &v);
bool readOneFile(const OmnString &sourcefilename, vector<DayFiles*> &v);
bool procData(const AosBuffPtr &buff, vector<DayFiles*> &v);

int 
main(int argc, char **argv)
{
	OmnApp theApp(argc, argv);

	int index = 1;
	OmnString sourcefilename;
	OmnString startday;
	OmnString endday;
	while (index < argc)
	{
		if (strcmp(argv[index], "-sourcefile") == 0 && index < argc-1)
		{
			sourcefilename << argv[index+1]; 	
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-startday") == 0 && index < argc-1)
		{
			startday  << argv[index+1]; 	
			index += 2;
			continue;
		}
		
		if (strcmp(argv[index], "-endday") == 0 && index < argc-1)
		{
			endday << argv[index+1]; 	
			index += 2;
			continue;
		}
		index++;
	}
	std::string s1(startday.data()); 
	date d1(from_undelimited_string(s1));

	std::string s2(endday.data()); 
	date d2(from_undelimited_string(s2));

	aos_assert_r(d1 <= d2, false);

	vector<DayFiles*> v;
	int id_index = 0;
	DayFiles *file;
	while(d1 <= d2)	
	{
		id_index++;
		file = OmnNew DayFiles(to_iso_string(d1), id_index);		
		v.push_back(file);
		d1 = d1 + days(1);
	}
	OmnThreadedObjPtr thisPtr(file, false);
	OmnThreadPtr thread = OmnNew OmnThread(thisPtr, "create_raw_data", 0, true, true, __FILE__, __LINE__);
	thread->start();

	proc(sourcefilename, v);

	DayFiles::close();

	while (1)
	{
		OmnSleep(1);
		if(DayFiles::hasFinished())
		{
			for(int i=0; i<v.size(); i++)
			{
				v[i]->saveTailBuff();
			}
			break;
		}
	}

	theApp.exitApp();
	OmnScreen << "exit now ! ---------" << endl;
	return 0;
	
}


bool proc(const OmnString &dirname, vector<DayFiles*> &v)
{
	DIR *dir;
	struct dirent *file;

	if((dir = opendir(dirname.data())) == NULL)
	{
		OmnAlarm << "no dir" << enderr;
		return false;
	}

	while((file = readdir(dir)) != NULL)
	{
		if(strncmp(file->d_name, ".", 1) == 0)  continue;

		OmnString fileName = dirname;
		fileName << "/" << file->d_name;
		readOneFile(fileName, v);
	}
	closedir(dir);
}

bool readOneFile(const OmnString &sourcefilename, vector<DayFiles*> &v)
{
	const char * scfile = sourcefilename.data();
	OmnFilePtr sourcefile = OmnNew OmnFile(scfile, OmnFile::eReadOnly AosMemoryCheckerArgs);;
	aos_assert_r(sourcefile->isGood(), false);

	u64 pos = 0;
	const int memsize = 5000000;
	AosBuffPtr  buff = OmnNew AosBuff(memsize AosMemoryCheckerArgs);
	char *data = buff->data();
	while(1)
	{
		int bytesread = sourcefile->readToBuff(pos, memsize, data);
		aos_assert_r(bytesread > 0, false);
		int p = bytesread;
		while(1)
		{
			aos_assert_r(p >= 1, false);
			if (buff->data()[p-1] == 0x0a) 
				break;
			p--;
		}
		buff->setDataLen(p);
		pos += p;	
		procData(buff, v);	
		if (bytesread != memsize)
			break;
	}
	sourcefile->closeFile();
}

bool procData(const AosBuffPtr &buff, vector<DayFiles*> &v)
{
	char *data = buff->data();
	while(data < buff->data()+buff->dataLen())
	{
		int p = 0;
		while(1)
		{
			if (data[p] == 0x0a) 
			{	p++;
				break;
			}
			p++;
		}

		if(filterData(data))
		{
			data += p;
			continue;
		}

		for (int i=0; i<v.size(); i++)
		{
//			v[i]->addDataToQueue(data, p);
			v[i]->appendData(data, p);
		}
		data += p;
	}
	return true;
}

bool
filterData(char * data)
{
	OmnString type(data, 2);
	if( type == "ac" || type == "br" || type == "de" || type == "hl" || type == "ib" || type == "id" || type == "iy" || type == "ml" ||
		type == "ph" || type == "pt" || type == "sg" || type == "sr" || type == "ss" || type == "vc" || type == "ve" || type == "vf" ||
		type == "vh" || type == "vn" || type == "vr" || type == "vs" || type == "vt" || type == "bj" || type == "cr" || type == "cw" ||
		type == "gg" || type == "gn" || type == "hp" || type == "hm" || type == "ia" || type == "ig" || type == "ij" || type == "im" ||
		type == "ir" || type == "mm" || type == "nd" || type == "on" || type == "pg" || type == "pr" || type == "vb" || type == "vd" ||
		type == "vi" || type == "vv" || type == "vw" || type == "wa" || type == "wn" || type == "vj")
	{
		return false;
	}
	return true;
}


int
getDataLen(OmnString type)
{
	if(type == "ac") return 795;
	if(type == "br") return 537;
	if(type == "de") return 645;
	if(type == "hl") return 666;
	if(type == "ib") return 482;
	if(type == "id") return 349;
	if(type == "iy") return 521;
	if(type == "ml") return 624;
	if(type == "ph") return 611;
	if(type == "pt") return 362;
	if(type == "sg") return 478;
	if(type == "sr") return 452;
	if(type == "ss") return 443;
	if(type == "vc") return 792;
	if(type == "ve") return 645;
	if(type == "vf") return 792;
	if(type == "vh") return 792;
	if(type == "vn") return 792;
	if(type == "vr") return 792;
	if(type == "vs") return 792;
	if(type == "vt") return 792;
	
	return -1;
}

