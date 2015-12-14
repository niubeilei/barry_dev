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
// 03/26/2008: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AbsSemanObj_PtrList_h
#define Aos_AbsSemanObj_PtrList_h

#include "AbsSemanObj/AbsSemanObj.h"


class AosPtrList : public AosAbsSemanObj
{
public:
	virtual int	integrityCheck(AosListElemPtr &head) const;
	virtual int	maxLength() const;
	virtual int length() const = 0;
	virtual int	removeElement(const AosListElemPtr &e);
	virtual int	clear();
	virtual int	split(AosPtrList &another);
	virtual int	merge(const AosPtrList &list);
	virtual int	append(const AosListElemPtr &e);
	virtual int	prepend(const AosListElemPtr &e);
	virtual int	insertBefore(
					const AosListElemPtr &e, 
					const AosListElemPtr &a);
	virtual int	insertAfter(
					const AosListElemPtr &e, 
					const AosListElemPtr &a);
	virtual int	pop(AosListElemPtr &e);
	virtual int	push(AosListElemPtr &e);
};

#endif
