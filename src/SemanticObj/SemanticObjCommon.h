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
// 12/07/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SemanticObj_SemanticObjCommon_h
#define Aos_SemanticObj_SemanticObjCommon_h

#include "SemanticObj/SemanticObj.h"
#include "SemanticRules/SemanticRuleUtil.h"
#include <list>

class AosSemanticObjCommon : public AosSemanticObj
{
protected:
	std::string				mName;
	AosSOType::E		    mType;
	void 			       *mInst;
	bool					mStatus;
	AosRuleList				mIntegrityRules;

public:
	AosSemanticObjCommon(const std::string &name, 
						 const AosSOType::E type, 
						 void *inst);
	~AosSemanticObjCommon();

	virtual AosSOType::E	getType() const {return mType;} 
	virtual std::string		getListnerName() const {return mName;}

	virtual bool	start();
	virtual bool	stop();
	virtual u32		getHashKey() const;
	virtual bool	isSameObj(const AosSemanticObjPtr &rhs) const;
	virtual bool	integrityCheck(std::list<u32> &error_ids);
};

#endif

