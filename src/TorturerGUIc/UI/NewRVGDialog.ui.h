/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/


void NewRVGDialog::init()
{
    if (AosProductMgr::mGlobalData)
    {
	mProductNamecomboBox->insertItem(AosProductMgr::mGlobalData->getName());
    }
    int productNum = AosProductMgr::mProducts.size();
    int i;
    for (i=0; i<productNum; i++)
    {
	mProductNamecomboBox->insertItem(AosProductMgr::mProducts[i]->getName());
    }
    if (mProductNamecomboBox->count() > 0)
    {
	mProductNamecomboBox->setCurrentItem(0);
    }
    mRVGsIconView->clear();
     for (i=1; i<AosRVGType::eLastEntry; i++)
     {
         mRVGsIconView->setCurrentItem(new QIconViewItem(mRVGsIconView, AosRVGType::enum2Str((AosRVGType::E)i), QPixmap("images/filenew")));
     }

}


std::string NewRVGDialog::getProductName()
{
    return std::string(mProductNamecomboBox->currentText().ascii());
}


AosRVGType::E NewRVGDialog::getRVGType()
{
    QIconViewItem* item = mRVGsIconView->currentItem();
    if (item)
    {
	 return AosRVGType::str2Enum(item->text().ascii());
    }
    else
    {
	return AosRVGType::eCharsetRSG;
    }
}
