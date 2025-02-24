#ifndef GUI_H
#define GUI_H

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QString>

class GUI : public QWidget {
    Q_OBJECT

public:
    GUI(QWidget *parent = nullptr);
    QString getInputText() const;
    void appendChatLog(const QString &text);

signals:
    void sendMessageSignal(const QString &message);

private:
    QTextEdit *chatLog;
    QLineEdit *inputField;
};

#endif // GUI_H