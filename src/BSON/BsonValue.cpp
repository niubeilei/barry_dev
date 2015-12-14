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
// 2015/01/28 Created by Rain
////////////////////////////////////////////////////////////////////////////
#include "BSON/BsonValue.h"

#include "BSON/BSON.h"


AosBsonValue::AosBsonValue(
		AosBSON  *Ptr, 
		const AosBsonField::Type type,
		const u32 name,
		const i64 &pos)
:
mBSON(Ptr),
mValueType(type),
mU32Name(name),
mStrName(""),
mPos(pos)
{
}


AosBsonValue::AosBsonValue(
		AosBSON  *Ptr, 
		const AosBsonField::Type type,
		const OmnString &name,
		const i64 &pos)
:
mBSON(Ptr),
mValueType(type),
mU32Name(0),
mStrName(name),
mPos(pos)
{
}


AosBsonValue::~AosBsonValue()
{
   mBSON = 0;
}


AosValueRslt
AosBsonValue::getValue(const OmnString &name)
{
   //can not go to here
   OmnThrowException("override failed");
   return AosValueRslt();
}


AosValueRslt
AosBsonValue::getValue(const u64 &name)
{
   //can not go to here
   OmnThrowException("override failed");
   return AosValueRslt();
}

	
AosValueRslt
AosBsonValue::getValue(const i64 &name)
{
   //can not go to here
   OmnThrowException("override failed");
   return AosValueRslt();
}


AosValueRslt
AosBsonValue::getValue(const i32 idx)
{
   //can not go to here
   OmnThrowException("override failed");
   return AosValueRslt();
}

bool
AosBsonValue::insert(OmnString &name, AosValueRslt &value)
{
   return false;
}

bool
AosBsonValue::insert(u64 &name, AosValueRslt &value)
{
   return false;
}

bool
AosBsonValue::insert(i64 &name, AosValueRslt &value)
{
   return false;
}

bool
AosBsonValue::parse()
{
   return false;
}


AosBsonValue &
AosBsonValue::setValue (const u64 &value)
{
    if (AosBsonField::eFieldTypeU64 == mValueType || mValueType == AosBsonField::eFieldTypeNull)
	{
	    if (mU32Name > 0)
		{
			mBSON->setValue(mU32Name, value, mPos);
		    mValueType = AosBsonField::eFieldTypeU64;
		}
		else if(mStrName != "")
		{
		    mBSON->setValue(mStrName, value, mPos);
			mValueType = AosBsonField::eFieldTypeU64;
		}
        else
		{
	       OmnAlarm << "error" << enderr; 
		}
	}
	else
	{
		OmnAlarm << "error" << enderr;
	}
	return *this;
}


AosBsonValue &
AosBsonValue::setValue (const i64 &value)
{
    if (AosBsonField::eFieldTypeInt64 == mValueType || mValueType == AosBsonField::eFieldTypeNull)
	{
	    if (mU32Name > 0)
		{
			mBSON->setValue(mU32Name, value, mPos);
		    mValueType = AosBsonField::eFieldTypeInt64;
		}
		else if(mStrName != "")
		{
		    mBSON->setValue(mStrName, value, mPos);
			mValueType = AosBsonField::eFieldTypeInt64;
		}
        else
		{
	       OmnAlarm << "error" << enderr; 
		}
	}
	else
	{
		OmnAlarm << "error" << enderr;
	}
	return *this;
}

	
AosBsonValue &
AosBsonValue::setValue (const double value)
{
    if (AosBsonField::eFieldTypeDouble == mValueType || mValueType == AosBsonField::eFieldTypeNull)
	{
	    if (mU32Name > 0)
		{
			mBSON->setValue(mU32Name, value, mPos);
		    mValueType = AosBsonField::eFieldTypeDouble;
		}
		else if(mStrName != "")
		{
		    mBSON->setValue(mStrName, value, mPos);
			mValueType = AosBsonField::eFieldTypeDouble;
		}
        else
		{
	       OmnAlarm << "error" << enderr; 
		}
	}
	else
	{
		OmnAlarm << "error" << enderr;
	}
	return *this;
}


AosBsonValue &
AosBsonValue::setValue(const OmnString &value)
{
    if (AosBsonField::eFieldTypeString  == mValueType || mValueType == AosBsonField::eFieldTypeNull)
	{
	    if (mU32Name > 0)
		{
			mBSON->setValue(mU32Name, value, mPos);
		    mValueType = AosBsonField::eFieldTypeString;
		}
		else if(mStrName != "")
		{
		    mBSON->setValue(mStrName, value, mPos);
			mValueType = AosBsonField::eFieldTypeString;
		}
        else
		{
	       OmnAlarm << "error" << enderr; 
		}
	}
	else
	{
		OmnAlarm << "error" << enderr;
	} 
	return *this;
}


AosBsonValue &
AosBsonValue::operator = (const OmnString &value)
{
	return setValue(value);
}

	
AosBsonValue &
AosBsonValue::operator = (const double value)
{
	return setValue(value);
}

	
AosBsonValue &
AosBsonValue::operator = (const i64 &value)
{
	return setValue(value);
}


AosBsonValue &
AosBsonValue::operator = (const u64 &value)
{
	return setValue(value);
}


