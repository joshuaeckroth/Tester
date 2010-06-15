#include "assignment.h"
#include "testcase.h"

void Assignment::setId(QString _id)
{
    id = _id;
}

QString Assignment::getId() const
{
    return id;
}

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

QList<TestCase*> Assignment::getTestCases()
{
    return tests;
}
