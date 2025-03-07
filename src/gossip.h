#ifndef GOSSIP_H
#define GOSSIP_H

#include <QObject>
#include <QVariantMap>
#include <QPair>
#include <QSet>
#include <QMap>
#include <QTimer>
#include <QRandomGenerator>
#include <QHostAddress>
#include <QList>
#include <QUdpSocket>
#include "message.h"

struct RouteEntry {
    QString ipAddress;
    quint16 port;
    quint32 seqNo;
    bool isDirect;  // Flag for direct connections (for NAT traversal)
    
    RouteEntry() : seqNo(0), isDirect(false) {}
    RouteEntry(const QString& ip, quint16 p, quint32 seq, bool direct = false)
        : ipAddress(ip), port(p), seqNo(seq), isDirect(direct) {}
};

class Gossip : public QObject
{
    Q_OBJECT
public:
    explicit Gossip(QObject *parent = nullptr);
    ~Gossip();

    void setNodeId(const QString& nodeId);
    QString getNodeId() const;
    void addNeighbor(const QString& ipAddress, quint16 port);
    void removeNeighbor(const QString& ipAddress, quint16 port);
    void handleIncomingMessage(const QVariantMap& message, const QString& senderIP, quint16 senderPort);
    void sendChatMessage(const QString& text);
    void sendPrivateMessage(const QVariantMap& message);

signals:
    void sendMessage(const QString& ipAddress, quint16 port, const QVariantMap& message);
    void messageReceived(const QString& sender, const QString& message);
    void availableNodesChanged(const QStringList& nodes);

private:
    QString myNodeId;
    quint32 currentSeqNo;
    QSet<QPair<QString, quint16>> neighbors;
    QMap<QString, QSet<quint32>> receivedMessages; // Origin -> set of sequence numbers
    QMap<QString, RouteEntry> routingTable;  // NodeID -> RouteEntry
    QTimer* routeRumorTimer;
    
    void processMessage(const QVariantMap& message, const QString& senderIP, quint16 senderPort);
    void processRouteRumor(const QVariantMap& message, const QString& senderIP, quint16 senderPort);
    void updateRoutingTable(const QString& origin, const QString& senderIP, quint16 senderPort, quint32 seqNo, bool isDirect = false);
    void sendRouteRumor();
    void forwardPrivateMessage(const QVariantMap& message);
    bool isBetterRoute(const RouteEntry& oldRoute, const RouteEntry& newRoute);
};

#endif // GOSSIP_H