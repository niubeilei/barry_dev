////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DbRcdSQLite.cpp
// Description:
//	This is the database record for SQLite.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "SQLite/DbRcdSQLite.h"

#include "Alarm/Alarm.h"
#include "Database/DbTable.h"
#include "Debug/Debug.h"
#include "SQLite/DbTableSQLite.h"
#include "Porting/Atol.h"


OmnDbRcdSQLite::~OmnDbRcdSQLite()
{
	if (mDeleteFlag)
	{
		sqlite3_free_table(mDbRslt);
	}
}


OmnIpAddr 
OmnDbRcdSQLite::getAddr(const int index, const OmnString &d, OmnRslt &rslt) const
{
	return OmnIpAddr(getStr(index, d, rslt));
}


OmnString
OmnDbRcdSQLite::getStr(const int index, const OmnString &d, OmnRslt &rslt) const 
{
	if (index >= mNumFields)
	{
		rslt = OmnWarn << "Accessing DbRecordSQLite. Index out of bound: "
			<< index
			<< ". The record is: \n"
			<< toString() << enderr;
		return d;
	}

	if (!mDbRslt[mCrtIndex*mNumFields + index])
	{
	    return d;
	}
	
	return mDbRslt[mCrtIndex*mNumFields + index];
}


void
OmnDbRcdSQLite::getBinary(const int index, 
						 char *buff, 
						 const uint size, 
						 OmnRslt &rslt) const 
{
	if (index >= mNumFields)
	{
		rslt = OmnWarn << "Accessing DbRecordSQLite. Index out of bound: "
			<< index
			<< ". The record is: \n"
			<< toString() << enderr;
		return;
	}

	if (!mDbRslt[mCrtIndex*mNumFields + index])
	{       
		rslt = false;
	    return;
	}   
	//
	//To get binary data from database.
	// 
	
	memcpy(buff,mDbRslt[mCrtIndex*mNumFields + index],size);
	rslt = true;
	return;
}


char
OmnDbRcdSQLite::getChar(const int index, const char d, OmnRslt &rslt) const 
{
    if (index >= mNumFields)
    {
        rslt = OmnWarn << "Accessing DbRecordSQLite. Index out of bound: "
            << index
            << ". The record is: \n"
            << toString() << enderr;
        return d;
    }

	if (!mDbRslt[mCrtIndex*mNumFields + index])
	{
		return d; 
	}
 
	return mDbRslt[mCrtIndex*mNumFields + index][0];
}


bool
OmnDbRcdSQLite::getBool(const int index, const bool d, OmnRslt &rslt) const 
{
    if (index >= mNumFields)
    {
        rslt = OmnWarn << "Accessing DbRecordSQLite. Index out of bound: "
            << index
            << ". The record is: \n"
            << toString() << enderr;
        return d;
    }

	if (!mDbRslt[mCrtIndex*mNumFields + index])
	{
		return d; 
	}
 
	return (mDbRslt[mCrtIndex*mNumFields + index] != 0);	
}


int
OmnDbRcdSQLite::getInt(const int index, const int d, OmnRslt &rslt) const
{
    if (index >= mNumFields)
    {
        rslt = OmnWarn << "Accessing DbRecordSQLite. Index out of bound: "
            << index
            << ". The record is: \n"
            << toString() << enderr;
        return d;
    }

	if (!mDbRslt[mCrtIndex*mNumFields + index])
	{
		return d;
	}

    return atoi(mDbRslt[mCrtIndex*mNumFields + index]);
}


// 
// Chen Ding, 03/20/2007
//
u32
OmnDbRcdSQLite::getU32(const int index, const u32 d, OmnRslt &rslt) const
{
    if (index >= mNumFields)
    {
        rslt = OmnWarn << "Accessing DbRecordSQLite. Index out of bound: "
            << index
            << ". The record is: \n"
            << toString() << enderr;
        return d;
    }

	if (!mDbRslt[mCrtIndex*mNumFields + index])
	{
		return d;
	}

    return (u32)atoi(mDbRslt[mCrtIndex*mNumFields + index]);
}


int64_t
OmnDbRcdSQLite::getInt64(const int index, const int64_t &d, OmnRslt &rslt) const
{
    if (index >= mNumFields)
    {
        rslt = OmnWarn << "Accessing DbRecordSQLite. Index out of bound: "
            << index
            << ". The record is: \n"
            << toString() << enderr;
        return d;
    }

    if (!mDbRslt[mCrtIndex*mNumFields + index])
    {
        return d;
    }

    return OmnAtoll(mDbRslt[mCrtIndex*mNumFields + index]);
}


u64
OmnDbRcdSQLite::getU64(const int index, const u64 &d, OmnRslt &rslt) const
{
    if (index >= mNumFields)
    {
        rslt = OmnWarn << "Accessing DbRecordSQLite. Index out of bound: "
            << index
            << ". The record is: \n"
            << toString() << enderr;
        return d;
    }

    if (!mDbRslt[mCrtIndex*mNumFields + index])
    {
        return d;
    }

    return OmnAtoull(mDbRslt[mCrtIndex*mNumFields + index]);
}


double
OmnDbRcdSQLite::getDouble(const int index, const double d, OmnRslt &rslt) const
{
    if (index >= mNumFields)
    {
        rslt = OmnWarn << OmnErrId::eWarnProgramError
            << "Accessing DbRecordSQLite. Index out of bound: "
            << index
            << ". The record is: \n"
            << toString() << enderr;
        return d;
    }

    if (!mDbRslt[mCrtIndex*mNumFields + index])
    {
        return d;
    }

    return atof(mDbRslt[mCrtIndex*mNumFields + index]);
}


OmnString
OmnDbRcdSQLite::toString() const
{
	OmnString str = "DbRcdSQLite: ";
	for (int i=0; i<mNumFields; i++)
	{
	    if (!mDbRslt[mCrtIndex*mNumFields + i])
	    {
	        //
	        // It is a null field
	        //
	        str << "\n";
	    }      
	    else    
	    {
	        str << "\n" << OmnString(mDbRslt[mCrtIndex*mNumFields + i]);
	    }
	}       
	        
	return str;
}


OmnDateTime		
OmnDbRcdSQLite::getDateTime(const int index, 
						   OmnRslt &rslt) const
{
	 OmnDateTime time(getStr(index, "", rslt).getBuffer(), 
		 OmnDateTime::eYYYYMMDDHHMMSS);
	 return time;
}
