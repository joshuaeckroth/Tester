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

    QString outputClean = output;
    outputClean.remove(QRegExp("\\s"));

    InputOutput *nextIo = t->peekNextInputOutput();

    QString ioPrompt = io->getPrompt();

    QString ioPromptClean = ioPrompt;
    ioPromptClean.remove(QRegExp("\\s"));

    QString ioOutput = io->getOutput();

    QString ioOutputClean = ioOutput;
    ioOutputClean.remove(QRegExp("\\s"));

    QString nextPrompt;
    QString nextPromptClean;
    if(nextIo)
    {
        nextPrompt = nextIo->getPrompt();

        nextPromptClean = nextPrompt;
        nextPromptClean.remove(QRegExp("\\s"));
    }

    if(io)
    {
        if(waitingPrompt)
        {
            if(0 == outputClean.compare(ioPromptClean, Qt::CaseInsensitive))
            {
                p->write(QString("%1\n").arg(io->getInput()).toAscii());
                waitingPrompt = false;

                programResult += QString("%1 <span class=\"input\">%2</span><br/>")
                                 .arg(output).arg(io->getInput());
                testCaseResult += QString("%1 <span class=\"input\">%2</span><br/>")
                                  .arg(ioPrompt).arg(io->getInput());
            }
            else
            {
                programResult += QString("%1").arg(markMismatch(ioPrompt, output));
                testCaseResult += QString("%1").arg(ioPrompt);

                p->kill();
            }
        }
        else
        {
            if(0 == outputClean.compare(QString("%1%2").arg(ioOutputClean).arg(nextPromptClean), Qt::CaseInsensitive))
            {
                programResult += (QString("%1").arg(output)).replace(QRegExp("\\r?\\n"), "<br/>");
                testCaseResult += QString("%1<br/>%2").arg(ioOutput).arg(nextPrompt);

                io = t->nextInputOutput();
                if(io)
                {
                    p->write(QString("%1\n").arg(io->getInput()).toAscii());

                    programResult += QString(" <span class=\"input\">%1</span><br/>").arg(io->getInput());
                    testCaseResult += QString(" <span class=\"input\">%1</span><br/>").arg(io->getInput());
                }
            }
            else
            {
                programResult += (QString("%1").arg(markMismatch(QString("%1%2").arg(ioOutput).arg(nextPrompt), output)))
                                 .replace(QRegExp("\\r?\\n"), "<br/>");
                testCaseResult += QString("%1<br/>%2").arg(ioOutput).arg(nextPrompt);

                p->kill();
            }
        }
    }
}

void Runner::processFinished(int code, QProcess::ExitStatus status)
{
    qDebug() << QString("finished: %1, %2\n%3\n\n%4").arg(code).arg(status).arg(testCaseResult).arg(programResult);
}

QString Runner::markMismatch(QString a, QString b)
{
    QRegExp space("\\s");
    int i, j;
    for(i = 0, j = 0; i < a.length() && j < b.length(); i++, j++)
    {
        // skip spaces
        while(space.exactMatch(QString(a[i]))) i++;
        while(space.exactMatch(QString(b[j]))) j++;

        if(i < a.length() && j < b.length())
        {
            if(0 != QString(a[i]).compare(QString(b[j]), Qt::CaseInsensitive))
            {
                return QString("%1<span class=\"mismatch\">%2</span>%3")
                        .arg(b.left(j)).arg(b[j]).arg(b.right(b.size()-j-1));
            }
        }
        else
        {
            return QString("%1<span class=\"mismatch\">&nbsp;</span>").arg(b);
        }
    }

    int pos;
    if(j < b.length() && (pos = space.indexIn(b.right(b.size()-j))) != -1)
    {
        return QString("%1<span class=\"mismatch\">%2</span>%3")
                .arg(b.left(pos+j-1)).arg(b[pos+j-1]).arg(b.right(b.size()-pos-j));
    }

    return b;
}
