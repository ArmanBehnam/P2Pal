# Programming Assignment 1
## Project Structure (Example - C++ with Qt6)
```bash
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
```

## Implementation Highlights (C++ with Qt6)

* **GUI (gui.h/gui.cpp):**
    * Use `QTextEdit` for the chat log and `QLineEdit` (or `QPlainTextEdit`) for the input.
    * Connect signals and slots for message input and display.
    * Set focus to the input field on startup.
* **Network (network.h/network.cpp):**
    * Use `QUdpSocket` for sending and receiving messages.
    * Implement functions for sending and receiving `QVariantMap` messages.
    * Handle local port discovery.
* **Message (message.h/message.cpp):**
    * Define a `Message` class or struct with `ChatText`, `Origin`, and `Sequence number` fields.
    * Implement serialization and deserialization using `QVariantMap`.
* **Gossip (gossip.h/gossip.cpp):**
    * Implement Rumor Mongering:
        * Use `QTimer` for resending messages.
        * Maintain a map of messages and their status (sent, acknowledged).
    * Implement Anti-Entropy:
        * Use a map to store vector clock information (seen messages from each origin).
        * Implement logic to compare vector clocks and request missing messages.
* **Peer (peer.h/peer.cpp):**
    * Implement peer discovery, and peer list management.

## Documentation (docs/design.md, docs/testing.md)

* **design.md:**
    * Detailed description of the gossip protocol implementation.
    * Explanation of the vector clock mechanism.
    * Network communication details.
    * GUI design.
* **testing.md:**
    * Description of test cases.
    * Testing procedures.
    * Testing results.

## Build Instructions (docs/README.md and scripts/build.sh)

* **Qt Project File (P2Pal.pro):**
    * Configure project settings, including dependencies.
* **build.sh:**

```bash
#!/bin/bash
qmake P2Pal.pro
make
```

* Or using CMake:

```bash
#!/bin/bash
mkdir build
cd build
cmake ..
make
```

## Test Cases (tests/)

* **Unit Tests (test_message.cpp, test_gossip.cpp, test_network.cpp, test_gui.cpp):**
    * Use a testing framework (e.g., Qt Test).
    * Test message serialization/deserialization, gossip protocol logic, network communication, and GUI functionality.
    * Test corner cases, and error handling.
* **Integration Tests (run_instances.sh, test.sh):**
    * Verify message propagation between multiple instances.
    * Test peer discovery and message ordering.

## Automation Scripts (scripts/)

* **run_instances.sh:**

```bash
#!/bin/bash
./P2Pal &
./P2Pal &
./P2Pal &
./P2Pal &
```

* **test.sh:**

```bash
#!/bin/bash
./P2Pal_tests # or ctest if using cmake
./scripts/run_instances.sh
sleep 5 # Allow instances to connect
# Perform manual verification of message propagation
```

* **deploy.sh:**
    * If you have a deployment environment, create a script to copy the executable and dependencies.

**Important Considerations:**

* **Error Handling:** Implement robust error handling for network operations, message processing, and GUI interactions.
* **Concurrency:** If you use threads, ensure proper synchronization to avoid race conditions.
* **Security:** For a production-ready application, consider security measures (e.g., encryption).
* **Scalability:** For larger networks, optimize the gossip protocol and peer discovery mechanisms.
* **User Experience:** Design a user-friendly GUI with clear feedback and error messages.