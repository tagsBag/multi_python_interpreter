#include "context.h"

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "strategy.h"

Context::Context(Strategy * strategy)
{
    m_strategy = strategy;
}

void Context::write_log(std::string log)
{
    std::cout << log << std::endl;
}

using namespace boost::python;


BOOST_PYTHON_MODULE(function)
{
    // This will enable user-defined docstrings and python signatures,
    // while disabling the C++ signatures
    scope().attr("__version__") = "1.0.0";
    scope().attr("__doc__") = "a demo module to use boost_python.";
    docstring_options local_docstring_options(true, false, false);

    class_<Context>("Context")
            //.def(init<>()) //mid init()方法 只适用用于 python 独立导入 c++ 导出类
            //.def(init<std::string, int>())
            .def("write_log",&Context::write_log,"write log to c++ host");
}

BOOST_PYTHON_MODULE(student)
{
    ///mid 不能定义 第二个 导出,此处若有导出代码则会运行时奔溃
}
