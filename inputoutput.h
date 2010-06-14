#ifndef INPUTOUTPUT_H
#define INPUTOUTPUT_H

#include <QString>

class InputOutput
{
public:
    InputOutput();
    void setPrompt(QString p);
    void setInput(QString i);
    void setOutput(QString o);

private:
    QString prompt, input, output;
};

#endif // INPUTOUTPUT_H
