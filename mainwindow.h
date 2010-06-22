#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMutex>

class AssignmentSet;
class Assignment;
class Runner;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(AssignmentSet *as, QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private slots:
    void assignmentChanged(int i);
    void chooseProgram();
    void previewTests();
    void runTests();
    void runnerFinished(Runner*);
    void runnerError(Runner*, QString);

private:
    Ui::MainWindow *ui;
    QString program;
    QString lastDir;
    bool programValid;
    AssignmentSet *assignmentSet;
    Assignment *assignment;
    QMutex mutex;
    QList<Runner*> runners;
    QString testResults;
    int numPassed;

    static const QString testResultsHtml;

    void removeRunner(Runner* r);
};

#endif // MAINWINDOW_H
