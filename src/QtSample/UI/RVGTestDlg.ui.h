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


void RVGTestDlg::startTestBtn_clicked()
{
    if (!mRVG)
    {
	return;
    }
    mValuesListBox->clear();
    uint times = mTestTimesLineEdit->text().toUInt();
    for (uint i=0; i<times; i++)
    {
	AosValue value;
	mRVG->nextValue(value);
	mValuesListBox->insertItem(value.toString());
    }
}


void RVGTestDlg::initialize()
{
    mValuesListBox->clear();
    if (!mRVG)
    {
	return;
    }
    mProductNameLineEdit->setText(mRVG->getProductName());
    mRVGNamelineEdit->setText(mRVG->getName());
}
