#ifndef RUNNER_H
#define RUNNER_H

#include <QThread>
#include <QProcess>

class TestCase;
class InputOutput;

class Runner : public QThread
{
    Q_OBJECT
public:
    Runner(QString _program, TestCase *_t);
    void run();

private slots:
    void error(QProcess::ProcessError);
    void processStarted();
    void readyReadStandardOutput();
    void processFinished(int, QProcess::ExitStatus);

private:
    QProcess *p;
    QString program;
    TestCase *t;
    InputOutput *io;
    bool waitingPrompt;
};

#endif // RUNNER_H
