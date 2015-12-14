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


void TCEditDlg::mOkBtn_clicked()
{
    if (mTCChanged)
    {
	postData();
	accept();
    }
    else
    {
	reject();
    }
}


void TCEditDlg::mCancelBtn_clicked()
{
    reject();
}


void TCEditDlg::TCEditDlg_destroyed( QObject * )
{

}


void TCEditDlg::mTCSendContent_textChanged()
{
    mTCChanged = true;
}


void TCEditDlg::mTCRecvContent_textChanged()
{
    mTCChanged = true;
}


void TCEditDlg::popData()
{
    QString str;
    str.sprintf("%d", mTCData->getType());
    mTypeLineEdit->setText(str);
    
    if (mTCData->getID() < 0)
    {
	mTCIDValue->setText("automatic set");
    }
    else
    {
	str.sprintf("%d", mTCData->getID());
	mTCIDValue->setText(str);
    }
    
    int contentType = mTCData->getContentType();
    char* buf;
    int len;
    // set send content
    mTCData->getSendBuf(buf);   
    len = mTCData->getSendLength();
    if (contentType == eTCDATA_TEXT)
    {
	char * txtBuf = new char[len+1];
	memcpy(txtBuf, buf, len);
	txtBuf[len] = '\0';
	str = txtBuf;
	delete [] txtBuf;
    }
    else if (contentType == eTCDATA_BINARY)
    {
	std::string byteString;
	byteStreamToString((unsigned char*)buf, len, byteString);
	str = byteString;
    }
    mTCSendContent->setText(str);    
    
    // set receive content
    mTCData->getRecvBuf(buf);   
    len = mTCData->getRecvLength();
    if (contentType == eTCDATA_TEXT)
    {
	char * txtBuf = new char[len+1];
	memcpy(txtBuf, buf, len);
	txtBuf[len] = '\0';
	str = txtBuf;
	delete [] txtBuf;
    }
    else if (contentType == eTCDATA_BINARY)
    {
	std::string byteString;
	byteStreamToString((unsigned char*)buf, len, byteString);
	str = byteString;
    }
    mTCRecvContent->setText(str);    
    
    mTCChanged = false;
}



void TCEditDlg::postData()
{
    // set type
    QString str;
    str = mTypeLineEdit->text();
    int type  = str.toInt();
    if (type <= 0)
    {
	showMessage("type invalid");
	return;
    }
    mTCData->setType(type);
    
    // set id
    if (mTCData->getID() < 0)
    {
	mTCData->setID(time(NULL));
    }
    
    int contentType = mTCData->getContentType();  
   
    if (contentType == eTCDATA_TEXT)
    {
	 // set send length and content
	str = mTCSendContent->text();
	unsigned int len = str.length();
	mTCData->setSendLength(len);
	mTCData->setSendBuf(str.ascii());
	
	 // set receive length and content
	str = mTCRecvContent->text();
	len = str.length();
	mTCData->setRecvLength(len);
	mTCData->setRecvBuf(str.ascii());
    }
    else if (contentType == eTCDATA_BINARY)
    {
	 // set send length and content
	unsigned char tmpStream[1024]={0};
	unsigned int tmpLen = 1024;
	str = mTCSendContent->text();
//	showMessage(str);
	byteStringToStream(str.ascii(), tmpStream, tmpLen);
	mTCData->setSendLength(tmpLen);
	mTCData->setSendBuf((char*)tmpStream);
	
	// set receive length and content
	str = mTCRecvContent->text();
//	showMessage(str);
	byteStringToStream(str.ascii(), tmpStream, tmpLen);
	mTCData->setRecvLength(tmpLen);
	mTCData->setRecvBuf((char*)tmpStream);	
    }    
}


void TCEditDlg::showMessage( QString str )
{
	QErrorMessage msg(this, "error");
	msg.message(str);
	msg.exec();
}
