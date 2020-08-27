#ifndef INTERPRETERMANAGER_H
#define INTERPRETERMANAGER_H
//#include <boost/python.hpp>

#include <Python.h>


using namespace std;

#include <Python.h>

#include <string>
#include <vector>
#include <thread>

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///mid 用局部自动变量方式,自动初始化 并 清理 python 环境
///mid 可以在 main 中定义为 自动变量
struct Initializer
{
    Initializer();
    ~Initializer();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// allow other threads to run
///mid 允许 其他 python 解释器 执行 其他 c 线程 的 python 代码
///mid 在此类 的 实例 有效的 范围内 当前线程 释放 gil,其他线程才有 可能 获取 GIL
class ReleaseGIL
{
public:
    ReleaseGIL();
    ~ReleaseGIL();

private:
    PyThreadState* _state;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// restore the thread state when the object goes out of scope
///mid 在调用 Py_NewInterpreter 需要保存 之前 占用 GIL 的线程 的 状态,创建后恢复,否则新建的 会 直接 获取 GIL
///mid 同 SwapThreadState 不同,此类 仅为了 恢复
class RestoreThreadState
{
public:
    RestoreThreadState();
    ~RestoreThreadState();

private:
    PyThreadState* m_thread_state;      ///mid 当前 占用 GIL 的 thread 的线程状态
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// swap the current thread state with ts, restore when the object goes out of scope
///mid 将 ts 切换为 占用 GIL 锁,析构时 恢复
///mid 同 RestoreThreadState 不同,此类 为了 切换后执行,然后恢复
class SwapThreadState
{
public:
    SwapThreadState(PyThreadState* ts);
    ~SwapThreadState();

private:
    PyThreadState* m_old_thread_state;
};

// create new thread state for interpreter interp, make it current, and clean up on destruction
class ThreadState
{
public:
    ThreadState(PyInterpreterState* interp);
    ~ThreadState();
    operator PyThreadState*();

private:
    PyThreadState* m_thread_state;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// perform the necessary setup and cleanup for a new thread running using a specific interpreter
///mid 将当前线程 和 interpreter绑定,结束后解绑
struct BindThreadWithInterpreter
{
    BindThreadWithInterpreter(PyInterpreterState* interp) : _state(interp)
    {
    }
    ThreadState _state;
    SwapThreadState _swap{ _state };
};

// represent a sub interpreter
class SubInterpreter
{
public:
    SubInterpreter();
    ~SubInterpreter();

    PyInterpreterState* interpreter_state();

private:
    PyThreadState* m_thread_state;
};

#endif // INTERPRETERMANAGER_H
