#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "p2pal.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("P2Pal");
    app.setApplicationVersion("1.0");

    // Set up command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription("P2Pal - Peer-to-Peer Chat Application");
    parser.addHelpOption();
    parser.addVersionOption();

    // Add port option
    QCommandLineOption portOption(QStringList() << "p" << "port",
                                "Specify the UDP port to use (default is auto-select)",
                                "port");
    parser.addOption(portOption);

    // Process the command line arguments
    parser.process(app);

    // Get the port option if specified
    quint16 port = 0; // Default to auto-select
    if (parser.isSet(portOption)) {
        bool ok;
        port = parser.value(portOption).toUShort(&ok);
        if (!ok) {
            qCritical("Invalid port number specified.");
            return 1;
        }
    }

    // Create and initialize the P2Pal application
    P2Pal p2pal;
    if (!p2pal.initialize(port)) {
        qCritical("Failed to initialize P2Pal.");
        return 1;
    }

    // Show the GUI
    p2pal.getGUI()->show();

    // Run the application
    return app.exec();
}