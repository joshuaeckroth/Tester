#include "runner.h"
#include <QProcess>
#include <QByteArray>
#include <QDebug>
#include <QMetaType>

#include "testcase.h"
#include "inputoutput.h"

Runner::Runner(QString _program, TestCase *_t)
    : p(NULL), program(_program), t(_t), io(NULL)
{
}

void Runner::run()
{
    t->reset();
    io = t->nextInputOutput();
    waitingPrompt = true;
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
    QString output(p->readAllStandardOutput());
    output.remove(QRegExp("\\s"));

    InputOutput *nextIo = t->peekNextInputOutput();

    QString ioPrompt = io->getPrompt();
    ioPrompt.remove(QRegExp("\\s"));

    QString ioOutput = io->getOutput();
    ioOutput.remove(QRegExp("\\s"));

    QString nextPrompt;
    if(nextIo)
    {
        nextPrompt = nextIo->getPrompt();
        nextPrompt.remove(QRegExp("\\s"));
    }

    if(io)
    {
        if(waitingPrompt)
        {
            if(0 == output.compare(ioPrompt, Qt::CaseInsensitive))
            {
                qDebug() << QString("matched prompt: %1").arg(ioPrompt);

                p->write(QString("%1\n").arg(io->getInput()).toAscii());
                waitingPrompt = false;
            }
            else
            {
                qDebug() << QString("failed prompt: %1\ngot: %2").arg(ioPrompt).arg(output);
            }
        }
        else
        {
            if(0 == output.compare(QString("%1%2").arg(ioOutput).arg(nextPrompt), Qt::CaseInsensitive))
            {
                qDebug() << QString("match output + next prompt: %1/%2").arg(ioOutput).arg(nextPrompt);
                io = t->nextInputOutput();
                if(io)
                    p->write(QString("%1\n").arg(io->getInput()).toAscii());
            }
            else
            {
                qDebug() << QString("failed match output + next prompt: %1/%2--\ngot: %3")
                        .arg(ioOutput).arg(nextPrompt).arg(output);
            }
        }
    }
}

void Runner::processFinished(int code, QProcess::ExitStatus status)
{
    qDebug() << QString("finished: %1, %2").arg(code).arg(status);
}
