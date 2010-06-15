#ifndef INPUTOUTPUT_H
#define INPUTOUTPUT_H

#include <QString>

class InputOutput
{
public:
    InputOutput();
    void setPrompt(QString p);
    QString getPrompt() const;
    void setInput(QString i);
    QString getInput() const;
    void setOutput(QString o);
    QString getOutput() const;

private:
    QString prompt, input, output;
};

#endif // INPUTOUTPUT_H
