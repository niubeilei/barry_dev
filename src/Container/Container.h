////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2015/01/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Container_Container_h
#define AOS_Container_Container_h

#include "SEInterfaces/ContainerObj.h"

class AosContainer : public AosContainerObj
{
	OmnDefineRCObject;

private:

public:
	AosContainer(const OmnString &container_name);
	~AosContainer();

	OmnString			 getTablename() const;
	int 				 getNumColmns() const ;
	vector<AosColumnPtr> getColumns() const;
	AosColumnPtr 		 getColumn(const OmnString &name) const;
	bool				 dropTable(AosRundata *rdata);
	bool				 addColumn(AosRundata *rdata, const AosColumnPtr &column);
	bool				 removeColumn(AosRundata *rdata, const OmnString &name);
	vector<OmnString>	 getPrimaryKey() const;

	bool				 canModifyField(
							AosRundata *rdata, 
							const OmnString &name,
							const AosValueRslt &old_value, 
							const AosValueRslt &new_value);
	
	bool				 canModifyFields(
							AosRundata *rdata, 
							const OmnString &name,
							const vector<AosValueRslt> &old_values, 
							const vector<AosValueRslt> &new_values);
	
	bool				 isUnique(
							AosRundata *rdata, 
							const OmnString &field_name,
							const OmnString &field_value);

	bool				 isUnique(
							AosRundata *rdata, 
							const OmnString &field_name,
							const u64 field_value);

	bool				 isUnique(
							AosRundata *rdata, 
							const OmnString &field_name,
							const AosValueRslt &field_value);

	bool				 isUnique(
							AosRundata *rdata, 
							const vector<OmnString> &names, 
							const vector<AosValueRslt> &values);

	static AosContainerPtr createContainer(
							AosRundata *rdata, 
							const AosJqlStatementObjPtr &table_def);
};
#endif
