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

void MyDialog::OpenFileBtn_clicked()
{
    QString str = EditFileName->text();
    if (str.isEmpty())
    {
	return;
    }
    
    mTCConfig.setConfigFileName(str.ascii());
    if (mTCConfig.readFile() < 0)
    {
	return;
    }
    updateDataList();
}


void MyDialog::TCDataList_doubleClicked( QListBoxItem * pItem)
{
    QString text = pItem->text();
    int id=getTCIDFromString(text);
    TCEditDlg dlg(this);
    mTCConfig.getTCData(id, dlg.mTCData);
    if (dlg.mTCData != NULL)
    {
	dlg.popData();
	dlg.exec();
    }
}


void MyDialog::mSaveBtn_clicked()
{
    mTCConfig.saveFile();
}


void MyDialog::mAddBtn_clicked()
{
    TCEditDlg dlg(this);
    dlg.mTCData = new TCData();
    dlg.popData();
    if (dlg.exec() == QDialog::Accepted)
    {
	mTCConfig.addTCData(*dlg.mTCData);
	updateDataList();
    }
    delete dlg.mTCData;
}


void MyDialog::mDeleteBtn_clicked()
{
    QString str = TCDataList->currentText();
    if (str.isEmpty())
    {
	return;
    }

    int id=getTCIDFromString(str);
    str.sprintf("id=%d", id);
   mTCConfig.deleteTCData(id);
   updateDataList();
}


int MyDialog::getTCIDFromString( QString str )
{
    int pos = str.find("id=");
    if (pos < 0)
	return -1;
    str = str.mid(pos+3);
    pos = str.find(',');
    if (pos <0)
	return -1;
    str = str.left(pos);
    return str.toInt();
}


void MyDialog::updateDataList()
{
    TCDataList->clear();
    for (unsigned int i=0; i<mTCConfig.mTCDataPtrVector.size(); i++)
    {
	QString str;	
	str.sprintf("type=%s, id=%d, sendLen=%d, RecvLen=%d\n",
		  mTCConfig.mTCDataPtrVector[i]->getTypeString().c_str(),
		  mTCConfig.mTCDataPtrVector[i]->getID(),
		  mTCConfig.mTCDataPtrVector[i]->getSendLength(),
		  mTCConfig.mTCDataPtrVector[i]->getRecvLength()
		    );
	TCDataList->insertItem(str);
    }
}
