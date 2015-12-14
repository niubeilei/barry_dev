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

#include "TorturerGUI/TorTree.h"
#include "Torturer/Product.h"
#include <qheader.h>


AosProductTreeItem::AosProductTreeItem(
		QListView* parent, 
		const AosProductPtr &product)
:
AosBaseTreeItem(parent, eTreeItem_Product)
{
	mProduct = product;

	if (mProduct)
	{
		init();
	}
}


AosProductTreeItem::~AosProductTreeItem()
{
}


void 
AosProductTreeItem::init()
{
    setText(0, mProduct->getName());
	setOpen(TRUE);

	//
	// Create Torturer tree
	//
	AosBaseTreeItem* item = new AosBaseTreeItem(this, eTreeItem_Torturers);
	item->setText(0, "Torturer");
	int torturerNum = mProduct->mTorturers.size();
	for (int i=0; i<torturerNum; i++)
	{
		new AosTorturerTreeItem(item, mProduct->mTorturers[i]);
	}

	//
	// Create Module tree
	//
	item = new AosBaseTreeItem(this, eTreeItem_Modules);
	item->setText(0, "Module");
	int moduleNum = mProduct->mModules.size();
	for (int i=0; i<moduleNum; i++)
	{
		new AosModuleTreeItem(item, mProduct->mModules[i]);
	}

	//
	// Create Command tree
	//
	item = new AosBaseTreeItem(this, eTreeItem_Commands);
	item->setText(0, "Command");
	int cmdNum = mProduct->mCommands.size();
	for (int i=0; i<cmdNum; i++)
	{
		new AosCommandTreeItem(item, mProduct->mCommands[i]);
	}

	//
	// Create RVG tree
	//
	item = new AosBaseTreeItem(this, eTreeItem_RVGs);
	item->setText(0, "RVG");
	int rvgNum = mProduct->mRVGs.size();
	for (int i=0; i<rvgNum; i++)
	{
		new AosRVGTreeItem(item, mProduct->mRVGs[i]);
	}

	//
	// Create Table tree
	//
	item = new AosBaseTreeItem(this, eTreeItem_Tables);
	item->setText(0, "Table");
	int tableNum = mProduct->mTables.size();
	for (int i=0; i<tableNum; i++)
	{
		new AosTableTreeItem(item, mProduct->mTables[i]);
	}
}


AosBaseTreeItem::AosBaseTreeItem(
		QListViewItem* parent, 
		TreeItemType type)
:
QListViewItem(parent),
mType(type)
{
}


AosBaseTreeItem::AosBaseTreeItem(QListView* parent, TreeItemType type)
:
QListViewItem(parent),
mType(type)
{
}


AosBaseTreeItem::~AosBaseTreeItem()
{
}


TreeItemType
AosBaseTreeItem::getType()
{
	return mType;
}


void 
AosBaseTreeItem::setType(TreeItemType type)
{
	mType = type;
}


//
// get the tree item, if type is eTreeItem_RVGs, eTreeItem_Tables,
// eTreeItem_Commands, eTreeItem_Modules, eTreeItem_Torturers, 
// we can ignore the second parameter "name"
//
AosBaseTreeItem* 
AosBaseTreeItem::getTreeItem(
		std::string productName, 
		std::string name, 
		TreeItemType type)
{
	AosTorTree* root = AosTorTree::instance();
	if (root == NULL)
	{
		return NULL;
	}

	AosBaseTreeItem* product = (AosBaseTreeItem*)
		root->findItem(productName, 0);
	if (product == NULL)
	{
		return NULL;
	}

	switch (type)
	{
		case eTreeItem_RVGs:
			{
				QListViewItemIterator it(product);
				while (it.current())
				{
					if (it.current()->text(0) == "RVG")
					{
						return (AosBaseTreeItem*)it.current();
					}
					++ it;
				}
			}
			break;
		case eTreeItem_RVG:
			break;
		case eTreeItem_Tables:
			{
				QListViewItemIterator it(product);
				while (it.current())
				{
					if (it.current()->text(0) == "Table")
					{
						return (AosBaseTreeItem*)it.current();
					}
					++ it;
				}
			}
			break;
		case eTreeItem_Table:
			break;
		case eTreeItem_Commands:
			{
				QListViewItemIterator it(product);
				while (it.current())
				{
					if (it.current()->text(0) == "Command")
					{
						return (AosBaseTreeItem*)it.current();
					}
					++ it;
				}
			}
			break;
		case eTreeItem_Command:
			break;
		case eTreeItem_Modules:
			{
				QListViewItemIterator it(product);
				while (it.current())
				{
					if (it.current()->text(0) == "Modules")
					{
						return (AosBaseTreeItem*)it.current();
					}
					++ it;
				}
			}
			break;
		case eTreeItem_Module:
			break;
		case eTreeItem_Torturers:
			{
				QListViewItemIterator it(product);
				while (it.current())
				{
					if (it.current()->text(0) == "Torturer")
					{
						return (AosBaseTreeItem*)it.current();
					}
					++ it;
				}
			}
			break;
		case eTreeItem_Torturer:
			break;
		default:
			break;
	}

	return NULL;
}


