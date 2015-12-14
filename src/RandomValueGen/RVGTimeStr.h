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
// 02/18/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_RandomValueGen_RVGTimeStr_h
#define Omn_RandomValueGen_RVGTimeStr_h

#include "aosUtil/Types.h"
#include "Random/RandomTypes.h"
#include "Parms/RVG.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/IpAddr.h"
#include "Util/ValList.h"
#include "Random/Ptrs.h"

class AosRVGTimeStr : public AosRVG
{
	OmnDefineRCObject;

	enum
	{
		eGenerateNotInTable,
		eGenerateInTable,
		eGenerateInvalid,
		
		eDefaultNotInTableWeight = 30,
		eDefaultInTableWeight = 30,
		eDefaultInvalidWeight = 40,
		
		eDefaultSelectFromTablePct = 50,
		eDefaultSelectFromRecordPct = 50,
		eDefaultMinInvalidStrLen = 1,
		eDefaultMaxInvalidStrLen = 16
	};

private:
	OmnString	mBadValue;
	AosRandomIntegerPtr	mBadValueSelector;
	int			mSelectFromTablePct;

	int					mNotInTableWeight;
	int					mInTableWeight;
	int					mInvalidValueWeight;
	int					mMinInvalidStrLen;
	int					mMaxInvalidStrLen;
	AosTimeStrFormat_e	mFormat;

public:
	AosRVGTimeStr(const AosCliCmdTorturerPtr &cmd);
	AosRVGTimeStr(const OmnString &name, 
			   const OmnString &fname, 
			   const AosRVGKeyType keyType,
			   const bool correctOnly, 
			   const OmnString &badValue, 
			   const AosGenTablePtr &table,
			   const OmnDynArray<AosGenTablePtr> &depTables,
			   const OmnDynArray<OmnString> &fieldNames);
	virtual ~AosRVGTimeStr();

	virtual bool nextStr(OmnString &value, 
					const AosGenTablePtr &data, 
					const AosGenRecordPtr &record,
					bool &isGood,
					const bool correctOnly,
					const bool selectFromRecord,
					AosRVGReturnCode &rcode, 
					OmnString &errmsg);
	virtual bool nextInt(
					int &value, 
					const AosGenTablePtr &data, 
					const AosGenRecordPtr &record, 
					bool &isCorrect, 
					const bool correctOnly,
					const bool selectFromRecord,
					AosRVGReturnCode &rcode, 
					OmnString &errmsg);
	virtual bool nextInt(
					u32 &value, 
					const AosGenTablePtr &data, 
					const AosGenRecordPtr &record, 
					bool &isCorrect, 
					const bool correctOnly,
					const bool selectFromRecord,
					AosRVGReturnCode &rcode, 
					OmnString &errmsg);

	static AosRVGTimeStrPtr	createInstance(
									const AosCliCmdTorturerPtr &cmd,
									const OmnXmlItemPtr &def, 
									const OmnString &cmdTag, 
									OmnVList<AosGenTablePtr> &tables);

	virtual bool config(const OmnXmlItemPtr &def, 
					const OmnString &cmdTag, 
					OmnVList<AosGenTablePtr> &tables);
	virtual bool 	newRecordAdded(const AosGenRecordPtr &newRecord) const;
	virtual bool 	recordDeleted(const AosGenRecordPtr &record) const;
	virtual bool 	setRecord(const AosGenRecordPtr &record) const;
	virtual OmnString toString(const u32 indent) const;
	virtual bool 	getCrtValue(int &value) const;
	virtual bool 	getCrtValue(u16 &value) const;
	virtual bool 	getCrtValue(u32 &value) const;
	virtual bool 	getCrtValue(char *&value) const;
	virtual bool 	getCrtValue(OmnString &value) const;
	virtual bool 	getCrtValueAsArg(OmnString &value, 
									 OmnString &decl,
									 const u32 argIndex) const;
	virtual OmnString getNextValueFuncCall() const;
	virtual AosRVGType getRVGType() const {return eAosRVGType_TimeStr;}

	bool	nextExist(OmnString &value, 
					const AosGenTablePtr &data, 
				    const AosGenRecordPtr &record,
				    bool &isGood, 
					const bool correctOnly,
			 	    const bool selectFromRecord,
					AosRVGReturnCode &rcode,
				    OmnString &errmsg);
	bool	nextNew(OmnString &value, 
	                const AosGenTablePtr &data, 
				    const AosGenRecordPtr &record,
			 	    const bool selectFromRecord,
				    bool &isGood, 
					const bool correctOnly,
					AosRVGReturnCode &rcode,
				    OmnString &errmsg);
	bool	nextNotKey(OmnString &value, 
	                const AosGenTablePtr &data, 
				    const AosGenRecordPtr &record,
				    bool &isGood, 
					const bool correctOnly,
					const bool selectFromRecord,
					AosRVGReturnCode &rcode,
				    OmnString &errmsg);
	bool	generateBadValue(OmnString &value,
					const AosGenTablePtr &data, 
					const AosGenRecordPtr &record,
					AosRVGReturnCode &rcode,
					OmnString &errmsg);

private:
	bool			createSelectors();
	OmnString		genValidValue();
	OmnString		genInvalidValue();

};

#endif

