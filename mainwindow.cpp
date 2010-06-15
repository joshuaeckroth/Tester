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

    QList<Assignment*> assignments = as->getAssignments();
    for(int i = 0; i < assignments.size(); i++)
    {
        ui->assignmentComboBox->addItem(assignments[i]->getName());
    }
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
