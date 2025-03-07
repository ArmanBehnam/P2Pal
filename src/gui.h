#ifndef GUI_H
#define GUI_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QLabel>
#include <QSplitter>
#include <QInputDialog>
#include <QObject>
#include <QHostAddress>
#include <QList>
#include <QPair>
#include <QSet>
#include <QTimer>
#include <QUdpSocket>
#include <QVariantMap>

class GUI : public QMainWindow
{
    Q_OBJECT
public:
    explicit GUI(QWidget *parent = nullptr);
    ~GUI();

    void displayMessage(const QString& sender, const QString& message);
    void displaySystemMessage(const QString& message);
    void updateNodeList(const QStringList& nodes);

signals:
    void messageSent(const QString& message);
    void privateMessageRequested(const QString& nodeId, const QString& message);

private slots:
    void onSendButtonClicked();
    void onNodeSelected(QListWidgetItem* item);

private:
    QWidget* centralWidget;
    QVBoxLayout* mainLayout;
    QTextEdit* chatArea;
    QLineEdit* messageInput;
    QPushButton* sendButton;
    QListWidget* nodeListWidget;
    QLabel* nodeListLabel;
    QSplitter* splitter;

    void setupUI();
};

#endif // GUI_H