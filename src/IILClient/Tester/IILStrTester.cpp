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
// 2011/09/05 Created by ken Lee	
////////////////////////////////////////////////////////////////////////////
#include "IILClient/Tester/IILStrTester.h"

#include "AppMgr/App.h"
#include "QueryRslt/QueryRslt.h"
#include "IILClient/IILClient.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include "Util/UtUtil.h"

const int sgIILNum = 6;
const int sgMaxDocs = 1000000;
const int sgMaxStrLen = 80;

static int			sgNumDocs[sgIILNum];
static u64          sgIILIDs[sgIILNum];
static u64          sgDocids[sgIILNum][sgMaxDocs];
static char *       sgValues[sgIILNum][sgMaxDocs];

static u64			sgDocid = 10000;
static int 			sgTotalDocs = 0;
static int			sgTotal = 0;
static int			sgAdd = 0;
static int			sgDel = 0;
static int			sgCheck = 0;


AosIILStrTester::AosIILStrTester()
{
	for (int i=0; i<sgIILNum; i++)
	{
		sgIILIDs[i] = 0;
		for (int j=0; j<sgMaxDocs; j++)
		{
			sgDocids[i][j] = 0;
			sgValues[i][j] = new char[sgMaxStrLen+2];
		}
	}
}


AosIILStrTester::~AosIILStrTester()
{
}


bool      	
AosIILStrTester::start()
{
	cout << " IIL Str Test start ..." << endl;
	basicTest();
	return true;
}


bool 
AosIILStrTester::basicTest()
{
	while(sgTotalDocs < sgMaxDocs)
	{
		sgTotal++;
		if(sgTotal % 1000 == 0)
		{
			OmnScreen << "sgTotal:" << sgTotal 
					  << ", sgAdd:" << sgAdd 
					  << ", sgDel:" << sgDel
					  << ", sgCheck:" << sgCheck << endl;
		}
		int opr = rand() % 100;
		if(opr < 60)
		{
//u64 t1 = OmnGetTimestamp();
			aos_assert_r(addEntry(), false);
//OmnScreen << "total time: " << OmnGetTimestamp() - t1 << endl;	
			sgAdd++;
		}
		else if(opr < 90)
		{
			aos_assert_r(removeEntry(), false);
			sgDel++;
		}
		else
		{
			aos_assert_r(checkEntry(), false);
			sgCheck++;
		}
	}

	int sgTotalTries = 10000000;
	for(int i=0; i<sgTotalTries; i++)
	{
		if(i % 1000 == 0)
		{
			OmnScreen << "sgTries :" << i << endl;
		}
		aos_assert_r(checkEntry(), false);
	}
	OmnSleep(100000);
	return true;
}


bool
AosIILStrTester::getUnique(
		const u64 &iilid,
		bool &value_unique, 
		bool &docid_unique)
{
	// 1. iilid % 4 == 0: value and docid are not unique
	// 2. iilid % 4 == 1: value not unique, docid unique
	// 3. iilid % 4 == 2: value unique, docid not unique
	// 4. iilid % 4 == 3: value and docid unique
	u64 i = iilid % 4;
	switch(i)
	{
	case 0 : value_unique = false; docid_unique = false; break;
	case 1 : value_unique = false; docid_unique = true;  break;
	case 2 : value_unique = true;  docid_unique = false; break;
	case 3 : value_unique = true;  docid_unique = true;  break;
	}
	return true;	   		
}


bool
AosIILStrTester::needCreateNew(const bool unique)
{
	bool createNew;
	if(unique)
	{
		createNew = true;
	}
	else
	{
		int r = rand() % 100;
		createNew = r < 50 ? true : false;
	}
	return createNew;
}


OmnString
AosIILStrTester::getValue(
		const int &idx,
		const bool value_unique)
{
	aos_assert_r(idx!=-1, false);
	bool createNew = needCreateNew(value_unique);
	OmnString value;
	if(createNew || sgNumDocs[idx]==0)
	{
		int len = (rand() % 20) + 5;
		char data[40];
		AosRandomLetterStr(len, data);
		value = data;
	}
	else
	{
		int s = rand() % sgNumDocs[idx];
		value = sgValues[idx][s];
	}
	return value;
}


