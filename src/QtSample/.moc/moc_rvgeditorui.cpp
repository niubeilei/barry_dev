/****************************************************************************
** RVGEditorUI meta object code from reading C++ file 'rvgeditorui.h'
**
** Created: Wed Feb 20 23:57:35 2008
**      by: The Qt MOC ($Id: moc_rvgeditorui.cpp,v 1.2 2015/01/06 08:57:50 andy Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../.ui/rvgeditorui.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *RVGEditorUI::className() const
{
    return "RVGEditorUI";
}

QMetaObject *RVGEditorUI::metaObj = 0;
static QMetaObjectCleanUp cleanUp_RVGEditorUI( "RVGEditorUI", &RVGEditorUI::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString RVGEditorUI::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "RVGEditorUI", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString RVGEditorUI::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "RVGEditorUI", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* RVGEditorUI::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QMainWindow::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_0 = {"RVGEditorUI_usesTextLabelChanged", 1, param_slot_0 };
    static const QUMethod slot_1 = {"AddValueBtn_clicked", 0, 0 };
    static const QUMethod slot_2 = {"DelValueBtn_clicked", 0, 0 };
    static const QUMethod slot_3 = {"ValueEditBtn_clicked", 0, 0 };
    static const QUParameter param_slot_4[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_4 = {"NamelineEdit_textChanged", 1, param_slot_4 };
    static const QUParameter param_slot_5[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_5 = {"WeightlineEdit_textChanged", 1, param_slot_5 };
    static const QUParameter param_slot_6[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_6 = {"TypeLineEdit_textChanged", 1, param_slot_6 };
    static const QUParameter param_slot_7[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_7 = {"RVGTypeLineEdit_textChanged", 1, param_slot_7 };
    static const QUParameter param_slot_8[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_8 = {"MinLineEdit_textChanged", 1, param_slot_8 };
    static const QUParameter param_slot_9[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_9 = {"SeperatorComboBox_textChanged", 1, param_slot_9 };
    static const QUParameter param_slot_10[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_10 = {"MaxLineEdit_textChanged", 1, param_slot_10 };
    static const QUMethod slot_11 = {"languageChange", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "RVGEditorUI_usesTextLabelChanged(bool)", &slot_0, QMetaData::Public },
	{ "AddValueBtn_clicked()", &slot_1, QMetaData::Public },
	{ "DelValueBtn_clicked()", &slot_2, QMetaData::Public },
	{ "ValueEditBtn_clicked()", &slot_3, QMetaData::Public },
	{ "NamelineEdit_textChanged(const QString&)", &slot_4, QMetaData::Public },
	{ "WeightlineEdit_textChanged(const QString&)", &slot_5, QMetaData::Public },
	{ "TypeLineEdit_textChanged(const QString&)", &slot_6, QMetaData::Public },
	{ "RVGTypeLineEdit_textChanged(const QString&)", &slot_7, QMetaData::Public },
	{ "MinLineEdit_textChanged(const QString&)", &slot_8, QMetaData::Public },
	{ "SeperatorComboBox_textChanged(const QString&)", &slot_9, QMetaData::Public },
	{ "MaxLineEdit_textChanged(const QString&)", &slot_10, QMetaData::Public },
	{ "languageChange()", &slot_11, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"RVGEditorUI", parentObject,
	slot_tbl, 12,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_RVGEditorUI.setMetaObject( metaObj );
    return metaObj;
}

void* RVGEditorUI::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "RVGEditorUI" ) )
	return this;
    return QMainWindow::qt_cast( clname );
}

bool RVGEditorUI::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: RVGEditorUI_usesTextLabelChanged((bool)static_QUType_bool.get(_o+1)); break;
    case 1: AddValueBtn_clicked(); break;
    case 2: DelValueBtn_clicked(); break;
    case 3: ValueEditBtn_clicked(); break;
    case 4: NamelineEdit_textChanged((const QString&)static_QUType_QString.get(_o+1)); break;
    case 5: WeightlineEdit_textChanged((const QString&)static_QUType_QString.get(_o+1)); break;
    case 6: TypeLineEdit_textChanged((const QString&)static_QUType_QString.get(_o+1)); break;
    case 7: RVGTypeLineEdit_textChanged((const QString&)static_QUType_QString.get(_o+1)); break;
    case 8: MinLineEdit_textChanged((const QString&)static_QUType_QString.get(_o+1)); break;
    case 9: SeperatorComboBox_textChanged((const QString&)static_QUType_QString.get(_o+1)); break;
    case 10: MaxLineEdit_textChanged((const QString&)static_QUType_QString.get(_o+1)); break;
    case 11: languageChange(); break;
    default:
	return QMainWindow::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool RVGEditorUI::qt_emit( int _id, QUObject* _o )
{
    return QMainWindow::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool RVGEditorUI::qt_property( int id, int f, QVariant* v)
{
    return QMainWindow::qt_property( id, f, v);
}

bool RVGEditorUI::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
