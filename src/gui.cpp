#include "gui.h"
#include <QDateTime>
#include <QDebug>

GUI::GUI(QWidget *parent) : QMainWindow(parent)
{
    setupUI();
}

GUI::~GUI()
{
}

void GUI::setupUI()
{
    // Create central widget and main layout
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    mainLayout = new QVBoxLayout(centralWidget);
    
    // Create a splitter for resizable sections
    splitter = new QSplitter(Qt::Horizontal);
    
    // Create chat area
    chatArea = new QTextEdit();
    chatArea->setReadOnly(true);
    
    // Create node list section
    QWidget* nodeListContainer = new QWidget();
    QVBoxLayout* nodeListLayout = new QVBoxLayout(nodeListContainer);
    
    nodeListLabel = new QLabel("Available Nodes:");
    nodeListWidget = new QListWidget();
    
    nodeListLayout->addWidget(nodeListLabel);
    nodeListLayout->addWidget(nodeListWidget);
    
    // Add chat area and node list to splitter
    splitter->addWidget(chatArea);
    splitter->addWidget(nodeListContainer);
    
    // Set initial sizes for splitter
    QList<int> sizes;
    sizes << 700 << 300;  // 70% for chat, 30% for node list
    splitter->setSizes(sizes);
    
    // Create input area
    QHBoxLayout* inputLayout = new QHBoxLayout();
    messageInput = new QLineEdit();
    sendButton = new QPushButton("Send");
    
    inputLayout->addWidget(messageInput);
    inputLayout->addWidget(sendButton);
    
    // Add all components to main layout
    mainLayout->addWidget(splitter);
    mainLayout->addLayout(inputLayout);
    
    // Connect signals and slots
    connect(sendButton, &QPushButton::clicked, this, &GUI::onSendButtonClicked);
    connect(messageInput, &QLineEdit::returnPressed, this, &GUI::onSendButtonClicked);
    connect(nodeListWidget, &QListWidget::itemClicked, this, &GUI::onNodeSelected);
    
    // Set window properties
    resize(800, 600);
    setWindowTitle("P2Pal");
    
    // Display welcome message
    displaySystemMessage("Welcome to P2Pal! Connected and ready to communicate.");
}

void GUI::displayMessage(const QString& sender, const QString& message)
{
    QString timestamp = QDateTime::currentDateTime().toString("[hh:mm:ss]");
    QString formattedMessage = QString("%1 %2: %3").arg(timestamp).arg(sender).arg(message);
    chatArea->append(formattedMessage);
}

void GUI::displaySystemMessage(const QString& message)
{
    QString timestamp = QDateTime::currentDateTime().toString("[hh:mm:ss]");
    QString formattedMessage = QString("%1 *System*: %2").arg(timestamp).arg(message);
    chatArea->append(formattedMessage);
}

void GUI::updateNodeList(const QStringList& nodes)
{
    nodeListWidget->clear();
    for (const QString& node : nodes) {
        nodeListWidget->addItem(node);
    }
    
    // Update label to show count
    nodeListLabel->setText(QString("Available Nodes (%1):").arg(nodes.size()));
}

void GUI::onSendButtonClicked()
{
    QString message = messageInput->text().trimmed();
    if (!message.isEmpty()) {
        emit messageSent(message);
        messageInput->clear();
    }
}

void GUI::onNodeSelected(QListWidgetItem* item)
{
    if (!item) return;
    
    QString nodeId = item->text();
    bool ok;
    QString message = QInputDialog::getText(
        this, 
        "Private Message", 
        QString("Send private message to %1:").arg(nodeId),
        QLineEdit::Normal,
        "",
        &ok
    );
    
    if (ok && !message.isEmpty()) {
        emit privateMessageRequested(nodeId, message);
    }
}