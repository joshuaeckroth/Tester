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

MainWindow::MainWindow(AssignmentSet *as, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->middleLayout->setStretchFactor(ui->testCaseTree, 2);
    ui->middleLayout->setStretchFactor(ui->testResults, 4);

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
    QString testResultsPlaceholders;
    for(int i = 0; i < testCases.size(); i++)
    {
        testResultsPlaceholders += QString("Running test case %1...").arg(testCases[i]->getName());
        Runner *r = new Runner(QString(program), testCases[i]);
        runners << r;
        connect(r, SIGNAL(finished()), this, SLOT(runnerFinished()));
        r->start();
    }
    ui->runningLabel->setText(QString("Running %1 tests...").arg(testCases.size()));
    ui->testResults->setHtml(testResultsPlaceholders);
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
            QString testResults = ui->testResults->toHtml();
            testResults.replace(QString("Running test case %1...").arg(t->getName()),
                                QString("Test case %1:<br/>%2<br/>%3")
                                .arg(t->getName()).arg(runners[i]->getTestCaseResult()).arg(runners[i]->getProgramResult()));
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