u64
AosIILStrTester::getDocid(
		const int &idx,
		const bool docid_unique)
{
	aos_assert_r(idx!=-1, false);
	bool createNew = needCreateNew(docid_unique);
	u64 docid;
	if(createNew || sgNumDocs[idx]==0)
	{
		docid = sgDocid++;
	}
	else
	{
		int s = rand() % sgNumDocs[idx];
		docid = sgDocids[idx][s];
	}
	return docid;
}


int
AosIILStrTester::compareValueDocid(const char * v1, const OmnString &v2, const u64 &d1, const u64 &d2)
{
	int c1 = strcmp(v1, v2.data());
	int c2 = d1 < d2 ? -1 : (d1 == d2 ? 0 : 1);
	
	if(c1 == 0) return c2;
	else return c1;
}


int
AosIILStrTester::findValueDocid(
		const int idx,
		const OmnString &value,
		const u64 &docid)
{
	for(int i=0; i<sgNumDocs[idx]; i++)
	{
		if(compareValueDocid(sgValues[idx][i], value, sgDocids[idx][i], docid) == 1) return i;
	}
	return sgNumDocs[idx];
}


bool
AosIILStrTester::addValueToArray(
		const int idx,	
		const OmnString &value,
		const u64 &docid)
{
	int vidx = findValueDocid(idx, value, docid);

	for(int i=sgNumDocs[idx]; i>vidx; i--)
	{
		strcpy(sgValues[idx][i], sgValues[idx][i-1]);
		sgDocids[idx][i] = sgDocids[idx][i-1];
	}
	strcpy(sgValues[idx][vidx], value.data());
	sgDocids[idx][vidx] = docid;

	return true;
}


bool
AosIILStrTester::removeValueFromArray(
		const int idx,	
		const int vidx)
{
	for(int i = vidx; i<sgNumDocs[idx]; i++)
	{
		sgDocids[idx][i] = sgDocids[idx][i+1];
		strcpy(sgValues[idx][i], sgValues[idx][i+1]);
	}
	strcpy(sgValues[idx][sgNumDocs[idx]-1], "");
	sgDocids[idx][sgNumDocs[idx]-1] = 0;
	return true;
}


bool 		
AosIILStrTester::addEntry() 
{
	int i = rand() % sgIILNum;
	u64 iilid = sgIILIDs[i];

	AosRundataPtr rdata = OmnApp::getRundata();
	if(iilid == 0)
	{
		AosIILType type = eAosIILType_Str;
		bool rslt = AosIILClient::getSelf()->createIILPublic(iilid, type, rdata);
		aos_assert_r(rslt, false);
		sgIILIDs[i] = iilid;
	}
	aos_assert_r(iilid, false);
	
	if(sgNumDocs[i] >= sgMaxDocs) return true;

	bool value_unique, docid_unique;
	getUnique(iilid, value_unique, docid_unique);
	OmnString value = getValue(i, value_unique);
	u64 docid = getDocid(i, docid_unique);

OmnScreen << "add,iilid:" << iilid << ",docid:" << docid << ",value:" << value << endl;

	bool rslt = AosIILClient::getSelf()->addStrValueDoc(iilid, value, docid, value_unique, docid_unique, rdata);
	if(!rslt)
	{
		OmnAlarm << "addDoc failed, iilid:" << iilid << ",docid:" << docid << ",value:" << value << enderr;
		return false;
	}
	addValueToArray(i, value, docid);
	
	sgNumDocs[i]++;
	sgTotalDocs++;
	return true;
}


bool
AosIILStrTester::removeEntry()
{
	int i = rand() % sgIILNum;
	u64 iilid = sgIILIDs[i];
	if(iilid == 0) return true;
	if(sgNumDocs[i] == 0) return true;

	int idx = rand() % sgNumDocs[i];
	u64 docid = sgDocids[i][idx];
	OmnString value = sgValues[i][idx];
	AosRundataPtr rdata = OmnApp::getRundata();

OmnScreen << "remove, iilid:" << iilid << ",value:" << value << ",docid:" << docid << ",idx:" << idx << ",mNum:" << sgNumDocs[i] << endl;

	bool rslt = AosIILClient::getSelf()->removeValueDoc(iilid, value, docid, rdata);
	if(!rslt)
	{
		OmnAlarm << "remove doc failed, iilid:" << iilid << ",value:" << value << ",docid:" << docid << ",idx:" << idx << ",mNum:" << sgNumDocs[i] << enderr;
		return false;
	}
	
	removeValueFromArray(i, idx);
	sgNumDocs[i]--;
	sgTotalDocs--;
	return true;
}