AosBsonValue &
AosBsonValue::operator = (const AosValueRslt &value)
{
	AosDataType::E valueType = value.getType();
    switch(valueType)
	{
	  /* case AosDataType::eFloat:
            {
			   float vv = value.getDoubleValue(0);
			   *this = vv;
			   return *this;
			}
       */
	   case AosDataType::eDouble: 
            {
			   double vv = value.getDouble();	
			   return setValue(vv);
			}

	   case AosDataType::eString: 
            {
			   OmnString vv = value.getStr();
               return setValue(vv);
			}

	 //  case AosDataType::eNull:
     //       {
	//		}

	/*   case AosDataType::eU8: 
            {
                u8 vv = value.getU64Value(0);
				*this = vv;
				return *this;	
			}
     
	   case AosDataType::eU16:    
            {
			    u16 vv = value.getU64Value(0);
				*this = vv;
				return *this;
			}

	   case AosDataType::eU32:    
            {
			    u32 vv = value.getU32Value(0);
				*this = vv;
				return *this;
			}
    */
	   case AosDataType::eU64:    
			{
			   u64 vv = value.getU64();
			   return setValue(vv);
			}

	/*   case AosDataType::eInt8:   
            {
			   i8 vv = value.getIntValue(0);
			   *this = vv;
			   return *this;
			}

	   case AosDataType::eInt16:  
            {
			   i16 vv = value.getIntValue(0);
			   *this = vv;
			   return *this;
			}

	   case AosDataType::eInt32:  
            {
			   i32 vv = value.getIntValue(0);
			   *this = vv;
			   return *this;
			}
       */
	   case AosDataType::eInt64:  
            {
			   i64 vv = value.getI64();
			   return setValue(vv); 
			}

	 /*  case AosDataType::eChar:   
			{
			   char vv = value.getChar(0);
			   *this = vv;
			   return *this;
			}
      */
	 //  case AosDataType::eVarInt: 
	//		break;

	 //  case AosDataType::eVarUint:
	//		break;

	   default:                   
			return *this;
	}
}


AosBsonValue &
AosBsonValue::operator [] (const OmnString &name)
{
   //should be never goto here
   AosBsonValuePtr ptr = OmnNew AosBsonValue(this->mBSON, AosBsonField::eFieldTypeNull, "", -1);   
   return  *(ptr.getPtr());
}


AosBsonValue &
AosBsonValue::operator [] (const u64 &name)
{
   //should be never goto here
   AosBsonValuePtr ptr = OmnNew AosBsonValue(this->mBSON, AosBsonField::eFieldTypeNull, "", -1);   
   return  *(ptr.getPtr());
}


AosBsonValue &
AosBsonValue::operator [] (const i64 &name)
{
   //should be never goto here
   AosBsonValuePtr ptr = OmnNew AosBsonValue(this->mBSON, AosBsonField::eFieldTypeNull, "", -1);   
   return  *(ptr.getPtr());
}


AosBsonValue &  
AosBsonValue::operator [] (const u32 idx)
{
   //should be never goto here
   OmnThrowException("override failed")
   return *this;
}

AosBsonValue::operator u64 ()
{
   AosValueRslt vv;
   bool rslt;
   if(mU32Name > 0)
   {
      rslt = mBSON->getValue(mU32Name, vv);
   }
   else if(mStrName != "")
   {
      rslt = mBSON->getValue( mStrName, vv);
   }
   else
   {
      OmnAlarm << "error" << enderr;
   }
   if(!rslt) return 0;
   u64 value = vv.getU64();
   return value;
}


AosBsonValue::operator i64 ()
{
   AosValueRslt vv;
   bool rslt;
   if(mU32Name > 0)
   {
      rslt = mBSON->getValue(mU32Name, vv);
   }
   else if(mStrName != "")
   {
      rslt = mBSON->getValue( mStrName, vv);
   }
   else
   {
      OmnAlarm << "error" << enderr;
   }
   if(!rslt) return 0;
   i64 value = vv.getI64();
   return value;
}


AosBsonValue::operator double ()
{
   AosValueRslt vv;
   bool rslt;
   if(mU32Name > 0)
   {
      rslt = mBSON->getValue(mU32Name, vv);
   }
   else if(mStrName != "")
   {
      rslt = mBSON->getValue( mStrName, vv);
   }
   else
   {
      OmnAlarm << "error" << enderr;
   }
   if(!rslt) return 0;
   i64 value = vv.getDouble();
   return value;
}


AosBsonValue::operator OmnString ()
{
   AosValueRslt vv;
   bool rslt;
   if(mU32Name > 0)
   {
      rslt = mBSON->getValue(mU32Name, vv);
   }
   else if(mStrName != "")
   {
      rslt = mBSON->getValue(mStrName, vv);
   }
   else
   {
      OmnAlarm << "error" << enderr;
   }
   if(!rslt) return OmnString("");
   OmnString value = vv.getStr();
   return value;
}


AosBsonValue::operator AosValueRslt ()
{
   AosValueRslt vv;
   bool rslt;
   if(mU32Name > 0)
   {
      rslt = mBSON->getValue(mU32Name, vv);
   }
   else if(mStrName != "")
   {
      rslt = mBSON->getValue(mStrName, vv);
   }
   else
   {
      OmnAlarm << "error" << enderr;
   }
   if(!rslt) 
   {
	  vv.setNull(); 
	  return vv;
   }
   return vv;
}


/*AosBsonValue::operator i64 ()
{
   //should not goto here
   OmnThrowException("override failed!");
   return -1;
}

AosBsonValue::operator double ()
{
   //should not goto here
   OmnThrowException("override failed!");
   return 0.0;
}
*/	
bool
AosBsonValue::getBuff(AosBsonField **tsFields, AosBuff* mBuffRaw)
{
   return false; 
}

