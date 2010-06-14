#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include <QList>
#include <QString>

class TestCase;

class Assignment
{
public:
    void setName(QString _name);
    QString getName() const;
    void addTestCase(TestCase *t);

private:
    QString name;
    QList<TestCase*> tests;
};

#endif // ASSIGNMENT_H
