////////////////////////////////////////////////////////////////////////////
// 2011/5/25	Created by ken
////////////////////////////////////////////////////////////////////////////

#include "SearchEngine/TesterNew/IILStrUniqueTester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/DocTags.h"
#include "IILMgr/IILMgr.h"
#include "IILMgr/IILStr.h"
#include "IILMgr/IIL.h"
#include "IILClient/IILClient.h"
#include "SearchEngine/SeCommon.h"
#include "SearchEngine/DocServer.h"
#include "SearchEngine/Ptrs.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"

#include <vector>


const OmnString sgSiteid = "100";
const int sgMaxDocs = 1000000;
const int sgMaxStrLen = 80;
const bool sgIsNumAlpha = true;

static char *		sgValues[sgMaxDocs];
static u64			sgDocids[sgMaxDocs];
static char *		sgIILValues[sgMaxDocs];
static u64			sgIILDocids[sgMaxDocs];



AosIILStrUniqueTester::AosIILStrUniqueTester()
:
mTotal(0),
mAddOpr(0),
mDelOpr(0),
mModOpr(0),
mVerifyOpr(0)
{
	mName = "IILStrUnqiueTester";

	// Initialize sgValues
	for (int i=0; i<sgMaxDocs; i++)
	{
		sgDocids[i] = 0;
		sgValues[i] = new char[sgMaxStrLen+2];
		sgIILDocids[i] = 0;
		sgIILValues[i] = new char[sgMaxStrLen+2];
	}
	mDocid = 10000;
	u64 iilid = AosIILMgr::getSelf()->createIILPublic(eAosIILType_Str,false,mRunData);
	AosIILType iiltype = eAosIILType_Invalid;
	AosIILPtr iil =  AosIILMgr::getSelf()->loadIILByIDPublic(
						iilid,
						iiltype,
						false,
						true,
						mRunData);
	
	mIIL = (AosIILStr*)iil.getPtr();
//	mIIL = (AosIILStr*)AosIILMgr::getSelf()->getIILPublic("ken_test__a", true, eAosIILType_Str).getPtr();
}


bool AosIILStrUniqueTester::start()
{
	mRunData =  OmnApp::getRundata();
	cout << "Start IILStrUnique Tester ..." << endl;
	torture();
	return true;
}


bool
AosIILStrUniqueTester::torture()
{
	srand(8888);
	int total = 100000000;
	int group = 0;
	mTotal = 0;
	mAddOpr = 0;
	mDelOpr = 0;
	mModOpr = 0;
	mVerifyOpr = 0;
	mNumDocs = 0;
	for (int i=0; i<total; i++)
	{
		if (++group == 1000)
		{
			mTotal += group;
			group = 0;
			cout << " mTotal = " << mTotal
				 << " mAdd = " << mAddOpr
				 << " mDel = " << mDelOpr
				 << " mMod = " << mModOpr
				 << " mVerify = " << mVerifyOpr
				 << " mNumDocs" << mNumDocs << endl;
		}
	
		int opr = rand() % 100;
		if(opr < 30)
		{
			aos_assert_r(addEntry(), false);
			mAddOpr++;
		}
		else if(opr < 50)
		{
			aos_assert_r(deleteEntry(), false);
			mDelOpr++;
		}
		else if(opr < 70)
		{
			aos_assert_r(modifyEntry(), false);
			mModOpr++;
		}
		else
		{
			aos_assert_r(verifyEntry(), false);
			mVerifyOpr++;
		}
	}
	return true;
}


OmnString 
AosIILStrUniqueTester::createNewString()
{
	OmnString value;
	int r1 = rand() % 100;
	if(r1 >= 20 || mNumDocs == 0)
	{
		// random pick a string, but it may be in the list
		int len = (rand() % 20) + 5;
		char data[40];
		AosRandomLetterStr(len, data);
		value = data;
	}
	else
	{
		// pick one value from the list
		int seqno = rand() % mNumDocs;
		value = sgValues[seqno];
	}
	return value;
}


bool
AosIILStrUniqueTester::addEntry()
{
	if (mNumDocs >= sgMaxDocs) 
	{
		return true;
	}

	OmnString value = createNewString();
	u64 docid = mDocid++;
	bool rslt = mIIL->addDocSafe(value, docid, false, false, false, mRunData);
	if(!rslt)
	{
		OmnAlarm << "fail to add doc: " << docid << ":" << value << enderr;
		return false;
	}

	rslt = addValueToArray(value, docid);
	if(!rslt)
	{
		OmnAlarm << "fail to add value to array: " << docid << ":" << value << enderr;
		return false;
	}
	mNumDocs++;
	return true;
}


