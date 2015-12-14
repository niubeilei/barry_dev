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
// This is a utility to select docs.
//
// Modification History:
// 04/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/ValueRslt.h"


AosValueRslt::AosValueRslt()
{
	setNull();
}


AosValueRslt::AosValueRslt(const AosValueRslt &rhs)
{
	if (rhs.mType == AosDataType::eLongString)
	{
		int len = strlen((char *)rhs.mValue.obj_value);
		mValue.obj_value = OmnNew char[len+1];
		strcpy((char *)mValue.obj_value, (char *)rhs.mValue.obj_value);
	}
	else if (rhs.mType == AosDataType::eDateTime)
	{
		AosDateTime *dt = (AosDateTime*)rhs.mValue.obj_value;
		mValue.obj_value = OmnNew AosDateTime(*dt);
	}
	else
	{
		mValue = rhs.mValue;
	}
	mType = rhs.mType;
}

AosValueRslt::AosValueRslt(const u64 value)
{
	mType = AosDataType::eU64;
	mValue.u64_value = value;
}


AosValueRslt::AosValueRslt(const bool value)
{
	mType = AosDataType::eBool;
	mValue.bool_value = value;
}


AosValueRslt::AosValueRslt(const i64 value)
{
	mType = AosDataType::eInt64;
	mValue.i64_value = value;
}


AosValueRslt::AosValueRslt(const double value)
{
	mType = AosDataType::eDouble;
	mValue.double_value = value;
}

AosValueRslt::AosValueRslt(const OmnString &value)
{
	int len = value.length();
	if (len >= eDefaultSize)
	{
		mType = AosDataType::eLongString;
		mValue.obj_value = OmnNew char[len+1];
		strcpy((char *)mValue.obj_value , value.data());
	}
	else
	{
		mType = AosDataType::eShortString;
		strcpy(mValue.str_value, value.data());
	}
}

AosValueRslt::AosValueRslt(const char value)
{
	mType = AosDataType::eChar;
	mValue.char_value = value;
}

AosValueRslt::AosValueRslt(const AosDateTime &value)
{
	mType = AosDataType::eDateTime;
	mValue.obj_value = OmnNew AosDateTime(value);
}

AosValueRslt::AosValueRslt(const AosTimeDuration *value)
{
}

AosValueRslt::~AosValueRslt()
{
	deleteMemory();
}

void 
AosValueRslt::reset() 
{
}

void 
AosValueRslt::setDouble(const double vv) 
{
	deleteMemory();
	mType = AosDataType::eDouble;
	mValue.double_value = vv;
}


void 
AosValueRslt::setU64(const u64 vv) 
{
	deleteMemory();
	mType = AosDataType::eU64;
	mValue.u64_value = vv;
}


void 
AosValueRslt::setI64(const i64 vv) 
{
	deleteMemory();
	mType = AosDataType::eInt64;
	mValue.i64_value = vv;
}


void
AosValueRslt::setStr(const OmnString &vv) 
{	
	deleteMemory();
	int len = vv.length();
	if (len >= eDefaultSize)
	{
		mType = AosDataType::eLongString;
		mValue.obj_value = OmnNew char[len+1];
		strcpy((char *)mValue.obj_value , vv.data());
	}
	else
	{
		mType = AosDataType::eShortString;
		strcpy(mValue.str_value, vv.data());
	}
}

void 
AosValueRslt::setCStr(const char *vv, int len)
{
	deleteMemory();
	if (len >= eDefaultSize)
	{
		mType = AosDataType::eLongString;
		mValue.obj_value = OmnNew char[len+1];
		strncpy((char *)mValue.obj_value, vv, len);
		((char *)mValue.obj_value)[len] = '\0';
	}
	else
	{
		mType = AosDataType::eShortString;
		strncpy(mValue.str_value, vv, len);
		mValue.str_value[len] = '\0';
	}
}


void
AosValueRslt::setDateTime(const AosDateTime &dt) 
{
	deleteMemory();
	mType = AosDataType::eDateTime;
	mValue.obj_value = OmnNew AosDateTime(dt);
}


