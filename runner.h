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
    QString getProgramResult() const;
    TestCase *getTestCase() const;

signals:
    void runnerFinished(Runner*);
    void runnerError(Runner*, QString);

private slots:
    void error(QProcess::ProcessError);
    void readyReadStandardOutput();
    void processFinished(int, QProcess::ExitStatus);

private:
    QProcess *p;
    QString program;
    TestCase *t;
    InputOutput *io;
    bool waitingPrompt;
    QString programResult;

    QString markMismatch(QString a, QString b);
};

#endif // RUNNER_H
