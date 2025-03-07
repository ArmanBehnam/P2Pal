#ifndef PEER_H
#define PEER_H

#include <QObject>
#include <QHostAddress>
#include <QList>
#include <QPair>
#include <QSet>
#include <QTimer>
#include <QUdpSocket>
#include <QVariantMap>

#include "network.h"

class PeerManager : public QObject {
    Q_OBJECT

public:
    explicit PeerManager(Network *network, QObject *parent = nullptr);
    ~PeerManager();

    void addPeer(const QHostAddress &address, quint16 port);
    void removePeer(const QHostAddress &address, quint16 port);
    void discoverPeers();
    QList<QPair<QHostAddress, quint16>> getPeers() const;
    QPair<QHostAddress, quint16> getRandomPeer() const;

private slots:
    void checkPeers();

private:
    Network *network;
    QList<QPair<QHostAddress, quint16>> peers;
    QTimer *peerCheckTimer;

    bool isPeerInList(const QHostAddress &address, quint16 port) const;
};

#endif // PEER_H