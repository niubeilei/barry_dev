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
// 2013/08/12 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CommandProc_CmdProcSimple_h
#define Aos_CommandProc_CmdProcSimple_h

#include "CommandProc/CommandProc.h"

class AosCmdProcSimple : public AosCommandProc
{
	OmnDefineRCObject;

private:
	int			mNumResponded;

public:
	AosCmdProcSimple(const AosCmdProcSimple &rhs);
	AosCmdProcSimple(
		const u32 version);
	~AosCmdProcSimple(){}

	virtual bool run(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &cmd);

	// AosJimo Interface
	virtual bool run(   const AosRundataPtr &rdata);
	virtual AosJimoPtr cloneJimo(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &worker_doc,
						const AosXmlTagPtr &jimo_doc) const;
	virtual bool config(const AosRundataPtr &rdata,
						const AosXmlTagPtr &worker_doc);
	virtual void * getMethod(const AosRundataPtr &rdata, 
						const OmnString &name, 
						AosMethodId::E &method_id);


private:
	bool 		init(         
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &jimo_doc);
	bool		registerMethods();

	bool		 sendToAll(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &cmd);
	bool		 sendToSelected(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &cmd);
	bool		 constructResponse(const AosRundataPtr &rdata);
	bool		 comandFinished(
					const AosRundataPtr &rdata,
					const int py_id,
					const bool status,
					const OmnString &results);
};

#endif

