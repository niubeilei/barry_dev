/****************************************************************************
** AosTorTree meta object code from reading C++ file 'TorTree.h'
**
** Created: Wed Feb 20 23:57:30 2008
**      by: The Qt MOC ($Id: moc_TorTree.cpp,v 1.2 2015/01/06 08:57:50 andy Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../TorTree.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *AosTorTree::className() const
{
    return "AosTorTree";
}

QMetaObject *AosTorTree::metaObj = 0;
static QMetaObjectCleanUp cleanUp_AosTorTree( "AosTorTree", &AosTorTree::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString AosTorTree::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "AosTorTree", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString AosTorTree::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "AosTorTree", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* AosTorTree::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QListView::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"AosTorTree", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_AosTorTree.setMetaObject( metaObj );
    return metaObj;
}

void* AosTorTree::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "AosTorTree" ) )
	return this;
    return QListView::qt_cast( clname );
}

bool AosTorTree::qt_invoke( int _id, QUObject* _o )
{
    return QListView::qt_invoke(_id,_o);
}

bool AosTorTree::qt_emit( int _id, QUObject* _o )
{
    return QListView::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool AosTorTree::qt_property( int id, int f, QVariant* v)
{
    return QListView::qt_property( id, f, v);
}

bool AosTorTree::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
