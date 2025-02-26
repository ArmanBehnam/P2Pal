#include <QtTest>
#include "../src/gui.h"

class TestGUI : public QObject {
    Q_OBJECT

private slots:
    void testTextInput();
};

void TestGUI::testTextInput() {
    GUI gui;
    
    // Test initial empty input
    QVERIFY(gui.getInputText().isEmpty());
    
    // You could extend this with more GUI tests if needed
}

QTEST_MAIN(TestGUI)
#include "test_gui.moc"