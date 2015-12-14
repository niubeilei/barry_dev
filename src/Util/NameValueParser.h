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
// Modification History
// 2015/05/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Util_NameValueParser_h
#define Aos_Util_NameValueParser_h

#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Rundata/Rundata.h"

class AosNameValueDoc;

class AosNameValueParser : public OmnRCObject
{
	OmnDefineRCObject;
	int				mErrStartIdx;
	OmnString		mErrMsg;
	OmnString		mPriName;       //this member data for return errmsg

public:
	enum E
	{
		eMaxNameLen = 256
	};

public:
	AosNameValueParser(){};
	bool parse( 
			AosRundata *rdata, 
			const char *data,  
			const int len,     
			int &cursor,       
			AosNameValueDoc &doc);

private:
	bool parseNext(
			AosRundata *rdata,
			const char *data,
			const int len,
			int &cursor,
			AosNameValueDoc &doc);

	void skipWhiteSpace(
			AosRundata *rdata, 
			const char *data,  
			const int len,     
			int &cursor);

	bool nextName(
			AosRundata *rdata,
			const char *data, 
			const int len,    
			int &cursor,      
			OmnString &name,  
			const int pos);

	bool nextDocValue(
			AosRundata *rdata,    
			AosNameValueDoc &doc,        
			const char *data,     
			const int len,        
			int &cursor,          
			const int pos);

	bool nextTuple(
			AosRundata *rdata,    
			AosNameValueDoc &doc,        
			const char *data,     
			const int len,        
			int &cursor,          
			const int pos);

	bool nextArray(
			AosRundata *rdata,    
			AosNameValueDoc &doc,        
			const char *data,     
			const int len,        
			int &cursor,          
			const int pos);

	bool nextQuotedValue(
			AosRundata *rdata,               
			AosNameValueDoc &doc,                   
			const char *data,                
			const int len,                   
			int &cursor,                     
			const char quote,                
			const int pos);

	bool nextUnquotedValue(
			AosRundata *rdata,    
			AosNameValueDoc &doc,        
			//const OmnString &name,
			const char *data,     
			const int len,        
			int &cursor,          
			const int pos);

	bool expectChar(
			AosRundata *rdata,       
			const char *data,        
			const int len,           
			int &cursor,             
			const char expected_char,
			const int pos);
};

#endif

