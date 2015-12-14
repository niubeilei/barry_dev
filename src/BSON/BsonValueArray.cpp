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
// 2014/08/17 Created by Rain
////////////////////////////////////////////////////////////////////////////
//

#include "BSON/BsonValueArray.h"
#include "BSON/BSON.h"

AosBsonValueArrayItr::AosBsonValueArrayItr(
                                AosBSON  *mptr,
                                AosBsonField::Type type,
                                const OmnString &name,
                                AosBsonValueArray *mp,
                                const AosValueRslt &value)
:AosBsonValue(mptr, type, name, -1),
mArray(mp),
mValue(value)
{

}


AosBsonValueArrayItr::~AosBsonValueArrayItr()
{
    mArray = 0;
}

AosBsonValue &
AosBsonValueArrayItr::operator = (const AosValueRslt& value)
{
    mValue = value;
	return *this;
}

AosBsonValueArrayItr::operator OmnString ()
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



AosValueRslt&
AosBsonValueArrayItr::getValue()
{
   return mValue;
}

//////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
AosBsonValueArray::AosBsonValueArray(
		                  AosBSON  *ptr, 
						  AosBsonField::Type type,
						  OmnString &name,
						  i64 pos)
:
AosBsonValue(ptr, AosBsonField::eFieldTypeGenericArray, name, pos)
{
}


AosBsonValueArray::AosBsonValueArray(
		                  AosBSON  *ptr, 
						  AosBsonField::Type type,
						  OmnString &name,
						  AosBsonField::Type dataType,
						  i64 pos)
:
AosBsonValue(ptr, AosBsonField::eFieldTypeArray, name, pos)
{
}


AosBsonValueArray::~AosBsonValueArray()
{
}


AosBsonValue&
AosBsonValueArray::operator [] (const u32 idx)
{
    if(idx < 0 || idx > mValues.size())
	{
	   OmnThrowException("wrong idx");
	   return *this;
	}
	return *(mValues[idx].getPtr());
}

/*
bool
AosBsonValueArray::setValue(vector<AosValueRslt> &values)
{
    mValues.clear();
	if(values.size() == 0) return true;   
	for(int i = 0; i < values.size(); i++)
	{
	   if(mValueType == AosBsonField::eFieldTypeArray)

	}
    return true;
}
*/

