////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjCreator.h
// Description:
//	This class is used to create messages. This is a singleton class.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_ObjMgr_ObjFactory_h
#define Omn_ObjMgr_ObjFactory_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/SmartList.h"

template <class ObjT, class IdT, class CreatorT>
class OmnObjFactory : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	struct Entry
	{
		ObjType	mObj;
		IdT		mId;
	};

	OmnSmartList<CreatorT>	mCreators;

public:
	OmnObjFactory() {}
	virtual ~OmnObjFactory() {}

	ObjType	createObj(const IdT &id)
	{
		CreatorT creator = mCreators.get

	// 
	// ObjFactoryInterface 
	//
	virtual bool	registerMsgCreator(const OmnMsgId::E msgId, 
									   const OmnMsgCreatorPtr &creator);

	// 
	// Singleton class interface
	//
	virtual bool			start();
	virtual bool			stop();
	virtual OmnString       getSysObjName() const {return "OmnTime";}
	virtual OmnSingletonObjId::E  getSysObjId() const {return OmnSingletonObjId::eObjFactory;}
	virtual OmnRslt         config(const OmnXmlParserPtr &def);
};
#endif
