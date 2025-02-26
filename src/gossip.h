#ifndef GOSSIP_H
#define GOSSIP_H

#include <QObject>
#include <QTimer>
#include <QMap>
#include <QVariantMap>
#include <QSet>

#include "network.h"
#include "message.h"

// Structure to track rumor status
struct RumorStatus {
    int sequenceNumber;
    bool acknowledged;
    QTimer* timer;
};

class Gossip : public QObject {
    Q_OBJECT

public:
    explicit Gossip(Network *network, const QString &nodeId, QObject *parent = nullptr);
    ~Gossip();

    void initialize();
    void spreadRumor(const Message &message);
    bool processIncomingRumor(const Message &message);
    void processStatusMessage(const QVariantMap &statusMap);
    QVariantMap createStatusMessage() const;

private slots:
    void resendRumor();
    void sendAntiEntropyStatus();

private:
    Network *network;
    QString nodeId;
    
    // Storage for all received messages
    QMap<QString, QMap<int, Message>> messages; // origin -> (sequence -> message)
    
    // Track which messages have been acknowledged
    QMap<QString, QMap<int, RumorStatus>> rumorStatus; // origin -> (sequence -> status)
    
    // Vector clock to track what we've seen from each origin
    QMap<QString, int> vectorClock; // origin -> highest sequence seen
    
    // Timer for anti-entropy
    QTimer *antiEntropyTimer;
    
    // Helper methods
    void sendMessageToPeer(const Message &msg, const QHostAddress &address, quint16 port);
    void sendStatusToPeer(const QHostAddress &address, quint16 port);
    QPair<QHostAddress, quint16> getRandomPeer() const;
};

#endif // GOSSIP_H