bool
AosIILStrUniqueTester::addValueToArray(
		const OmnString &value, 
		const u64 &docid)
{
	int idx = findValue(value, docid);
	
	for(int i=mNumDocs; i>idx; i--)
	{
		strcpy(sgValues[i],sgValues[i-1]);
		sgDocids[i] = sgDocids[i-1];
	}
	
	strcpy(sgValues[idx], value);
	sgDocids[idx] = docid;
	return true;
}


int 
AosIILStrUniqueTester::findValue(const OmnString &value, const u64 &docid)
{
	int idx, rslt;
	for(idx=0; idx<mNumDocs; idx++)
	{
		if(sgIsNumAlpha)
		{
			int len = strlen(sgValues[idx]);
			int vlen = value.length();
			if(len < vlen)
			{
				rslt = -1;
				continue;
			}
			else if(len > vlen)
			{
				rslt = 1;
				return idx;
			}
		}
		
		rslt = strcmp(sgValues[idx], value.data());
		if(rslt == 0 && docid <sgDocids[idx])
		{
			return idx;
		}
		if(rslt == 1)
		{
			return idx;
		}
	}
	return idx;
}
		

int
AosIILStrUniqueTester::findValueDocid(
		const OmnString &value,
		const u64 &docid) 
{
	for(int i=0; i<mNumDocs; i++)
	{
		int rslt = strcmp(sgValues[i], value.data());
		if(rslt == 0)
		{
			if((docid - sgDocids[i]) == 0)
			{
				return i;
			}
		}
	}
	return -1;
}


OmnString 
AosIILStrUniqueTester::getOprValue(const AosOpr opr, const OmnString &value)
{
	OmnString rslt;
	switch(opr)
	{
	case eAosOpr_like :
		 {
		 	int len = value.length();
		 	int idx = rand() % len;
		 	int slen = rand() % (len - idx);
		 	rslt = value.substr(idx, idx + slen);
		 	break;
		 }
	default :
		 rslt = value;
		 break;
	}
	return rslt;
}


bool
AosIILStrUniqueTester::checkOprValue(const AosOpr opr, const char * c, const OmnString &value)
{
	int rslt;
	bool haverslt = false;
	if(sgIsNumAlpha)
	{
		int len = strlen(c);
		int vlen = value.length();
		if(len < vlen)
		{
			haverslt = true;
			rslt = -1;
		}
		else if(len > vlen)
		{
			haverslt = true;
			rslt = 1;
		}
	}
	if(!haverslt)
	{
		rslt = strcmp(c, value.data());
	}
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


bool
AosIILStrUniqueTester::verifyEntry()
{
	if (mNumDocs == 0)
	{
		return true;
	}
	
	int idx = rand() % mNumDocs;
	OmnString value = sgValues[idx];

	int o = rand() % 8;
	AosOpr opr;
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
		default :
			OmnAlarm << "no opr" << enderr;
			return false;
	}

	OmnString opr_value = getOprValue(opr, value);
	//return verifyIIL(opr, opr_value);
	return queryIIL(opr, opr_value);
}


bool
AosIILStrUniqueTester::verifyIIL(const AosOpr opr, const OmnString &value)
{
	int idx = -10, iilidx = -10;
	int tmpidx, tmpiilidx;
	bool rslt, found;
	OmnString unique_value, unique_value2, last_unique;
	for (int i=0; i<mNumDocs; i++)
	{
		if(strcmp(unique_value.data(), sgValues[i]) == 0)
		{
			continue;
		}
		unique_value = sgValues[i];
	
		rslt = checkOprValue(opr, sgValues[i], value);
		if(!rslt)
		{
			continue;
		}
		
		tmpidx = idx;
		tmpiilidx = iilidx;
		last_unique = unique_value2;
		
		rslt = mIIL->nextUniqueValueSafe(idx, iilidx, false, opr, value, unique_value2, found, mRunData);
		if(!rslt)
		{
			OmnAlarm << "fail to get docid form iil, value: " << value << enderr;
			return false;
		}

		if(found && unique_value2 != unique_value)
		{
			rslt = mIIL->nextUniqueValueSafe(tmpidx, tmpiilidx, false, opr, value, last_unique, found, mRunData);
			mIIL->copyData(
				sgIILValues, 
				sgIILDocids, 
				(int)AOSSENG_TORTURER_MAX_DOCS, 
				mNumDocs,
				mRunData);
			OmnAlarm << "fail to match the unique_value, value:" << value << ", unique_value:" << unique_value << ", unique_value2:" << unique_value2
					 << ", idx:" << idx << ", iilidx:" << iilidx 
					 << ", i:" << i << ", sgIILDocids: " << sgIILDocids[i] << ", sgIILValues:" << sgIILValues[i] 
					 << ", sgDocids:" << sgDocids[i] << ", sgValues:" << sgValues[i]
					 << ", opr:" << AosOpr_toStr(opr) << enderr; 
			return false;
		}
	}
	return true;
}


