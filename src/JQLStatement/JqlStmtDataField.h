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
// 2013/10/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlStmtDataField_H
#define AOS_JQLStatement_JqlStmtDataField_H

#include "JQLStatement/JqlStatement.h"
#include "SEInterfaces/DataFieldType.h"
#include "Util/String.h"
#include "JQLExpr/Ptrs.h"

//typedef vector<AosExprNameValuePtr> AosExprNameValues;

class AosJqlStmtDataField : public AosJqlStatement
{
public:
	OmnString				mErrmsg;
	
private:
	//data from JQLParser
	OmnString				mFieldName;
	OmnString				mFieldType;
	OmnString 				mDataType;
	OmnString 				mValue;
	OmnString				mIdfamily;
	int64_t					mMaxLen;
	OmnString				mOffset;
	OmnString				mDefValue;
	OmnString 				mFromFieldName;
	OmnString 				mIILName;
	OmnString				mNeedSplit;
	OmnString				mNeedSwap;
	OmnString				mUseKeyAsValue;
	OmnString 				mFromName;
	OmnString 				mToName;
	OmnString 				mFieldFormater;


	// virtual field
	bool					mIsVirtual;
	OmnString 				mTableName;
	OmnString				mMapName;
	OmnString				mNOTNULL;	
	bool					mIsInfoField;

	AosJQLDataFieldTypeInfoPtr mFieldTypeInfo;
	AosExprList*			mMappingExprList;

	static std::map<OmnString, OmnString>        mFieldTypes;

public:
	AosJqlStmtDataField(const OmnString &errmsg);
	AosJqlStmtDataField();
	~AosJqlStmtDataField();

	//getter/setters
	void initVector();
	void setName(OmnString name);
	void setValue(OmnString value);
	void setValue(AosExprObj *value);
	void setFormater(OmnString formater);
	void setIsInfoField(bool flag) { mIsInfoField = flag; }
	void setType(OmnString type);
	void setDataType(AosJQLDataFieldTypeInfo *type);
	bool setType(AosJQLDataFieldTypeInfo *type);
	void setMapping(AosExprList *expr);
	void setIdfamily(OmnString idfamily);
	void setMaxLen(int64_t len);
	void setOffset(int64_t len);
	void setDefValue(OmnString def_value);
	void setFromFieldName(OmnString fieldname);
	void setIILName(OmnString iilname);
	void setNeedSplit(bool flag);
	void setNeedSwap(bool flag);
	void setUseKeyAsValue(bool flag);
	void setFromName(OmnString name);
	void setToName(OmnString name);
	void setNOTNULL(OmnString notnull);

	// virtual field
	void setTable(OmnString name);
	void setMapName(OmnString name);
	void isVirtual();

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual OmnString getErrmsg()const {return mErrmsg;};
	virtual AosJqlStatement *clone();
	virtual void dump();

	bool createDataField(const AosRundataPtr &rdata);  
	bool showDataFields(const AosRundataPtr &rdata);   
	bool describeDataField(const AosRundataPtr &rdata);
	bool dropDataField(const AosRundataPtr &rdata);   

	AosXmlTagPtr convertToXml(const AosRundataPtr &rdata);

	OmnString getIndexName(const AosRundataPtr &rdata, OmnString name);

	u64 getFieldLen(const OmnString &type);
	OmnString getFieldConf(const AosRundataPtr &rdata);
};

#endif
