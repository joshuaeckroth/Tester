#include "runner.h"
#include <QProcess>
#include <QByteArray>
#include <QDebug>
#include <QMetaType>

#include "testcase.h"

Runner::Runner(QString _program, TestCase *_t)
    : p(NULL), program(_program), t(_t)
{
}

void Runner::run()
{
    t->reset();
    p = new QProcess;

    qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");

    connect(p, SIGNAL(error(QProcess::ProcessError)), this, SLOT(error(QProcess::ProcessError)));
    connect(p, SIGNAL(started()), this, SLOT(processStarted()));
    connect(p, SIGNAL(readyReadStandardOutput()), this, SLOT(readyReadStandardOutput()));
    connect(p, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processFinished(int,QProcess::ExitStatus)));
    p->start(program);

    exec();
}

void Runner::error(QProcess::ProcessError e)
{
    QString error;
    switch(e)
    {
    case QProcess::FailedToStart: error = "Failed to start"; break;
    case QProcess::Crashed: error = "Crashed"; break;
    default: error = "Unknown error";
    }

    qDebug() << error;
}

void Runner::processStarted()
{
    qDebug() << QString("started.");
}

void Runner::readyReadStandardOutput()
{
    QByteArray b = p->readAllStandardOutput();
    QString output(b);
    qDebug() << QString("output: %1").arg(output);
    if(b == QString("Enter x: "))
    {
        p->write(QString("33\n").toAscii());
    }
    else
    {
        p->write("99\n");
    }
}

void Runner::processFinished(int code, QProcess::ExitStatus status)
{
    qDebug() << QString("finished: %1, %2").arg(code).arg(status);
}
