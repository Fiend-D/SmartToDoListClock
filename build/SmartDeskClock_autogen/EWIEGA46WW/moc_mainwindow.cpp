/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../mainwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_MainWindow_t {
    uint offsetsAndSizes[104];
    char stringdata0[11];
    char stringdata1[12];
    char stringdata2[1];
    char stringdata3[7];
    char stringdata4[13];
    char stringdata5[24];
    char stringdata6[6];
    char stringdata7[11];
    char stringdata8[16];
    char stringdata9[6];
    char stringdata10[7];
    char stringdata11[8];
    char stringdata12[17];
    char stringdata13[8];
    char stringdata14[5];
    char stringdata15[14];
    char stringdata16[5];
    char stringdata17[17];
    char stringdata18[5];
    char stringdata19[5];
    char stringdata20[10];
    char stringdata21[11];
    char stringdata22[16];
    char stringdata23[15];
    char stringdata24[17];
    char stringdata25[21];
    char stringdata26[19];
    char stringdata27[12];
    char stringdata28[14];
    char stringdata29[16];
    char stringdata30[18];
    char stringdata31[15];
    char stringdata32[9];
    char stringdata33[17];
    char stringdata34[12];
    char stringdata35[5];
    char stringdata36[17];
    char stringdata37[18];
    char stringdata38[6];
    char stringdata39[20];
    char stringdata40[10];
    char stringdata41[20];
    char stringdata42[10];
    char stringdata43[18];
    char stringdata44[19];
    char stringdata45[18];
    char stringdata46[16];
    char stringdata47[16];
    char stringdata48[9];
    char stringdata49[17];
    char stringdata50[8];
    char stringdata51[20];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_MainWindow_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
        QT_MOC_LITERAL(0, 10),  // "MainWindow"
        QT_MOC_LITERAL(11, 11),  // "lockChanged"
        QT_MOC_LITERAL(23, 0),  // ""
        QT_MOC_LITERAL(24, 6),  // "locked"
        QT_MOC_LITERAL(31, 12),  // "layerChanged"
        QT_MOC_LITERAL(44, 23),  // "MainWindow::WindowLayer"
        QT_MOC_LITERAL(68, 5),  // "layer"
        QT_MOC_LITERAL(74, 10),  // "updateTime"
        QT_MOC_LITERAL(85, 15),  // "onQuoteReceived"
        QT_MOC_LITERAL(101, 5),  // "quote"
        QT_MOC_LITERAL(107, 6),  // "author"
        QT_MOC_LITERAL(114, 7),  // "emotion"
        QT_MOC_LITERAL(122, 16),  // "onWeatherUpdated"
        QT_MOC_LITERAL(139, 7),  // "weather"
        QT_MOC_LITERAL(147, 4),  // "temp"
        QT_MOC_LITERAL(152, 13),  // "onWeatherPoem"
        QT_MOC_LITERAL(166, 4),  // "poem"
        QT_MOC_LITERAL(171, 16),  // "onStyleGenerated"
        QT_MOC_LITERAL(188, 4),  // "name"
        QT_MOC_LITERAL(193, 4),  // "desc"
        QT_MOC_LITERAL(198, 9),  // "styleData"
        QT_MOC_LITERAL(208, 10),  // "toggleLock"
        QT_MOC_LITERAL(219, 15),  // "updateLockState"
        QT_MOC_LITERAL(235, 14),  // "setWindowLayer"
        QT_MOC_LITERAL(250, 16),  // "cycleWindowLayer"
        QT_MOC_LITERAL(267, 20),  // "startGenerateAIStyle"
        QT_MOC_LITERAL(288, 18),  // "generateWithPrompt"
        QT_MOC_LITERAL(307, 11),  // "saveAIStyle"
        QT_MOC_LITERAL(319, 13),  // "rejectAIStyle"
        QT_MOC_LITERAL(333, 15),  // "regenerateStyle"
        QT_MOC_LITERAL(349, 17),  // "showStyleSelector"
        QT_MOC_LITERAL(367, 14),  // "switchProvider"
        QT_MOC_LITERAL(382, 8),  // "provider"
        QT_MOC_LITERAL(391, 16),  // "toggleVisibility"
        QT_MOC_LITERAL(408, 11),  // "fadeInQuote"
        QT_MOC_LITERAL(420, 4),  // "text"
        QT_MOC_LITERAL(425, 16),  // "onAddTodoClicked"
        QT_MOC_LITERAL(442, 17),  // "onTodoItemClicked"
        QT_MOC_LITERAL(460, 5),  // "index"
        QT_MOC_LITERAL(466, 19),  // "onTodoItemCompleted"
        QT_MOC_LITERAL(486, 9),  // "completed"
        QT_MOC_LITERAL(496, 19),  // "onTodoHeightChanged"
        QT_MOC_LITERAL(516, 9),  // "newHeight"
        QT_MOC_LITERAL(526, 17),  // "onPomodoroStarted"
        QT_MOC_LITERAL(544, 18),  // "onPomodoroFinished"
        QT_MOC_LITERAL(563, 17),  // "onPomodoroStopped"
        QT_MOC_LITERAL(581, 15),  // "checkSmartTheme"
        QT_MOC_LITERAL(597, 15),  // "applySmartTheme"
        QT_MOC_LITERAL(613, 8),  // "timeSlot"
        QT_MOC_LITERAL(622, 16),  // "enableSmartTheme"
        QT_MOC_LITERAL(639, 7),  // "enabled"
        QT_MOC_LITERAL(647, 19)   // "onSmartThemeToggled"
    },
    "MainWindow",
    "lockChanged",
    "",
    "locked",
    "layerChanged",
    "MainWindow::WindowLayer",
    "layer",
    "updateTime",
    "onQuoteReceived",
    "quote",
    "author",
    "emotion",
    "onWeatherUpdated",
    "weather",
    "temp",
    "onWeatherPoem",
    "poem",
    "onStyleGenerated",
    "name",
    "desc",
    "styleData",
    "toggleLock",
    "updateLockState",
    "setWindowLayer",
    "cycleWindowLayer",
    "startGenerateAIStyle",
    "generateWithPrompt",
    "saveAIStyle",
    "rejectAIStyle",
    "regenerateStyle",
    "showStyleSelector",
    "switchProvider",
    "provider",
    "toggleVisibility",
    "fadeInQuote",
    "text",
    "onAddTodoClicked",
    "onTodoItemClicked",
    "index",
    "onTodoItemCompleted",
    "completed",
    "onTodoHeightChanged",
    "newHeight",
    "onPomodoroStarted",
    "onPomodoroFinished",
    "onPomodoroStopped",
    "checkSmartTheme",
    "applySmartTheme",
    "timeSlot",
    "enableSmartTheme",
    "enabled",
    "onSmartThemeToggled"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_MainWindow[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      31,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  200,    2, 0x06,    1 /* Public */,
       4,    1,  203,    2, 0x06,    3 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       7,    0,  206,    2, 0x08,    5 /* Private */,
       8,    3,  207,    2, 0x08,    6 /* Private */,
      12,    2,  214,    2, 0x08,   10 /* Private */,
      15,    1,  219,    2, 0x08,   13 /* Private */,
      17,    3,  222,    2, 0x08,   15 /* Private */,
      21,    0,  229,    2, 0x08,   19 /* Private */,
      22,    0,  230,    2, 0x08,   20 /* Private */,
      23,    1,  231,    2, 0x08,   21 /* Private */,
      24,    0,  234,    2, 0x08,   23 /* Private */,
      25,    0,  235,    2, 0x08,   24 /* Private */,
      26,    0,  236,    2, 0x08,   25 /* Private */,
      27,    0,  237,    2, 0x08,   26 /* Private */,
      28,    0,  238,    2, 0x08,   27 /* Private */,
      29,    0,  239,    2, 0x08,   28 /* Private */,
      30,    0,  240,    2, 0x08,   29 /* Private */,
      31,    1,  241,    2, 0x08,   30 /* Private */,
      33,    0,  244,    2, 0x08,   32 /* Private */,
      34,    1,  245,    2, 0x08,   33 /* Private */,
      36,    0,  248,    2, 0x08,   35 /* Private */,
      37,    1,  249,    2, 0x08,   36 /* Private */,
      39,    2,  252,    2, 0x08,   38 /* Private */,
      41,    1,  257,    2, 0x08,   41 /* Private */,
      43,    0,  260,    2, 0x08,   43 /* Private */,
      44,    0,  261,    2, 0x08,   44 /* Private */,
      45,    0,  262,    2, 0x08,   45 /* Private */,
      46,    0,  263,    2, 0x08,   46 /* Private */,
      47,    2,  264,    2, 0x08,   47 /* Private */,
      49,    1,  269,    2, 0x08,   50 /* Private */,
      51,    1,  272,    2, 0x08,   52 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, 0x80000000 | 5,    6,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,    9,   10,   11,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,   13,   14,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QJsonObject,   18,   19,   20,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   32,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   35,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   38,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,   38,   40,
    QMetaType::Void, QMetaType::Int,   42,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   48,   13,
    QMetaType::Void, QMetaType::Bool,   50,
    QMetaType::Void, QMetaType::Bool,   50,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.offsetsAndSizes,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_MainWindow_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'lockChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'layerChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<MainWindow::WindowLayer, std::false_type>,
        // method 'updateTime'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onQuoteReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onWeatherUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onWeatherPoem'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onStyleGenerated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonObject &, std::false_type>,
        // method 'toggleLock'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'updateLockState'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setWindowLayer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'cycleWindowLayer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'startGenerateAIStyle'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'generateWithPrompt'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'saveAIStyle'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'rejectAIStyle'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'regenerateStyle'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showStyleSelector'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'switchProvider'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'toggleVisibility'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'fadeInQuote'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onAddTodoClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onTodoItemClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onTodoItemCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'onTodoHeightChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onPomodoroStarted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onPomodoroFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onPomodoroStopped'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'checkSmartTheme'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'applySmartTheme'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'enableSmartTheme'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'onSmartThemeToggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->lockChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->layerChanged((*reinterpret_cast< std::add_pointer_t<MainWindow::WindowLayer>>(_a[1]))); break;
        case 2: _t->updateTime(); break;
        case 3: _t->onQuoteReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 4: _t->onWeatherUpdated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 5: _t->onWeatherPoem((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->onStyleGenerated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[3]))); break;
        case 7: _t->toggleLock(); break;
        case 8: _t->updateLockState(); break;
        case 9: _t->setWindowLayer((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 10: _t->cycleWindowLayer(); break;
        case 11: _t->startGenerateAIStyle(); break;
        case 12: _t->generateWithPrompt(); break;
        case 13: _t->saveAIStyle(); break;
        case 14: _t->rejectAIStyle(); break;
        case 15: _t->regenerateStyle(); break;
        case 16: _t->showStyleSelector(); break;
        case 17: _t->switchProvider((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 18: _t->toggleVisibility(); break;
        case 19: _t->fadeInQuote((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 20: _t->onAddTodoClicked(); break;
        case 21: _t->onTodoItemClicked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 22: _t->onTodoItemCompleted((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 23: _t->onTodoHeightChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 24: _t->onPomodoroStarted(); break;
        case 25: _t->onPomodoroFinished(); break;
        case 26: _t->onPomodoroStopped(); break;
        case 27: _t->checkSmartTheme(); break;
        case 28: _t->applySmartTheme((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 29: _t->enableSmartTheme((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 30: _t->onSmartThemeToggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MainWindow::*)(bool );
            if (_t _q_method = &MainWindow::lockChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MainWindow::*)(MainWindow::WindowLayer );
            if (_t _q_method = &MainWindow::layerChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 31)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 31;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 31)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 31;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::lockChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MainWindow::layerChanged(MainWindow::WindowLayer _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
