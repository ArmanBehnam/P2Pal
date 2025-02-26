#include "p2pal.h"
#include <QDateTime>

P2Pal::P2Pal(QObject *parent)
    : QObject(parent)
    , gui(new GUI())
    , network(new Network())
    , currentSeqNum(0)
{
    // Generate unique node ID
    nodeId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    
    // Create gossip and peer manager instances
    gossip = new Gossip(network, nodeId);
    peerManager = new PeerManager(network);
    
    // Connect GUI signals
    connect(gui, &GUI::sendMessageSignal, this, &P2Pal::sendMessage);
    
    // Set window title with node ID
    gui->setWindowTitle(QString("P2Pal - Node %1").arg(nodeId.left(8)));
}

P2Pal::~P2Pal() {
    delete gui;
    delete network;
    delete gossip;
    delete peerManager;
}

bool P2Pal::initialize(quint16 port) {
    // Bind to specified port
    network->bindToPort(port);
    
    // Connect network signals for incoming messages
    connect(network->getSocket(), &QUdpSocket::readyRead, 
            this, &P2Pal::processIncomingMessage);
    
    // Initialize gossip protocol
    gossip->initialize();
    
    // Discover peers
    peerManager->discoverPeers();
    
    // Update window title with port info
    gui->setWindowTitle(QString("P2Pal - Node %1 (Port %2)")
                       .arg(nodeId.left(8))
                       .arg(network->getSocket()->localPort()));
    
    return true;
}

void P2Pal::sendMessage(const QString &text) {
    if (text.isEmpty()) {
        return;
    }
    
    // Increment sequence number
    currentSeqNum++;
    
    // Create message
    Message msg;
    msg.ChatText = text;
    msg.Origin = nodeId;
    msg.SequenceNumber = currentSeqNum;
    
    // Display our own message
    displayMessage(msg);
    
    // Send to gossip protocol for distribution
    gossip->spreadRumor(msg);
}

void P2Pal::processIncomingMessage() {
    QVariantMap receivedMap = network->receiveMessage();
    
    // Check if this is a rumor message
    if (receivedMap.contains("ChatText") && 
        receivedMap.contains("Origin") && 
        receivedMap.contains("SequenceNumber")) {
        
        Message msg = Message::fromVariantMap(receivedMap);
        
        // Process via gossip protocol
        if (gossip->processIncomingRumor(msg)) {
            // Display message if it's new
            displayMessage(msg);
        }
    }
    // Process status message for anti-entropy
    else if (receivedMap.contains("Want")) {
        gossip->processStatusMessage(receivedMap);
    }
}

void P2Pal::addPeer(const QString &address) {
    QStringList parts = address.split(':');
    if (parts.size() == 2) {
        bool ok;
        quint16 port = parts[1].toUShort(&ok);
        if (ok) {
            peerManager->addPeer(QHostAddress(parts[0]), port);
        }
    }
}

void P2Pal::displayMessage(const Message &message) {
    QString shortOrigin = message.Origin.left(8);
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    
    QString formattedMsg = QString("[%1] %2 (#%3): %4")
                              .arg(timestamp)
                              .arg(shortOrigin)
                              .arg(message.SequenceNumber)
                              .arg(message.ChatText);
    
    gui->appendChatLog(formattedMsg);
}