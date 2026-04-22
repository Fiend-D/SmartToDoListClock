cmake_minimum_required(VERSION 3.16)
project(SmartDeskClock VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network Gui)

# ✅ 查找 X11
find_package(X11 REQUIRED)
if(NOT X11_FOUND)
    message(FATAL_ERROR "X11 not found")
endif()

qt_add_executable(SmartDeskClock
    main.cpp
    mainwindow.cpp mainwindow.h
    clockwidget.cpp clockwidget.h
    weatherwidget.cpp weatherwidget.h
    aiquoteservice.cpp aiquoteservice.h
    configmanager.cpp configmanager.h
    emotionanalyzer.cpp emotionanalyzer.h
    aivisualengine.cpp aivisualengine.h
    x11globalshortcut.cpp x11globalshortcut.h
    todoitem.cpp todoitem.h
    todowidget.cpp todowidget.h
    addtododialog.cpp addtododialog.h
    neumorphism.h
)

target_link_libraries(SmartDeskClock PRIVATE
    Qt6::Core 
    Qt6::Widgets 
    Qt6::Network 
    Qt6::Gui
    # ✅ 链接 X11
    ${X11_LIBRARIES}
    X11::xcb  # 现代 CMake 推荐方式，如果不可用则用 ${X11_LIBRARIES}
)

# ✅ 包含 X11 头文件路径
target_include_directories(SmartDeskClock PRIVATE 
    ${X11_INCLUDE_DIR}
)