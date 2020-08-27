#include "strategy.h"


#define INIT_MODULE PyInit_function
extern "C" PyObject* INIT_MODULE();

// Parses the value of the active python exception
// NOTE SHOULD NOT BE CALLED IF NO EXCEPTION
namespace py = boost::python;
std::string Strategy::parse_python_exception()
{
    PyObject *type_ptr = NULL, *value_ptr = NULL, *traceback_ptr = NULL;
    // Fetch the exception info from the Python C API
    PyErr_Fetch(&type_ptr, &value_ptr, &traceback_ptr);

    // Fallback error
    std::string ret("Unfetchable Python error");
    // If the fetch got a type pointer, parse the type into the exception string
    if(type_ptr != NULL){
        py::handle<> h_type(type_ptr);
        py::str type_pstr(h_type);
        // Extract the string from the boost::python object
        py::extract<std::string> e_type_pstr(type_pstr);
        // If a valid string extraction is available, use it
        //  otherwise use fallback
        if(e_type_pstr.check())
            ret = e_type_pstr();
        else
            ret = "Unknown exception type";
    }
    // Do the same for the exception value (the stringification of the exception)
    if(value_ptr != NULL){
        py::handle<> h_val(value_ptr);
        py::str a(h_val);
        py::extract<std::string> returned(a);
        if(returned.check())
            ret +=  ": " + returned();
        else
            ret += std::string(": Unparseable Python error: ");
    }
    // Parse lines from the traceback using the Python traceback module
    if(traceback_ptr != NULL){
        py::handle<> h_tb(traceback_ptr);
        // Load the traceback module and the format_tb function
        py::object tb(py::import("traceback"));
        py::object fmt_tb(tb.attr("format_tb"));
        // Call format_tb to get a list of traceback strings
        py::object tb_list(fmt_tb(h_tb));
        // Join the traceback strings into a single string
        py::object tb_str(py::str("\n").join(tb_list));
        // Extract the string, check the extraction, and fallback in necessary
        py::extract<std::string> returned(tb_str);
        if(returned.check())
            ret += ": " + returned();
        else
            ret += std::string(": Unparseable Python traceback");
    }
    return ret;
}

void strategy_function(PyInterpreterState* interpreter_state, const char* tname)
{   ///mid 在 interpreter_state 代表 的 解释器 环境下 执行 python 代码
    std::string code = R"PY(
from __future__ import print_function
import sys
print("TNAME: sys.xxx={}".format(getattr(sys, 'xxx', 'attribute not set')))
    )PY";

    code.replace(code.find("TNAME"), 5, tname);

    BindThreadWithInterpreter scope(interpreter_state);
    PyRun_SimpleString(code.c_str());
}

Strategy::Strategy(std::string tname)
{
    m_interpreter_state = PyThreadState_Get()->interp;          ///mid 用当前 解释器 执行
    m_interpreter_state = m_interpreter.interpreter_state();    ///mid 用 策略 特有 解释器 执行


    {

        {   ///mid 1) 初始化环境
            init("function",&INIT_MODULE);
        }
        {
            m_context = std::make_shared<Context>(this);

            m_main_namespace["param01"] = 1000;
        }
    }

}

void Strategy::init(const std::string& module, PyObject*(*initfunc)())
{
    try
    {
        ReleaseGIL gil;                     ///mid 构造函数 是主线程调用,调用时 主 解释器 拥有 GIL,执行是 新 解释器,所以主解释器 需要放弃 GIL
        BindThreadWithInterpreter scope(m_interpreter_state);

        ///mid 1) 往 python 运行时环境添加 C++ 暴露的方法,使python可以导入使用这些方法
        ///mid   必须在 PyInitialize() 前调用
        PyImport_AppendInittab(module.c_str(),initfunc);

        ///mid 2) 初始化 python 运行环境
        //Py_Initialize();
        boost::python::object main_module = boost::python::import("__main__");

        m_main_namespace = boost::python::extract<boost::python::dict>(main_module.attr("__dict__"));

        boost::python::import(module.c_str());              ///mid 这个必须有,否则 按 apply 后奔溃
    }
    catch(boost::python::error_already_set& e)
    {
        //on_error();
    }
}

void Strategy::on_start(std::string tname)
{
    try
    {
        {   ///mid 读入策略文件
            std::string script_file_path = "C:/Users/admin/Desktop/test01/multi_python_interpreter/python_scripts/strategy.py";
            QFile file(script_file_path.c_str());
            if(file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QTextStream in(&file);
                m_script = in.readAll().toStdString();
                file.close();
            }
        }
        BindThreadWithInterpreter scope(m_interpreter_state);


        {   ///mid 3) 执行 文本 python,传入 参数 字典 namespace_
            boost::python::exec(m_script.c_str(),m_main_namespace);     ///mid 此语句 执行时,会将 m_main_namespace 中的数据 传入 python 文件执行环境,同时将 python 文件 的执行数据 存入 m_main_namespace
            //m_main_namespace["Context"] = boost::python::ptr(m_context.get());

            if(m_main_namespace.has_key("on_start"))
            {
                on_start_py = m_main_namespace["on_start"];
                std::cout <<"c++ on_start() called."<<std::endl;
                on_start_py();
                /*  extract 是将 python 对象的值 提取 转化 为 C++ 类型的值,如果不需要返回值则直接执行即可
                    on_start_py = m_main_namespace.attr("on_start")
                    boost::python::object ret = on_start_py();
                    std::cout << boost::python::extract(ret) << std::endl;
                    //boost::python::extract<double>(on_start_py());    ///mid 这种形式是执行并提取返回值
                */
            }
        }
    }
    catch(boost::python::error_already_set& e)
    {
        on_error();
        //PyErr_PrintEx(0);
        //PyErr_Print();
    }

}

void Strategy::on_tick(std::string tname)
{
    std::string code = R"PY(
from __future__ import print_function
import sys
print("TNAME:on_tick(). sys.xxx={}".format(getattr(sys, 'xxx', 'attribute not set')))
    )PY";

    code.replace(code.find("TNAME"), 5, tname);

    BindThreadWithInterpreter scope(m_interpreter_state);

    PyRun_SimpleString(code.c_str());
}

void Strategy::on_error()
{
    BindThreadWithInterpreter scope(m_interpreter_state);       ///mid 切换到 策略 解释器 并获取错误输出

    std::cout <<"Error occured.";
    std::string perror_str = parse_python_exception();
    std::cout << "Error:" << perror_str << std::endl;
}
