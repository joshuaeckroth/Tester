#include <QtGui/QApplication>
#include <QFile>
#include <QDebug>
#include "mainwindow.h"
#include "runner.h"
#include "assignmentset.h"
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

    //Runner *r = new Runner(QString("c:\\users\\josh\\documents\\school\\202\\TestExample\\debug\\TestExample.exe"));
    //r->start();

    QApplication a(argc, argv);
    MainWindow w(as);
    w.show();
    return a.exec();
}
