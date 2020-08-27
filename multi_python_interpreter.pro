QT += widgets

TEMPLATE = app
CONFIG += console c++11
#CONFIG -= app_bundle
#CONFIG -= qt


win32{#mid {要紧随其后,否则无效,1)解决界面中文乱码问题.2)解决源码文件 utf-8 编码加载问题
        QMAKE_CXXFLAGS += /utf-8
}

####################################################################################################
#mid boost_python 和 python 库说明
#mid 1) 编译器匹配
#mid boost_python,python,当前开发环境所使用的 msvc 需要版本一致
#mid 比如当前开发环境为 vs2017(14.1) ,msvc 15.9.16,所以,boost_python 和 python 都需要选对应版本 msvc 编译的
#mid 2) python 版本匹配
#mid boost_python 和 python 的版本需要匹配
#mid 比如 boost_python 如果为 3.8,则 python 的版本必须为 3.8
#mid 3) dll 说明
#mid 编译后,运行前,必须要将对应依赖的 python dll 拷贝到目标编译目标目录下,否则闪退
####################################################################################################
#mid 包含头文件目录
INCLUDEPATH += D:/boost_1_73_0_14_1_64
INCLUDEPATH += D:/mid/Python380b3/include

#mid 导入依赖库
LIBS += -LD:/boost_1_73_0_14_1_64/lib64-msvc-14.1
LIBS += -lboost_python38-vc141-mt-gd-x64-1_73

LIBS += -LD:/mid/Python380b3/libs/
LIBS += -lpython38
####################################################################################################

####################################################################################################
#mid 指定 临时 文件 目录
OBJECTS_DIR = obj
MOC_DIR = moc
RCC_DIR = rc
UI_DIR = ui
####################################################################################################

####################################################################################################
#mid 拷贝 dll 文件
CONFIG(debug,debug|release){#mid 此处的相对位置为构建目标目录的相对位置
    DESTDIR = debug
    PYTHON_DLL_FILE = D:/mid/Python380b3/python38.dll
    BOOST_PYTHON_DLL_FILE = D:/boost_1_73_0_14_1_64/lib64-msvc-14.1/boost_python38-vc141-mt-gd-x64-1_73.dll
} else {
    DESTDIR = release
    PYTHON_DLL_FILE = D:/mid/Python380b3/python38.dll
    BOOST_PYTHON_DLL_FILE = D:/boost_1_73_0_14_1_64/lib64-msvc-14.1/boost_python38-vc141-mt-gd-x64-1_73.dll
}
win32{
    #mid 经测试 win 下拷贝有几个要求
    #mid 1) 文件路径必须没有空格
    #mid 2) 将/替换为\\才能正确识别路径(qt使用 /)
    #mid 3) 也有可能是目标文件夹没有权限
    message("当前系统:win,复制 python38")
    DIR1 =  $$PYTHON_DLL_FILE                               #mid 需要用通配符,因为 QMAKE_POST_LINK += xcopy $$DIR11 $$DIR21 /i /y 只能出现一次,两个文件分别 xcopy 会失败
    DIR2  = $$DESTDIR/                                      #mid 拷贝到程序运行目录
    DIR11 = $$replace(DIR1, /, \\)                          #mid replace函数的第一个参数必须是大写
    DIR21 = $$replace(DIR2, /, \\)

    message($$DIR11)
    message("to========")
    message($$DIR21)
    QMAKE_POST_LINK += xcopy $$DIR11 $$DIR21 /i /y
}
####################################################################################################

SOURCES += \
        context.cpp \
        interpreters_manager.cpp \
        main.cpp \
        strategy.cpp

HEADERS += \
    context.h \
    interpreters_manager.h \
    strategy.h

DISTFILES += \
    python_scripts/setting_test.py \
    python_scripts/strategy.py