bool
AosBsonValueArray::getBuff(AosBsonField **tsFields, AosBuff* mBuffRaw)
{
	bool rslt;
	AosBsonField* field;
    aos_assert_r(mBuffRaw, NULL);
	if(mValueType == AosBsonField::eFieldTypeArray)
	{
	    field = tsFields[AosBsonField::eFieldTypeArray];
	}
	else
	{
        field = tsFields[AosBsonField::eFieldTypeGenericArray];	
	}
    if(!field) return false;
	vector<AosValueRslt> values;
	for(int i = 0; i < mValues.size(); i++)
	{
	   values.push_back(mValues[i]->getValue());
	}
    rslt = field->setField(mStrName, values,  mBuffRaw);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosBsonValueArray::parse()
{
    bool rslt;
	AosValueRslt vv;
	AosBuff *mBuff = mBSON->getCrtBuff();
	char* data = mBuff->data();
	i64 pos = mPos;
	AosBsonField::Type type = static_cast<AosBsonField::Type>(data[pos]); 
	rslt = AosBsonField::skipEname(0, mBuff, pos);
	aos_assert_r(rslt, false);
	mBuff->setCrtIdx(pos);
    if(type == AosBsonField::eFieldTypeArray)
	{
	   AosBsonField::Type dataType = static_cast<AosBsonField::Type>(data[pos]++);
	   mBuff->setCrtIdx(pos);
	   u32 numbers = mBuff->getU32(0);
	   pos += 4;
	   pos += 4;
	   for(int i = 0; i < numbers; i++)
	   {
	       rslt = parseArray(dataType, mBuff, pos);
		   aos_assert_r(rslt, false);
	   }
	}
	else if(type == AosBsonField::eFieldTypeGenericArray)
	{
	   u32 numbers = mBuff->getU32(0);
	   pos += 4;
	   pos += 4;
	   for(int i = 0; i < numbers; i++)
	   {
		   AosBsonField::Type dataType = static_cast<AosBsonField::Type>(data[pos]++);
		   rslt = parseArray(dataType, mBuff, pos);
		   aos_assert_r(rslt, false);
	   }
	}
	else
	{
	   return false;
	}
	mBuff->setCrtIdx(pos);
	return true;
}


bool
AosBsonValueArray::parseArray(
		AosBsonField::Type type,
		AosBuff*   mBuff,
		i64  &pos)
{
	AosValueRslt value;
	mBuff->setCrtIdx(pos);
    AosBsonValueArrayItrPtr itr;
	switch(type)
	{
	    case AosBsonField::eFieldTypeFloat:
		{
		   float vv = mBuff->getFloat(0.0);
		   value.setDouble(vv);
		   pos = pos + sizeof(float);
		   itr = OmnNew AosBsonValueArrayItr(mBSON, AosBsonField::eFieldTypeFloat, mStrName, this, value);
		   mValues.push_back(itr);
		   return true;
		}

		case AosBsonField::eFieldTypeDouble:
		{
		   double vv = mBuff->getDouble(0.0);
		   value.setDouble(vv);
		   pos = pos + sizeof(double);
		   itr = OmnNew AosBsonValueArrayItr(mBSON, AosBsonField::eFieldTypeDouble, mStrName, this, value);
		   mValues.push_back(itr);
		   return true;
		
		}

		case AosBsonField::eFieldTypeString: 
		{
		   OmnString vv = mBuff->getOmnStr("");
		   value.setStr(vv);
		   pos = mBuff->getCrtIdx();
		   itr = OmnNew AosBsonValueArrayItr(mBSON, AosBsonField::eFieldTypeString, mStrName, this, value);
		   mValues.push_back(itr);
		   return true;
		}

		case AosBsonField::eFieldTypeU8:
		{
		   u8 vv = mBuff->getU8(0);
		   value.setU64(vv);
		   pos = pos + sizeof(u8);
		   itr = OmnNew AosBsonValueArrayItr(mBSON, AosBsonField::eFieldTypeU8, mStrName, this, value);
		   mValues.push_back(itr);
		   return true;
		}

		case AosBsonField::eFieldTypeU16:     
		{
		   u16 vv = mBuff->getU16(0);
		   value.setU64(vv);
		   pos = pos + sizeof(u16);
		   itr = OmnNew AosBsonValueArrayItr(mBSON, AosBsonField::eFieldTypeU16, mStrName, this, value);
		   mValues.push_back(itr);
		   return true;
		}

		case AosBsonField::eFieldTypeU32:  
		{
		   u32 vv = mBuff->getU32(0);
		   value.setU64(vv);
		   pos = pos + sizeof(u32);
		   itr = OmnNew AosBsonValueArrayItr(mBSON, AosBsonField::eFieldTypeU32, mStrName, this, value);
		   mValues.push_back(itr);
		   return true;
		}

		case AosBsonField::eFieldTypeU64: 
		{
		   u64 vv = mBuff->getU64(0);
		   value.setU64(vv);
		   pos = pos + sizeof(u64);
		   itr = OmnNew AosBsonValueArrayItr(mBSON, AosBsonField::eFieldTypeU64, mStrName, this, value);
		   mValues.push_back(itr);
		   return true;
		}

		case AosBsonField::eFieldTypeInt8:  
		{
		   i8 vv = (i8)mBuff->getChar(0);
		   value.setI64(vv);
		   pos = pos + sizeof(i8);
		   itr = OmnNew AosBsonValueArrayItr(mBSON, AosBsonField::eFieldTypeInt8, mStrName, this, value);
		   mValues.push_back(itr);
		   return true;
		}

		case AosBsonField::eFieldTypeInt16:   
		{
		   i16 vv = mBuff->getI16(0);
		   value.setI64(vv);
		   pos = pos + sizeof(i16);
		   itr = OmnNew AosBsonValueArrayItr(mBSON, AosBsonField::eFieldTypeInt16, mStrName, this, value);
		   mValues.push_back(itr);
		   return true;
		}

		case AosBsonField::eFieldTypeInt32: 
		{
		   i32 vv = mBuff->getInt(0);
		   value.setI64(vv);
		   pos = pos + sizeof(i32);
		   itr = OmnNew AosBsonValueArrayItr(mBSON, AosBsonField::eFieldTypeInt32, mStrName, this, value);
		   mValues.push_back(itr);
		   return true;
		}

		case AosBsonField::eFieldTypeInt64: 
		{
		   i64 vv = mBuff->getI64(0);
		   value.setI64(vv);
		   pos = pos + sizeof(i64);
		   itr = OmnNew AosBsonValueArrayItr(mBSON, AosBsonField::eFieldTypeInt64, mStrName, this, value);
		   mValues.push_back(itr);
		   return true;
		}

		case AosBsonField::eFieldTypeChar: 
		{
		   char vv = mBuff->getChar(0);
		   value.setChar(vv);
		   pos = pos + sizeof(char);
		   itr = OmnNew AosBsonValueArrayItr(mBSON, AosBsonField::eFieldTypeChar, mStrName, this, value);
		   mValues.push_back(itr);
		   return true;
		}

	/*	case AosDataType::eVarInt:  
		{
		   double vv = mBuff->getDouble(0.0);
		   value = AosValueRslt(vv);
		   pos = pos + sizeof(double);
		   AosBsonValueArrayItrPtr itr = OmnNew AosBsonValueArrayItr(mBSON, AosBsonField::eFieldTypeDouble, mStrName, this, value);
		   mValues.push_back(itr);
		   return true;
		}

		case AosDataType::eVarUint: 
		{
		   double vv = mBuff->getDouble(0.0);
		   value = AosValueRslt(vv);
		   pos = pos + sizeof(double);
		   AosBsonValueArrayItrPtr itr = OmnNew AosBsonValueArrayItr(mBSON, AosBsonField::eFieldTypeDouble, mStrName, this, value);
		   mValues.push_back(itr);
		   return true;
		} */

		default:                    return false;
	}
}


AosValueRslt
AosBsonValueArray::getValue(const i32 idx)
{
	AosValueRslt vv;
   if(idx > mValues.size() || idx < 0)
   {
	   return vv;
   }
   if(mValues.size() == 0)
   {
	   vv.setBool(false);
	   return vv;
   }
   return  mValues[idx]->getValue();
}


/*
//get normal array
bool
AosBsonValueArray::getNormalValue(
		AosRundata   *rdata,
		AosBsonField** &tsFields, 
		OmnString &name, 
		AosBuff* &mBuffRaw)
{
	AosBsonField::Type data_type = getType(mValues[0]);
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeArray];
	mBuffRaw->gotoEnd();
	bool rslt = field->setType(rdata, mBuffRaw);
	if(!rslt) return false;

	AosValueRslt vv;
	field = tsFields[AosBsonField::eFieldTypeString];

	mBuffRaw->gotoEnd();
	rslt = mBuffRaw->setChar(AosBsonField::eStringFieldName);
	if(!rslt) return false;
	rslt = field->setFieldNameStr(name, mBuffRaw);
	if(!rslt) return false;
	mBuffRaw->gotoEnd();
	mBuffRaw->setChar(data_type);
	u32 n = mValues.size();
	// set the numbers of data in array to the buff 
	mBuffRaw->setU32(n);
	field  = tsFields[AosBsonField::eFieldTypeArray];
	// appendArray length to the buff
	mBuffRaw->gotoEnd();
	i64 tempIdx = mBuffRaw->getCrtIdx();
	mBuffRaw->setU32(4);

	// append datas to buff
	for(int i = 0; i < mValues.size(); i++)
	{
		rslt = field->appendFieldValue(rdata, data_type, mValues[i], mBuffRaw);
		if(!rslt) return false;
	}
	mBuffRaw->gotoEnd();
	u32 mDocLength = mBuffRaw->dataLen();
	//set Array Length
	mBuffRaw->setCrtIdx(tempIdx);
	mBuffRaw->setU32(mDocLength - tempIdx - 4);
	return true;
}

//get Ceneric Array
bool                                                       
AosBsonValueArray::getGenericValue(
		     AosRundata   *rdata,
		     AosBsonField** &tsFields, 
			 OmnString   &name, 
			 AosBuff* &mBuffRaw)
{
     // The format is:
     //  valueType(0x34) + nameType +name + value
	 //  value = u32(number of entries)+ arrayLength + array(value1, value2, value3, ....., value)
	 bool rslt;
     mBuffRaw->gotoEnd();
     rslt = mBuffRaw->setChar(AosBsonField::eFieldTypeGenericArray);
     if(!rslt) return false;

     rslt = mBuffRaw->setChar(AosBsonField::eStringFieldName);
     if(!rslt) return false;

     //set name as string'varuint(length) + 'string'(contents)'
     rslt = AosBsonField::setFieldNameStr(name, mBuffRaw);
     if(!rslt) return false;

	 //set the numbers of array into buff
     mBuffRaw->gotoEnd();
	 u32 number = mValues.size();
	 rslt = mBuffRaw->setU32(number);
     if(!rslt) return false;
	 
	 //before set arrayLength wo should set value first ,so wo can 
	 //get arrayLength 
	 //get current Idx
	 mBuffRaw->gotoEnd();
     i64 tempIdx = mBuffRaw->getCrtIdx();
	 mBuffRaw->setU32(0); //use for store arrayLength
      
	 //store values
	 //first:get the type of the data
     AosBsonField* field = tsFields[AosBsonField::eFieldTypeGenericArray];
	 for(u32 i = 0; i < mValues.size(); i++)
	 {
		//get the element data type
        AosDataType::E dataType = mValues[i].getType();
		AosBsonField::Type data_type;
		switch(dataType)
	    {
			// U64 dataType
			case(AosDataType::eU64):
			{
		       // AosBsonField* field = tsFields[AosBsonField::eFieldTypeU64];
			    //two step: first ,add a data type
				//second: add the value to the buff
			    data_type = AosBsonField::eFieldTypeU64;
                mBuffRaw->gotoEnd();
				//store the element pos to vector
	            rslt = mBuffRaw->setChar(data_type);
	            if(!rslt) return false;

	            //set Value
				rslt = field->appendFieldValue(rdata, data_type, mValues[i], mBuffRaw);
	            if(!rslt) return false;
			}
		    break;
			
			// U32 dataType
			case(AosDataType::eU32):
			{
				data_type = AosBsonField::eFieldTypeU32;
                mBuffRaw->gotoEnd();
	            rslt = mBuffRaw->setChar(data_type);
	            if(!rslt) return false;
				rslt = field->appendFieldValue(rdata, data_type, mValues[i], mBuffRaw);
	            if(!rslt) return false;
			}
			break;

			//U16 
			case(AosDataType::eU16):
			{
				data_type = AosBsonField::eFieldTypeU16;
                mBuffRaw->gotoEnd();
	            rslt = mBuffRaw->setChar(data_type);
	            if(!rslt) return false;
				rslt = field->appendFieldValue(rdata, data_type, mValues[i], mBuffRaw);
	            if(!rslt) return false;
			}
            break;

			//U8
			case(AosDataType::eU8):
			{
				data_type = AosBsonField::eFieldTypeU8;
                mBuffRaw->gotoEnd();
	            rslt = mBuffRaw->setChar(data_type);
	            if(!rslt) return false;
				rslt = field->appendFieldValue(rdata, data_type, mValues[i], mBuffRaw);
	            if(!rslt) return false;
			}
            break;
			
			//eChar
			case(AosDataType::eChar):
			 {    
		        //AosBsonField* field = tsFields[AosBsonField::eFieldTypeChar];
				data_type = AosBsonField::eFieldTypeChar;
                mBuffRaw->gotoEnd();
	            rslt = mBuffRaw->setChar(data_type);
	            if(!rslt) return false;
				rslt = field->appendFieldValue(rdata, data_type, mValues[i], mBuffRaw);
	            if(!rslt) return false;
					
			 }
			break;

			//Int64
			case(AosDataType::eInt64):
            {
				data_type = AosBsonField::eFieldTypeInt64;
                mBuffRaw->gotoEnd();
	            rslt = mBuffRaw->setChar(data_type);
	            if(!rslt) return false;
				rslt = field->appendFieldValue(rdata, data_type, mValues[i], mBuffRaw);
	            if(!rslt) return false;
			}
			break;
		    
			//Int32
			case(AosDataType::eInt32):
            {
				data_type = AosBsonField::eFieldTypeInt32;
                mBuffRaw->gotoEnd();
	            rslt = mBuffRaw->setChar(data_type);
	            if(!rslt) return false;
				rslt = field->appendFieldValue(rdata, data_type, mValues[i], mBuffRaw);
	            if(!rslt) return false;
			}
			break;
			
			//Int16
			case(AosDataType::eInt16):
            {
		      //  AosBsonField* field = tsFields[AosBsonField::eFieldTypeInt16];
				data_type = AosBsonField::eFieldTypeInt16;
                mBuffRaw->gotoEnd();
	            rslt = mBuffRaw->setChar(data_type);
	            if(!rslt) return false;
				rslt = field->appendFieldValue(rdata, data_type, mValues[i], mBuffRaw);
	            if(!rslt) return false;
			}
			break;

			//Int8
			case(AosDataType::eInt8):
            {
		      //  AosBsonField* field = tsFields[AosBsonField::eFieldTypeInt8];
				data_type = AosBsonField::eFieldTypeInt8;
                mBuffRaw->gotoEnd();
	            rslt = mBuffRaw->setChar(data_type);
	            if(!rslt) return false;
				rslt = field->appendFieldValue(rdata, data_type, mValues[i], mBuffRaw);
	            if(!rslt) return false;
			}
			break;

			//eBool
			case(AosDataType::eBool):
			{
				data_type = AosBsonField::eFieldTypeBool;
                mBuffRaw->gotoEnd();
	            rslt = mBuffRaw->setChar(data_type);
	            if(!rslt) return false;
				rslt = field->appendFieldValue(rdata, data_type, mValues[i], mBuffRaw);
	            if(!rslt) return false;
			}
			break;

			//Float
			case(AosDataType::eFloat):
			{
		      //  AosBsonField* field = tsFields[AosBsonField::eFieldTypeFloat];
				data_type = AosBsonField::eFieldTypeFloat;
                mBuffRaw->gotoEnd();
	            rslt = mBuffRaw->setChar(data_type);
	            if(!rslt) return false;
				rslt = field->appendFieldValue(rdata, data_type, mValues[i], mBuffRaw);
	            if(!rslt) return false;
			}
			break;

			//Double
			case(AosDataType::eDouble):
			{
		       // AosBsonField* field = tsFields[AosBsonField::eFieldTypeDouble];
				data_type = AosBsonField::eFieldTypeDouble;
                mBuffRaw->gotoEnd();
	            rslt = mBuffRaw->setChar(data_type);
	            if(!rslt) return false;
				rslt = field->appendFieldValue(rdata, data_type, mValues[i], mBuffRaw);
	            if(!rslt) return false;
			}
			break;

			//VarInt
			case(AosDataType::eVarInt):
			{
		       // AosBsonField* field = tsFields[AosBsonField::eFieldTypeVarInt];
				data_type = AosBsonField::eFieldTypeVarInt;
                mBuffRaw->gotoEnd();
	            rslt = mBuffRaw->setChar(data_type);
	            if(!rslt) return false;
				rslt = field->appendFieldValue(rdata, data_type, mValues[i], mBuffRaw);
	            if(!rslt) return false;
			}
			break;

			//VarUint
			case(AosDataType::eVarUint):
			{
		       // AosBsonField* field = tsFields[AosBsonField::eFieldTypeVarUint];
				data_type = AosBsonField::eFieldTypeVarUint;
                mBuffRaw->gotoEnd();
	            rslt = mBuffRaw->setChar(data_type);
	            if(!rslt) return false;
				rslt = field->appendFieldValue(rdata, data_type, mValues[i], mBuffRaw);
	            if(!rslt) return false;
			}
			break;
			
			//String
			case(AosDataType::eString):
			{
		       // AosBsonField* field = tsFields[AosBsonField::eFieldTypeString];
				data_type = AosBsonField::eFieldTypeString;
                mBuffRaw->gotoEnd();
	            rslt = mBuffRaw->setChar(data_type);
	            if(!rslt) return false;
				rslt = field->appendFieldValue(rdata, data_type, mValues[i], mBuffRaw);
	            if(!rslt) return false;
			}
			break;

            default:
		    OmnAlarm << "invalid data type" << enderr;
	    }
	 }
	mBuffRaw->gotoEnd();
	u32 mDocLength = mBuffRaw->dataLen();
	// set array length
	// set the pointer to the begin of the ArrayLength
	mBuffRaw->setCrtIdx(tempIdx);
	mBuffRaw->setU32(mDocLength - tempIdx - 4);
    return true;
}
*/

AosBsonField::Type                  
AosBsonValueArray::getType(AosValueRslt value)
{
    switch (value.getType())
    {
	case AosDataType::eFloat:   return AosBsonField::eFieldTypeFloat;

	case AosDataType::eDouble:  return AosBsonField::eFieldTypeDouble;

	case AosDataType::eString:  return AosBsonField::eFieldTypeString;

	case AosDataType::eNull:    return AosBsonField::eFieldTypeNull;

	case AosDataType::eU8:      return AosBsonField::eFieldTypeU8;

	case AosDataType::eU16:     return AosBsonField::eFieldTypeU16;

	case AosDataType::eU32:     return AosBsonField::eFieldTypeU32;

	case AosDataType::eU64:     return AosBsonField::eFieldTypeU64;

	case AosDataType::eInt8:    return AosBsonField::eFieldTypeInt8;

	case AosDataType::eInt16:   return AosBsonField::eFieldTypeInt16;

	case AosDataType::eInt32:   return AosBsonField::eFieldTypeInt32;

	case AosDataType::eInt64:   return AosBsonField::eFieldTypeInt64;

	case AosDataType::eChar:    return AosBsonField::eFieldTypeChar;

	case AosDataType::eVarInt:  return AosBsonField::eFieldTypeVarInt;

	case AosDataType::eVarUint: return AosBsonField::eFieldTypeVarUint;

	default:                    return AosBsonField::eInvalidFieldType;
    }
}
