////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DbRcdMySql.cpp
// Description:
//	This is the database record for MySQL.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "MySQL/DbRcdMySql.h"

#include "Alarm/Alarm.h"
#include "Database/DbTable.h"
#include "Debug/Debug.h"
#include "Porting/Atol.h"
#include "util_c/strutil.h"


OmnDbRcdMySql::~OmnDbRcdMySql()
{
}


OmnIpAddr 
OmnDbRcdMySql::getAddr(const int index, const OmnString &d, OmnRslt &rslt) const
{
	return OmnIpAddr(getStr(index, d, rslt));
}


OmnString
OmnDbRcdMySql::getStr(const int index, const OmnString &d, OmnRslt &rslt) const 
{
	if (index >= mNumFields)
	{
		rslt = OmnWarn << "Accessing DbRecordMySql. Index out of bound: "
			<< index
			<< ". Number of fields: " << mNumFields
			<< ". The record is: \n"
			<< toString() << enderr;
		return d;
	}

	if (!mRow[index])
	{
		return d;
	}

	return mRow[index];	
}


void
OmnDbRcdMySql::getBinary(const int index, 
						 char *buff, 
						 const uint size, 
						 OmnRslt &rslt) const 
{
	if (index >= mNumFields)
	{
		rslt = OmnWarn << "Accessing DbRecordMySql. Index out of bound: "
			<< index
			<< ". The record is: \n"
			<< toString() << enderr;
		return;
	}

	if (!mRow[index])
	{
		rslt = OmnAlarm << "Rwo is null" << enderr;
		return;
	}

	memcpy(buff, mRow[index], size);
}


char
OmnDbRcdMySql::getChar(const int index, const char d, OmnRslt &rslt) const 
{
    if (index >= mNumFields)
    {
        rslt = OmnWarn << "Accessing DbRecordMySql. Index out of bound: "
            << index
            << ". The record is: \n"
            << toString() << enderr;
        return d;
    }

	if (!mRow[index])
	{
		return d; 
	}
 
	return mRow[index][0];	
}


bool
OmnDbRcdMySql::getBool(const int index, const bool d, OmnRslt &rslt) const 
{
    if (index >= mNumFields)
    {
        rslt = OmnWarn << "Accessing DbRecordMySql. Index out of bound: "
            << index
            << ". The record is: \n"
            << toString() << enderr;
        return d;
    }

	if (!mRow[index])
	{
		return d; 
	}
 
	return (mRow[index][0] != 0);	
}


int
OmnDbRcdMySql::getInt(const int index, const int d, OmnRslt &rslt) const
{
    if (index >= mNumFields)
    {
        rslt = OmnWarn << "Accessing DbRecordMySql. Index out of bound: "
            << index
            << ". The record is: \n"
            << toString() << enderr;
        return d;
    }

	if (!mRow[index])
	{
		return d;
	}

    return atoi(mRow[index]);
}


/*
OmnUint8
OmnDbRcdMySql::getUint8(const int index, const OmnUint8 &d, OmnRslt &rslt) const
{
    if (index >= mNumFields)
    {
        rslt = OmnWarn << "Accessing DbRecordMySql. Index out of bound: "
            << index
            << ". The record is: \n"
            << toString() << enderr;
        return d;
    }

	if (!mRow[index])
	{
		return d;
	}

	OmnUint8 value(mRow[index], 1);
	return value;
}
*/
	

int64_t
OmnDbRcdMySql::getInt64(const int index, const int64_t &d, OmnRslt &rslt) const
{
    if (index >= mNumFields)
    {
        rslt = OmnWarn << "Accessing DbRecordMySql. Index out of bound: "
            << index
            << ". The record is: \n"
            << toString() << enderr;
        return d;
    }

    if (!mRow[index])
    {
        return d;
    }

    return OmnAtoll(mRow[index]);
}


u64
OmnDbRcdMySql::getU64(const int index, const u64 &d, OmnRslt &rslt) const
{
    if (index >= mNumFields)
    {
        rslt = OmnWarn << "Accessing DbRecordMySql. Index out of bound: "
            << index
            << ". The record is: \n"
            << toString() << enderr;
        return d;
    }

    if (!mRow[index])
    {
        return d;
    }

    return OmnAtoull(mRow[index]);
}


double
OmnDbRcdMySql::getDouble(const int index, const double d, OmnRslt &rslt) const
{
    if (index >= mNumFields)
    {
        rslt = OmnWarn << OmnErrId::eWarnProgramError
            << "Accessing DbRecordMySql. Index out of bound: "
            << index
            << ". The record is: \n"
            << toString() << enderr;
        return d;
    }

    if (!mRow[index])
    {
        return d;
    }

    return atof(mRow[index]);
}


OmnString
OmnDbRcdMySql::toString() const
{
	OmnString str = "DbRcdMySql: ";
	for (int i=0; i<mNumFields; i++)
	{
		if (!mRow[i])
		{
			//
			// It is a null field
			//
			str << "\n";
		}
		else
		{
			str << "\n" << OmnString(mRow[i]);
		}
	}

	return str;
}


OmnDateTime		
OmnDbRcdMySql::getDateTime(const int index, 
						   OmnRslt &rslt) const
{
	 OmnDateTime time(getStr(index, "", rslt).getBuffer(), 
		 OmnDateTime::eYYYYMMDDHHMMSS);
	 return time;
}


u32             
OmnDbRcdMySql::getU32(const int index, const u32 d, OmnRslt &rslt) const
{
    if (index >= mNumFields)
    {
        rslt = OmnWarn << "Accessing DbRecordMySql. Index out of bound: "
            << index
            << ". The record is: \n"
            << toString() << enderr;
        return d;
    }

	if (!mRow[index])
	{
		return d;
	}

	u32 vv;
    aos_assert_r(!aos_atou32(mRow[index], strlen(mRow[index]), &vv), d);
	return vv;
}


// Tracy, 06/09/2011
bool 
OmnDbRcdMySql::getValue(const int idx, const OmnString &type, OmnString &value) const
{
	OmnRslt rslt;
	if(type == "int")
	{
		int ival = getInt(idx, -1, rslt);
		value << ival;
	}
	else if(type == "string")
	{
		OmnString strval = getStr(idx, "", rslt);
		value = strval;
	}
	return true;	
}

