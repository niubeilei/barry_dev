////////////////////////////////////////////////////////////////////////////
// 2011/5/25	Created by ken
////////////////////////////////////////////////////////////////////////////

#include "SearchEngine/TesterNew/IILStrLikeTester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/DocTags.h"
#include "SearchEngine/Ptrs.h"
#include "IILMgr/IILMgr.h"
#include "IILMgr/IILStr.h"
#include "IILMgr/IIL.h"
#include "IILClient/IILClient.h"
#include "SearchEngine/SeCommon.h"
#include "SearchEngine/DocServer.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util/UtUtil.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"


const OmnString sgSiteid = "100";
const int sgMaxDocs = 1000000;
const int sgMaxStrLen = 80;

static char *		sgValues[sgMaxDocs];
static u64			sgDocids[sgMaxDocs];
static char *		sgIILValues[sgMaxDocs];
static u64			sgIILDocids[sgMaxDocs];



AosIILStrLikeTester::AosIILStrLikeTester()
:
mTotal(0),
mAddOpr(0),
mDelOpr(0),
mModOpr(0)
{
	mName = "DocServerTester";

	mRunData =  OmnApp::getRundata();
	// Initialize sgValues
	for (int i=0; i<sgMaxDocs; i++)
	{
		sgDocids[i] = 0;
		sgValues[i] = new char[sgMaxStrLen+2];
		sgIILDocids[i] = 0;
		sgIILValues[i] = new char[sgMaxStrLen+2];
	}
	mDocid = 10000;
	AosIILType iiltype = eAosIILType_Str;
//	mIIL = (AosIILStr*)AosIILMgr::getSelf()->getIILPublic("ken_test__a", true, eAosIILType_Str).getPtr();
	u64 iilid = AosIILMgr::getSelf()->createIILPublic(
						iiltype,
						false,
						mRunData);
	AosIILPtr iilptr;// = AosIILMgr::getSelf()->loadIILByIDPublic(iilid,iiltype,mRunData);
	mIIL = (AosIILStr*)iilptr.getPtr();
}


bool AosIILStrLikeTester::start()
{
	cout << "Start OmnString Tester ..." << endl;
	torture();
	return true;
}


bool
AosIILStrLikeTester::torture()
{
	srand(8888);
	int total = 100000000;
	int group = 0;
	mTotal = 0;
	mAddOpr = 0;
	mDelOpr = 0;
	mModOpr = 0;
	mNumDocs = 0;
	for (int i=0; i<total; i++)
	{
		if (++group == 1000)
		{
			mTotal += group;
			group = 0;
			cout << "mTotal=" << mTotal << " Add=" << mAddOpr << " Del=" << mDelOpr << " Mod=" << mModOpr << " mNumDocs" << mNumDocs << endl;
		}
	
		int opr = rand() % 100;
		if(opr < 40)
		{
		//	cout << "add" << endl;
			aos_assert_r(addEntry(), false);
			mAddOpr++;
		}
		else if(opr < 70)
		{
		//	cout << "del" << endl;
			aos_assert_r(deleteEntry(), false);
			mDelOpr++;
		}
		else
		{
		//	cout << "mdf" << endl;
			aos_assert_r(modifyEntry(), false);
			mModOpr++;
		}
	}
	return true;
}


OmnString 
AosIILStrLikeTester::createNewString()
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
AosIILStrLikeTester::addEntry()
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
		OmnAlarm << "fail to add doc: " << docid << ":" << value<< enderr;
		return false;
	}

	rslt = addValueToArray(value, docid);
	if(!rslt)
	{
		OmnAlarm << "fail to add value to array: " << docid << ":" << value<< enderr;
		return false;
	}
	mNumDocs++;

	aos_assert_r(verifyIIL(value, docid), false);
	return true;
}


bool
AosIILStrLikeTester::addValueToArray(
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
AosIILStrLikeTester::findValue(const OmnString &value, const u64 &docid)
{
	int idx, rslt;
	for(idx=0; idx<mNumDocs; idx++)
	{
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
AosIILStrLikeTester::findValueDocid(
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
AosIILStrLikeTester::getLikeValue(const OmnString &value)
{
	int len = value.length();
	int idx = rand() % len;
	int slen = rand() % (len - idx);
	OmnString rslt = value.substr(idx, idx + slen);
	return rslt;
}


bool
AosIILStrLikeTester::verifyIIL(const OmnString &value, const u64 &docid)
{
	return verifyLike(value, docid);	
}

bool
AosIILStrLikeTester::verifyLike(const OmnString &value, const u64 &docid)
{
	OmnString likevalue = getLikeValue(value);

	int idx = -10;
	int iilidx = -10;
	bool isUnique = false;
	bool rslt;
	u64 iildocid;
	char *r;
	int tmpidx;
	for (int i=0; i<mNumDocs; i++)
	{
		r = strstr(sgValues[i], (char *)likevalue.data());
		if(r == NULL)
		{
			continue;
		}
		tmpidx = idx;
		rslt = mIIL->nextDocidSafe(idx, iilidx, false, 
		 		eAosOpr_like, likevalue, iildocid, isUnique, mRunData);
		if(!rslt)
		{
			OmnAlarm << "fail to get docid form iil, likevalue: " << likevalue << enderr;
			return false;
		}

		if(sgDocids[i] != iildocid)
		{
			idx = tmpidx;
			rslt = mIIL->nextDocidSafe(idx, iilidx, false, 
		 		eAosOpr_like, likevalue, iildocid, isUnique,mRunData);

			mIIL->copyData(
				sgIILValues, 
				sgIILDocids, 
				(int)AOSSENG_TORTURER_MAX_DOCS, 
				mNumDocs,
				mRunData);
			
			OmnAlarm << "fail to match the docid, i:" << i << ", sgDocids: " << sgDocids[i] << ", iildocid:" << iildocid
					 << ", likevalue :" << likevalue << ", sgValues:" << sgValues[i] << ", IILValue:" << sgIILValues[i]
					 << ", IILDocid:" << sgIILDocids[i] << enderr;
			return false;
		}
	}
	return true;
}


bool 
AosIILStrLikeTester::deleteEntry()
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

	aos_assert_r(verifyIIL(value, docid), false);
	return true;
}


bool
AosIILStrLikeTester::removeValueFromArray(
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
AosIILStrLikeTester::modifyEntry()
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

	aos_assert_r(verifyIIL(newvalue, docid), false);
	return true;
}


bool 
AosIILStrLikeTester::modifyValueFromArray(
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
