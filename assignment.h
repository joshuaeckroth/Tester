#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include <QList>
#include <QString>

class TestCase;

class Assignment
{
public:
    void setId(QString _id);
    QString getId() const;
    void setName(QString _name);
    QString getName() const;
    void addTestCase(TestCase *t);
    QList<TestCase*> getTestCases();

private:
    QString id;
    QString name;
    QList<TestCase*> tests;
};

#endif // ASSIGNMENT_H
