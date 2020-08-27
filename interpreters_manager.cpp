#include "interpreters_manager.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
Initializer::Initializer()
{
    Py_InitializeEx(1);

    /*mid initialize threading and acquire GIL */
    PyEval_InitThreads();       ///#mid 初始化并获取全局解释器锁,在创建第二个线程或者参与其他线程操作(如 PyEval_ReleaseThread(tstate))之前，应该在主线程中调用它。
}

Initializer::~Initializer()
{
    Py_Finalize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/*mid python.org 上的用法如下
    1 Py_BEGIN_ALLOW_THREADS
    2 ...Do some blocking I/O operation...
    3 Py_END_ALLOW_THREADS

    参照以上用法,可理解为 此 自动变量 有效 的代码范围 不再 持有
*/
ReleaseGIL::ReleaseGIL()
{
    ///mid 参照自 Py_BEGIN_ALLOW_THREADS,当前 线程 释放全局锁的意思
    _state = PyEval_SaveThread();       ///mid PyEval_ReleaseLock 一直弃用,我们应该使用 PyEval_SaveThread 为了释放吉尔:
}

ReleaseGIL::~ReleaseGIL()
{
    ///mid 参照自 Py_END_ALLOW_THREADS,当前线程 在此获取 全局锁
    PyEval_RestoreThread(_state);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
SubInterpreter::SubInterpreter()
{
    RestoreThreadState restore;                 ///mid 保存 当前 占有 GIL 的线程的状态,并在析构时恢复
    m_thread_state = Py_NewInterpreter();
}

SubInterpreter::~SubInterpreter()
{
    if( m_thread_state )
    {
        SwapThreadState sts(m_thread_state);

        Py_EndInterpreter(m_thread_state);
    }
}

PyInterpreterState* SubInterpreter::interpreter_state()
{
    return m_thread_state->interp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//mid 给 将要使用 interpreter 的线程 创建一个 新的 线程状态
ThreadState::ThreadState(PyInterpreterState* interp)
{
    m_thread_state = PyThreadState_New(interp);
    PyEval_RestoreThread(m_thread_state);
}

ThreadState::~ThreadState()
{
    PyThreadState_Clear(m_thread_state);
    PyThreadState_DeleteCurrent();
}

ThreadState::operator PyThreadState*()
{
    return m_thread_state;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
SwapThreadState::SwapThreadState(PyThreadState* ts)
{
    m_old_thread_state = PyThreadState_Swap(ts);
}

SwapThreadState::~SwapThreadState()
{
    PyThreadState_Swap(m_old_thread_state);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
RestoreThreadState::RestoreThreadState()
{
    m_thread_state = PyThreadState_Get();
}

RestoreThreadState::~RestoreThreadState()
{
    PyThreadState_Swap(m_thread_state);
}
