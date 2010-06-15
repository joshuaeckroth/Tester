#include "testsreader.h"
#include "assignmentset.h"
#include "assignment.h"
#include "testcase.h"
#include "inputoutput.h"

TestsReader::TestsReader(AssignmentSet *_assignmentSet)
     : assignmentSet(_assignmentSet)
{ }

bool TestsReader::read(QIODevice *device)
{
    xml.setDevice(device);

    if(xml.readNextStartElement())
    {
        if (xml.name() == "assignmentSet")
            readAllAssignments();
        else
            xml.raiseError("The file is not a valid test cases file.");
    }

    return !xml.error();
}

QString TestsReader::errorString() const
{
    return QString("%1\nLine %2, column %3")
            .arg(xml.errorString())
            .arg(xml.lineNumber())
            .arg(xml.columnNumber());
}

void TestsReader::readAllAssignments()
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "assignmentSet");

    while(xml.readNextStartElement())
    {
        if(xml.name() == "assignment")
        {
            Assignment *a = new Assignment;
            readOneAssignment(a);
            assignmentSet->addAssignment(a);
        }
        else
            xml.skipCurrentElement();
    }
}

void TestsReader::readOneAssignment(Assignment *a)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "assignment");

    a->setId(xml.attributes().value("id").toString());
    a->setName(xml.attributes().value("name").toString());

    while(xml.readNextStartElement())
    {
        if(xml.name() == "test")
        {
            TestCase *t = new TestCase;
            readTestCase(t);
            a->addTestCase(t);
        }
        else
            xml.skipCurrentElement();
    }
}

void TestsReader::readTestCase(TestCase *t)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "test");

    t->setName(xml.attributes().value("name").toString());

    while(xml.readNextStartElement())
    {
        if(xml.name() == "io")
        {
            InputOutput *io = new InputOutput;
            readInputOutput(io);
            t->addInputOutput(io);
        }
        else
            xml.skipCurrentElement();
    }
}

void TestsReader::readInputOutput(InputOutput *io)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "io");

    while(xml.readNextStartElement())
    {
        if(xml.name() == "prompt")
            io->setPrompt(xml.readElementText());
        else if(xml.name() == "input")
            io->setInput(xml.readElementText());
        else if(xml.name() == "output")
            io->setOutput(xml.readElementText());
        else
            xml.skipCurrentElement();
    }
}

