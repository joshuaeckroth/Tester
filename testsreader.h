#ifndef TESTSREADER_H
#define TESTSREADER_H

#include <QIODevice>
#include <QString>
#include <QXmlStreamReader>

class AssignmentSet;
class Assignment;
class TestCase;
class InputOutput;

class TestsReader
{
public:
    TestsReader(AssignmentSet *_assignmentSet);
    bool read(QIODevice *device);
    QString errorString() const;

private:
    void readAllAssignments();
    void readOneAssignment(Assignment *a);
    void readTestCase(TestCase *t);
    void readInputOutput(InputOutput *io);
    bool readNextStartElement();
    void skipCurrentElement();

    QXmlStreamReader xml;
    AssignmentSet *assignmentSet;
};

#endif // TESTSREADER_H
