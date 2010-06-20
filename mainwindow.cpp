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

// colors from: http://tango.freedesktop.org/Tango_Icon_Theme_Guidelines
const QString MainWindow::testResultsHtml = QString("<html><head><style type=\"text/css\">"
                                                    "pre { margin-left: 20px; }\n"
                                                    "p.running { color: #729fcf; }\n"
                                                    "span.error { font-weight: bold; color: #ef2929; }\n"
                                                    "span.input { font-weight: bold; text-decoration: underline; }\n"
                                                    "pre.desired { color: #204a87; }\n"
                                                    "pre.goodresult { color: #4e9a06; }\n"
                                                    "pre.badresult { color: #ef2929; }\n"
                                                    "span.mismatch { color: #a40000; font-weight: bold; "
                                                    " background-color: #eeeeec; text-decoration: underline; }\n"
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

    connect(ui->previewButton, SIGNAL(clicked()), this, SLOT(previewTests()));
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
    testResults.clear();
    ui->testResults->clear();
}

void MainWindow::chooseProgram()
{
    QFileDialog d(this, "Choose program...");
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

void MainWindow::previewTests()
{
    testResults = testResultsHtml;
    QList<TestCase*> testCases = assignment->getTestCases();
    for(int i = 0; i < testCases.size(); i++)
    {
        testResults.replace("<!--REPLACE-->",
                            QString("<p>%1:<pre class=\"desired\">%2</pre></p><!--REPLACE-->")
                            .arg(testCases[i]->getName()).arg(testCases[i]->desiredResult()));
    }
    ui->testResults->setHtml(testResults);
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
        connect(r, SIGNAL(runnerFinished(Runner*)), this, SLOT(runnerFinished(Runner*)));
        connect(r, SIGNAL(runnerError(Runner*,QString)), this, SLOT(runnerError(Runner*,QString)));
        r->start();
    }
    if(runners.size() == 1)
        ui->runningLabel->setText(QString("Running 1 test..."));
    else
        ui->runningLabel->setText(QString("Running %1 tests...").arg(runners.size()));
    ui->testResults->setHtml(testResults);
    mutex.unlock();
}

void MainWindow::runnerFinished(Runner *r)
{
    mutex.lock();
    if(runners.contains(r))
    {
        TestCase *t = r->getTestCase();
        testResults.replace(QString("<p class=\"running\">Running %1...</p>").arg(t->getName()),
                            QString("<p>%1:<pre class=\"desired\">%2</pre><pre class=\"%3result\">%4</pre></p>")
                            .arg(t->getName()).arg(t->desiredResult())
                            .arg(r->getProgramResult().contains("mismatch") ? "bad" : "good")
                            .arg(r->getProgramResult()));
        ui->testResults->setHtml(testResults);
        removeRunner(r);
    }
    mutex.unlock();
}

void MainWindow::runnerError(Runner* r, QString e)
{
    mutex.lock();
    if(runners.contains(r))
    {
        TestCase *t = r->getTestCase();
        testResults.replace(QString("<p class=\"running\">Running %1...</p>").arg(t->getName()),
                            QString("<p class=\"running\">Running %1... <span class=\"error\">%2; partial result:</span>"
                                    "<pre class=\"badresult\">%3</pre></p>")
                            .arg(t->getName()).arg(e).arg(r->getProgramResult()));
        ui->testResults->setHtml(testResults);
        removeRunner(r);
    }
    mutex.unlock();
}

void MainWindow::removeRunner(Runner *r)
{
    r->wait();
    disconnect(r, SIGNAL(runnerFinished(Runner*)), this, SLOT(runnerFinished(Runner*)));
    disconnect(r, SIGNAL(runnerError(Runner*,QString)), this, SLOT(runnerError(Runner*,QString)));
    delete r;

    runners.removeAt(runners.indexOf(r));
    if(runners.isEmpty())
    {
        ui->programButton->setEnabled(true);
        ui->runTestsButton->setEnabled(true);
        ui->runningLabel->setText("Ready.");
    }
    else
    {
        if(runners.size() == 1)
            ui->runningLabel->setText(QString("Running 1 test..."));
        else
            ui->runningLabel->setText(QString("Running %1 tests...").arg(runners.size()));
    }
}
