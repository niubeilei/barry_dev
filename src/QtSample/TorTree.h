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
// Monday, January 28, 2008: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_TorturerGUI_TorTree_h
#define Aos_TorturerGUI_TorTree_h

#include "RVG/ConfigMgr.h"
#include "RVG/RVG.h"
#include "RVG/Table.h"
#include "Torturer/Product.h"
#include "Torturer/Command.h"
#include "Torturer/Module.h"
#include "Torturer/Torturer.h"
#include "Torturer/Ptrs.h"
#include "RVG/Ptrs.h"
#include "Util/Ptrs.h"
#include <qlistview.h>
#include <qobject.h>

class QListView;

enum TreeItemType
{
	eTreeItem_Product = 0,
	eTreeItem_RVGs,
	eTreeItem_RVG,
	eTreeItem_Tables,
	eTreeItem_Table,
	eTreeItem_Commands,
	eTreeItem_Command,
	eTreeItem_Modules,
	eTreeItem_Module,
	eTreeItem_Torturers,
	eTreeItem_Torturer

};


class AosBaseTreeItem : public QListViewItem
{
private:
	TreeItemType mType;

public:
	AosBaseTreeItem(QListViewItem* parent, TreeItemType type);
	AosBaseTreeItem(QListView* parent, TreeItemType type);
	virtual ~AosBaseTreeItem();

	TreeItemType getType();
	void setType(TreeItemType type);
	
	static AosBaseTreeItem* getTreeItem(std::string productName, std::string name, TreeItemType type);
};


class AosProductTreeItem : public AosBaseTreeItem
{
private:
	AosProductPtr	mProduct;

public:
	AosProductTreeItem(QListView* parent, const AosProductPtr &product);
	~AosProductTreeItem();

	void init();

};


class AosTorTree : public QListView
{
	Q_OBJECT

private:
	static AosTorTree* mTorTree;

public:
	AosTorTree(QWidget * parent = 0, const char * name = 0, Qt::WFlags f = 0 );

public:
	~AosTorTree();

	void init();
	static AosTorTree* instance(QWidget * parent = 0);

};


#endif
