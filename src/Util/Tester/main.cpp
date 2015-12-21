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

#include "Tester/TestMgr.h"
          
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
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Timer/Timer.h"
#include "Util/Tester/UtilTestSuite.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/ExeReport.h"
#include "Util/MemoryChecker.h"
#include "Util/CompareFun.h"
#include "Util/BuffArray.h"
#include "Util/Tester/ValueRsltTester.h"
#include "XmlParser/XmlItem.h"
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <iostream>
#include "DataRecord/RecordFixBin.h"
#include "DataField/DataFieldBool.h"
#include "DataField/DataFieldStr.h"
using namespace std;
int gAosShowNetTraffic = 1;
  
 
int test111(const bool dd)
{
	cout << dd << endl;
	return -1;
}

int test();


bool less_str(const OmnString &left, const OmnString &right)
{
	int len1 = left.length();
	int len2 = right.length();
	if (len1 < len2) return true;
	if (len1 > len2) return false;
	if (left < right) return true;
	return false;
}

bool less_char(const char *left, const char *right)
{
	int len1 = strlen(left);
	int len2 = strlen(right);
	if (len1 < len2) return true;
	if (len1 > len2) return false;
	if (strcmp(left, right) < 0) return true;
	return false;
}

bool less_second(const u32 left, const u32 right)
{
	return left > right;
}

bool testLowerboundOmnString()
{
	vector<OmnString> vw;
	OmnString aa = "aaaaaa";
	OmnString bb = "bbbbb";
	OmnString cc = "cccc";
	vw.push_back(aa);
	vw.push_back(bb);
	vw.push_back(cc);

	sort(vw.begin(), vw.end(), less_str);
	vector<OmnString>::iterator itr = vw.begin();
	for (; itr != vw.end(); itr++)
	{
		cout << "Array: " << (*itr).data() << endl;
	}
	typedef vector<OmnString>::iterator VWIter;
	VWIter p = lower_bound(vw.begin(), vw.end(), aa, less_str);
	if (p != vw.end()) cout << "To add: " << aa.data() << ":" << (*p).data() << endl;

	p = lower_bound(vw.begin(), vw.end(), bb, less_str);
	if (p != vw.end()) cout << "To add: " << bb.data() << ":" << (*p).data() << endl;

	p = lower_bound(vw.begin(), vw.end(), cc, less_str);
	if (p != vw.end()) cout << "To add: " << cc.data() << ":" << (*p).data() << endl;

	return true;
}

bool testLowerboundChar()
{
	vector<char *> vw;
	char aa[20];
	char bb[20];
	char cc[20];
	strcpy(aa, "aaaaaa");
	strcpy(bb, "bbbbb");
	strcpy(cc, "cccc");
	vw.push_back(aa);
	vw.push_back(bb);
	vw.push_back(cc);

	sort(vw.begin(), vw.end(), less_str);
	vector<char *>::iterator itr = vw.begin();
	for (; itr != vw.end(); itr++)
	{
		cout << "Array: " << (*itr) << endl;
	}
	typedef vector<char *>::iterator VWIter;
	VWIter p = lower_bound(vw.begin(), vw.end(), aa, less_char);
	if (p != vw.end()) cout << "To add: " << aa << ":" << (*p) << endl;

	p = lower_bound(vw.begin(), vw.end(), bb, less_char);
	if (p != vw.end()) cout << "To add: " << bb << ":" << (*p) << endl;

	p = lower_bound(vw.begin(), vw.end(), cc, less_char);
	if (p != vw.end()) cout << "To add: " << cc << ":" << (*p) << endl;

	return true;
}

struct less_mag : public binary_function<u32, u32, bool> 
{
	less_mag(const int gg) : ttt(gg){}
	int ttt;

	bool operator()(u32 x, u32 y) 
	{ 
		cout << "ttt: " << ttt << endl;
		return x < y; 
	}
};

bool testSort()
{
	u32 aa[10] = {6, 2, 4, 100, 64, 79, 22, 9203, 1634, 333};
	// vector<u32> vec(&aa[0], &aa[10]);

	// sort(vec.begin(), vec.end(), less_mag(100));
	sort(aa, aa+10, less_mag(100));
	// sort(vec.begin(), vec.end(), less_second);
	// sort(vec.begin(), vec.end(), less<u32>());
	// sort(vec.begin(), vec.end());
	// vector<u32>::iterator itr = vec.begin();
	// for (; itr != vec.end(); itr++)
	// {
	// 	cout << "Item: " << *itr << endl;
	// }
	return true;
}


