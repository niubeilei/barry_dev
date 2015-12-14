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
// This action sets a value to rundata:
//
// Modification History:
// 04/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActSqlServerToDb.h"

#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DocSelector/DocSelector.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DocSelObj.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/OmnNew.h"
#include "Util/DynArray.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "QueryClient/QueryClient.h"
#include "SyBaseMgr/SyBaseMgr.h"
#include "API/AosApi.h"
#include <string>
#include <vector>
using namespace std;

AosActSqlServerToDb::AosActSqlServerToDb(const bool flag)
:
AosSdocAction(AOSACTTYPE_SQLSERVERTODB, AosActionType::eSqlServerToDb, flag),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
}


AosActSqlServerToDb::~AosActSqlServerToDb()
{
}


bool
AosActSqlServerToDb::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	mEvent = eNoEvent;
	aos_assert_r(mRundata, false);
	bool rslt = toImport(mRundata);
	if (!rslt)
	{
		OmnAlarm << "Faild to Import Data" << enderr;
	}
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (mEvent == eNoEvent)
		{
			mCondVar->wait(mLock);
		}

		switch (mEvent)
		{
		case eFinished:
			 state = OmnThrdStatus::eExit;
			 mLock->unlock();
			 return true;

		case eNoEvent:
			 break;

		default:
			 OmnAlarm << "Unrecognized event: " << mEvent << enderr;
			 mLock->unlock();
			 return false;
		}
		mEvent = eNoEvent;
		mLock->unlock();
	}
	return true;
}

bool
AosActSqlServerToDb::signal(const int threadLogicId)
{
	mLock->lock();
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool
AosActSqlServerToDb::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}

bool	
AosActSqlServerToDb::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	//format <action zky_type="sqlservertodb">
	//			<connectionstring host="192.168.99.231" port="1433" database="Testers" username="sa" passwd="123456"/>
	//			<imports>
	//				<import>
	//					<sqlstring>select * from student</sqlstring>
	//					<dbconfig container="xxxx"/>
	//				</import>
	//				<import>
	//					...
	//					...
	//				</import>
	//			<imports/>
	//		 </action>
	if (!sdoc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	AosXmlTagPtr constr = sdoc->getFirstChild("connectionstring");
	aos_assert_rr(constr, rdata, false);
	mHost = constr->getAttrStr("host", "");
	aos_assert_rr(mHost != "", rdata, false);
	mPort = constr->getAttrU32("port", 0);
	mDbName = constr->getAttrStr("database", "");
	aos_assert_rr(mDbName != "", rdata, false);
	mUserName = constr->getAttrStr("username", "sa");
	mPassWord = constr->getAttrStr("passwd", "");
	mImports = sdoc->getFirstChild("imports");	
	if (!mImports)
	{
		return true;
	}
	mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
	if (mThread)
	{
		// Should never happen
		AosSetErrorU(rdata, "internal_error");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	OmnThreadedObjPtr thisptr(this, false);
	mThread = OmnNew OmnThread(thisptr, "import sqlserver", 0, false, true, __FILE__, __LINE__);
	mThread->start();
	rdata->setOk();
	return true;
}


AosActionObjPtr 
AosActSqlServerToDb::clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	aos_assert_r(def, 0);

	try
	{
		return OmnNew AosActSqlServerToDb(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;

}

bool
AosActSqlServerToDb::toImport(const AosRundataPtr &rdata)
{
	/*
	AosSyBaseMgrPtr syBase = OmnNew AosSyBaseMgr(mHost, mUserName, mPassWord, mDbName , mPort);
	aos_assert_rr(syBase, rdata, false);	
	syBase->initConnection();
	bool status = syBase->getConnectionStatus();
	if (!status)
	{
		return false;
	}
	AosXmlTagPtr import = mImports->getFirstChild();
	vector< vector<AosColumnPtr> > dataset;
	while(import)
	{
		OmnString sqlstr = import->getNodeText("sqlstring");
		aos_assert_rr(sqlstr != "", rdata, false);
		OmnString container = import->xpathQuery("dbconfig/@container");
		aos_assert_rr(container != "", rdata, false);
		dataset = syBase->executeDataSet(sqlstr);
		AosXmlParser parser;
		AosXmlTagPtr doc = parser.parse("<doc />", "" AosMemoryCheckerArgs);
		aos_assert_rr(doc, rdata, false);
		for (u32 i=0; i<dataset.size(); i++)
		{
			vector<AosColumnPtr> cols = dataset[i];
			AosXmlTagPtr record = doc->clone(AosMemoryCheckerArgsBegin);
			for (u32 j=0; j<cols.size(); j++)
			{
				OmnString name = cols[j]->getColName();
				OmnString value = cols[j]->getColBuffer();
				value.normalizeWhiteSpace(true, true);
				int type = cols[j]->getColType();
				if (type == SYBTEXT || type == SYBNTEXT)
				{
					record->addNode1(name);
					AosXmlTagPtr textNode = record->getFirstChild(name);
					textNode->setNodeText(value, true);
				}
				else
				{
					record->setAttr(name, value);
				}
			}
			record->setAttr(AOSTAG_PARENTC, container);
			record->setAttr(AOSTAG_CTNR_PUBLIC, "true");
			record->setAttr(AOSTAG_PUBLIC_DOC, "true");
			AosXmlTagPtr createdoc = AosCreateDoc(record, rdata);
			aos_assert_rr(createdoc, rdata, false);
			OmnScreen << createdoc->toString() << endl;
		}
		dataset.clear();
		import = mImports->getNextChild();
	}
	*/
	OmnNotImplementedYet;

	return true;
}
