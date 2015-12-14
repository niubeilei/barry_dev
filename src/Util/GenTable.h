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
// Generic Table serves as a database table. It contains a list
// of records, each of which is expressed by instances  of AosGenRecord. 
// Each record is identified through names. 
//
// Modification History:
//	11/15/2006: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Util_GenTable_h
#define Util_GenTable_h

#include "aosUtil/Types.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/ValList.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/DynArray.h"
#include "Util/GenTableUtil.h"
#include "XmlParser/Ptrs.h"


class AosGenTable : virtual public OmnRCObject
{
	OmnDefineRCObject;

public: 
	enum
	{
		eArrayInitSize = 100,
		eArrayIncSize = 50,
		eArrayMaxSize = 10000
	};

	struct Field
	{
		OmnString		mName;
		u32				mIndex;
		AosGenFieldType	mType;
	};

private:
	OmnDynArray<AosGenRecordPtr, 
				eArrayInitSize, eArrayIncSize, eArrayMaxSize>	mRecords;
	OmnDynArray<Field, 
				eArrayInitSize, eArrayIncSize, eArrayMaxSize>				mFieldDef;
	OmnDynArray<OmnString, 
				eArrayInitSize, eArrayIncSize, eArrayMaxSize>			mKeyFields;
	OmnDynArray<AosGenFieldType, 
				eArrayInitSize, eArrayIncSize, eArrayMaxSize>	mKeyType;
	AosGenRecordPtr	mDefaultRecord;
	u32				mMax;
	u32				mNumInt;
	u32				mNumStr;
	u32				mNumU32;
	u32				mNumTables;
	OmnString		mName;

public:
	AosGenTable(
			const u32 numInt, 
			const u32 numU32, 
			const u32 numStr, 
			const u32 numTable, 
			const u32 max = 0); 
	AosGenTable(const OmnXmlItemPtr &def);
	~AosGenTable();

	bool	config(const OmnXmlItemPtr &def);
	bool	clearTable();
	AosGenRecordPtr	createRecord();
	AosGenRecordPtr	appendRecord();
	bool	appendRecord(const AosGenRecordPtr &record);
	bool	removeRecord(const OmnString &name);
	bool	removeRecord(const AosGenRecordPtr &record);
	bool	removeRecords(
				const OmnDynArray<OmnString, 
					eArrayInitSize, eArrayIncSize, eArrayMaxSize> &fields, 
			 	const OmnDynArray<OmnString, eArrayInitSize, 
					eArrayIncSize, eArrayMaxSize> &values);
	bool	removeRecordRandomly();
	void	setMax(const u32 max) {mMax = max;}
	u32		getMax() const {return mMax;}
	int		entries() const;
	bool	isFull() const {return mMax > 0 && (u32)mRecords.entries() >= mMax;}

	AosGenRecordPtr	getRecord(const OmnString &name, const OmnString &key);
	AosGenRecordPtr	getRecord(const u32 fieldIndex, const OmnString &key);
	AosGenRecordPtr	getRecord(const u32 fieldIndex, const int key);
	AosGenRecordPtr	getRecord(const u32 fieldIndex, const u32 key);
	AosGenRecordPtr	getRecord(const u32 index);
	AosGenRecordPtr getRecordByKey(const AosGenRecordPtr &record);

	AosGenTablePtr	getSubtable(const OmnString &name, const OmnString &key, const u32 subtableIndex);
	AosGenTablePtr	getSubtable(const u32 fieldIndex, const OmnString &key, const u32 subtableIndex);
	AosGenTablePtr	getRandomSubtable(const u32 subtableIndex);
	AosGenTablePtr	getRandomSubtable();
	AosGenRecordPtr	getRandomRecord();
	AosGenRecordPtr	getRandomRecord(u32 &index);
	OmnString	getStr(const u32 recordIndex, const OmnString &name, const OmnString &dflt);
	OmnString	getStr(const u32 recordIndex, const u32 fieldIndex, const OmnString &dflt);
	OmnString	getRandomStr(const OmnString &name, const OmnString &dflt, u32 &index);
	OmnString	getRandomStr(const u32 fieldIndex, const OmnString &dflt, u32 &index);
	int			getRandomInt(const OmnString &name, const int dflt, u32 &index);
	int			getRandomInt(const u32 fieldIndex, const int dflt, u32 &index);
	u32			getRandomU32(const OmnString &name, bool &found, u32 &index);
	u32			getRandomU32(const u32 fieldIndex, bool &found, u32 &index);
	int			getInt(const u32 recordIndex, const OmnString &name, const int dflt);
	int			getInt(const u32 recordIndex, const u32 fieldIndex, const int dflt);
	u32			getU32(const u32 recordIndex, const OmnString &name, const u32 dflt);
	u32			getU32(const u32 recordIndex, const u32 fieldIndex, const u32 dflt);
	bool		setValue(const u32 recordIndex, const OmnString &name, const OmnString &value);
	bool		setValue(const u32 recordIndex, const u32 fieldIndex, const OmnString &value);
	bool		setValue(const u32 recordIndex, const OmnString &name, const int value);
	bool		setValue(const u32 recordIndex, const u32 fieldIndex, const int value);
	bool		setValue(const u32 recordIndex, const OmnString &name, const u32 value);
	bool		setValue(const u32 recordIndex, const u32 fieldIndex, const u32 value);
	bool 	valueExist(const OmnString &name, const OmnString &value, AosGenRecordPtr &record);
	bool 	valueExist(const u32 fieldIndex, const OmnString &value, AosGenRecordPtr &record);
	bool 	valueExist(const OmnString &, const int &value, AosGenRecordPtr &record);
	bool 	valueExist(const u32 fieldIndex, const int &value, AosGenRecordPtr &record);
	bool 	valueExist(const OmnString &, const u32 &value, AosGenRecordPtr &record);
	bool 	valueExist(const u32 fieldIndex, const u32 &value, AosGenRecordPtr &record);
	int		entryExist(const int *intv, const u16 *intIndex, const u16, 
					   const OmnString *strv, const u16 *strIndex, const u16) const; 
	bool	appendRecord(const int *intv, const u16 *intIndex, const u16, 
					   const OmnString *strv, const u16 *strIndex, const u16); 
	AosGenTablePtr	getTable(const u32 index, const OmnString &key);
	AosGenRecordPtr recordExist(const AosGenRecordPtr &record, const bool removeFlag = false);
	OmnString	toString() const;
	int			getFieldIndex(const OmnString &name, AosGenFieldType &type) const;
	OmnString	getFieldName(const u32 fieldIndex) const;
	AosGenFieldType getFieldType(const OmnString &name) const;
	OmnString	getName() const {return mName;}
	void		resetFlags(const u32 flag);
	OmnString	dumpRecordsWithNotFlagged(const u32 flag);
	bool		anyRecordNotMarked(const u32 flag);
	int			getFieldNames(OmnDynArray<OmnString, 
					eArrayInitSize, eArrayIncSize, eArrayMaxSize> &names) const;
};
#endif

