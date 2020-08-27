#ifndef CONTEXT_H
#define CONTEXT_H

#include <iostream>
#include <string>

#include <boost/python.hpp>

class Strategy;

class Context
{
public:
    Context(){}
    Context(Strategy * strategy);

public:
    bool operator==(Context const& s) const { return m_strategy == s.m_strategy; }
    bool operator!=(Context const& s) const { return m_strategy != s.m_strategy; }

public:
    void write_log(std::string);

private:
    Strategy * m_strategy;
};

#endif // CONTEXT_H
