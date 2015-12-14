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
// A Name-Value doc is in the following format:
// 	(
// 		name: doc_value, 
// 		name: doc_value, 
// 		...
// 		name: doc_value
// 	)
//
// where 'doc_value' can be one of the following:
// 	1. Quoted string,
// 	2. Unquoted string,
// 	3. An array: [doc_value, doc_value, ..., doc_value],
// 	4. A tuple: (doc_value, doc_value, ...),
// 	5. A name-value doc: (name:doc_value, name:doc_value, ...)
//
// Modification History:
// 2015/05/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/NameValueDoc.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"

#include "API/AosApiP.h"

static int sgNameValueCreated = 0;

AosNameValueDoc::AosNameValueDoc()
:
mType(eNullDoc),
mBuff(AosBuff(AosMemoryCheckerArgsBegin)),
mPoison(1234567890)
{
	sgNameValueCreated++;
	OmnScreen << "NameValue Created: " << this << endl;
	OmnScreen << "mPosition: " << mPoison << endl;
	OmnScreen << "Total Name value: " << sgNameValueCreated << endl;
}


AosNameValueDoc::AosNameValueDoc(const Type type)
:
mType(type),
mBuff(AosBuff(AosMemoryCheckerArgsBegin)),
mPoison(1234567891)
{
	sgNameValueCreated++;
	OmnScreen << "NameValue Created: " << this << endl;
	OmnScreen << "mPosition: " << mPoison << endl;
	OmnScreen << "Total Name value: " << sgNameValueCreated << endl;
}


AosNameValueDoc::AosNameValueDoc(
		const Type type,
		const char *data,
		const int len) 
:
mType(type),
mBuff(AosBuff(AosMemoryCheckerArgsBegin)),
mPoison(1234567892)
{
	// This constructor assumes (data, len) defines the following:
	// 	doc_value, doc_value, ..., doc_value
	//
	AosBuff buff(const_cast<char*>(data), len, len, true AosMemoryCheckerArgs);
	if (!initValue(buff))
	{
		OmnThrowException("invalid_doc");
		return;
	}

	sgNameValueCreated++;
	OmnScreen << "NameValue Created: " << this << endl;
	OmnScreen << "mPosition: " << mPoison << endl;
	OmnScreen << "Total Name value: " << sgNameValueCreated << endl;
}


AosNameValueDoc::AosNameValueDoc(AosBuff &buff)
:
mType(eNullDoc),
mBuff(AosBuff(AosMemoryCheckerArgsBegin)),
mPoison(1234567893)
{
	// 'buff' holds a value:
	// 		value_type		(u8)
	// 		value_contents	(variable)

	sgNameValueCreated++;
	OmnScreen << "NameValue Created: " << this << endl;
	OmnScreen << "mPosition: " << mPoison << endl;
	OmnScreen << "Total Name value: " << sgNameValueCreated << endl;

	mType = (Type)buff.getU8(0);

	if (!initValue(buff))
	{
		OmnThrowException("invalid_doc");
		return;
	}
}


bool
AosNameValueDoc::initValue(AosBuff &buff)
{
	// This function assumes the current position of 'buff' holds 
	// a DocValue. Its format is:
	// 		field_type		(u8)
	// 		contents		(variable)
	// Note that the field type has already been consumed.
	switch (mType)
	{
	case eStrValue:
		 // 	field_type
		 // 	OmnString
		 return buff.getOmnStr(mStrValue);

	case eTupleValue:
	case eArrayValue:
		 return parseArrayValue(buff);

	case eDoc:
		 mBuff.setData1(buff.data(), buff.dataLen(), true);
		 return true;

	default:
		 break;
	}

	OmnAlarm << "unrecognized type: " << mType << enderr;
	return false;
}


AosNameValueDoc::AosNameValueDoc(const AosNameValueDoc &rhs)
:
mBuff(AosBuff(AosMemoryCheckerArgsBegin))
{
	sgNameValueCreated++;
	OmnScreen << "NameValue Created: " << this << endl;
	OmnScreen << "mPosition: " << mPoison << endl;
	OmnScreen << "Total Name value: " << sgNameValueCreated << endl;

	mBuff = rhs.mBuff;
	mType = rhs.mType;
	mStrValue = rhs.mStrValue;
	mDocValues = rhs.mDocValues;
	mPoison = rhs.mPoison;
}


