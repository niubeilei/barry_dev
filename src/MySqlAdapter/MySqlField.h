////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/12/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_MySqlAdapter_MySqlField_h
#define Aos_MySqlAdapter_MySqlField_h

#include "Conds/Ptrs.h"
#include "SEInterfaces/MySqlFieldObj.h"
#include "SEUtil/FieldOpr.h"
#include "MySqlAdapter/Ptrs.h"
#include "Util/UtUtil.h"


class AosMySqlField : virtual public AosMySqlFieldObj 
{
public:
	OmnString                   mCatalog;       //always "def"                   
	OmnString                   mSchema;        //DB name
	OmnString                   mTableName;     
	OmnString                   mOrgTableName;
	OmnString                   mFieldName;
	OmnString                   mOrgFieldName;
	u8                          mFixedLen;
	OmnString  					mCharacterSet;
	int32_t                     mColumnLen;
	u8							mType;          //field type
	char						mFlagsFirstByte;
	char						mFlagsSecondByte;
	u8                          mDecimals;
	int16_t                     mFiller;
	OmnString					mValue;
	
public:
	AosMySqlField();
	~AosMySqlField();

	virtual bool encodeHeader(
					const AosRundataPtr &rdata, 
					const AosBuffPtr &buff) = 0; 

	virtual bool encodeField(
					const AosRundataPtr &rdata, 
					const AosBuffPtr &buff) = 0;

	virtual bool setSchema(OmnString value){mSchema = value; return true;}
	virtual bool setTableName(OmnString value){mTableName = value; return true;}
	virtual bool setOrgTableName(OmnString value){mOrgTableName = value; return true;}
	virtual bool setFieldName(OmnString value){mFieldName = value; return true;}
	virtual bool setOrgFieldName(OmnString value){mOrgFieldName = value; return true;}
	//virtual bool setCharacterSet(OmnString value);
	virtual bool setColumnLen(int32_t value){mColumnLen = value; return true;}
	//virtual bool setTyoe(char value){mType = value; return true;}
//	virtual bool setFlags();
	virtual bool setDecimals(u8 value){mDecimals = value; return true;}
	virtual bool setFiller(int16_t value){mFiller = value; return true;}
	virtual bool setValue(OmnString value){mValue = value; return true;}
	//virtual bool write(char value);
	//virtual bool write(OmnString value);
	//virtual bool write(u8 value);
	//virtual bool write(int16_t value);
	//virtual bool write(int32_t value);
};

#endif
