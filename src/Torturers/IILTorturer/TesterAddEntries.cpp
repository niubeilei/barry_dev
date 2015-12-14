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
//   
//
// Modification History:
//
////////////////////////////////////////////////////////////////////////////
#include "Torturers/IILTorturer/TesterAddEntries.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Torturers/XmlDocTorturer/TesterLogin.h"
#include "SengTorUtil/SengTesterMgr.h"
#include "Torturers/XmlDocTorturer/Ptrs.h"
#include "SengTorUtil/SengTesterThrd.h"
#include "SengTorUtil/SengTesterFileMgr.h"
#include "SengTorUtil/StUtil.h"
#include "SengTorUtil/StDoc.h"
#include "SengTorUtil/StOprTranslator.h"
#include "SEModules/SeRundata.h" 
#include "SmartDoc/SmartDoc.h"
#include "Tester/TestPkg.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "SEUtil/Objid.h"
#include "SEUtil/Objid.h"
#include "SengTorUtil/Ptrs.h"
#include "Thrift/Jimo_types.h"


extern shared_ptr<AosJimoAPIClient> gThriftClient;


AosIILAddEntriesTester::AosIILAddEntriesTester()
:
AosSengTester(AosSengTester_AddEntry, "add_entry", AosTesterId::eAddEntry),
mIILCache(OmnNew AosStIILCache())
{
	init();
}


AosIILAddEntriesTester::AosIILAddEntriesTester(const AosIILAddEntriesTester &rhs)
:
AosSengTester(AosSengTester_AddEntry, "add_entry", AosTesterId::eAddEntry)
{

	mIILCache = rhs.mIILCache;
	mMaxNumIILs = rhs.mMaxNumIILs;
	mWeightAddEntryByName = rhs.mWeightAddEntryByName;
	mWeightAddEntryByID = rhs.mWeightAddEntryByID;
	mWeightCreateIIL = rhs.mWeightCreateIIL;
	mWeightCheckIIL = rhs.mWeightCheckIIL;
	mTotalWeights = rhs.mTotalWeights;

	mOperators = rhs.mOperators;
	mRdata = rhs.mRdata;
}


AosSengTesterPtr
AosIILAddEntriesTester::clone()
{
	return OmnNew AosIILAddEntriesTester(*this);
}


AosIILAddEntriesTester::~AosIILAddEntriesTester()
{
}


bool 
AosIILAddEntriesTester::init()
{
	mMaxNumIILs = eDftMaxNumIILs;
	mWeightAddEntryByName = eDftWeightAddEntryByName;
	mWeightAddEntryByID = eDftWeightAddEntryByID;
	mWeightCreateIIL = eDftWeightCreateIIL;
	mWeightCheckIIL = eDftWeightCheckIIL;
	mTotalWeights = mWeightAddEntryByName +
				mWeightAddEntryByID +
				mWeightCreateIIL +
				mWeightCheckIIL;

	calculateWeights();
	mRdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	return true;
}


bool 
AosIILAddEntriesTester::configTester(const AosXmlTagPtr &config)
{
    // <config ...>
    //      <testers>
    //          <iil_add_entries weight="30"                
    //              add_by_iilname_weight="45"
    //              add_by_iilid_weight="45"
    //              create_iil_weight="45"
    //              check_iil_weight="45"
	//              />
    //      </testers>
    //  </config>
	if (!config) return true;
	AosXmlTagPtr tag = config->getFirstChild("testers");
	if (!tag) return true;

	tag = tag->getFirstChild("iil_add_entries");
	if (!tag) return true;
	
	if (!AosSengTester::configTester(tag)) return false; 

	mWeightAddEntryByName = tag->getAttrInt("add_by_iilname_weight", mWeightAddEntryByName);
	aos_assert_r(mWeightAddEntryByName >=0, false);

	mWeightAddEntryByID = tag->getAttrInt("add_by_iilid_weight", mWeightAddEntryByID);
	aos_assert_r(mWeightAddEntryByID >=0, false);

	mWeightCreateIIL = tag->getAttrInt("create_iil_weight", mWeightCreateIIL);
	aos_assert_r(mWeightCreateIIL >=0, false);

	mWeightCheckIIL = tag->getAttrInt("check_iil_weight", mWeightCheckIIL);
	aos_assert_r(mWeightCheckIIL >=0, false);

	calculateWeights();
	return true;
}


bool
AosIILAddEntriesTester::calculateWeights()
{
	mTotalWeights = mWeightAddEntryByName + 
			 mWeightAddEntryByID + 
			 mWeightCreateIIL +
			 mWeightCheckIIL;
	mOperators.clear();

	for (int i=0; i<mWeightAddEntryByName; i++)
	{
		mOperators.push_back(eAddEntryByName);
	}

	for (int i=0; i<mWeightAddEntryByID; i++)
	{
		mOperators.push_back(eAddEntryByID);
	}

	for (int i=0; i<mWeightCreateIIL; i++)
	{
		mOperators.push_back(eCreateIIL);
	}

	for (int i=0; i<mWeightCheckIIL; i++)
	{
		mOperators.push_back(eCheckIIL);
	}

	return true;
}


