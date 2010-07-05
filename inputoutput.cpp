#include "inputoutput.h"

InputOutput::InputOutput()
{ }

void InputOutput::setPrompt(QString p)
{
    prompt = p;
}

QString InputOutput::getPrompt() const
{
    return prompt;
}

void InputOutput::setInput(QString i)
{
    input = i;
}

QString InputOutput::getInput() const
{
    return input;
}

void InputOutput::setOutput(QString o)
{
    output = o;
}

QString InputOutput::getOutput() const
{
    return output;
}
