#include <QApplication>
#include "ui/main_win.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    QIcon appIcon("icon.png");
    app.setWindowIcon(appIcon);
    
    MainWindow window;
    window.setWindowTitle("ComSock");
    window.resize(800, 600);
    window.show();
    
    return app.exec();
} 