bool 
AosIILAddEntriesTester::test()
{
	bool rslt;
	if(!checkLogin())
	{
		rslt = login();
		aos_assert_r(rslt, false);
	}

	JmoCallData call_data= mMgr->getCallData();
	AosStIILPtr iil = mIILCache->pickIIL(gThriftClient, call_data);
	aos_assert_r(iil, false);

	OmnString iilname = iil->getIILName();
	u64 iilid = iil->getIILID();

	Opr opr = pickOperation();
	switch (opr)
	{
	case eAddEntryByName:
		 rslt = addEntryByName(iilname);
		 aos_assert_r(rslt, false);
		 break;

	case eAddEntryByID:
		 rslt = addEntryByID(iilid);
		 aos_assert_r(rslt, false);
		 break;

	case eCreateIIL:
		 rslt = createIIL();
		 aos_assert_r(rslt, false);
		 break;

	case eCheckIIL:
		 rslt = checkIIL();
		 aos_assert_r(rslt, false);
		 break;

	default:
		 OmnAlarm << "Unrecognized: " << opr << enderr;
		 return false;
	}
	return true;
}


AosIILAddEntriesTester::Opr
AosIILAddEntriesTester::pickOperation()
{
	aos_assert_r(mTotalWeights > 0, eInvalidOpr);
	int idx = rand() % mTotalWeights;
	return mOperators[idx];
}


bool
AosIILAddEntriesTester::addEntryByName(OmnString &iilname)
{
	u64 docid;
	bool key_unique, docid_unique;
	JmoRundata rdata;
	bool rslt;

	JmoCallData call_data = mMgr->getCallData();
	std::string iil_name = iilname.data();
	int num = OmnRandom::intByRange(
							1, 3, 100,
							4, 20, 50,
							21, 100, 20,
							101, 1000, 5);
//num = 5;
	aos_assert_r(num > 0, false);

	AosStIILPtr iil = mIILCache->getIILByIILName(iilname);
	aos_assert_r(iil, false);

	if (iil->isStrIIL())
	{
		std::string key;
		bool added;
		for (int i=0; i<num; i++)
		{
			rslt = iil->pickEntry(key, docid, key_unique, 
							docid_unique, added, mRdata.getPtr());
			aos_assert_r(rslt, false);

			gThriftClient->addEntryByKey_IILName(rdata, call_data, iil_name,
										key, docid, key_unique, docid_unique);
			aos_assert_r(rdata.rcode == JmoReturnCode::SUCCESS, false);
		}
	}
	else
	{
		u64 id;
		bool added;
		for(int i=0; i<num; i++)
		{
			rslt = iil->pickEntry(id, docid, key_unique, 
						docid_unique, added, mRdata.getPtr());
			aos_assert_r(rslt, false);

			gThriftClient->addEntryByID_IILName(rdata, call_data, iil_name,
								id, docid, key_unique, docid_unique);
			aos_assert_r(rdata.rcode == JmoReturnCode::SUCCESS, false);
		}
	}
	return true;
}


bool
AosIILAddEntriesTester::addEntryByID(u64 &iilid)
{
	u64 docid;
	bool key_unique, docid_unique;
	JmoRundata rdata;
	bool rslt;
	JmoCallData call_data= mMgr->getCallData();
	AosStIILPtr iil = mIILCache->getIILByIILID(iilid);
	aos_assert_r(iil, false);
	int num = OmnRandom::intByRange(1, 5, 30, 
									6, 30, 100, 
									31, 100, 10);
	aos_assert_r(num > 0, false);
	if (iil->isStrIIL())
	{
		std::string key;
		bool added;
		for (int i=0; i<num; i++)
		{
			rslt = iil->pickEntry(key, docid, key_unique, docid_unique, added, mRdata.getPtr());
			aos_assert_r(rslt, false);
			gThriftClient->addEntryByKey_IILID(rdata, call_data, (i64)iilid, key, docid, key_unique, docid_unique);
			aos_assert_r(rdata.rcode == JmoReturnCode::SUCCESS, false);
		}
	}
	else
	{
		u64 id;
		bool added;
		for (int i=0; i<num; i++)
		{
			iil->pickEntry(id, docid, key_unique, docid_unique, added, mRdata.getPtr());
			gThriftClient->addEntryByID_IILID(rdata, call_data, (i64)iilid, id, docid, key_unique, docid_unique);
			aos_assert_r(rdata.rcode == JmoReturnCode::SUCCESS, false);
		}
	}

	return true;
}


bool
AosIILAddEntriesTester::createIIL()
{
	if (!checkLogin())   
	{
		bool rslt = login();
		aos_assert_r(rslt, false);
	}

	if (mIILCache->getNumIILs() > mMaxNumIILs) return true;

	JmoCallData call_data = mMgr->getCallData();
	mIILCache->createIIL(gThriftClient, call_data);

	return true;
}


