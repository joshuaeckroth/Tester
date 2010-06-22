#include <QtGui/QApplication>
#include <QFile>
#include <QFileInfo>
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
    QString program;

    QStringList args = a.arguments();
    int i;
    if((i = args.indexOf("-a")) != -1)
    {
        assignment = args[i+1];
        program = args[i+2];
    }

    if(!assignment.isEmpty() && !program.isEmpty())
    {
        QFile p(program);
        QFileInfo pf(p);
        if(!pf.exists() || !pf.isExecutable() || pf.isDir())
        {
            qDebug() << QString("Cannot open program: %1").arg(program);
            return -2;
        }

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
            return -3;
        }

        int failed = 0;
        for(int i = 0; i < testCases.size(); i++)
        {
            Runner *r = new Runner(program, testCases[i]);
            r->start();
            if(!r->wait(5000))
            {
                qDebug() << QString("Timed out on test %1.").arg(r->getTestCase()->getName());
                failed++;
            }
            else
            {
                if(r->getProgramResult().contains("goodresult"))
                {
                    qDebug() << QString("Passed test %1.").arg(r->getTestCase()->getName());
                }
                else
                {
                    qDebug() << QString("Failed test %1.").arg(r->getTestCase()->getName());
                    failed++;
                }
            }
        }
        qDebug() << QString("Passed %1/%2 tests.").arg(testCases.size() - failed).arg(testCases.size());
        return 0;
    }
    else // so assignment.isEmpty() or program.isEmpty()
    {
        MainWindow w(as);
        w.show();
        return a.exec();
    }
}