void
AosValueRslt::setBool(const bool vv) 
{
	deleteMemory();
	mType = AosDataType::eBool;
	mValue.bool_value = vv;
}


void
AosValueRslt::setChar(const char vv) 
{
	deleteMemory();
	mType = AosDataType::eChar;
	mValue.char_value = vv;
}


void
AosValueRslt::setBuff(const AosBuffPtr &buff) 
{
	deleteMemory();
	mType = AosDataType::eBuff;
	mValue.obj_value = OmnNew AosBuffPtr(buff);
}


const AosDateTime& 	
AosValueRslt::getDateTime() const 
{
	AosDateTime return_value;
	aos_assert_r(mType == AosDataType::eDateTime, return_value);
	AosDateTime *dt = (AosDateTime*)mValue.obj_value;
	return *dt;
}



char
AosValueRslt::getChar() const 
{
	aos_assert_r(mType == AosDataType::eChar, '\0');
	return mValue.char_value;
}


AosBuffPtr
AosValueRslt::getBuff() const 
{
	aos_assert_r(mType == AosDataType::eBuff, NULL);
	return *(AosBuffPtr*)mValue.obj_value;
}


AosDataType::E 
AosValueRslt::getType() const
{
	if (mType == AosDataType::eShortString || mType == AosDataType::eLongString)
		return AosDataType::eString;
	return mType;
}


AosValueRslt&
AosValueRslt::operator = (const AosValueRslt &rhs) 
{	
	// a = a; 
	if (this == &rhs) return *this;
	deleteMemory();

	mType = rhs.mType;
	if (mType == AosDataType::eLongString)
	{
		int len = strlen((char *)(rhs.mValue.obj_value));
		mValue.obj_value = OmnNew char[len+1];
		strcpy((char *)mValue.obj_value, (char *)rhs.mValue.obj_value);
	}
	else if (mType == AosDataType::eDateTime)
	{
		AosDateTime *dt = (AosDateTime*)rhs.mValue.obj_value;
		mValue.obj_value = OmnNew AosDateTime(*dt);
	}
	else
	{
		mValue = rhs.mValue;
	}
	return *this;
}


bool
AosValueRslt::getBool() const
{
	switch(mType)
	{
	case AosDataType::eChar:
		 return mValue.char_value !=  0;
	case AosDataType::eBool:
		 return mValue.bool_value;
	case AosDataType::eInt64:
		 return mValue.i64_value !=  0;
	case AosDataType::eU64:
		 return mValue.u64_value !=  0;
	case AosDataType::eDouble:
		 return mValue.double_value !=  0;
	case AosDataType::eString:
		 return strlen((char *)mValue.obj_value) != 0;
	default:
		 OmnAlarm << enderr;
		 return false;
	}
	return false;
}

u64
AosValueRslt::getU64() const
{
	switch(mType)
	{
	case AosDataType::eChar:
		 return mValue.char_value;
	case AosDataType::eBool:
		 return mValue.bool_value;
	case AosDataType::eInt64:
		 return mValue.i64_value;
	case AosDataType::eU64:
		 return mValue.u64_value;
	case AosDataType::eDouble:
		 return mValue.double_value;
	case AosDataType::eDateTime:
		 {
			 AosDateTime *dt = (AosDateTime*)(mValue.obj_value);
			 return (u64)dt->to_time_t();
		 }
	default:
		 OmnAlarm << enderr;
		 return 0;
	}
	return 0;
}

i64
AosValueRslt::getI64() const 
{
	switch(mType)
	{
	case AosDataType::eChar:
		 return mValue.char_value;
	case AosDataType::eBool:
		 return mValue.bool_value;
	case AosDataType::eInt64:
		 return mValue.i64_value;
	case AosDataType::eU64:
		 return mValue.u64_value;
	case AosDataType::eDouble:
		 return mValue.double_value;
	case AosDataType::eDateTime:
		 {
			 AosDateTime *dt = (AosDateTime*)(mValue.obj_value);
			 return (int64_t)dt->to_time_t();
		 }
	default:
		 OmnAlarm << enderr;
		 return 0;
	}
	return 0;
}

