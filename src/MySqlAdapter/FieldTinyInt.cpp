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
// 2013/12/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "MySqlAdapter/FieldTinyInt.h"
#include "MySqlAdapter/MySqlFieldType.h"

AosMySqlFieldTinyInt::AosMySqlFieldTinyInt()
{
	mCatalog = "def";
	mSchema = "mysql";
	mFixedLen = 12;
	mType = 1;
	mColumnLen = 1;
	mFlagsFirstByte = '\000';
	mFlagsSecondByte = '\000';
	mDecimals = 0;
	mFiller = 0;
}


AosMySqlFieldTinyInt::~AosMySqlFieldTinyInt()
{
}


bool
AosMySqlFieldTinyInt::encodeHeader(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	AosBuffPtr head = OmnNew AosBuff(100 AosMemoryCheckerArgs); 

	u8 len;
	aos_assert_r(mCatalog != "", false);
	len = mCatalog.length();
	aos_assert_r(len == mCatalog.length(), false);
	head->setU8(len);
	head->setBuff(mCatalog.getBuffer(), (int)len);

	aos_assert_r(mSchema != "", false);
	len = mSchema.length();                      
	aos_assert_r(len == mSchema.length(), false);
	head->setU8(len);                                
	head->setBuff(mSchema.getBuffer(), (int)len);     

	aos_assert_r(mTableName != "", false);
	len = mTableName.length();                      
	aos_assert_r(len == mTableName.length(), false);
	head->setU8(len);                                
	head->setBuff(mTableName.getBuffer(), (int)len);     

	aos_assert_r(mOrgTableName != "", false);
	len = mOrgTableName.length();                      
	aos_assert_r(len == mOrgTableName.length(), false);
	head->setU8(len);                                
	head->setBuff(mOrgTableName.getBuffer(), (int)len);     


	aos_assert_r(mFieldName != "", false);
	len = mFieldName.length();                     
	aos_assert_r(len == mFieldName.length(), false);
	head->setU8(len);                                
	head->setBuff(mFieldName.getBuffer(), (int)len);    

	aos_assert_r(mOrgFieldName != "", false);
	len = mOrgFieldName.length();                     
	aos_assert_r(len == mOrgFieldName.length(), false);
	head->setU8(len);                                
	head->setBuff(mOrgFieldName.getBuffer(), len);    

	aos_assert_r(mFixedLen == 12, false);
	head->setU8(mFixedLen);
		
	// character set
	head->setU8(0x3f);
	head->setU8(0x00);

	// field column length
	head->setInt(mColumnLen);

	// field type
	head->setU8(mType);

	// flags
	head->setChar(mFlagsFirstByte);
	head->setChar(mFlagsSecondByte);
	head->setU8(mDecimals);
	head->setI16(mFiller);

	u32 head_len = head->dataLen();
	buff->setInt(head_len);
	buff->setBuff(head->data(), head_len); 
	return true;
}


bool
AosMySqlFieldTinyInt::encodeField(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	// 'data' is encoded using "AosBuff". 
	// 	select age, addr ...
	//	int age = data->getU32(0);
	u8 len = mValue.length(); 
	aos_assert_r(len == mValue.length(), false);
	buff->setU8(len);
	buff->setBuff(mValue.getBuffer(), len);
	return true;
}

