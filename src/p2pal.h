#ifndef P2PAL_H
#define P2PAL_H

#include <QObject>
#include <QUuid>
#include <QTimer>
#include <QMap>
#include <QVariantMap>
#include <QHostAddress>

#include "gui.h"
#include "network.h"
#include "message.h"
#include "gossip.h"
#include "peer.h"

class P2Pal : public QObject {
    Q_OBJECT

public:
    explicit P2Pal(QObject *parent = nullptr);
    ~P2Pal();

    bool initialize(quint16 port = 0);
    GUI* getGUI() const { return gui; }

public slots:
    void sendMessage(const QString &text);
    void processIncomingMessage();
    void addPeer(const QString &address);

private:
    GUI *gui;
    Network *network;
    Gossip *gossip;
    PeerManager *peerManager;

    QString nodeId;
    int currentSeqNum;

    void displayMessage(const Message &message);
};

#endif // P2PAL_H