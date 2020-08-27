#ifndef STRATEGY_H
#define STRATEGY_H

#include <iostream>

#include <QFile>
#include <QTextStream>
#include <QApplication>

#include <boost/python.hpp>

#include "interpreters_manager.h"

#include "context.h"

// runs in a new thread
void strategy_function(PyInterpreterState* interpreter_state, const char* tname);

class Strategy
{
public:
    Strategy(std::string tname);
    ~Strategy()
    {
    }
    void on_start(std::string tname);

    void on_tick(std::string tname);

    void init(const std::string& module, PyObject*(*initfunc)());

    void on_error();
    std::string parse_python_exception();

private:
    boost::python::object on_start_py;
    boost::python::object on_stop_py;
    boost::python::object on_tick_py;
    boost::python::object on_order_py;
    boost::python::object on_trade_py;
    boost::python::object on_timer_py;

private:
    boost::python::dict m_main_namespace;
    std::string m_script;
    std::string m_script_file_path;

    std::shared_ptr<Context> m_context;

    SubInterpreter m_interpreter;                   ///mid 每个策略 自己 创建 一个 解释器
    PyInterpreterState* m_interpreter_state;
};

#endif // STRATEGY_H
