#include "p2pal.h"
#include <QDebug>

P2Pal::P2Pal(QObject *parent) : QObject(parent), gossip(nullptr), network(nullptr), gui(nullptr), noForwardMode(false)
{
}

P2Pal::~P2Pal()
{
    if (gossip) delete gossip;
    if (network) delete network;
    if (gui) delete gui;
}

void P2Pal::initialize(quint16 port, const QString& connectTo, bool noForward)
{
    this->noForwardMode = noForward;
    
    // Initialize network
    network = new Network(this);
    bool success = network->initialize(port);
    if (!success) {
        qDebug() << "Failed to initialize network on port" << port;
        return;
    }
    
    // Initialize gossip
    gossip = new Gossip(this);
    
    // Initialize GUI
    gui = new GUI();
    gui->setWindowTitle(QString("P2Pal - %1:%2").arg(network->getLocalAddress().toString()).arg(network->getPort()));
    gui->show();
    
    // Set node ID to the local address and port
    QString nodeId = QString("%1:%2")
                        .arg(network->getLocalAddress().toString())
                        .arg(network->getPort());
    gossip->setNodeId(nodeId);
    
    // Connect signals and slots
    connect(gui, &GUI::messageSent, this, &P2Pal::onMessageSent);
    connect(gui, &GUI::privateMessageRequested, this, &P2Pal::onPrivateMessageRequested);
    connect(gossip, &Gossip::sendMessage, network, &Network::sendMessage);
    connect(gossip, &Gossip::messageReceived, gui, &GUI::displayMessage);
    connect(gossip, &Gossip::availableNodesChanged, this, &P2Pal::onAvailableNodesChanged);
    connect(network, &Network::messageReceived, this, &P2Pal::onReceivedMessage);
    connect(network, &Network::connectionAccepted, this, &P2Pal::onConnectionAccepted);
    connect(network, &Network::connectionRequested, this, &P2Pal::onConnectionRequested);
    
    // Connect to another node if specified
    if (!connectTo.isEmpty()) {
        QStringList parts = connectTo.split(":");
        QString host;
        quint16 connectPort = 0;
        
        if (parts.size() == 1) {
            // Assume it's only a port, connect to localhost
            host = "127.0.0.1";
            connectPort = parts[0].toUShort();
        } else if (parts.size() == 2) {
            // IP:port format
            host = parts[0];
            connectPort = parts[1].toUShort();
        }
        
        if (!host.isEmpty() && connectPort > 0) {
            network->connectToHost(host, connectPort);
        }
    }
}

quint16 P2Pal::getPort() const
{
    return network ? network->getPort() : 0;
}

void P2Pal::onMessageSent(const QString& message)
{
    gossip->sendChatMessage(message);
}

void P2Pal::onReceivedMessage(const QVariantMap& message, const QString& senderIP, quint16 senderPort)
{
    // If in noForward mode, only forward route rumors, not chat messages
    if (noForwardMode && message.contains("ChatText") && !message.contains("Dest")) {
        // Still display the message if it's for us
        QString origin = message["Origin"].toString();
        QString chatText = message["ChatText"].toString();
        gui->displayMessage(origin, chatText);
        return;  // Do not forward regular chat messages
    }
    
    // Process the message normally
    gossip->handleIncomingMessage(message, senderIP, senderPort);
}

void P2Pal::onConnectionAccepted(const QString& ipAddress, quint16 port)
{
    gossip->addNeighbor(ipAddress, port);
    gui->displaySystemMessage(QString("Connected to %1:%2").arg(ipAddress).arg(port));
}

void P2Pal::onConnectionRequested(const QString& ipAddress, quint16 port)
{
    gossip->addNeighbor(ipAddress, port);
    gui->displaySystemMessage(QString("Accepted connection from %1:%2").arg(ipAddress).arg(port));
}

void P2Pal::onAvailableNodesChanged(const QStringList& nodes)
{
    // Update GUI with available nodes
    gui->updateNodeList(nodes);
}

void P2Pal::onPrivateMessageRequested(const QString& destNodeId, const QString& message)
{
    sendPrivateMessage(destNodeId, message);
}

void P2Pal::sendPrivateMessage(const QString& destNodeId, const QString& messageText)
{
    QVariantMap msg;
    msg["Dest"] = destNodeId;
    msg["Origin"] = gossip->getNodeId();
    msg["ChatText"] = messageText;
    msg["HopLimit"] = 10;  // Set an appropriate hop limit
    
    // Send private message
    gossip->sendPrivateMessage(msg);
    
    // Display outgoing message in our own GUI
    gui->displayMessage(QString("%1 (private to %2)").arg(gossip->getNodeId()).arg(destNodeId), messageText);
}