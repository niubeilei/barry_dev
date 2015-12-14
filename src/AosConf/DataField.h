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
// Modification History:
// 04/25/2014 Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AosConf_DataField_h
#define Aos_AosConf_DataField_h

#include "AosConf/AosConfig.h"
#include <iostream>
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

namespace AosConf
{
class DataField : public AosConf::AosConfBase
{
	public:
		DataField();
		~DataField();

		DataField(const AosXmlTagPtr &xml);
		virtual string getConfig();

	static bool	getVirtualField(
							const AosXmlTagPtr &table_doc,
							const AosXmlTagPtr &schema_doc,
							vector<AosXmlTagPtr> &inputs,
							vector<AosXmlTagPtr> &outputs);

	static bool convertVirtualField(
							const AosXmlTagPtr &vfconf,
							vector<boost::shared_ptr<DataField> > &v,
							vector<boost::shared_ptr<DataField> > &inputFields,
							vector<boost::shared_ptr<DataField> > &outputFields);


};
}

#endif



