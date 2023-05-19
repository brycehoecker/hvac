/****************************************************************************
** VHVControl meta object code from reading C++ file 'VHVControl.h'
**
** Created: Thu Mar 28 07:58:29 2013
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../include/VHVControl.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *VHVControl::className() const
{
    return "VHVControl";
}

QMetaObject *VHVControl::metaObj = 0;
static QMetaObjectCleanUp cleanUp_VHVControl( "VHVControl", &VHVControl::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString VHVControl::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "VHVControl", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString VHVControl::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "VHVControl", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* VHVControl::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QCanvas::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"VHVControl", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_VHVControl.setMetaObject( metaObj );
    return metaObj;
}

void* VHVControl::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "VHVControl" ) )
	return this;
    return QCanvas::qt_cast( clname );
}

bool VHVControl::qt_invoke( int _id, QUObject* _o )
{
    return QCanvas::qt_invoke(_id,_o);
}

bool VHVControl::qt_emit( int _id, QUObject* _o )
{
    return QCanvas::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool VHVControl::qt_property( int id, int f, QVariant* v)
{
    return QCanvas::qt_property( id, f, v);
}

bool VHVControl::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES


const char *VHVControlView::className() const
{
    return "VHVControlView";
}

QMetaObject *VHVControlView::metaObj = 0;
static QMetaObjectCleanUp cleanUp_VHVControlView( "VHVControlView", &VHVControlView::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString VHVControlView::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "VHVControlView", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString VHVControlView::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "VHVControlView", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* VHVControlView::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QCanvasView::staticMetaObject();
    static const QUParameter param_signal_0[] = {
	{ "string", &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod signal_0 = {"status", 1, param_signal_0 };
    static const QUParameter param_signal_1[] = {
	{ "pmt", &static_QUType_ptr, "VPMTIcon", QUParameter::In },
	{ "mouse", &static_QUType_ptr, "QMouseEvent", QUParameter::In },
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod signal_1 = {"clickedPMT", 3, param_signal_1 };
    static const QMetaData signal_tbl[] = {
	{ "status(const QString&)", &signal_0, QMetaData::Protected },
	{ "clickedPMT(VPMTIcon*,QMouseEvent*,int)", &signal_1, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"VHVControlView", parentObject,
	0, 0,
	signal_tbl, 2,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_VHVControlView.setMetaObject( metaObj );
    return metaObj;
}

void* VHVControlView::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "VHVControlView" ) )
	return this;
    return QCanvasView::qt_cast( clname );
}

// SIGNAL status
void VHVControlView::status( const QString& t0 )
{
    activate_signal( staticMetaObject()->signalOffset() + 0, t0 );
}

#include <qobjectdefs.h>
#include <qsignalslotimp.h>

// SIGNAL clickedPMT
void VHVControlView::clickedPMT( VPMTIcon* t0, QMouseEvent* t1, int t2 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 1 );
    if ( !clist )
	return;
    QUObject o[4];
    static_QUType_ptr.set(o+1,t0);
    static_QUType_ptr.set(o+2,t1);
    static_QUType_int.set(o+3,t2);
    activate_signal( clist, o );
}

bool VHVControlView::qt_invoke( int _id, QUObject* _o )
{
    return QCanvasView::qt_invoke(_id,_o);
}

bool VHVControlView::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: status((const QString&)static_QUType_QString.get(_o+1)); break;
    case 1: clickedPMT((VPMTIcon*)static_QUType_ptr.get(_o+1),(QMouseEvent*)static_QUType_ptr.get(_o+2),(int)static_QUType_int.get(_o+3)); break;
    default:
	return QCanvasView::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool VHVControlView::qt_property( int id, int f, QVariant* v)
{
    return QCanvasView::qt_property( id, f, v);
}

bool VHVControlView::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