bool 
AosIILStrUniqueTester::deleteEntry()
{
	if (mNumDocs == 0)
	{
		return true;
	}
	
	int idx = rand() % mNumDocs;
	OmnString value = sgValues[idx];
	u64	docid = sgDocids[idx];

	bool rslt = mIIL->removeDocSafe(value, docid, mRunData);
	if(!rslt)
	{
		OmnAlarm << "Failed to delete IIL value, value : " << value << ", docid : " << docid << enderr;
		return false;
	}
	
	rslt = removeValueFromArray(value, docid);
	if(!rslt)
	{
		OmnAlarm << "Failed to delete Array value, value : " << value << ", docid : " << docid << enderr;
		return false;
	}
	mNumDocs--;
	return true;
}


bool
AosIILStrUniqueTester::removeValueFromArray(
		const OmnString &value,
		const u64 &docid)
{
	int idx = findValueDocid(value, docid);

	if(idx == -1)
	{
		OmnAlarm << "Failed to find value from Array, value :" << value << enderr;
		return false;
	}
	
	for(int i = idx; i < mNumDocs; i++)
	{
		strcpy(sgValues[i],sgValues[i+1]);
		sgDocids[i] = sgDocids[i+1];
	}
	
	strcpy(sgValues[mNumDocs-1], "");
	sgDocids[mNumDocs-1] = 0;
	return true;
}


bool
AosIILStrUniqueTester::modifyEntry()
{
	if(mNumDocs == 0)
	{
		return true;
	}
	int idx = rand() % mNumDocs;
	OmnString oldvalue = sgValues[idx];
	u64 docid = sgDocids[idx];
	
	OmnString newvalue = createNewString();
	
	bool rslt = mIIL->modifyDocSafe(oldvalue, newvalue, docid, false, false, false, mRunData);
	if(!rslt)
	{
		OmnAlarm << "Failed to modify IIL value: " << oldvalue << enderr;
		return false;
	}
	
	rslt = modifyValueFromArray(oldvalue, newvalue, docid);
	if(!rslt)
	{
		OmnAlarm << "Failed to modify Array value: " << oldvalue << enderr;
		return false;
	}
	return true;
}


bool 
AosIILStrUniqueTester::modifyValueFromArray(
		const OmnString &oldvalue, 
		const OmnString &newvalue, 
		const u64 &docid)
{
	int rslt = removeValueFromArray(oldvalue, docid);
	aos_assert_r(rslt,false);
	mNumDocs--;
	
	rslt = addValueToArray(newvalue, docid);
	aos_assert_r(rslt,false);
	mNumDocs++;

	return rslt;
}


bool
AosIILStrUniqueTester::queryIIL(const AosOpr opr, const OmnString &value)
{
	vector<OmnString> values;
	values.clear();
	bool rslt = mIIL->queryValueSafe(values, opr, value, true, mRunData);
	if(!rslt)
	{
		values.clear();
		rslt = mIIL->queryValueSafe(values, opr, value, true, mRunData);
	}
	aos_assert_r(rslt, false);	
	
	OmnString unique_value;
	int m = 0, k = 0;
	for (int i=0; i<mNumDocs; i++)
	{
		if(strcmp(unique_value.data(), sgValues[i]) == 0)
		{
			continue;
		}
		rslt = checkOprValue(opr, sgValues[i], value);
		if(!rslt)
		{
			continue;
		}
		unique_value = sgValues[i];
		k++;
	
		if(m >= (int)values.size())
		{
			OmnAlarm << "error" << enderr;
			return false;
		}

		if(values[m] != unique_value)
		{
			OmnAlarm << "unique_value error, i:" << i << ", value:" << values[m] << ",unique_value:"<< unique_value << ", k:" << k << enderr;
			return false;
		}
		m++;
	}
	if(m != (int)values.size())
	{
		OmnAlarm << "m:" << m << ",size:" << values.size() << enderr;
		return false;
	}
	return true;
}