double
AosValueRslt::getDouble() const
{
	switch(mType)
	{
	case AosDataType::eChar:
		 return mValue.char_value;
	case AosDataType::eBool:
		 return mValue.bool_value;
	case AosDataType::eDouble:
		 return mValue.double_value;
	case AosDataType::eInt64:
		 return mValue.i64_value;
	case AosDataType::eU64:
		 return mValue.u64_value;
	default:
		 OmnAlarm << enderr;
		 return 0;
	}
	return 0;

}

OmnString
AosValueRslt::getStr() const 
{
	char ch[24];
	switch(getType())
	{
	case AosDataType::eChar:
		 ch[0] =  mValue.char_value;
		 ch[1] = 0;
		 break;
	case AosDataType::eBool:
		 if (mValue.bool_value)
		 	strcpy(ch, "true");
		 else
		 	strcpy(ch, "false");
		 break;
	case AosDataType::eInt64:
		 sprintf(ch, "%ld", mValue.i64_value);
		 break;
	case AosDataType::eU64:
		 sprintf(ch, "%lu", mValue.u64_value);
		 break;
	case AosDataType::eDouble:
		 sprintf(ch, "%lf", mValue.double_value);
		 break;
	case AosDataType::eString:
		 {
			 OmnString v1(getCStrValue(), strlen(getCStrValue()));
			 return v1;
		 }
	case AosDataType::eDateTime:
		 {
			 AosDateTime *dt = (AosDateTime*)(mValue.obj_value);
			 OmnString v1;
			 return v1 << dt->toString();
		 }
	default:
		OmnAlarm << enderr;
		return "";
	}
	OmnString vv(ch, strlen(ch));
	return vv;
}

void 
AosValueRslt::deleteMemory()
{
	if (mType == AosDataType::eDateTime)
	{
		OmnDelete (AosDateTime*)(mValue.obj_value);
		return;
	}                                                          
	else if (mType == AosDataType::eLongString)
	{
		OmnDelete (char*)mValue.obj_value;
		return;
	}
	else if (mType == AosDataType::eBuff)
	{
		OmnDelete (AosBuffPtr*)mValue.obj_value;
	}
	return;
}


const char* 		
AosValueRslt::getCStrValue() const 
{
	if (mType == AosDataType::eShortString)
		return mValue.str_value;
	if (mType == AosDataType::eLongString)
		return (char *)(mValue.obj_value);

	OmnAlarm << enderr;
	return NULL;
}

char *
AosValueRslt::getCStr(char *dest, int dest_len, int &len) 
{
	if (dest == NULL || dest_len <= 0)
		return NULL;

	char ch[24];
	switch(getType())
	{
	case AosDataType::eChar:
		 ch[0] =  mValue.char_value;
		 ch[1] = 0;
		 break;
	case AosDataType::eBool:
		 if (mValue.bool_value)
		 	strcpy(ch, "true");
		 else
		 	strcpy(ch, "false");
		 break;
	case AosDataType::eInt64:
		 sprintf(ch, "%ld", mValue.i64_value);
		 break;
	case AosDataType::eU64:
		 sprintf(ch, "%lu", mValue.u64_value);
		 break;
	case AosDataType::eDouble:
		 sprintf(ch, "%lf", mValue.double_value);
		 break;
	case AosDataType::eString:
		 {
			 const char *ch = getCStrValue();
			 len = strlen(ch);
			 if (len >= dest_len) return NULL;
			 strncpy(dest, ch, len);
			 dest[len] = '\0';
			 return dest;
		 }
	case AosDataType::eDateTime:
		 {
			 AosDateTime *dt = (AosDateTime*)(mValue.obj_value);
			 const char *ch = dt->toString().data();
			 len = strlen(ch);
			 if (len >= dest_len) return NULL;
			 strncpy(dest, ch, len);
			 dest[len] = '\0';
			 return dest;
		 }
		OmnAlarm << enderr;
		return NULL;

	default:
		OmnAlarm << enderr;
		return NULL;
	}
	len = strlen(ch);
	if (len >= dest_len) return NULL;
	strncpy(dest, ch, len);
	dest[len] = '\0';
	return dest;
}


