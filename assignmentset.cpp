#include "assignmentset.h"
#include "assignment.h"

AssignmentSet::AssignmentSet()
{
}

QList<Assignment*> AssignmentSet::getAssignments()
{
    return assignments;
}

void AssignmentSet::addAssignment(Assignment *a)
{
    assignments << a;
}

