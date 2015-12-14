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
// AosGenRecord contains an array of OmnStrings, integers, and/or
// AosGenTables. 
// Each of them serves as a data field. Each data field is accessed
// through indices. When creating the instance, one tells how many
// each type of fields is. Each instance of this class serves as a record. 
//
// Modification History:
//	11/15/2006: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Util_GenRecord_h
#define Util_GenRecord_h

#include "aosUtil/Types.h"
#include "Event/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/DynArray.h"
#include "Util/GenTableUtil.h"

typedef AosGenRecordPtr (*AosRecordCreateFunc)(const AosEventObjCreatedPtr &event);

class AosGenRecord : virtual public OmnRCObject
{
	OmnDefineRCObject;
	
public:
	enum
	{
		eMaxIntegers = 200, 
		eMaxU32s = 200, 
		eMaxStrings = 200,
		eMaxSubtables = 200,
		eMaxFields = 100,
		eArrayInitSize = 100,
		eArrayIncSize = 50,
		eArrayMaxSize = 10000
	};

	enum FieldMark
	{
		eNotMarked = 0,
		eUndefined,

		eMarked,
		ePrimaryKey,
		eSubkey
	};


private: 
	OmnString	 	mName;
	int		   * 	mIntegers;
	u32 	   * 	mU32s;
	OmnString  * 	mStrs;
	AosGenTablePtr*	mSubtables;
	u32 		 	mNumInt; 
	u32 		 	mNumU32; 
	u32 		 	mNumStr;
	u32				mNumSubtables;
	u32				mFlag;

	FieldMark	*	mMarks;

	AosGenTablePtr  mTable;	// The table in which the record resides.

public:
	AosGenRecord(const AosGenTablePtr &table, 
				 const OmnString &name, 
				 const u32 numIntegers, 
				 const u32 numU32s, 
				 const u32 numStrings, 
				 const u32 numSubtables);
	AosGenRecord(const AosGenRecord &rhs);
	virtual ~AosGenRecord();

	OmnString	getName() const {return mName;}
	OmnString	toString() const;
	OmnString	getContents() const;

	bool		set(const u32 index, const OmnString &value);
	bool		set(const u32 index, const int &value);
	bool		set(const u32 index, const u32 &value);
	bool		set(const u32 index, const AosGenTablePtr &value);

	bool		set(const OmnString &fieldName, const OmnString &value);
	bool		set(const OmnString &fieldName, const int &value);
	bool		set(const OmnString &fieldName, const u32 &value);
	bool		set(const OmnString &fieldName, const AosGenTablePtr &value);

	void		setFlag(const u32 flag) {mFlag = flag;}
	u32			getFlag() const {return mFlag;}

	bool		getStr(const u32 index, OmnString &value) const;
	bool		getInt(const u32 index, int &value) const;
	bool		getU32(const u32 index, u32 &value) const;
	AosGenTablePtr 	getTable(const u32 index);

	bool		getValue(const OmnString &fieldName, OmnString &value) const;
	bool		getStr(const OmnString &fieldName, OmnString &value) const;
	bool		getInt(const OmnString &fieldName, int &value) const;
	bool		getU32(const OmnString &fieldName, u32 &value) const;
	AosGenTablePtr 	getTable(const OmnString &fieldName);

	bool		isSame(const u32 index, const OmnString &value);
	bool		isSame(const u32 index, const int value);
	bool		isSame(const u32 index, const u32 value);
	bool		isSame(const u32 index, const AosGenTablePtr &value);

	bool		isSame(const OmnString &fieldName, const OmnString &value);
	bool		isSame(const OmnString &fieldName, const int value);
	bool		isSame(const OmnString &fieldName, const u32 value);
	bool		isSame(const OmnString &fieldName, const AosGenTablePtr &value);
	bool		isSame(const OmnDynArray<OmnString, 
						eArrayInitSize, eArrayIncSize, eArrayMaxSize> &fields, 
					   const OmnDynArray<OmnString, 
					   	eArrayInitSize, eArrayIncSize, eArrayMaxSize> &values);

	bool		isSame(const int *intv, const u16 *intIndex, const u16, 
					   const OmnString *strv, const u16 *strIndex, const u16) const; 

	// 
	// The following three functions are used to maintain marks
	// Marks are used to indicate whether a field value has been
	// set. Use 'resetMarks()' to reset all the marks as not being
	// set. Use 'isFieldSet(...)' to check whether a field value has 
	// been set, and 'markField(...)' to mark a field as its field
	// value being set. 
	//
	void		resetMarks();
	FieldMark	getFieldMark(const AosGenFieldType type, const OmnString &fname);
	FieldMark	getFieldMark(const AosGenFieldType type, const u32 index);
	bool		isFieldMarked(const OmnString &fname);
	bool		isFieldMarked(const AosGenFieldType type, const u32 index);
	bool		markField(const AosGenFieldType type, 
						  const u32 index, 
						  const bool force = false, 
						  FieldMark mark = eMarked);
	bool		markField(const OmnString &fieldName, 
						  const bool force = false, 
						  FieldMark mark = eMarked);
	void		removeMarks();
	bool		recordMatch(const AosGenRecordPtr &record) const;
	bool 		fieldMatch(const OmnString &fieldName, 
						 AosGenFieldType fieldType, 
						 const AosGenRecordPtr &record);

private:
	bool		createFields();
	bool 		getFieldIndex(u32 &index, 
						 const OmnString &name, 
						 AosGenFieldType &type) const;
};

#endif