bool testBuffArray()
{
	/*
	AosCompareFunPtr cmp = new AosFunStrU641(10, true);
	AosRundataPtr rdata = OmnApp::getRundata();
	AosBuffArray array(cmp, false);
	array.addValue("a99999", 6, rdata);
	array.addValue("a88888", 6, rdata);
	array.addValue("a77777", 6, rdata);
	array.addValue("a66666", 6, rdata);
	array.addValue("a55555", 6, rdata);
	array.addValue("a44444", 6, rdata);
	array.addValue("a33333", 6, rdata);
	array.addValue("a22222", 6, rdata);
	array.addValue("a11111", 6, rdata);
	array.addValue("a00000", 6, rdata);

	array.addValue("999999", 6, rdata);
	array.addValue("888888", 6, rdata);
	array.addValue("777777", 6, rdata);
	array.addValue("666666", 6, rdata);
	array.addValue("555555", 6, rdata);
	array.addValue("444444", 6, rdata);
	array.addValue("333333", 6, rdata);
	array.addValue("222222", 6, rdata);
	array.addValue("111111", 6, rdata);
	array.addValue("000000", 6, rdata);

	array.sort();

	bool finished = false;
	char *entry;
	while (!finished)
	{
		array.nextValue(&entry, finished);
		if (finished) break;
		cout << entry << endl;
	}
	*/
	return true;
}

map<OmnString,OmnString> mCodes;
map<OmnString,OmnString> mCodesid;

bool initMap()
{
/*$datar['300 ']='555 ';
$datar['301 ']='552 ';
$datar['302 ']='556 ';
$datar['303 ']='553 ';
$datar['304 ']='564 ';
$datar['305 ']='551 ';
$datar['306 ']='558 ';
$datar['307 ']='554 ';
$datar['308 ']='562 ';
$datar['309 ']='565 ';
$datar['311 ']='563 ';
$datar['312 ']='550 ';
$datar['313 ']='557 ';
$datar['314 ']='561 ';
$datar['316 ']='559 ';
$datar['317 ']='566 ';
$datar['318 ']='5582';
$datar['030 ']='-1  ';*/
	mCodes["300 "]="555 ";
	mCodes["301 "]="552 ";
	mCodes["302 "]="556 ";
	mCodes["303 "]="553 ";
	mCodes["304 "]="564 ";
	mCodes["305 "]="551 ";
	mCodes["306 "]="558 ";
	mCodes["307 "]="554 ";
	mCodes["308 "]="562 ";
	mCodes["309 "]="565 ";
	mCodes["311 "]="563 ";
	mCodes["312 "]="550 ";
	mCodes["313 "]="557 ";
	mCodes["314 "]="561 ";
	mCodes["316 "]="559 ";
	mCodes["317 "]="566 ";
	mCodes["318 "]="5582";
	mCodes["030 "]="-1  ";


	mCodesid["300   "] = "555   ";
	mCodesid["301   "] = "552   ";
	mCodesid["302   "] = "556   ";
	mCodesid["303   "] = "553   ";
	mCodesid["304   "] = "564   ";
	mCodesid["305   "] = "551   ";
	mCodesid["306   "] = "558   ";
	mCodesid["307   "] = "554   ";
	mCodesid["308   "] = "562   ";
	mCodesid["309   "] = "565   ";
	mCodesid["311   "] = "563   ";
	mCodesid["312   "] = "550   ";
	mCodesid["313   "] = "557   ";
	mCodesid["314   "] = "561   ";
	mCodesid["316   "] = "559   ";
	mCodesid["317   "] = "566   ";
	mCodesid["318   "] = "5582  ";
	mCodesid["030   "] = "-1    ";
	return true;
}


