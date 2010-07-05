#ifndef ASSIGNMENTSET_H
#define ASSIGNMENTSET_H

#include <QList>

class Assignment;

class AssignmentSet
{
public:
    AssignmentSet();
    void addAssignment(Assignment *a);
    QList<Assignment*> getAssignments();

private:
    QList<Assignment*> assignments;
};

#endif // ASSIGNMENTSET_H
