#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "assignmentset.h"
#include "assignment.h"

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
        qDebug() << QString("program: %1").arg(program);
        QFile p(program);
        QFileInfo pf(p);
        if(!pf.exists() || !pf.isExecutable())
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