AosNameValueDoc::~AosNameValueDoc()
{
	mPoison = 9876543210;
	sgNameValueCreated--;
	OmnScreen << "NameValue deleted: " << this << endl;
	OmnScreen << "Total Name value: " << sgNameValueCreated << endl;
}


bool
AosNameValueDoc::clear()
{
	mType = eNullDoc;
	mBuff.clear();
	return true;
}


void
AosNameValueDoc::reset()
{
	mBuff.reset();
}


bool
AosNameValueDoc::hasMore()
{
	return mBuff.hasMore();
}


bool
AosNameValueDoc::parseArrayValue(AosBuff &buff)
{
	// The format is:
	// 		value_type			(u8)
	// 		size				(int)
	// 		value				(variable)
	// 		value				(variable)
	// 		...
	// 		value				(variable)
	int size = buff.getInt(-1);
	aos_assert_r(size >= 0, false);

	for (int i=0; i<size; i++)
	{
		try
		{
			AosNameValueDoc value(buff);
			mDocValues.push_back(value);
		}
	catch (...)
	{
		OmnAlarm << "Failed parsing name-value" << enderr;
		return false;
	}
	}


	return true;
}


bool 
AosNameValueDoc::nextField(
		AosRundata *rdata, 
		AosExprObjPtr &expr)
{
	// This function retrieves the next [name: value] and converts 
	// it to a name-value expr.
	// Fields are encoded as:
	// 		fieldname			(OmnString)
	// 		fieldtype			(u8)
	// 		fieldvalue			depend on specific field
	//

	// Retrieve field name
	OmnString name;
	if (!mBuff.getOmnStr(name))
	{
		AosLogError(rdata, true, "failed_retrieving_value") 
			<< AosFN("Name Value Doc") << mBuff.dumpData(true, "NameValueDoc") << enderr;
		return false;
	}
	
	//confirm the vale type
	//DocValueType type = (DocValueType)mBuff.getU8(0);
	//mBuff.setCrtIdx(mBuff.getCrtIdx() - 1);

	expr = convertToExpr(rdata, name, mBuff);
	aos_assert_r(expr, false);

	expr = Jimo::jimoCreateNameValueExpr(name, expr);
	
	return true;
	/*
	Type field_type = (Type) mBuff.getU8(0);
	if (field_type == eStrValueDoc)
	Type field_type = (Type) mBuff.getU8(0);
	if (field_type == eNormalDoc)
	{
		OmnString value;
		if (!mBuff.getOmnStr(value))
		{
			AosLogError(rdata, true, "failed_retrieving_value") 
				<< AosFN("Field Name") << name
				<< AosFN("Name Value Doc") << mBuff.dumpData(true, "NameValueDoc") << enderr;
			return false;
		}

		expr = Jimo::jimoCreateNameValueExpr(name, value);
		if (!expr) return false;
		return true;
	}

	if (field_type == eArrayDoc || field_type == eTupleDoc)
	{
		// It is an array of values.
		// 	fieldname			(OmnString)
		// 	field_type			(u8)
		// 	size				(int)
		// 	value				(variable)
		// 	value				(variable)
		// 	...
		// 	value				(variable)

		int size = mBuff.getInt(-1);
		aos_assert_r(size >= 0, false);

		vector<AosExprObjPtr> exprs;
		for (int i=0; i<size; i++)
		{
			AosExprObjPtr expr = convertToExpr(mBuff);
			aos_assert_r(expr, false);
		}

		expr = Jimo::jimoCreateNameValueExpr(name, exprs);
		if (!expr) return false;
		return true;
	}

	AosLogError(rdata, true, "unrecognized_type")
		<< AosFN("Type") << field_type << enderr;
	return false;
	*/
}