AosRVGTreeItem::AosRVGTreeItem(
		QListViewItem* parent, 
		aos_rvg_t *rvg)
:
AosBaseTreeItem(parent, eTreeItem_RVG),
mRVG(rvg)
{
    if (mRVG)
    {
		rvg->mf->hold(rvg);
		setText(0, rvg->name);
    }
}


AosRVGTreeItem::~AosRVGTreeItem()
{
}


AosTableTreeItem::AosTableTreeItem(QListViewItem* parent, const AosTablePtr &table)
:
AosBaseTreeItem(parent, eTreeItem_Table),
mTable(table)
{
    if (mTable)
    {
	setText(0, mTable->getName());
    }
}


AosTableTreeItem::~AosTableTreeItem()
{
}


AosCommandTreeItem::AosCommandTreeItem(QListViewItem* parent, const AosCommandPtr &cmd)
:
AosBaseTreeItem(parent, eTreeItem_Command),
mCommand(cmd)
{  
    if (mCommand)
    {
	setText(0, mCommand->getName());
    }
}


AosCommandTreeItem::~AosCommandTreeItem()
{
}


AosModuleTreeItem::AosModuleTreeItem(QListViewItem* parent, const AosModulePtr &module)
:
AosBaseTreeItem(parent, eTreeItem_Module),
mModule(module)
{
     if (mModule)
    {
	setText(0, mModule->getName());
    }
}


AosModuleTreeItem::~AosModuleTreeItem()
{
}


AosTorturerTreeItem::AosTorturerTreeItem(QListViewItem* parent, const AosTorturerPtr &torturer)
:
AosBaseTreeItem(parent, eTreeItem_Torturer),
mTorturer(torturer)
{
     if (mTorturer)
    {
	setText(0, mTorturer->getName());
    }
}


AosTorturerTreeItem::~AosTorturerTreeItem()
{
}


AosTorTree::AosTorTree(QWidget *parent, const char *name, WFlags f)
:
QListView(parent, name, f)
{
  //  init();
}


AosTorTree::~AosTorTree()
{
}


//
// initialize the torturer objects tree
//
void 
AosTorTree::init()
{
	clear();
	addColumn("Torturer Objects");
	setColumnWidthMode(0,Maximum);
    header()->setClickEnabled( FALSE );
//	header()->setStretchEnabled(TRUE);
	setRootIsDecorated(TRUE); 
	setSorting(-1);
//	setResizeMode(LastColumn);
	AosProductMgr::init();
	int productNum = AosProductMgr::mProducts.size();
	for (int i=0; i<productNum; i++)
	{
		new AosProductTreeItem(this, AosProductMgr::mProducts[i]);
	}
	if (AosProductMgr::mGlobalData)
	{
		new AosProductTreeItem(this, AosProductMgr::mGlobalData);
	}

}


AosTorTree* 
AosTorTree::instance(QWidget * parent)
{
	if (parent == NULL)
	{
		return mTorTree;
	}
	if (mTorTree == NULL)
	{
		mTorTree = new AosTorTree(parent);
	}
	return mTorTree;
}


AosTorTree* 
AosTorTree::mTorTree = NULL;