bool
AosIILAddEntriesTester::checkIIL()
{
	// This function picks one IIL to check.
	JmoCallData call_data= mMgr->getCallData();
	AosStIILPtr iil = mIILCache->pickIIL(gThriftClient, call_data);
	aos_assert_r(iil, false);

	i64 page_size = iil->getIILSize();
	OmnString iilname = iil->getIILName();
	u64 iilid = iil->getIILID();
	bool is_str_iil = iil->isStrIIL();

	// 1. Load the entire IIL. 
	JmoRundata jmo_rdata;
	JmoQueryOperator::type opr = JmoQueryOperator::AN;
	if (is_str_iil)
	{
		gThriftClient->queryIILByIILName02(jmo_rdata, 
						call_data, iilname, "", "", "", 0, 
						opr, false, page_size);
	}
	else
	{
		gThriftClient->queryIILByIILName03(jmo_rdata, 
						call_data, iilname, 0 , 0, 0, 0, 
						opr, false, page_size);
	}
	aos_assert_r(jmo_rdata.rcode == JmoReturnCode::SUCCESS, false);

	OmnString vv;
	u64 dd;
	bool key_unique = iil->getKeyUnique();
	vector<int> keyPos;
	i64 idx = 0;
	int num_docs = jmo_rdata.double_value;
	//aos_assert_r(page_size == num_docs*2, false);

	for (int i=0; i<page_size/2; i++)
	{
		aos_assert_r(idx < page_size, false);
		keyPos.clear();
		bool rslt = iil->nextValue(idx, vv, dd);
		aos_assert_r(rslt, false);
		aos_assert_r(jmo_rdata.rcode == JmoReturnCode::SUCCESS, false);

		int findKeyNum = 0;
		if (is_str_iil)
		{
			aos_assert_r(num_docs+1 == (i32)jmo_rdata.str_values.size(), false);
			aos_assert_r(num_docs+1 == (i32)jmo_rdata.int_values.size(), false);
			for (int i=0; i<num_docs; i++)
			{
				OmnString value = jmo_rdata.str_values[i];
				if (value == vv) 
				{
					findKeyNum++;
					keyPos.push_back(i);
				}
			}
			if (key_unique)
			{
				aos_assert_r(findKeyNum==1, false);
				int docidPos = keyPos[0];
				aos_assert_r(dd == (u64)jmo_rdata.int_values[docidPos], false);
			}
			else
			{
				int findDocidNum = 0;
				aos_assert_r(findKeyNum>=1, false);
				for (u32 i=0; i < keyPos.size(); i++)
				{
					int docidPos = keyPos[i];
					if ((u64)jmo_rdata.int_values[docidPos] == dd) findDocidNum++;
				}
				aos_assert_r(findDocidNum>=1, false);
			}
		}
		else
		{
			aos_assert_r((num_docs+1)*2 == (i32)jmo_rdata.int_values.size(), false);
			for (int i=0; i<num_docs*2; i+=2)
			{
				i64 key = jmo_rdata.int_values[i];
				OmnString value;
				value << key;
				i64 docid = jmo_rdata.int_values[i+1];

				if (value == vv) 
				{
					findKeyNum++;
					keyPos.push_back(i);
				}
			}
			if (key_unique)
			{
				aos_assert_r(findKeyNum==1, false);
				int docidPos = keyPos[0];
				aos_assert_r(dd == (u64)jmo_rdata.int_values[docidPos+1], false);
			}
			else
			{
				int findDocidNum = 0;
				aos_assert_r(findKeyNum>=1, false);
				for (int i=0; i<(i32)keyPos.size(); i++)
				{
					int docidPos = keyPos[i];
					if ((u64)jmo_rdata.int_values[docidPos+1] == dd) findDocidNum++;
				}
				aos_assert_r(findDocidNum>=1, false);
			}
		}
	} 

	gThriftClient->deleteIIL(jmo_rdata, call_data, iilname);
	aos_assert_r(jmo_rdata.rcode == JmoReturnCode::SUCCESS, false);

	bool rslt = mIILCache->deleteIIL(iilname);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIILAddEntriesTester::login()
{
	string username = "root";
	string password = "12345";
	string userdomain = "zky_sysuser";
	OmnString conf;
	conf <<  "<loginobj opr=\"login\" "
		 << "container=\"" << userdomain << "\" "
	 	 << "login_group=\"" << userdomain << "\" "
	 	 << "zky_uname=\"" << username << "\">"
	 	 << "<zky_passwd__n><![CDATA[" << password << "]]></zky_passwd__n>"
	 	 << "</loginobj>";     
	 
	bool rslt = mMgr->login(conf);
	aos_assert_r(rslt, false);  
	return true;
}


bool
AosIILAddEntriesTester::checkLogin()
{
	JmoCallData call_data= mMgr->getCallData();
	if (call_data.session_id != "") return true; 
	return false;
}