bool checkFileType(const OmnString &date, const OmnString &fname)
{
	OmnFile file(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	aos_assert_r(file.isGood(), false);
	int record_size = 0 ;
	const int memsize = 100000000;	// 100M
	AosBuff buff(memsize AosMemoryCheckerArgs);
	char *data = buff.data();

	// const int vc_size = 782;
	// const int ss_size = 443;
	// const int sg_size = 478;
	// const int id_size = 349;
	// const int ib_size = 482;
	// const int vt_size = 782;

	hash_map<const OmnString, OmnFilePtr, Omn_Str_hash, compare_str> filemap;
	hash_map<const OmnString, OmnFilePtr, Omn_Str_hash, compare_str>::iterator itr;
	hash_map<const OmnString, int, Omn_Str_hash, compare_str> sizemap;
	hash_map<const OmnString, int, Omn_Str_hash, compare_str>::iterator size_itr;

	u64 current_pos = 0;
	int remaining = 0;
	while (1)
	{
		int bytes_read = file.readToBuff(current_pos, memsize-remaining, &data[remaining]);
		if (bytes_read <= 0) 
		{
			OmnScreen << "Finished: " << fname << endl;
			return true;
		}

		int crt_idx = 0;
		while (data[crt_idx++] != '\n');

		while (crt_idx < bytes_read)
		{
			OmnString filetype(&data[crt_idx], 2);

			int actual_len = 2;
			while (data[crt_idx + actual_len++] != '\n');

			itr = filemap.find(filetype);
			OmnFilePtr file;
			if (itr == filemap.end())
			{
				// Did not find the type. Create one
				cout << "Found a new CDR type: " << filetype.data() << endl;
				OmnString fname = filetype;
				fname << "_" << date;
				file = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);
				aos_assert_r(file->isGood(), false);

				filemap[filetype] = file;
				sizemap[filetype] = actual_len;
			}
			else
			{
				file = itr->second;
				size_itr = sizemap.find(filetype);
				aos_assert_r(size_itr != sizemap.end(), false);
				if (size_itr->second != actual_len)
				{
					cout << "*****************: Size mismatch: " << actual_len 
						<< ":" << size_itr->second << endl;
				}
			}

			if (record_size != actual_len)
			{
				cout << "Record length incorrect. Expecting: " 
					<< record_size << ". Actual: " << actual_len << endl;
				if (crt_idx + 1000 < bytes_read) data[crt_idx + 1000] = 0;
				cout << "Found unrecognized line: " << crt_idx << ": " << &data[crt_idx] << endl;
				exit(1);
			}

			if (bytes_read - crt_idx < record_size)
			{
				break;
			}
			crt_idx += record_size;
		}

		aos_assert_r(bytes_read >= crt_idx, false);
		remaining = bytes_read - crt_idx;
		memcpy(data, &data[crt_idx], remaining);
	}

	return true;
}

