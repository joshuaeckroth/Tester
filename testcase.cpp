#include "testcase.h"

TestCase::TestCase()
{ }

void TestCase::setName(QString _name)
{
    name = _name;
}

QString TestCase::getName() const
{
    return name;
}

void TestCase::addInputOutput(InputOutput *p)
{
    io << p;
}

void TestCase::reset()
{
    currentIo = 0;
}

InputOutput *TestCase::nextInputOutput()
{
    if(currentIo >= io.size()) return NULL;
    return io[currentIo++];
}

InputOutput *TestCase::peekNextInputOutput() const
{
    if(currentIo >= io.size()) return NULL;
    return io[currentIo];
}
