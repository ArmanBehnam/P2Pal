#include <QApplication>
#include <QCommandLineParser>
#include "p2pal.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("P2Pal");
    app.setApplicationVersion("1.0");
    
    // Parse command line arguments
    QCommandLineParser parser;
    parser.setApplicationDescription("P2Pal - P2P Communication Application");
    parser.addHelpOption();
    parser.addVersionOption();
    
    // Add port option
    QCommandLineOption portOption(QStringList() << "p" << "port", 
                                "Port to listen on (default: random available port)", 
                                "port", "0");
    parser.addOption(portOption);
    
    // Add connect option
    QCommandLineOption connectOption(QStringList() << "c" << "connect", 
                                   "Connect to host:port or just port (for localhost)", 
                                   "address");
    parser.addOption(connectOption);
    
    // Add noforward option
    QCommandLineOption noForwardOption(QStringList() << "n" << "noforward", 
                                     "Do not forward chat messages (rendezvous mode)");
    parser.addOption(noForwardOption);
    
    parser.process(app);
    
    // Get port
    quint16 port = parser.value(portOption).toUShort();
    
    // Get connect address
    QString connectTo = parser.value(connectOption);
    
    // Get noforward flag
    bool noForward = parser.isSet(noForwardOption);
    
    // Initialize P2Pal
    P2Pal p2pal;
    p2pal.initialize(port, connectTo, noForward);
    
    return app.exec();
}