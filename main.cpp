#include <iostream>

#include <boost/python.hpp>

#include <QFile>
#include <QTextStream>
#include <QApplication>

#include "strategy.h"

using namespace std;



int main()
{
    Initializer init;

    cout << "Hello World! >>>>>" << endl;

    SubInterpreter s1;
    SubInterpreter s2;

    {   ///mid 不指定 python 解释器,直接用 主解释器 执行 python script file,为了测试 其他解释器 状态中 是否 能 调用 主 解释器 状态 数据
        std::string script_file_path = "C:/Users/admin/Desktop/test01/multi_python_interpreter/python_scripts/setting_test.py";
        QFile file(script_file_path.c_str());
        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);

            QString script = in.readAll();

            boost::python::exec(script.toStdString().c_str());     ///mid 此语句 执行时,会将 m_main_namespace 中的数据 传入 python 文件执行环境,同时将 python 文件 的执行数据 存入 m_main_namespace
            file.close();
        }
    }



    {
        Strategy strategy("strategy instance");

        {   ///mid 启动策略
            std::function<void(std::string)> thread_function = std::bind(&Strategy::on_start,&strategy,std::placeholders::_1);

            std::thread tick_generater(thread_function,"strategy");
            //tick_generater.detach();

            ReleaseGIL t;   ///mid 此后,之前 的 t1,t2,t3,t4 才会被执行,和 PyEval_InitThreads 成对 使用,一个获取,一个释放
            tick_generater.join();
        }

        {   ///mid on_tick
            std::function<void(std::string)> thread_function = std::bind(&Strategy::on_tick,&strategy,std::placeholders::_1);

            std::thread tick_generater(thread_function,"strategy");
            //tick_generater.detach();

            ReleaseGIL t;   ///mid 此后,之前 的 t1,t2,t3,t4 才会被执行,和 PyEval_InitThreads 成对 使用,一个获取,一个释放
            tick_generater.join();
        }
    }
    std::thread t1{strategy_function, s1.interpreter_state(), "t1(s1)"};


    PyInterpreterState *interp = PyThreadState_Get()->interp;   ///mid 获取当前 拥有 GIL 的线程的状态
    std::thread t4{strategy_function, interp, "t4(main)"};                      ///mid PyEval_InitThreads() 后 只有主线程 获取了 GIL,释放前

    std::thread t2{strategy_function, s2.interpreter_state(), "t2(s2)"};
    std::thread t3{strategy_function, s1.interpreter_state(), "t3(s1)"};

    ReleaseGIL t;   ///mid 此后,之前 的 t1,t2,t3,t4 才会被执行,和 PyEval_InitThreads 成对 使用,一个获取,一个释放

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    cout << "Hello World! <<<<<" << endl;

    return 0;
}
