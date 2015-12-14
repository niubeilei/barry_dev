/****************************************************************************
** RVGEditorDlg meta object code from reading C++ file 'RVGEditorDlg.h'
**
** Created: Sun Mar 16 15:36:35 2008
**      by: The Qt MOC ($Id: moc_RVGEditorDlg.cpp,v 1.2 2015/01/06 08:57:51 andy Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../.ui/RVGEditorDlg.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *RVGEditorDlg::className() const
{
    return "RVGEditorDlg";
}

QMetaObject *RVGEditorDlg::metaObj = 0;
static QMetaObjectCleanUp cleanUp_RVGEditorDlg( "RVGEditorDlg", &RVGEditorDlg::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString RVGEditorDlg::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "RVGEditorDlg", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString RVGEditorDlg::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "RVGEditorDlg", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* RVGEditorDlg::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    static const QUMethod slot_0 = {"languageChange", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "languageChange()", &slot_0, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"RVGEditorDlg", parentObject,
	slot_tbl, 1,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_RVGEditorDlg.setMetaObject( metaObj );
    return metaObj;
}

void* RVGEditorDlg::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "RVGEditorDlg" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool RVGEditorDlg::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: languageChange(); break;
    default:
	return QDialog::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool RVGEditorDlg::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool RVGEditorDlg::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool RVGEditorDlg::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
