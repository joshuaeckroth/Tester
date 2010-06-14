#include "assignment.h"
#include "testcase.h"

void Assignment::setName(QString _name)
{
    name = _name;
}

QString Assignment::getName() const
{
    return name;
}

void Assignment::addTestCase(TestCase *t)
{
    tests << t;
}