AosExprObjPtr
AosNameValueDoc::convertToExpr(
		AosRundata *rdata, 
		const OmnString &name,
		AosBuff &buff)
{
	// 'buff' starts with an AosNameValueDo's value. 
	// 		value_type		(u8)
	// 		value_contents	(variable)
	DocValueType type = (DocValueType)buff.getU8(0);
	OmnString value;
	switch (type)
	{
	case eStrValue:
		 if (!buff.getOmnStr(value)) return NULL;
		 else
		 {
			 //OmnString errmsg = "";
			 //value << ";";
			 return Jimo::jimoCreateStrExpr(value);
			 //AosExprObjPtr expr = AosParseExpr(value, errmsg, rdata);
			 //aos_assert_r(expr,0);
			 //return expr; 
		 }
	
	case eTupleValue:
	case eArrayValue:
		 {
			 // value_type			(u8)
			 // size				(int)
			 // value		
			 // value
			 // ...
			  int size = buff.getInt(-1);
			  aos_assert_r(size >= 0, 0);
			  
			  vector<AosExprObjPtr> exprs;
			  for (int i=0; i<size; i++)
			  {
				  AosExprObjPtr expr = convertToExpr(rdata, name, buff);
				  aos_assert_r(expr, 0);
				  exprs.push_back(expr);
			  }
			  return Jimo::jimoCreateNameValueExpr(exprs);
		 };
	
	case eDocValue:
		 // value_type       (u8) 
		 // dataLength       (int)
		 // we assume that the value format must be the following format:
		 //     name:value  ..... name:value
		 //
		 {
			 int data_len;
			 buff.getCharStr(data_len); 
			 buff.setCrtIdx(buff.getCrtIdx() - data_len);
			 int buffLen = buff.getCrtIdx() + data_len;
			 vector<AosExprObjPtr> exprs;
			 OmnString docValueName = "";
			 while(buff.getCrtIdx() < buffLen)
			 {
			 	buff.getOmnStr(docValueName);
				AosExprObjPtr expr = convertToExpr(rdata, docValueName, buff);
				expr = Jimo::jimoCreateNameValueExpr(docValueName, expr);
				exprs.push_back(expr);
			 }
			 return Jimo::jimoCreateNameValueExpr(exprs);
		 }

	default:
		 break;
	}

	OmnShouldNeverComeHere;
	return NULL;
}


bool
AosNameValueDoc::appendField(
		const OmnString &name, 
		const AosNameValueDoc &value)
{
	// The format is:
	// 	name			(OmnString)
	// 	value_type		(u8)
	// 	size			(u32)
	// 	contents		(char str)
	aos_assert_r(name != "", false);

	switch (mType)
	{
	case eNullDoc:
		 mType = eDoc;
		 break;
		 
	case eTupleDoc:
	case eArrayDoc:
	case eStrValueDoc:
		 OmnAlarm << "incompatible doc type: " << mType << enderr;
		 return false;

	case eDoc:
		 break;

	default:
		 OmnAlarm << "Unrecognized doc type: " << mType << enderr;
		 return false;
	}

	// Append the name
	mBuff.setOmnStr(name);
	value.serialize(mBuff);
	return true;
}


OmnString
AosNameValueDoc::getValue(
		const OmnString &name, 
		const OmnString &dft, 
		AosBuff &buff)
{
	// The doc is encoded as:
	// 		name			(OmnString)
	// 		value_type		(u8)
	// 		...
	aos_assert_r(mType == eNullDoc, dft);
	buff.reset();
	OmnString value_name;
	OmnString value;
	bool rslt;
	while (buff.hasMore())
	{
		rslt = buff.getOmnStr(value_name); 
		aos_assert_r(rslt, dft);

		u8 value_type = buff.getU8(0);	
		switch (value_type)
		{
		case eStrValueDoc:
			 if (name == value_name)
			 {
			 	rslt = buff.getOmnStr(value);
			 	aos_assert_r(rslt, dft);
			 	return value;
			 }
			 buff.skipOmnStr();
			 break;

		case eArrayDoc:
		case eTupleDoc:
			 if (name == value_name)
			 {
				 OmnAlarm << "value_type_mismatch" << enderr;
				 return dft;
			 }

			 skipValue(value_type, buff);
			 break;

		default:
			 OmnAlarm << "Invalid type: " << value_type << enderr;
			 return dft;
		}
	}

	return dft;
}