bool
AosIILStrTester::checkEntry()
{
	int idx = rand() % sgIILNum;
	u64 iilid = sgIILIDs[idx];
	if(iilid == 0) return true;
	if(sgNumDocs[idx] == 0) return true;

	AosOpr opr;
	OmnString str;
	aos_assert_r(getOprAndStr(idx, opr, str), false);

	AosRundataPtr rdata = OmnApp::getRundata();
	AosQueryRsltPtr query_rslt = OmnNew AosQueryRslt();
	query_rslt->setWithValues(true);

OmnScreen << "check iil, iilid:" << iilid << ",opr:" << AosOpr_toStr(opr) << ",value:" << str << endl;

	bool rslt = AosIILClient::getSelf()->querySafe2(iilid, query_rslt, 0, opr, str, rdata);
	if(!rslt)
	{
		OmnAlarm << "query failed,iilid:" << iilid << ",opr:" << AosOpr_toStr(opr) << ",value:" << str << enderr;
		return false;
	}
OmnScreen << "check iil back, iilid:" << iilid << ",opr:" << AosOpr_toStr(opr) << ",value:" << str << endl;
	
	u64 d1, d2;
	char * v1;
	OmnString v2;
	for(int i=0; i<sgNumDocs[idx]; i++)
	{
		v1 = sgValues[idx][i];
		d1 = sgDocids[idx][i];
		if(!checkOprValue(opr, v1, str)) continue;

		rslt = query_rslt->nextDocidValue(d2, v2, rdata);
		aos_assert_r(rslt, false);
	
		if(compareValueDocid(v1, v2, d1, d2) != 0)
		{
			OmnAlarm << "check error" << enderr;
			return false;
		}
	}
	if(query_rslt->hasMoreDocid())
	{
		OmnAlarm << "not finished" << enderr;
		return false;
	}

	return true;
}


bool
AosIILStrTester::getOprAndStr(
		const int idx,
		AosOpr &opr,
		OmnString &str)
{
	aos_assert_r(sgNumDocs[idx], false);

	int i = rand() % sgNumDocs[idx];
	OmnString value = sgValues[idx][i];

	int o = rand() % 8;
	switch(o)
	{
	case 0 : opr = eAosOpr_gt; break;
	case 1 : opr = eAosOpr_ge; break;
	case 2 : opr = eAosOpr_eq; break;
	case 3 : opr = eAosOpr_lt; break;
	case 4 : opr = eAosOpr_le; break;
	case 5 : opr = eAosOpr_ne; break;
	case 6 : opr = eAosOpr_an; break;
	case 7 : opr = eAosOpr_like; break;
	default : OmnAlarm << "no opr" << enderr; return false;
	}
	
	if(opr == eAosOpr_like) 
	{
		int len = value.length();
		int idx = rand() % len;
		int slen = rand() % (len - idx);
		str = value.substr(idx, idx + slen);
	}
	else
	{
		str = value;
	}
	return true;
}


bool
AosIILStrTester::checkOprValue(
		const AosOpr opr,
		const char * c,
		const OmnString &value)
{
	int rslt = strcmp(c, value.data());
	switch (opr)
	{
	case eAosOpr_le: return rslt <= 0;
	case eAosOpr_lt: return rslt < 0;
	case eAosOpr_eq: return rslt == 0;
	case eAosOpr_gt: return rslt > 0;
	case eAosOpr_ge: return rslt >= 0;
	case eAosOpr_ne: return rslt != 0;
	case eAosOpr_an: return true;
	case eAosOpr_like:
	{
		const char * d = strstr(c, value.data());
		return d != NULL;
	}
	default: return false;
	}
	return false;
}

