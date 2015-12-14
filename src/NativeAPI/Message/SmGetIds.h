////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SmGetIds.h
// Description:
//	This message is used by repository realms to get IDs. This is
//  important because repository may be hosted by multiple realm
//  images. When one creates a new object such as LDE, it needs to
//  set the LDE ID, which has to be unique across the entire 
//  realm. When needed, a realm image reserves a block of IDs and
//  send this message to all other masters. These masters must 
//  acknowledge the message and updates their local database to reflect
//  the ID usage.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef OMN_Message_SmGetIds_h
#define OMN_Message_SmGetIds_h

/*
 * Commented out by Chen Ding, 08/23/2005
 * This class should not depend on SCVS
 *
#include "Message/Req.h"
#include "Scvs/ScvsTypes.h"
#include "Scvs/Ptrs.h"


class OmnSmGetIds : public OmnReq
{
	OmnDefineRCObject;

public:
	enum IdType
	{
		eFirstValidEntry,

		eInvalid,

		eLastValidEntry
	};

	enum
	{
		eIdType,
		eIdStarting,
		eNumIds
	};

private:
	int		mIdType;
	int		mIdStarting;
	int		mNumIds;

public:
	OmnSmGetIds()
		:
	mIdType(eInvalid),
	mIdStarting(0),
	mNumIds(0)
	{
	}

	virtual ~OmnSmGetIds() {}

	virtual OmnMsgPtr		createInst(OmnSerialFrom &s);
	virtual OmnMsgId::E		getMsgId() const {return OmnMsgId::eSmScvsReq;}
	virtual OmnMsgId::E		getSubtype() const {return OmnMsgId::eSmGetIds;}
	virtual OmnString		toString() const;
	virtual OmnRslt			serializeFrom(OmnSerialFrom &s);
	virtual OmnRslt			serializeTo(OmnSerialTo &s) const;
	virtual OmnMsgPtr		clone() const;
	virtual OmnRespPtr		getResp();

	IdType	getIdType() const {return (IdType)mIdType;}
	void	setIdType(const IdType t) {mIdType = t;}

	int		getIdStarting() const {return mIdStarting;}
	void	setIdStarting(const int s) {mIdStarting = s;}

	int		getNumIds() const {return mNumIds;}
	void	setNumIds(const int n) {mNumIds = n;}
};
*/


#endif

