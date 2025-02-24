# Programming Assignment 1
## Project Structure (Example - C++ with Qt6):

P2Pal/
├── src/
│   ├── main.cpp         // Entry point, GUI setup
│   ├── p2pal.h          // P2Pal class declaration
│   ├── p2pal.cpp        // P2Pal class implementation
│   ├── network.h        // Network communication logic
│   ├── network.cpp
│   ├── gossip.h         // Gossip protocol logic
│   ├── gossip.cpp
│   ├── message.h        // Message structure and serialization
│   ├── message.cpp
│   ├── peer.h           // Peer management
│   ├── peer.cpp
│   └── gui.h            // GUI related code
│   └── gui.cpp
├── tests/
│   ├── test_message.cpp // Unit tests for message handling
│   ├── test_gossip.cpp  // Unit tests for gossip protocol
│   ├── test_network.cpp // Unit tests for network functionality
│   └── test_gui.cpp     // Unit test for gui functionality
├── scripts/
│   ├── build.sh         // Build script
│   ├── run_instances.sh // Script to run multiple instances
│   ├── test.sh          // Test script
│   └── deploy.sh        // Deployment script (if needed)
├── docs/
│   ├── README.md        // Project overview, build instructions, usage
│   ├── design.md        // Detailed design documentation
│   └── testing.md       // Testing procedures and results
├── P2Pal.pro            // Qt project file (or CMakeLists.txt)
└── .gitignore