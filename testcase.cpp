#include <QDebug>
#include "testcase.h"
#include "inputoutput.h"

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

QString TestCase::desiredResult() const
{
    QString result;
    for(int i = 0; i < io.size(); i++)
    {
        result += QString("%1<span class=\"input\">%2</span><br/>%3<br/>")
                  .arg(io[i]->getPrompt()).arg(io[i]->getInput()).arg(io[i]->getOutput());
    }
    return result;
}
