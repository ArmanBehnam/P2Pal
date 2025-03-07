#include "peer.h"
#include <QRandomGenerator>

PeerManager::PeerManager(Network *network, QObject *parent)
    : QObject(parent)
    , network(network)
{
    // Set up timer to periodically check peers
    peerCheckTimer = new QTimer(this);
    connect(peerCheckTimer, &QTimer::timeout, this, &PeerManager::checkPeers);
    peerCheckTimer->start(30000); // Check every 30 seconds
}

PeerManager::~PeerManager() {
    if (peerCheckTimer) {
        peerCheckTimer->stop();
        delete peerCheckTimer;
    }
}

void PeerManager::addPeer(const QHostAddress &address, quint16 port) {
    // Don't add if already in list
    if (isPeerInList(address, port)) {
        return;
    }
    
    // Don't add ourselves
    if (address == QHostAddress::LocalHost && 
        port == network->getSocket()->localPort()) {
        return;
    }
    
    peers.append(qMakePair(address, port));
}

void PeerManager::removePeer(const QHostAddress &address, quint16 port) {
    for (int i = 0; i < peers.size(); ++i) {
        if (peers[i].first == address && peers[i].second == port) {
            peers.removeAt(i);
            return;
        }
    }
}

void PeerManager::discoverPeers() {
    // Try to discover peers on common ports
    for (quint16 port = 5000; port < 5010; ++port) {
        // Skip our own port
        if (port == network->getSocket()->localPort()) {
            continue;
        }
        
        // Send a simple ping message
        QVariantMap pingMessage;
        pingMessage["Type"] = "Ping";
        network->sendMessage("127.0.0.1", port, pingMessage);
    }
}

QList<QPair<QHostAddress, quint16>> PeerManager::getPeers() const {
    return peers;
}

QPair<QHostAddress, quint16> PeerManager::getRandomPeer() const {
    if (peers.isEmpty()) {
        return qMakePair(QHostAddress(), quint16(0));
    }
    
    int index = QRandomGenerator::global()->bounded(peers.size());
    return peers.at(index);
}

void PeerManager::checkPeers() {
    for (const auto &peer : peers) {
        // Send a ping to each peer
        QVariantMap pingMessage;
        pingMessage["Type"] = "Ping";
        network->sendMessage(peer.first.toString(), peer.second, pingMessage);
    }
}

bool PeerManager::isPeerInList(const QHostAddress &address, quint16 port) const {
    for (const auto &peer : peers) {
        if (peer.first == address && peer.second == port) {
            return true;
        }
    }
    return false;
}