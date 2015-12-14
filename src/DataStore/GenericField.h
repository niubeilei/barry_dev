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
// 12/22/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataStore_GenericField_h
#define AOS_DataStore_GenericField_h

#include "aosUtil/Types.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlInterface/XmlRc.h"

class TiXmlElement;

class AosGenericField : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum DataType
	{
		eType_Invalid,
		eType_Str, 
		eType_Char,
		eType_Bool,
		eType_Int,
		eType_Int64,
		eType_U32,
		eType_U64,
		eType_Double
	};

	enum DbDataType
	{
		eDbType_Invalid
	};

private:
	DataType		mDataType;
	DbDataType		mDbDataType;
	OmnString		mFieldName;
	bool			mNotNull;
	bool			mPrimaryKey;
	OmnString		mDftValue;
	bool			mDftValueSet;
	bool			mValueSet;

	union
	{
		char *		m_str;
		char		m_char;
		bool		m_bool;
		int			m_int;
		int64_t		m_int64;
		u32			m_u32;
		u64			m_u64;
		double		m_double;
	} mValue;

public:
	AosGenericField();
	virtual ~AosGenericField();

	bool	parse(const TiXmlElement *elem, 
				AosXmlRc &errcode, 
				OmnString &errmsg);
	bool	parse(const OmnString &name, 
				const AosGenericField::DataType type,
				const OmnString &value, 
				const OmnString &dft_value);

	bool	setValue(const OmnString &value);
	bool	setValue(const char value);
	bool	setValue(const bool value);
	bool	setValue(const int value);
	bool	setValue(const u32 value);
	bool	setValue(const int64_t value);
	bool	setValue(const u64 value);
	bool	setValue(const double value);

	OmnString	getValueStr() const;
	DataType	getDataType() const {return mDataType;}
	OmnString	getFieldName() const {return mFieldName;}
	bool		isStrType() const;
	bool		isValueSet() const {return mValueSet;}
	bool		isMandatory() const {return mNotNull;}
	bool		isPrimaryKey() const {return mPrimaryKey;}
	bool		setValue(TiXmlElement *def, 
					AosXmlRc &errcode, 
					OmnString &errmsg);
	DataType 	convertDataType(const char *type);
	DbDataType 	convertDbDatatype(const char *);
	bool		getDftValue(OmnString &value);
	void		resetValue();

};
#endif
