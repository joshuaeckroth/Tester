#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "assignmentset.h"
#include "assignment.h"
#include "runner.h"
#include "testcase.h"

const QString MainWindow::testResultsHtml = QString("<html><head><style type=\"text/css\">"
                                                    "pre { margin-left: 20px; }\n"
                                                    "p.running { color: rgb(23, 22, 47); }\n"
                                                    "span.input { font-weight: bold; text-decoration: underline; }\n"
                                                    "pre.desired { color: rgb(232, 196, 117); }\n"
                                                    "pre.goodresult { color: rgb(255, 178, 72); }\n"
                                                    "pre.badresult { color: rgb(199, 96, 88); }\n"
                                                    "span.mismatch { color: rgb(137, 52, 109); font-weight: bold; "
                                                    " background-color: #dddddd; text-decoration: underline; }\n"
                                                    "</style></head><body><!--REPLACE--></body></html>");

MainWindow::MainWindow(AssignmentSet *as, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    assignmentSet = as;
    QList<Assignment*> assignments = assignmentSet->getAssignments();
    for(int i = 0; i < assignments.size(); i++)
    {
        ui->assignmentComboBox->addItem(assignments[i]->getName());
    }
    assignment = assignments[0];
    connect(ui->assignmentComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(assignmentChanged(int)));

    connect(ui->programButton, SIGNAL(clicked()), this, SLOT(chooseProgram()));

    programValid = false;
    ui->runTestsButton->setEnabled(false);

    connect(ui->runTestsButton, SIGNAL(clicked()), this, SLOT(runTests()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::assignmentChanged(int i)
{
    QList<Assignment*> assignments = assignmentSet->getAssignments();
    assignment = assignments[i];
}

void MainWindow::chooseProgram()
{
    QFileDialog d(this);
    if(!lastDir.isEmpty()) d.setDirectory(lastDir);
    d.exec();
    lastDir = d.directory().absolutePath();
    QStringList selected = d.selectedFiles();
    if(selected.isEmpty()) program = QString();
    else program = selected[0];

    if(program.isEmpty()) ui->programLabel->setText("No program loaded.");
    else
    {
        QFile p(program);
        QFileInfo pf(p);
        if(!pf.exists() || !pf.isExecutable() || pf.isDir())
        {
            ui->programLabel->setText("Program is invalid.");
            programValid = false;
            ui->runTestsButton->setEnabled(false);
        }
        else
        {
            ui->programLabel->setText(QString("Loaded %1.").arg(pf.baseName()));
            programValid = true;
            ui->runTestsButton->setEnabled(true);
        }
    }
}

void MainWindow::runTests()
{
    ui->runTestsButton->setEnabled(false);
    ui->programButton->setEnabled(false);
    QList<TestCase*> testCases = assignment->getTestCases();
    mutex.lock();
    testResults = testResultsHtml;
    for(int i = 0; i < testCases.size(); i++)
    {
        testResults.replace("<!--REPLACE-->",
                            QString("<p class=\"running\">Running %1...</p><!--REPLACE-->")
                            .arg(testCases[i]->getName()));
        Runner *r = new Runner(QString(program), testCases[i]);
        runners << r;
        connect(r, SIGNAL(finished()), this, SLOT(runnerFinished()));
        r->start();
    }
    ui->runningLabel->setText(QString("Running %1 tests...").arg(testCases.size()));
    ui->testResults->setHtml(testResults);
    mutex.unlock();
}

void MainWindow::runnerFinished()
{
    mutex.lock();
    for(int i = 0; i < runners.size(); i++)
    {
        if(runners[i] != NULL && runners[i]->isFinished())
        {
            TestCase *t = runners[i]->getTestCase();
            testResults.replace(QString("<p class=\"running\">Running test case %1...</p>").arg(t->getName()),
                                QString("<p>%1:<pre class=\"desired\">%2</pre><pre class=\"%3result\">%4</pre></p>")
                                .arg(t->getName()).arg(runners[i]->getTestCaseResult())
                                .arg(runners[i]->getProgramResult().contains("mismatch") ? "bad" : "good")
                                .arg(runners[i]->getProgramResult()));
            ui->testResults->setHtml(testResults);
            disconnect(runners[i], SIGNAL(finished()), this, SLOT(runnerFinished()));
            delete runners[i];
            runners[i] = NULL;
        }
    }

    bool allDone = true;
    for(int i = 0; i < runners.size(); i++)
    {
        if(runners[i] != NULL)
        {
            allDone = false;
            break;
        }
    }
    if(allDone)
    {
        ui->programButton->setEnabled(true);
        ui->runTestsButton->setEnabled(true);
        ui->runningLabel->setText("Ready.");
        runners.clear();
    }

    mutex.unlock();
}
