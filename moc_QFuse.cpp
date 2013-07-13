/****************************************************************************
** Meta object code from reading C++ file 'QFuse.hh'
**
** Created: Fri Jul 12 22:25:26 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "QFuse.hh"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QFuse.hh' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QFuse[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
       7,    6,    6,    6, 0x05,

 // slots: signature, parameters, type, tag, flags
      33,    6,   29,    6, 0x0a,
      42,    6,   29,    6, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QFuse[] = {
    "QFuse\0\0sigShutDownComplete()\0int\0"
    "doWork()\0shutDown()\0"
};

void QFuse::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QFuse *_t = static_cast<QFuse *>(_o);
        switch (_id) {
        case 0: _t->sigShutDownComplete(); break;
        case 1: { int _r = _t->doWork();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 2: { int _r = _t->shutDown();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QFuse::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QFuse::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QFuse,
      qt_meta_data_QFuse, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QFuse::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QFuse::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QFuse::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QFuse))
        return static_cast<void*>(const_cast< QFuse*>(this));
    return QObject::qt_metacast(_clname);
}

int QFuse::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void QFuse::sigShutDownComplete()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
