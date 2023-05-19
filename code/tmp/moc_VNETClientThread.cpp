/****************************************************************************
** VNETClientThread meta object code from reading C++ file 'VNETClientThread.h'
**
** Created: Thu Mar 28 07:58:32 2013
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../include/VNETClientThread.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *VNETClientThread::className() const
{
    return "VNETClientThread";
}

QMetaObject *VNETClientThread::metaObj = 0;
static QMetaObjectCleanUp cleanUp_VNETClientThread( "VNETClientThread", &VNETClientThread::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString VNETClientThread::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "VNETClientThread", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString VNETClientThread::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "VNETClientThread", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* VNETClientThread::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QObject::staticMetaObject();
    static const QUParameter param_signal_0[] = {
	{ "string", &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod signal_0 = {"printMessage", 1, param_signal_0 };
    static const QMetaData signal_tbl[] = {
	{ "printMessage(const QString&)", &signal_0, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"VNETClientThread", parentObject,
	0, 0,
	signal_tbl, 1,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_VNETClientThread.setMetaObject( metaObj );
    return metaObj;
}

void* VNETClientThread::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "VNETClientThread" ) )
	return this;
    return QObject::qt_cast( clname );
}

// SIGNAL printMessage
void VNETClientThread::printMessage( const QString& t0 )
{
    activate_signal( staticMetaObject()->signalOffset() + 0, t0 );
}

bool VNETClientThread::qt_invoke( int _id, QUObject* _o )
{
    return QObject::qt_invoke(_id,_o);
}

bool VNETClientThread::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: printMessage((const QString&)static_QUType_QString.get(_o+1)); break;
    default:
	return QObject::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool VNETClientThread::qt_property( int id, int f, QVariant* v)
{
    return QObject::qt_property( id, f, v);
}

bool VNETClientThread::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
