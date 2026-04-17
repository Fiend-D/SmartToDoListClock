#include "x11globalshortcut.h"
#include <QGuiApplication>
#include <QDebug>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

X11EventReader::X11EventReader(Display *display, QObject *parent)
    : QThread(parent), m_display(display) {}

void X11EventReader::stop() {
    m_running = false;
}

void X11EventReader::run() {
    while (m_running) {
        XEvent ev;
        XNextEvent(m_display, &ev);
        
        if (!m_running) break;
        
        if (ev.type == KeyPress) {
            XKeyEvent *key = &ev.xkey;
            emit keyPressed(key->keycode);
        }
    }
}

X11GlobalShortcut::X11GlobalShortcut(QObject *parent) : QObject(parent)
{
    if (QGuiApplication::platformName() != "xcb") {
        qWarning() << "不是 XCB 平台";
        return;
    }
    
    m_display = XOpenDisplay(nullptr);
    if (!m_display) {
        qWarning() << "无法打开 X11 显示";
        return;
    }
    
    m_reader = new X11EventReader(m_display, this);
    connect(m_reader, &X11EventReader::keyPressed, this, [this](int keycode) {
        for (auto it = m_shortcuts.begin(); it != m_shortcuts.end(); ++it) {
            if (it.value().registered && it.value().keycode == (quint32)keycode) {
                emit activated(it.key());
                break;
            }
        }
    });
    m_reader->start();
}

X11GlobalShortcut::~X11GlobalShortcut()
{
    unregisterAll();
    if (m_reader) {
        m_reader->stop();
        m_reader->wait();
        delete m_reader;
    }
    if (m_display) {
        XCloseDisplay(m_display);
    }
}

bool X11GlobalShortcut::registerShortcut(const QKeySequence &key, int id)
{
    if (!m_display) {
        qDebug() << "X11 未初始化";
        return false;
    }
    
    if (m_shortcuts.contains(id) && m_shortcuts[id].registered) {
        unregisterShortcut(id);
    }
    
    Qt::Key qtKey = Qt::Key(key[0].key() & ~Qt::KeyboardModifierMask);
    Qt::KeyboardModifiers mods = Qt::KeyboardModifiers(key[0].keyboardModifiers());
    
    quint32 keycode = nativeKeycode(qtKey);
    quint32 modifiers = nativeModifiers(mods);
    
    qDebug() << "注册 - Qt键:" << qtKey << "X11键码:" << keycode 
             << "修饰符:" << modifiers;
    
    if (keycode == 0) {
        qDebug() << "❌ 键码转换失败";
        return false;
    }
    
    Window root = DefaultRootWindow(m_display);
    
    XGrabKey(m_display, keycode, modifiers, root, False, 
             GrabModeAsync, GrabModeAsync);
    XGrabKey(m_display, keycode, modifiers | Mod2Mask, root, False,
             GrabModeAsync, GrabModeAsync);
    XGrabKey(m_display, keycode, modifiers | LockMask, root, False,
             GrabModeAsync, GrabModeAsync);
    XGrabKey(m_display, keycode, modifiers | Mod2Mask | LockMask, root, False,
             GrabModeAsync, GrabModeAsync);
    
    XFlush(m_display);
    
    m_shortcuts[id] = {keycode, modifiers, true};
    qDebug() << "✅ 注册成功 id:" << id;
    return true;
}

void X11GlobalShortcut::unregisterShortcut(int id)
{
    if (!m_shortcuts.contains(id) || !m_shortcuts[id].registered) return;
    
    Window root = DefaultRootWindow(m_display);
    Shortcut &sc = m_shortcuts[id];
    
    XUngrabKey(m_display, sc.keycode, sc.modifiers, root);
    XUngrabKey(m_display, sc.keycode, sc.modifiers | Mod2Mask, root);
    XUngrabKey(m_display, sc.keycode, sc.modifiers | LockMask, root);
    XUngrabKey(m_display, sc.keycode, sc.modifiers | Mod2Mask | LockMask, root);
    
    XFlush(m_display);
    sc.registered = false;
}

void X11GlobalShortcut::unregisterAll()
{
    for (auto it = m_shortcuts.begin(); it != m_shortcuts.end(); ++it) {
        unregisterShortcut(it.key());
    }
}

quint32 X11GlobalShortcut::nativeKeycode(Qt::Key key)
{
    KeySym keysym = 0;
    
    if (key >= Qt::Key_A && key <= Qt::Key_Z) {
        keysym = XK_a + (key - Qt::Key_A);
    }
    else if (key >= Qt::Key_0 && key <= Qt::Key_9) {
        keysym = XK_0 + (key - Qt::Key_0);
    }
    else if (key >= Qt::Key_F1 && key <= Qt::Key_F12) {
        keysym = XK_F1 + (key - Qt::Key_F1);
    }
    else {
        switch (key) {
        case Qt::Key_Escape: keysym = XK_Escape; break;
        case Qt::Key_Tab: keysym = XK_Tab; break;
        case Qt::Key_Backspace: keysym = XK_BackSpace; break;
        case Qt::Key_Return: keysym = XK_Return; break;
        case Qt::Key_Space: keysym = XK_space; break;
        case Qt::Key_H: keysym = XK_h; break;
        case Qt::Key_L: keysym = XK_l; break;
        case Qt::Key_T: keysym = XK_t; break;
        case Qt::Key_S: keysym = XK_s; break;
        case Qt::Key_R: keysym = XK_r; break;
        case Qt::Key_Q: keysym = XK_q; break;
        default: break;
        }
    }
    
    if (keysym == 0) return 0;
    return XKeysymToKeycode(m_display, keysym);
}

quint32 X11GlobalShortcut::nativeModifiers(Qt::KeyboardModifiers modifiers)
{
    quint32 native = 0;
    if (modifiers & Qt::ShiftModifier) native |= ShiftMask;
    if (modifiers & Qt::ControlModifier) native |= ControlMask;
    if (modifiers & Qt::AltModifier) native |= Mod1Mask;
    if (modifiers & Qt::MetaModifier) native |= Mod4Mask;
    return native;
}