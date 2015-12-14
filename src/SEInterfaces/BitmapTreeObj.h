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
// 2013/01/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_BitmapTreeObj_h
#define Aos_SEInterfaces_BitmapTreeObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosBitmapTreeObj : virtual public OmnRCObject
{
private:
	static AosBitmapTreeObjPtr		smObject;

public:
	static void setObject(const AosBitmapTreeObjPtr &d) {smObject = d;}
	static AosBitmapTreeObjPtr getObject() {return smObject;}

	virtual bool insertNodes(
					const AosRundataPtr &rdata,
					const AosIILExecutorObjPtr executor,
					const u64 &bitmap_id,
					const vector<u64> &nodes_list,
					const bool insert_bits) = 0;

	virtual bool insertBits(
					const AosRundataPtr &rdata,
					const AosIILExecutorObjPtr executor,
					const u64 &bitmap_id,
					const u64 &bits_parent_id,
					const u64 &bits_id) = 0;

	virtual bool removeBits(
					const AosRundataPtr &rdata,
					const AosIILExecutorObjPtr executor,
					const u64 &bitmap_id,
					const u64 &bits_parent_id,
					const u64 &bits_id) = 0;

	virtual bool getMemberIds(
					const AosRundataPtr &rdata, 
					const u64 node_id,
					vector<u64> &member_ids) = 0;

	virtual bool splitRootTree(
					const AosRundataPtr &rdata,
					const AosIILExecutorObjPtr executor,
					const u64 &edge_iilid,
				    const u64 &new_tree_id_1,
				    const u64 &new_tree_id_2,
				    const AosBitmapTreeObjPtr &tree1,
				    const AosBitmapTreeObjPtr &tree2) = 0;

	virtual bool setParent(
					const AosRundataPtr &rdata,
					const AosIILExecutorObjPtr executor,
					const u64 &leaf_id) = 0;
	virtual bool splitInnerTree(
					const AosRundataPtr &rdata,
					const AosIILExecutorObjPtr executor,
					const u64 &edge_iilid,
				    const u64 &new_tree_id,
				    const AosBitmapTreeObjPtr &tree) = 0;

	virtual bool saveBitmapTree(const AosRundataPtr &rdata) = 0;
	virtual void saveToBuff(const AosBuffPtr &buff) = 0;

	virtual bool rebuildStruct(const AosRundataPtr &rdata,
						const AosIILExecutorObjPtr executor,
						const u64* idlist, 
						const u32 &numsubiils) = 0;
	virtual bool rebuild(const AosRundataPtr &rdata,
						const AosIILExecutorObjPtr executor,
						const u64* idlist, 
						const u32 &numsubiils) = 0;
	virtual bool queryNodes(
						const AosRundataPtr &rdata,
						const u64 &start_id,
						const u64 &end_id,
					    vector<u64> &remote_node_list,
					    vector<u64> &local_parent_list,
					    vector<u64> &local_node_list) = 0;

	virtual u64	getFamilyNodeID(const u64 &child_id,vector<u64> &child_list) = 0;

	virtual u64	getBmpID()const = 0;

	virtual u64 getParentId(
					const AosRundataPtr &rdata,
					const u64 &leaf_id) = 0;

	virtual bool removeBitmaps(
						const AosRundataPtr &rdata,
						const AosIILExecutorObjPtr executor) = 0;

	virtual bool getLevel2Relations(const AosRundataPtr &rdata,
									vector<u64> &parent_list,
									vector<u64> &child_num_list) = 0;
	virtual bool buildLevel2Node(const AosRundataPtr &rdata,
								 const u64 &parent_id) = 0;
	virtual bool buildHighLevelNodes(const AosRundataPtr &rdata,
									 AosIILExecutorObjPtr &executor) = 0;
						
public:
};

#endif