bool 
AosValueRslt::operator < (const AosValueRslt &rhs) const 
{
	if (isNull()) return true;
	if (rhs.isNull()) return false;
	AosDataType::E type = AosDataType::autoTypeConvert(getType(), rhs.getType());
	return doComparison(eAosOpr_lt, type, *this, rhs);
}


bool
AosValueRslt::doComparison(
		const AosOpr opr,
		const AosDataType::E value_type,
		const AosValueRslt &lv,
		const AosValueRslt &rv) 
{
	//if (lv.mType == AosDataType::eNull || rv.mType == AosDataType::eNull) 
	//{
	//	return false;
	//}

	switch (opr)
	{
	case eAosOpr_gt:
		switch (value_type)	
		{
		case AosDataType::eChar:
			return (lv.getChar() > rv.getChar());
		case AosDataType::eBool:
			return (lv.getBool() > rv.getBool());
		case AosDataType::eString:
			return ((strcmp(lv.getCStrValue() , rv.getCStrValue()) > 0)); 
		case AosDataType::eU64: 
			return (lv.getU64() > rv.getU64());
		case AosDataType::eDateTime:
		case AosDataType::eInt64: 
			return (lv.getI64() > rv.getI64());
		case AosDataType::eDouble: 
			return (lv.getDouble() > rv.getDouble());
		default:	
			OmnAlarm << enderr;
			break;
		}
		break;

	case eAosOpr_ge:
		switch (value_type)	
		{
		case AosDataType::eChar:
			return (lv.getChar() >= rv.getChar());
		case AosDataType::eBool:
			return (lv.getBool() >= rv.getBool());
		case AosDataType::eString:
			return ((strcmp(lv.getCStrValue(), rv.getCStrValue()) >= 0)); 
		case AosDataType::eU64: 
			return (lv.getU64() >= rv.getU64());
		case AosDataType::eDateTime:
		case AosDataType::eInt64: 
			return (lv.getI64() >= rv.getI64());
		case AosDataType::eDouble: 
			return (lv.getDouble() >= rv.getDouble());
		default:	
			OmnAlarm << enderr;
			break;
		}
		break;

	case eAosOpr_eq:
		switch (value_type)	
		{
		case AosDataType::eChar:
			return (lv.getChar() == rv.getChar());
		case AosDataType::eBool:
			return (lv.getBool() == rv.getBool());
		case AosDataType::eString:
			return (strcmp(lv.getCStrValue(), rv.getCStrValue()) == 0); 
		case AosDataType::eU64: 
			return (lv.getU64() == rv.getU64());
		case AosDataType::eDateTime:
		case AosDataType::eInt64: 
			return (lv.getI64() == rv.getI64());
		case AosDataType::eDouble: 
			return (lv.getDouble() == rv.getDouble());
		default:	
			OmnAlarm << enderr;
			break;
		}
		break;

	case eAosOpr_lt:
		switch (value_type)	
		{
		case AosDataType::eChar:
			return (lv.getChar() < rv.getChar());
		case AosDataType::eBool:
			return (lv.getBool() < rv.getBool());
		case AosDataType::eString:
			return (strcmp(lv.getCStrValue(), rv.getCStrValue()) < 0); 
		case AosDataType::eU64: 
			return (lv.getU64() < rv.getU64());
		case AosDataType::eDateTime:
		case AosDataType::eInt64: 
			return (lv.getI64() < rv.getI64());
		case AosDataType::eDouble: 
			return (lv.getDouble() < rv.getDouble());
		default:	
			OmnAlarm << enderr;
			break;
		}
		break;

	case eAosOpr_le:
		switch (value_type)	
		{
		case AosDataType::eChar:
			return (lv.getChar() <= rv.getChar());
		case AosDataType::eBool:
			return (lv.getBool() <= rv.getBool());
		case AosDataType::eString:
			return (strcmp(lv.getCStrValue(), rv.getCStrValue()) <= 0); 
		case AosDataType::eU64: 
			return (lv.getU64() <= rv.getU64());
		case AosDataType::eDateTime:
		case AosDataType::eInt64: 
			return (lv.getI64() <= rv.getI64());
		case AosDataType::eDouble: 
			return (lv.getDouble() <= rv.getDouble());
		default:	
			OmnAlarm << enderr;
			break;
		}
		break;

	case eAosOpr_ne:
		switch (value_type)	
		{
		case AosDataType::eChar:
			return (lv.getChar() != rv.getChar());
		case AosDataType::eBool:
			return (lv.getBool() != rv.getBool());
		case AosDataType::eString:
			return (strcmp(lv.getCStrValue(), rv.getCStrValue()) != 0); 
		case AosDataType::eU64: 
			return (lv.getU64() != rv.getU64());
		case AosDataType::eDateTime:
		case AosDataType::eInt64: 
			return (lv.getI64() != rv.getI64());
		case AosDataType::eDouble: 
			return (lv.getDouble() != rv.getDouble());
		default:	
			OmnAlarm << enderr;
			break;
		}
		break;
	default:
		OmnAlarm << enderr;
		break;
	}

	OmnAlarm << enderr;
	return false;
}