bool
AosNameValueDoc::skipValue(
		const u8 type,
		AosBuff &buff)
{
	// This function skips the current field value. It assumes
	// the current position points right after the value type byte.
	switch (type)
	{
	case eStrValue:
		 buff.skipOmnStr();
		 return true;

	case eTupleValue:
	case eArrayValue:
		 {
		 	int size = buff.getInt(-1);
		 	for (int i=0; i<size; i++)
		 	{
				u8 tt = buff.getU8(0);
			 	skipValue(tt, buff);
		 	}
		 	return true;
		 }
	
	default:
		 break;
	}

	OmnAlarm << "Invalid value" << type << enderr;
	return false;
}


bool
AosNameValueDoc::getValue(
		const OmnString &name, 
		AosNameValueDoc &doc, 
		bool &found)
{
	aos_assert_r(mType == eDoc, false);

	found = false;
	mBuff.reset();
	OmnString nn;
	OmnString strValue;
	bool rslt;
	doc.clear();
	while (mBuff.hasMore())
	{
		rslt = mBuff.getOmnStr(nn); 
		aos_assert_r(rslt, false);
		u8 type = mBuff.getU8(0);	
		switch (type)
		{
		case eTupleDoc:
		case eArrayDoc:
			 if (name == nn)
			 {
			 	 // 	name			(OmnString)
				 // 	Type			(u8)
			 	 // 	size			(u32)
			 	 // 	contents		(char str)
				 found = true;
				 int size = mBuff.getInt(-1);
				 aos_assert_r(size, false);
				 if (size == 0) return true;

				 int data_len;
				 char *data = mBuff.getCharStr(data_len);
				 aos_assert_r(data, false);
				 AosBuff bb(data, data_len, data_len, true AosMemoryCheckerArgs);
				 for (int i=0; i<size; i++)
				 {
					 u8 tt = bb.getU8(0);
					 char *dd = bb.getCharStr(data_len);
					 aos_assert_r(dd, false);

					 try
					 {
					 	 AosNameValueDoc doc((Type)tt, dd, data_len);
					 	 mDocValues.push_back(doc);
					 }

					 catch (...)
					 {
						 OmnAlarm << "failed parsing the doc" << enderr;
						 return false;
					 }
				 }
				 return true;
			 }

			 // Need to skip
			 break;

		case eStrValueDoc:
			 // 	name		(OmnString)
			 // 	type		(u8)
			 // 	value		(OmnString)
			 {
				 if (name != nn)
				 {
					 mBuff.skipOmnStr();
					 mBuff.skipOmnStr();
				 }
				 mBuff.getOmnStr(strValue);
				 AosBuff buff(AosMemoryCheckerArgsBegin);
				 buff.setU8((u8)type);
				 buff.setOmnStr(strValue);
				 doc.setBuff(buff);
			 }
			 break;

		case eDoc:
			 // Its format is:
			 //		name		(OmnString),
			 //		eDoc		(u8)
			 //		buff		(xxx)
			 if (name == nn)
			 {   
				 int dataLen;
				 char* data = mBuff.getCharStr(dataLen);
				 AosBuff buff(data, dataLen, dataLen, true AosMemoryCheckerArgs);
				 doc.mType = eDoc;
				 doc.setBuff(buff);
			 }
			 else
			 {
				 mBuff.skipCharStr();
			 }
			 break;

		default:
			 OmnAlarm << "Invalid type: " << type << enderr;
			 return false;
		}
	}

	return true;
}


bool
AosNameValueDoc::setStrValue(const OmnString &value)
{
	mType = eStrValueDoc; 
	mStrValue = value;
	return true;
}


bool 
AosNameValueDoc::appendValue(const AosNameValueDoc &doc)
{
	// A Tuple doc buff format is:
	// 		field_type		(u8), it should be eTupleDoc
	// 		name			(OmnString)
	// 		size			(int)
	// 		contents		(variable)
	//
	aos_assert_r(mType == eTupleDoc || mType == eArrayDoc, false);
	mDocValues.push_back(doc);
	return true;
}


