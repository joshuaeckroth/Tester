#include <QtGui/QApplication>
#include <QFile>
#include <QDebug>
#include <QList>
#include <QStringList>
#include "mainwindow.h"
#include "runner.h"
#include "assignmentset.h"
#include "assignment.h"
#include "testcase.h"
#include "testsreader.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AssignmentSet *as = new AssignmentSet;
    QFile tests(":/tests/tests.xml");
    tests.open(QIODevice::ReadOnly);
    TestsReader reader(as);
    if(!reader.read(&tests))
    {
        qDebug() << QString("Error reading tests XML file: %1").arg(reader.errorString());
        return -1;
    }


    QString assignment;

    QStringList args = a.arguments();
    int i;
    if((i = args.indexOf("-a")) != -1)
    {
        assignment = args[i+1];
    }

    if(!assignment.isEmpty())
    {
        bool foundAssignment = false;
        QList<TestCase*> testCases;
        QList<Assignment*> assignments = as->getAssignments();
        for(int i = 0; i < assignments.size(); i++)
        {
            if(assignments[i]->getId() == assignment)
            {
                testCases = assignments[i]->getTestCases();
                foundAssignment = true;
            }
        }
        if(!foundAssignment)
        {
            qDebug() << QString("Invalid assignment: %1").arg(assignment);
            return -2;
        }
        for(int i = 0; i < testCases.size(); i++)
        {
            qDebug() << QString("current test case: %1").arg(testCases[i]->getName());
            Runner *r = new Runner(QString("c:\\users\\josh\\documents\\school\\202\\TestExample\\debug\\TestExample.exe"), testCases[i]);
            r->start();
        }
    }



    MainWindow w(as);
    w.show();
    return a.exec();
}