AosValueRslt
AosValueRslt::doArith(
		const ArithOpr::E opr,
		const AosDataType::E return_type,
		const AosValueRslt &lv,
		const AosValueRslt &rv)
{
	AosValueRslt vv;

	switch (opr)
	{
	case ArithOpr::eAdd:
		switch (return_type)	
		{
		case AosDataType::eU64: 
			return AosValueRslt(lv.getU64() + rv.getU64());
		case AosDataType::eInt64: 
			return AosValueRslt(lv.getI64() + rv.getI64());
		case AosDataType::eDouble: 
			return AosValueRslt(lv.getDouble() + rv.getDouble());
		default:	
			OmnAlarm << enderr;
			break;
		}
		break;

	case ArithOpr::eSub:
		switch (return_type)	
		{
		case AosDataType::eU64: 
			return AosValueRslt(lv.getU64() - rv.getU64());
		case AosDataType::eInt64: 
			return AosValueRslt(lv.getI64() - rv.getI64());
		case AosDataType::eDouble: 
			return AosValueRslt(lv.getDouble() - rv.getDouble());
		default:	
			OmnAlarm << enderr;
			break;
		}
		break;

	case ArithOpr::eMul:
		switch (return_type)	
		{
		case AosDataType::eU64: 
			return AosValueRslt(lv.getU64() * rv.getU64());
		case AosDataType::eInt64: 
			return AosValueRslt(lv.getI64() * rv.getI64());
		case AosDataType::eDouble: 
			return AosValueRslt(lv.getDouble() * rv.getDouble());
		default:	
			OmnAlarm << enderr;
			break;
		}
		break;

	case ArithOpr::eDiv:
		switch (return_type)	
		{
		case AosDataType::eU64: 
			return (!rv.getU64()) ? (vv) : (AosValueRslt(lv.getU64() / rv.getU64()));
		case AosDataType::eInt64: 
			return (!rv.getU64()) ? (vv) : (AosValueRslt(lv.getI64() / rv.getI64()));
		case AosDataType::eDouble: 
			return AosValueRslt(lv.getDouble() / rv.getDouble());
		default:	
			OmnAlarm << enderr;
			break;
		}
		break;

	case ArithOpr::eMod:
		switch (return_type)	
		{
		case AosDataType::eU64: 
			return (!rv.getU64()) ? (vv) : (AosValueRslt(lv.getU64() % rv.getU64()));
		case AosDataType::eInt64: 
			return (!rv.getI64()) ? (vv) : (AosValueRslt(lv.getI64() % rv.getI64()));
		default:	
			OmnAlarm << enderr;
			break;
		}
		break;

	default:
		OmnAlarm << enderr;
		break;
	}

	OmnAlarm << enderr;
	return vv;
}
