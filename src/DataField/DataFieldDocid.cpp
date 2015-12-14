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
// Modification History:
// 12/20/2013 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "DataField/DataFieldDocid.h"

#include "API/AosApiG.h"
#include "IILUtil/IILId.h"
#include "IILUtil/IILUtil.h"
#include "JQLStatement/JqlStmtSchema.h"
#include "XmlUtil/DocTypes.h"
#include "StorageEngine/StorageEngineMgr.h"
#include "StorageEngine/SengineGroupedDoc.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/StorageEngineObj.h"


AosDataFieldDocid::AosDataFieldDocid(const bool reg)
:
AosDataField(AosDataFieldType::eDocid, AOSDATAFIELDTYPE_DOCID, reg),
mDocids(0),
mNumDocids(0),
mRcdTypeKey(""),
mDocSize(-1),
mNumCubes(0)
{
}


AosDataFieldDocid::AosDataFieldDocid(const AosDataFieldDocid &rhs)
:
AosDataField(rhs),
mDocids(rhs.mDocids),
mNumDocids(rhs.mNumDocids),
mRcdTypeKey(rhs.mRcdTypeKey),
mDocSize(rhs.mDocSize),
mRecordDoc(rhs.mRecordDoc),
mNumCubes(rhs.mNumCubes)
{
}


AosDataFieldDocid::~AosDataFieldDocid()
{
}


bool
AosDataFieldDocid::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	mNumCubes = AosGetNumCubes(); 
	bool rslt = AosDataField::config(def, record, rdata);
	aos_assert_r(rslt, false);

	OmnString recordObjid = def->getAttrStr("zky_record_objid", "");
	if (recordObjid != "")
	{
		AosXmlTagPtr record_doc;
		AosXmlTagPtr doc = AosJqlStatement::getDocByObjid(rdata, recordObjid);      
		aos_assert_r(doc, NULL);

		AosXmlTagPtr rcd_conf = doc->getFirstChild("datarecord");
		aos_assert_r(rcd_conf, NULL);

		OmnString name = rcd_conf->getAttrStr("zky_datarecord_name");
		if (name != "")
		{
			record_doc = AosGetDocByKey(rdata, "", "zky_datarecord_name", name);
		}
		else
		{
			record_doc = rcd_conf;      
		}
		aos_assert_r(record_doc, NULL);

		//create datarecord
		AosDataRecordObjPtr rcd = AosDataRecordObj::createDataRecordStatic(record_doc, 
				NULL, rdata AosMemoryCheckerArgs); 
		aos_assert_r(rcd, NULL);

		mRcdTypeKey = rcd->getRecordName(); 
		aos_assert_r(mRcdTypeKey != "", 0);

		//mDocSize = rcd->getEstimateRecordLen();
		//aos_assert_r(mDocSize > 0, 0);

		mRecordDoc = rcd->getRecordDoc();
		aos_assert_r(mRecordDoc, NULL);
	}
	else
	{
		mRcdTypeKey = record->getRecordName();
		aos_assert_r(mRcdTypeKey != "", 0);

		//mDocSize = record->getEstimateRecordLen();
		if (mDocSize <= 0) mDocSize = -1;
		//aos_assert_r(mDocSize > 0, 0);

		mRecordDoc = record->getRecordDoc();
		aos_assert_r(mRecordDoc, 0);
	}

	return true;
}


bool
AosDataFieldDocid::getValueFromRecord(
		AosDataRecordObj* record,
		const char * data,
		const int len,
		int &idx,
		AosValueRslt &value, 
		const bool copy_flag,
		AosRundata* rdata)
{
	if (mDocSize == -1)
	{
		mDocSize = record->getEstimateRecordLen();
		aos_assert_r(mDocSize > 0, false);
	}

	u64 docid = getNextDocid(rdata, record);	
	aos_assert_r(docid != 0, false);
	value.setU64(docid);
	record->setDocid(docid);
	return true;
}


AosDataFieldObjPtr 
AosDataFieldDocid::clone(AosRundata *rdata) const
{
	return OmnNew AosDataFieldDocid(*this);
}


AosDataFieldObjPtr
AosDataFieldDocid::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosDataFieldDocid * field = OmnNew AosDataFieldDocid(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);

	return field;
}


u64
AosDataFieldDocid::getNextDocid(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	if (mNumDocids <= 0)
	{
		if (mGroupDocids.empty())
		{
			bool rslt = getDocid(rdata);
			aos_assert_r(rslt, false);
		}
		mDocids = mGroupDocids.front();
		mGroupDocids.pop();
		mNumDocids = AosStorageEngineObj::eMaxDocPerGroup;
	}
	u64 docid = mDocids;
	mDocids += mNumCubes;
	mNumDocids--;
	return docid;
}

bool
AosDataFieldDocid::getDocid(AosRundata* rdata)
{
	bool rslt = false;
	u32 group_size = AosStorageEngineObj::eMaxDocPerGroup;
	mNumDocids = group_size;
	u32 cube_id = 0;

	u64 init_value = AosIILID::getDGDIDInitValue(group_size, cube_id);
	u64 inc_value = AosIILID::getDGDIDIncValue(group_size);
	u64 iil_id = AosIILID::getDGDIDIILID();
	OmnString key;
	key << group_size << AosIILFIELDSEP << cube_id;
	rslt = AosIILClientObj::getIILClient()->
					incrementDocid(iil_id, key, mDocids, inc_value, init_value, true, rdata);
	aos_assert_r(rslt, false);
	mDocids = mDocids * mNumCubes;
	AosDocType::setDocidType(AosDocType::eGroupedDoc, mDocids);

	for (u32 i = 0; i < mNumCubes; i++)
	{
		mGroupDocids.push(mDocids + i);
	}
	return true;
}

