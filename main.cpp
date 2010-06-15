#include <QtGui/QApplication>
#include <QFile>
#include <QDebug>
#include <QList>
#include "mainwindow.h"
#include "runner.h"
#include "assignmentset.h"
#include "assignment.h"
#include "testcase.h"
#include "testsreader.h"

int main(int argc, char *argv[])
{
    AssignmentSet *as = new AssignmentSet;
    QFile tests(":/tests/tests.xml");
    tests.open(QIODevice::ReadOnly);
    TestsReader reader(as);
    if(!reader.read(&tests))
    {
        qDebug() << QString("Error reading tests XML file: %1").arg(reader.errorString());
        return -1;
    }

    QList<Assignment*> assignments = as->getAssignments();
    QList<TestCase*> testCases = assignments[0]->getTestCases();
    for(int i = 0; i < testCases.size(); i++)
    {
        qDebug() << QString("current test case: %1").arg(testCases[i]->getName());
        Runner *r = new Runner(QString("c:\\users\\josh\\documents\\school\\202\\TestExample\\debug\\TestExample.exe"), testCases[i]);
        r->start();
    }

    QApplication a(argc, argv);
    MainWindow w(as);
    w.show();
    return a.exec();
}
