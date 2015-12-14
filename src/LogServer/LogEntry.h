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
// 03/20/2009 Created by Sally Sheng
////////////////////////////////////////////////////////////////////////////////////
#ifndef AOS_LogServer_LogEntry_h
#define AOS_LogServer_LogEntry_h

#include "Util/String.h"
#include "TinyXml/TinyXml.h"
#include "Util/RCObject.h"


class AosLogEntry : public virtual OmnRCObject
{
	OmnDefineRCObject;

protected:
	OmnString mSeqno;
	OmnString mType;
	OmnString mTime;
	OmnString mUser;
	OmnString mApplication;
	OmnString mDesc;
	OmnString mContents;
//	OmnString mZone;
//	OmnString mPreamble;

public:
    AosLogEntry();
	AosLogEntry(TiXmlNode *logentry); 
    ~AosLogEntry();


	OmnString getSeqno() {return mSeqno;}
	OmnString getType() {return mType;} 
	OmnString getTime() {return mTime;}
	OmnString getUser() {return mUser;}
	OmnString getApplication() {return mApplication;}
	OmnString getDesc() {return mDesc;}
	OmnString getContent() {return mContents;}


private:
    void      setSeqno(const OmnString &seqno) {mSeqno = seqno;}
    void      setType(const OmnString &type) {mType = type;}
    void      setTime(const OmnString &time) {mTime = time;}
    void      setUser(const OmnString &user) {mUser = user;}
    void      setApplication(const OmnString &application) {mApplication = application;}
    void      setDesc(const OmnString &desc) {mDesc = desc;}
    void      setContent(const OmnString &content) {mContents = content;}

};

#endif


