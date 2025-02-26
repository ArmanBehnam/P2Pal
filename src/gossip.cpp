#include "gossip.h"
#include <QRandomGenerator>

Gossip::Gossip(Network *network, const QString &nodeId, QObject *parent)
    : QObject(parent)
    , network(network)
    , nodeId(nodeId)
{
    // Initialize vector clock for our own node
    vectorClock[nodeId] = 0;
    
    // Create anti-entropy timer (runs every 5 seconds)
    antiEntropyTimer = new QTimer(this);
    connect(antiEntropyTimer, &QTimer::timeout, this, &Gossip::sendAntiEntropyStatus);
}

Gossip::~Gossip() {
    // Clean up any active timers
    for (auto &originMap : rumorStatus) {
        for (auto &status : originMap) {
            if (status.timer) {
                status.timer->stop();
                delete status.timer;
            }
        }
    }
    
    if (antiEntropyTimer) {
        antiEntropyTimer->stop();
        delete antiEntropyTimer;
    }
}

void Gossip::initialize() {
    // Start anti-entropy timer
    antiEntropyTimer->start(5000); // 5 seconds
}

void Gossip::spreadRumor(const Message &message) {
    // Store the message
    QString origin = message.Origin;
    int seqNum = message.SequenceNumber;
    
    if (!messages.contains(origin)) {
        messages[origin] = QMap<int, Message>();
    }
    messages[origin][seqNum] = message;
    
    // Update vector clock
    vectorClock[origin] = std::max(vectorClock[origin], seqNum);
    
    // Set up rumor tracking
    if (!rumorStatus.contains(origin)) {
        rumorStatus[origin] = QMap<int, RumorStatus>();
    }
    
    RumorStatus status;
    status.sequenceNumber = seqNum;
    status.acknowledged = false;
    status.timer = new QTimer(this);
    
    connect(status.timer, &QTimer::timeout, this, &Gossip::resendRumor);
    status.timer->start(2000); // 2 second timer
    
    rumorStatus[origin][seqNum] = status;
    
    // Send to a random peer
    QPair<QHostAddress, quint16> peer = getRandomPeer();
    if (!peer.first.isNull()) {
        sendMessageToPeer(message, peer.first, peer.second);
    }
}

bool Gossip::processIncomingRumor(const Message &message) {
    QString origin = message.Origin;
    int seqNum = message.SequenceNumber;
    
    // Check if we've already seen this message
    if (messages.contains(origin) && messages[origin].contains(seqNum)) {
        // Already processed this message
        return false;
    }
    
    // Store the message
    if (!messages.contains(origin)) {
        messages[origin] = QMap<int, Message>();
    }
    messages[origin][seqNum] = message;
    
    // Update vector clock
    vectorClock[origin] = std::max(vectorClock[origin], seqNum);
    
    // Propagate to another random peer (with 50% probability)
    if (QRandomGenerator::global()->bounded(100) < 50) {
        QPair<QHostAddress, quint16> peer = getRandomPeer();
        if (!peer.first.isNull()) {
            sendMessageToPeer(message, peer.first, peer.second);
        }
    }
    
    return true;
}

void Gossip::processStatusMessage(const QVariantMap &statusMap) {
    QVariantMap want = statusMap["Want"].toMap();
    
    // Check if peer needs messages we have
    for (auto it = vectorClock.constBegin(); it != vectorClock.constEnd(); ++it) {
        QString origin = it.key();
        int ourMaxSeq = it.value();
        
        int peerMaxSeq = want.contains(origin) ? want[origin].toInt() : 0;
        
        if (ourMaxSeq > peerMaxSeq && messages.contains(origin)) {
            // We have messages the peer doesn't have
            // Send the next one in sequence
            int nextSeq = peerMaxSeq + 1;
            if (messages[origin].contains(nextSeq)) {
                // Get the peer address from network (assuming sender info is available)
                QHostAddress peerAddress = QHostAddress::LocalHost; // Placeholder
                quint16 peerPort = 0; // Placeholder
                
                sendMessageToPeer(messages[origin][nextSeq], peerAddress, peerPort);
                return;
            }
        }
    }
    
    // Check if peer has messages we need
    bool needMessages = false;
    for (auto it = want.constBegin(); it != want.constEnd(); ++it) {
        QString origin = it.key();
        int peerMaxSeq = it.value().toInt();
        
        int ourMaxSeq = vectorClock.contains(origin) ? vectorClock[origin] : 0;
        
        if (peerMaxSeq > ourMaxSeq) {
            needMessages = true;
            break;
        }
    }
    
    if (needMessages) {
        // Send our status to request missing messages
        QHostAddress peerAddress = QHostAddress::LocalHost; // Placeholder
        quint16 peerPort = 0; // Placeholder
        
        sendStatusToPeer(peerAddress, peerPort);
    }
    
    // Mark rumors as acknowledged
    for (auto originIt = rumorStatus.begin(); originIt != rumorStatus.end(); ++originIt) {
        QString origin = originIt.key();
        for (auto statusIt = originIt->begin(); statusIt != originIt->end(); ++statusIt) {
            int seqNum = statusIt.key();
            
            if (want.contains(origin) && want[origin].toInt() >= seqNum) {
                statusIt->acknowledged = true;
                if (statusIt->timer) {
                    statusIt->timer->stop();
                    delete statusIt->timer;
                    statusIt->timer = nullptr;
                }
            }
        }
    }
}

QVariantMap Gossip::createStatusMessage() const {
    QVariantMap status;
    status["Type"] = "Status";
    
    QVariantMap want;
    for (auto it = vectorClock.constBegin(); it != vectorClock.constEnd(); ++it) {
        want[it.key()] = it.value();
    }
    
    status["Want"] = want;
    return status;
}

void Gossip::resendRumor() {
    // Find an unacknowledged message
    for (auto originIt = rumorStatus.begin(); originIt != rumorStatus.end(); ++originIt) {
        QString origin = originIt.key();
        for (auto statusIt = originIt->begin(); statusIt != originIt->end(); ++statusIt) {
            int seqNum = statusIt.key();
            if (!statusIt->acknowledged && messages[origin].contains(seqNum)) {
                // Get a random peer
                QPair<QHostAddress, quint16> peer = getRandomPeer();
                if (!peer.first.isNull()) {
                    sendMessageToPeer(messages[origin][seqNum], peer.first, peer.second);
                }
                return;
            }
        }
    }
}

void Gossip::sendAntiEntropyStatus() {
    // Send status message to a random peer
    QPair<QHostAddress, quint16> peer = getRandomPeer();
    if (!peer.first.isNull()) {
        sendStatusToPeer(peer.first, peer.second);
    }
}

void Gossip::sendMessageToPeer(const Message &msg, const QHostAddress &address, quint16 port) {
    network->sendMessage(msg.toVariantMap(), address, port);
}

void Gossip::sendStatusToPeer(const QHostAddress &address, quint16 port) {
    network->sendMessage(createStatusMessage(), address, port);
}

QPair<QHostAddress, quint16> Gossip::getRandomPeer() const {
    // This should connect to the peer manager
    // For now, return a placeholder
    return qMakePair(QHostAddress("127.0.0.1"), quint16(5000));
}