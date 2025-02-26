#include "gui.h"

GUI::GUI(QWidget *parent) : QWidget(parent) {
    chatLog = new QTextEdit(this);
    chatLog->setReadOnly(true);
    inputField = new QLineEdit(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(chatLog);
    layout->addWidget(inputField);

    setLayout(layout);
    inputField->setFocus();

    connect(inputField, &QLineEdit::returnPressed, [this]() {
        emit sendMessageSignal(inputField->text());
        inputField->clear();
    });
}

QString GUI::getInputText() const {
    return inputField->text();
}

void GUI::appendChatLog(const QString &text) {
    chatLog->append(text);
}