bool AosVerifyFile(const OmnString &fname)
{
	OmnFile file(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	aos_assert_r(file.isGood(), false);
	const int memsize = 100000000;	// 100M
	AosBuff buff(memsize AosMemoryCheckerArgs);
	char *data = buff.data();

	u64 current_pos = 0;
	int remaining = 0;
	while (1)
	{
		int bytes_read = file.readToBuff(current_pos, memsize-remaining, &data[remaining]);
		if (bytes_read <= 0) 
		{
			OmnScreen << "Finished: " << fname << endl;
			return true;
		}

		int crt_idx = 0;
		int prev_length = 0;
		while (crt_idx < bytes_read)
		{
			// Find the end of the line
			int actual_len = 0;
			while (crt_idx + actual_len < bytes_read && data[crt_idx + actual_len++] != '\n');

			if (crt_idx + actual_len >= bytes_read) break;

			if (prev_length == 0) 
			{
				prev_length = actual_len;
			}
			else
			{
				OmnAlarm << "length incorrect: " << prev_length << ":" << actual_len << enderr;
				exit(1);
			}
		}

		aos_assert_r(bytes_read >= crt_idx, false);
		remaining = bytes_read - crt_idx;
		memcpy(data, &data[crt_idx], remaining);
	}

	return true;
}



void smoothing()
{
	int a[10];
	a[0] = 1297;
	a[1] = 1329;
	a[2] = 1386;
	a[3] = 1456;
	a[4] = 1503;
	a[5] = 1541;
	a[6] = 1569;
	a[7] = 1598;
	a[8] = 1617;
	a[9] = 1595;

	int repeat = 30;
	int len = 10;
	cout << "Data: " << endl;
	for (int i=0; i<len; i++) cout << a[i] << ", ";
	cout << endl;

	while (repeat--)
	{
		for (int i=0; i<len-1; i++) a[i] = (a[i] + a[i+1]) / 2;

		// cout << "Smooth: " << repeat << endl; 
		for (int i=0; i<len; i++) cout << a[i] << ", ";

		int nn = 0;
		for (int i=0; i<len-1; i++)
		{
			cout << a[i+1] - a[i] << ", ";
			if (a[i+1] - a[i] < 0) nn++;
		}
		cout << "Total Diffs: " << nn << endl;
	}
}


class recordSet
{
public:
	OmnString		mTypename;
	vector<OmnString> mData ;
};

int mCountc = 0;
bool
repairvc(OmnString &line)
{
	OmnString  str = line.subString(281,4);
	OmnString  rslt = "";
	
	if(mCodes[str]!="")
	{
		//cout << str.data()  << " - " <<  mCodes[str].data()  << endl ;
		
		rslt << line.subString(0,281) << mCodes[str] <<  line.subString(285,line.length()-4-281-1);
		rslt << "\n";
		//mCountc++;
		//if(mCountc > 20 )
		//{
			//exit(1);
		//}
	}
	else
	{
		rslt << line ;
	}
	/*
	 if(line.length() != 792)
	{
		OmnScreen << "line a" << line.length() << endl;
		OmnScreen <<  " line a " << line << endl;
		exit(1);
	}
	 
	if(rslt.length() != 792)
	{
		OmnScreen << "line c" << rslt.length() << endl;
		OmnScreen <<  " line c " << rslt << endl;
		exit(1);
	}
	 */
	line = "";
	OmnString str861 =  rslt.subString(56,3);
	OmnString str861s =  rslt.subString(56,24);
	str861s.normalizeWhiteSpace(true ,true );
	if ( str861 =="861" && str861s.length() == 13  ) //if($r=="861"&& strlen($rt)==13)
	{
		 
		line << rslt.subString(0, 56) << rslt.subString(58,22) << "  " <<  rslt.subString(80,rslt.length()-2-22-56-1);//$c= substr($c,0,56).substr($c,58,22)."  ".substr($c,80);
		line << "\n";
		//OmnScreen <<  " line d-0 "   << endl;
	}
	else
	{
		line << rslt;
		//OmnScreen <<  " line d-1 "   << endl;
	}
	
	 /*
	if(line.length() != 792)
	{
		OmnScreen << "line d" << line.length() << endl;
		OmnScreen <<  " line d " << line << endl;
		exit(1);
	}
	*/
	 
	/*	$r = substr($c,56,5);	$rt = substr($c,56,24);	$rt = trim($rt);	*/
	rslt = "";
	OmnString str17911 = line.subString(56,5);
	OmnString str17911s = line.subString(56,24);
	str17911s.normalizeWhiteSpace(true ,true );

	if(str17911 == "17911" && str17911s.length()> 11 )
	{
		//int itmp =  line.length() -5 -19 -56 ;
		//OmnScreen << itmp << endl;
		//OmnScreen << "line g1:" << line.length() << ":"  << line << endl;
		//OmnString strtmp =  line.subString(80,itmp);
		rslt   << line.subString(0, 56) << line.subString(61,19) <<  "     "   << line.subString(80,line.length() -5 -19 -56 -1 );
		rslt << "\n";
		//OmnScreen << "line g1-1:" << rslt.length() << ":"  << rslt << endl;
	}
	else
	{
		rslt   << line ;
 
		//OmnScreen << "line g2:" << line << endl;
	}
	line = "";
	line << rslt;
	return true;
}


bool
repairid(OmnString &line)
{

	OmnString  str = line.subString(145,6);
	OmnString  rslt = "";
	
	if(mCodesid[str]!="")
	{
		//cout << str.data()  << " - " <<  mCodes[str].data()  << endl ;
		
		rslt << line.subString(0,145) << mCodesid[str] <<  line.subString(151,line.length()-6-145-1);
		rslt << "\n";
		//mCountc++;
		//if(mCountc > 20 )
		//{
			//exit(1);
		//}
	}
	else
	{
		rslt << line ;
	}
	
	line = "";
	
	//OmnString str0086s =  rslt.subString(34,24);
	OmnString str0086 =  rslt.subString(34,4);
	
	if ( str0086 =="0086"    ) //if($r=="861"&& strlen($rt)==13)
	{
		line << rslt.subString(0, 34) << rslt.subString(38,20) << "    " <<  rslt.subString(58,rslt.length()-4-20-34-1);//$c= substr($c,0,56).substr($c,58,22)."  ".substr($c,80);
		line  << "\n";
	}
	else
	{
		line << rslt;
	}
	
	/*	$r = substr($c,56,5);	$rt = substr($c,56,24);	$rt = trim($rt);	*/
	rslt = "";

	OmnString str17911 = line.subString(34,7);
	OmnString str17911s = line.subString(56,24);
	str17911s.normalizeWhiteSpace(true ,true );
	/*
	if($r=="17911"&& strlen($rt)>11)
                {
         $c= substr($c,0,56).substr($c,61,19)."     ".substr($c,80);
			// echo $rt."\n";
                }
	*/
	if(str17911 == "1791101" && str17911s.length() == 17  )
	{
		//
		rslt << line.subString(0, 34) << line.subString(40,18) <<  "      " <<  line.subString(58,line.length()-6-18-34-1);
		rslt << "\n";
	}
	else
	{
		OmnString str17911n = line.subString(34,5);
		if( str17911n == "17911" && str17911s.length() > 11 )
		{
			rslt << line.subString(0, 34) << line.subString(39,19) <<  "     " <<  line.subString(58,line.length()-5-19-34-1);
			rslt << "\n";
		}
		else
		rslt << line ; 
	}
		

	line = "";
	line << rslt;
	return true;
}

vector<recordSet> mFileData;


bool
saveLine(OmnString type ,OmnString line)
{
	/* 	ac bj bl br de
hl hp hm ib id iy ml mm ph
pt sr ss sg va ve
vc vf vh vj vn vr vs vt vv
	*/
	
	if(!(type == "ac"   || type == "bj"   || type == "bl"   || type == "br"   || type == "de"   || type == "hl"   || type == "hp"   || type == "hm"   || type == "ib"   || type == "id"   || type == "iy"   || type == "ml"   || type == "mm"   || type == "ph"   || type == "pt"   || type == "sr"   || type == "ss"   || type == "sg"   || type == "va"   || type == "ve"   || type == "vc"   || type == "vf"   || type == "vh"   || type == "vj"   || type == "vn"   || type == "vr"   || type == "vs"   || type == "vt"   || type == "vv"  )) 
	{
		return true ;
	}

	if(type == "vc")
	{
		repairvc(line);
	}
	if( type == "id")
	{
		repairid(line);
	}
	for(u32 i = 0; i < mFileData.size() ; i++)
	{
		if(mFileData[i].mTypename == type )
		{
			line << "**********";
			mFileData[i].mData.push_back(line);
			return(true);
		}
		
	}
	
	recordSet record;
	record.mTypename = type; 
	line << "**********";
	record.mData.push_back(line);
	mFileData.push_back(record);
	return true;
}

bool
testDataFile(OmnString fname)
{
	if (fname == "")
	{
		cout << "error filename ";
		return false;
	}

	OmnFile file(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	aos_assert_r(file.isGood(), false);
	AosBuff buff1(2000 AosMemoryCheckerArgs);
	char *data1 = buff1.data();
	char a0 = data1[0];
	char a1 = data1[1];
	int i = 0;
	while(data1[i] != '\n') i++;
	i++;

	AosBuff buff(i AosMemoryCheckerArgs);
	u64 pos = 0;
	u64 filelen = file.getLength();
	u64 line = 0 ;
	char *data = buff.data();
	while(pos < filelen)
	{
		int bytes_read = file.readToBuff(pos, i, &data[0]);
		line++;
		pos += bytes_read;
		if(data[0] != a0 || data[1] != a1)
		{
			cout << "error line:" << line << endl;
			exit(1);
		}		
	}
	
	return true;
}


bool ConvertUnicomFile(OmnString fname)
{
	//To Convert the files for unicome . by xulei 20120711
	//
//	OmnScreen << mFileData.size() << endl ; 
	OmnFile file(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	aos_assert_r(file.isGood(), false);
	const int memsize = 1000000000;	// 100M
	AosBuff buff(memsize AosMemoryCheckerArgs);
	char *data = buff.data();
	u64 current_pos = 0;
	u64 remaining = 0;
	int bytes_read = file.readToBuff(current_pos, memsize, &data[remaining]);
	if(	bytes_read > 0 )
	{
		int crt_idx = 0;
		while (data[crt_idx++] != '\n'); // drop the first line 
		while(crt_idx +3  < bytes_read)
		{

		 
			int actual_len = 2;
			OmnString filetype(&data[crt_idx], 2);
			 
			while (data[crt_idx + actual_len++] != '\n');
			OmnString line(&data[crt_idx] , actual_len );
			crt_idx += actual_len;
			saveLine(filetype , line);

		}
	}

	for (u32 i = 0 ; i < mFileData.size() ; i++)
	{
		OmnString outFilename = mFileData[i].mTypename;
		outFilename << "_DATA";
		OmnString fileData = "";
		for(u32 j=0 ; j < mFileData[i].mData.size(); j++)
		{
			fileData << mFileData[i].mData[j];
		}
		OmnFile * fileo ;
	 
		fileo = OmnNew OmnFile(outFilename, OmnFile::eAppend AosMemoryCheckerArgs);
		if(fileo &&fileo->isGood() )
		{
			bool flag = true ;
			fileo->append(fileData, flag);
			 
		}
		else
		{
			fileo = OmnNew OmnFile(outFilename, OmnFile::eWriteCreate AosMemoryCheckerArgs);
		 
			aos_assert_r(fileo->isGood(), false);
			bool flag = true ;
			fileo->append(fileData, flag);
		}
	}
	return true;
}


bool write5G()
{
	for(int i =0 ; i < 5 ;i++)
    {
        OmnString str = OmnRandom::binaryStr(1000000,1000000);
        OmnString filename="";
	    filename << "/tmp/tmptmptmpbig_1aaa" << i << ".data";
        OmnFile file(filename, OmnFile::eReadOnly AosMemoryCheckerArgs);
		for(int j=0;j<1000;j++)
		{
			
			file.put(0, str.data(), str.length(), true);
		}
    }
	return true;
}

bool
testnfs01()
{
	cout << "testnfs 01" << endl;
    u32 t1  = OmnGetSecond();
    for(int i =0 ; i < 10000 ;i++)
    {
        OmnString str = OmnRandom::binaryStr(30000,30000);
        OmnString filename="";
        filename << "/tmp/aaa" << i << ".data";
        OmnFile file(filename, OmnFile::eCreate AosMemoryCheckerArgs);
        file.put(0, str.data(), str.length(), true);
    }
	u32 t2  = OmnGetSecond();
	u32 t01 = t2 - t1;
	cout << "local 10000 files write . 30K per file .time :   " << t01 << endl;
	write5G();
	t1  = OmnGetSecond();
	char buff[30000];
    for(int i =0 ; i < 10000 ;i++)
    {
        OmnString filename="";
        filename << "/tmp/aaa" << i << ".data";
		OmnString   readStr(const u64 &offset, const u32, const OmnString &);
        OmnFile file(filename, OmnFile::eReadOnly AosMemoryCheckerArgs);
		if(!file.isGood())
		{
			cout << "error " << i << endl;
		}
		
        file.readToBuff(0, 30000,buff);
		if(i < 10)
		{	 
			filename="";
			filename << "/tmp/bbb" << i << ".data";
			OmnFile file(filename, OmnFile::eCreate AosMemoryCheckerArgs);
			file.put(0, buff, 30000, true);
			//cout << buff << endl ;
		}
		buff[0]=12;
    }
	t2  = OmnGetSecond();
	t01 = t2 - t1;
	cout << "local read  10000 files write . 30K per file .time :   " << t01 << endl;


	t1  = OmnGetSecond();
    for(int i =0 ; i < 10000 ;i++)
    {
        OmnString str = OmnRandom::binaryStr(30000,30000);
        OmnString filename="";
        filename << "/tmp/nfs86/aaa" << i << ".data";
        OmnFile file(filename, OmnFile::eCreate AosMemoryCheckerArgs);
        file.put(0, str.data(), str.length(), true);
    }
	t2  = OmnGetSecond();
	t01 = t2 - t1;
	cout << "nfs 86 test 10000 files write . 30K per file .time :   " << t01 << endl;
	write5G();
	t1  = OmnGetSecond();
    for(int i =0 ; i < 10000 ;i++)
    {
        OmnString filename="";
        filename << "/tmp/nfs86/aaa" << i << ".data";
		OmnString   readStr(const u64 &offset, const u32, const OmnString &);
        OmnFile file(filename, OmnFile::eReadOnly AosMemoryCheckerArgs);
		char buff[30000];
        file.readToBuff(0, 30000,buff);
    }
	t2  = OmnGetSecond();
	t01 = t2 - t1;
	cout << "nfs 86 read  10000 files write . 30K per file .time :   " << t01 << endl;


	t1  = OmnGetSecond();		
    for(int i =0 ; i < 30000 ;i++)
    {
        OmnString str = OmnRandom::binaryStr(10000,10000);
        OmnString filename="";
        filename << "/tmp/1aaa" << i << ".data";
        OmnFile file(filename, OmnFile::eCreate AosMemoryCheckerArgs);
        file.put(0, str.data(), str.length(), true);
    }
	t2  = OmnGetSecond();
	t01 = t2 - t1;
	cout << "test 30000 files write . 10K per file .time :   " << t01 << endl;
	write5G();
	t1  = OmnGetSecond();
    for(int i =0 ; i < 30000 ;i++)
    {
       
        OmnString filename="";
        filename << "/tmp/1aaa" << i << ".data";
		OmnString   readStr(const u64 &offset, const u32, const OmnString &);
        OmnFile file(filename, OmnFile::eReadOnly AosMemoryCheckerArgs);
		char buff[10000];
        file.readToBuff(0, 10000,buff);
    }
	t2  = OmnGetSecond();
	t01 = t2 - t1;
	cout << "local read  30000 files write . 10K per file .time :   " << t01 << endl;

	t1  = OmnGetSecond();		
    for(int i =0 ; i < 30000 ;i++)
    {
        OmnString str = OmnRandom::binaryStr(10000,10000);
        OmnString filename="";
        filename << "/tmp/nfs86/1aaa" << i << ".data";
        OmnFile file(filename, OmnFile::eCreate AosMemoryCheckerArgs);
        file.put(0, str.data(), str.length(), true);
    }
	t2  = OmnGetSecond();
	t01 = t2 - t1;
	cout << "nfs 86 write 30000 files write . 10K per file .time :   " << t01 << endl;

	write5G();
	t1  = OmnGetSecond();
    for(int i =0 ; i < 30000 ;i++)
    {
       
        OmnString filename="";
        filename << "/tmp/nfs86/1aaa" << i << ".data";
		OmnString   readStr(const u64 &offset, const u32, const OmnString &);
        OmnFile file(filename, OmnFile::eReadOnly AosMemoryCheckerArgs);
		char buff[10000];
        file.readToBuff(0, 10000,buff);
    }
	t2  = OmnGetSecond();
	t01 = t2 - t1;
	cout << "nfs 86 read  30000 files write . 10K per file .time :   " << t01 << endl;

	t1  = OmnGetSecond();		
    for(int i =0 ; i < 5 ;i++)
    {
        OmnString str = OmnRandom::binaryStr(1000000,1000000);
        OmnString filename="";
        filename << "/tmp/big_1aaa" << i << ".data";
        OmnFile file(filename, OmnFile::eCreate AosMemoryCheckerArgs);
		for(int j=0;j<1000;j++)
		{
			file.put(j*1000000, str.data(), str.length(), true);
		}
    }
	t2  = OmnGetSecond();
	t01 = t2 - t1;
	cout << "test 5 files write . 1000M per file .time :   " << t01 << endl;
	write5G();

	t1  = OmnGetSecond();		
    for(int i =0 ; i < 5 ;i++)
    {
        OmnString str = OmnRandom::binaryStr(1000000,1000000);
        OmnString filename="";
        filename << "/tmp/big_1aaa" << i << ".data";
        OmnFile file(filename, OmnFile::eReadOnly AosMemoryCheckerArgs);
		for(int j=0;j<1000;j++)
		{
			char buff[1000000];
			file.readToBuff(0, 1000000,buff);
		}
    }
	t2  = OmnGetSecond();
	t01 = t2 - t1;
	cout << "local read  5 files write . 1000M per file .time :   " << t01 << endl;
	
	t1  = OmnGetSecond();		
    for(int i =0 ; i < 5 ;i++)
    {
        OmnString str = OmnRandom::binaryStr(1000000,1000000);
        OmnString filename="";
        filename << "/tmp/nfs86/big_1aaa" << i << ".data";
        OmnFile file(filename, OmnFile::eCreate AosMemoryCheckerArgs);
		for(int j=0;j<1000;j++)
		{
			file.put(j*1000000, str.data(), str.length(), true);
		}
    }
	t2  = OmnGetSecond();
	t01 = t2 - t1;
	cout << "test 5 files write . 1000M per file .time :   " << t01 << endl;
	write5G();
	t1  = OmnGetSecond();		
    for(int i =0 ; i < 5 ;i++)
    {
        OmnString str = OmnRandom::binaryStr(1000000,1000000);
        OmnString filename="";
        filename << "/tmp/nfs86/big_1aaa" << i << ".data";
        OmnFile file(filename, OmnFile::eReadOnly AosMemoryCheckerArgs);
		for(int j=0;j<1000;j++)
		{
			char buff[1000000];
			file.readToBuff(0, 1000000,buff);
		}
    }
	t2  = OmnGetSecond();
	t01 = t2 - t1;
	cout << "nfs86 read  5 files write . 1000M per file .time :   " << t01 << endl;

   return true;
}


class Chentest
{
public:
	Chentest()
	{
		cout << "Chentest created" << endl;
	}
	void *operator new(std::size_t size)
	{
		cout << "To new class ChenTest" << endl;
		return ::operator new(size);
	};
};

void *chenfunc()
{
	return 0;
}

#define ChenNew new(__FILE__, __LINE__) 


struct ltstr
{
	bool operator()(const char* s1, const char* s2) const
	{
		return strcmp(s1, s2) < 0;
	}
};

bool testmap()
{
	map<const char*, int, ltstr> months;
  
  	months["january"] = 1;
    months["february"] = 2;
  	months["march"] = 3;
	months["april"] = 4;
	months["may"] = 5;
	months["june"] = 6;
	months["july"] = 7;
	months["august"] = 8;
	months["september"] = 9;
	months["october"] = 10;
	months["december"] = 11;
	months["november"] = 12;
						  
	cout << "june -> " << months["june"] << endl;
	map<const char*, int, ltstr>::iterator cur  = months.find("june");
	map<const char*, int, ltstr>::iterator prev = cur;
	map<const char*, int, ltstr>::iterator next = cur;    
	++next;
	--prev;
	cout << "June: " << cur->first << ":" << cur->second << endl;
	cout << "Previous (in alphabetical order) is " << (*prev).first << endl;
	cout << "Next (in alphabetical order) is " << next->first << endl;
	return true;
}


int 
main(int argc, char **argv)
{
	OmnApp theApp(argc, argv);

	OmnAlarmMgr::config();

	AosXmlTagPtr app_config;

	int index = 1;
	int tries = 0;
	while (index < argc)
	{
		if (strcmp(argv[index], "-config") == 0)
		{
			// '-config fname'
			app_config = OmnApp::readConfig(argv[index+1]);
			aos_assert_r(app_config, false);
			OmnApp::setConfig(app_config);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-n") == 0 && index < argc-1)
		{
			tries = atoi(argv[index+1]);
			index += 2;
			continue;
		}
		
		if (strcmp(argv[index], "-wn") == 0 && index < argc-1)
		{
			int max_alarms = atoi(argv[index+1]);
			index += 2;
			OmnAlarmMgr::setMaxAlarms(max_alarms);
			continue;
		}
		
		index++;
	}

	AosDataFieldObj::setCreator(OmnNew AosDataFieldBool(false));
	AosDataFieldObj::setCreator(OmnNew AosDataFieldStr(false));
	AosDataField::init();
	AosDataRecordObj::setCreator(OmnNew AosRecordFixBin(false AosMemoryCheckerArgs));

	// This is the application part
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("Util/Tester", "Try", "Chen Ding");
	testMgr->setTries(tries);
	testMgr->addSuite(OmnUtilTestSuite::getSuite());

	cout << "Start Testing ..." << endl;
 
	testMgr->start();

	cout << "\nFinished. " << testMgr->getStat().data() << endl;

	testMgr = 0;
 
	AosExeReport report(10);
	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 

