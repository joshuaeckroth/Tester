#ifndef TESTCASE_H
#define TESTCASE_H

#include <QString>
#include <QList>

class InputOutput;

class TestCase
{
public:
    TestCase();
    void setName(QString _name);
    void addInputOutput(InputOutput *p);
    void reset();
    InputOutput *nextInputOutput();

private:
    QString name;
    QList<InputOutput*> io;
    int currentIo;
};

#endif // TESTCASE_H