bool
AosNameValueDoc::serialize(AosBuff &buff) const
{
	// This function serializes its value to 'buff'.
	u32 size = mDocValues.size();
	switch (mType)
	{
	case eTupleDoc:
	case eArrayDoc:
		 // It saves its contents into mBuff in the following format:
		 // 	eTupleDoc					(u8)
		 // 	size						(int)
		 // 	contents for first value	(variable)
		 // 	contents for second value	(variable)
		 // 	...
		 // For example, if it is ('f1', 'f2', 'f3')
		 // It should be encoded as:
		 // 	value_type (eTupleDoc)		(u8)
		 // 	3							(int)
		 // 	eStrValue					(u8)
		 // 	'f1'						(OmnString)
		 // 	eStrValue					(u8)
		 // 	'f2'						(OmnString)
		 // 	eStrValue					(u8)
		 // 	'f3'						(OmnString)
		 buff.appendU8(mType);
		 buff.appendInt(size);
		 for (u32 i=0; i<size; i++)
		 {
			 mDocValues[i].serialize(buff);
		 }
		 return true;

	case eStrValueDoc:
		 // Buff format:
		 // 	eStrValue		(u8)
		 // 	value			(OmnString)
		 buff.appendU8(eStrValue);
		 buff.appendOmnStr(mStrValue);
		 return true;

	case eDoc:
		 // (
		 //     first_name: "chen",
		 //	  	last_name: "ding",
		 //	  	addr: (street:"123", zipcode:345)
	     // )
		 buff.appendU8(mType);
		 buff.setCharStr(mBuff.data(), mBuff.dataLen());
		 return true;
	
	default:
		 break;
	}

	OmnShouldNeverComeHere;
	return false;
}


OmnString
AosNameValueDoc::dump()
{
	OmnString str;
	dump(str, 0);
	return str;
}


bool
AosNameValueDoc::dump(OmnString &str, const int level)
{
	// This function dumps the doc in a nice format.
	switch (mType)
	{
	case eTupleDoc:
		 return dumpTupleDoc(str, level);

	case eArrayDoc:
		 return dumpArrayDoc(str, level);

	case eStrValueDoc:
		 return dumpNormalDoc(str, level);

	case eDoc:
		 return dumpDoc(str, level);
	
	default:
		 break;
	}

	str << "*** Error Encountered: invalid doc type: " << mType;
	return false;
}


bool
AosNameValueDoc::dumpTupleDoc(OmnString &str, const int level)
{
	// This is a tuple doc. It dumps its contents as:
	// 		(
	// 			value, 
	// 			value,
	// 			...
	// 		)
	if (level > 0)
	{
		OmnString spaces(level*4, ' ', true);
		str << spaces;
	}

	str << "(\n";
	for (u32 i=0; i<mDocValues.size(); i++)
	{
		if (!dumpValue(str, level+1, (i>0))) return false;
	}

	if (level > 0)
	{
		OmnString spaces(level*4, ' ', true);
		str << spaces;
	}

	str << ")\n";
	return true;
}


bool
AosNameValueDoc::dumpArrayDoc(OmnString &str, const int level)
{
	// This is a tuple doc. It dumps its contents as:
	// 		[
	// 			value, 
	// 			value,
	// 			...
	// 		]
	if (level > 0)
	{
		OmnString spaces(level*4, ' ', true);
		str << spaces;
	}

	str << "[\n";
	for (u32 i=0; i<mDocValues.size(); i++)
	{
		if (!dumpValue(str, level+1, (i>0))) return false;
	}

	if (level > 0)
	{
		OmnString spaces(level*4, ' ', true);
		str << spaces;
	}

	str << "]\n";
	return true;
}


bool
AosNameValueDoc::dumpNormalDoc(OmnString &str, const int level)
{
	// Normal doc is in the form:
	// 	???
	OmnNotImplementedYet;
	return false;
}


bool
AosNameValueDoc::dumpDoc(OmnString &str, const int level)
{
	// A doc is in the form:
	// 	???
	OmnNotImplementedYet;
	return false;
}


bool
AosNameValueDoc::dumpValue(OmnString &str, const int level, const bool with_comma)
{
	// This function dumps a specific value. 
	OmnNotImplementedYet;
	return false;
}

