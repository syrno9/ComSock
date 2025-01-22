#include "main_win.h"
#include "dialogs/connect.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QMessageBox>
#include <QApplication>
#include <QDebug>
#include <QFontDatabase>
#include <QCursor>
#include <QMouseEvent>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("ComSock");
    resize(800, 600);
    
    // Style the buttons
    QString buttonStyle = R"(
        QPushButton {
            border: none;
            color: #f4f4f4;
            background: transparent;
            padding: 5px 10px;
            font-size: 14px;
        }
        QPushButton:hover {
            background: #404040;
        }
        QPushButton#closeButton:hover {
            background: #e81123;
        }
    )";
    
    QPixmap cursorPixmap("src/ui/cursor.png");
    QCursor customCursor(cursorPixmap);
    qApp->setOverrideCursor(customCursor);
    qApp->changeOverrideCursor(customCursor);
    int fontId = QFontDatabase::addApplicationFont("TerminusTTF-4.49.3.ttf");
    if (fontId != -1) {
        QString family = QFontDatabase::applicationFontFamilies(fontId).at(0);
        QFont terminusFont(family, 10);
        qApp->setFont(terminusFont);
    }

    // Set dark theme
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(45, 45, 45));
    darkPalette.setColor(QPalette::WindowText, QColor(220, 220, 220));
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(35, 35, 35));
    darkPalette.setColor(QPalette::Text, QColor(220, 220, 220));
    darkPalette.setColor(QPalette::Button, QColor(45, 45, 45));
    darkPalette.setColor(QPalette::ButtonText, QColor(220, 220, 220));
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, QColor(220, 220, 220));
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(128, 128, 128));
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(128, 128, 128));
    
    qApp->setPalette(darkPalette);

    // Initialize all pointers first
    client = new Client(this);
    channelList = new ChannelList(this);
    userList = new UserList(this);
    channelTabs = new QTabWidget(this);
    messageInput = new QLineEdit(this);
    
    // Set fixed widths for side panels
    channelList->setFixedWidth(100);
    userList->setFixedWidth(100);
    
    // Additional styling for widgets
    QString styleSheet = R"(

        * {
            font-size: 14px;
            background-color: #2b2b2b;
            color: #f4f4f4;
        }
        QMenuBar {
            background: #3c3c3c;
            border: 1px solid #555555;
        }
        QTabWidget::pane { 
            border: 1px solid #555555;
        }
        QTabBar::tab {
            background: #3c3c3c;
            color: #f4f4f4;
            padding: 5px;
            border: 1px solid #555555;
        }
        QTabBar::tab:selected {
            background: #555555;
        }
        QLineEdit {
            border: 1px solid #555555;
            padding: 2px;
            background: #3c3c3c;
            color: #f4f4f4;
        }
        QListWidget {
            border: 1px solid #555555;
            background: #3c3c3c;
            color: #f4f4f4;
        }
        QTextEdit {
            border: 1px solid #555555;
            background: #3c3c3c;
            color: #f4f4f4;
        }
        QSplitter {
            background: #3c3c3c;
        }
    )";
    setStyleSheet(styleSheet);
    
    // Setup UI
    setupMenuBar();
    setupLayout();
    
    // Connect signals
    connect(client, &Client::messageReceived, this, &MainWindow::handleMessageReceived);
    connect(client, &Client::userJoined, this, &MainWindow::handleUserJoined);
    connect(client, &Client::userLeft, this, &MainWindow::handleUserLeft);
    connect(messageInput, &QLineEdit::returnPressed, this, &MainWindow::sendMessage);
    connect(channelTabs, &QTabWidget::currentChanged, this, &MainWindow::handleTabChanged);
    
    // Show connect dialog on startup
    QTimer::singleShot(0, this, &MainWindow::showConnectDialog);
}

void MainWindow::setupMenuBar() {
    auto menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    
    auto fileMenu = menuBar->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Connect"), this, &MainWindow::showConnectDialog);
    fileMenu->addAction(tr("&Disconnect"), this, &MainWindow::handleDisconnect);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("E&xit"), qApp, &QApplication::quit);
    
    auto helpMenu = menuBar->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About"), this, &MainWindow::about);
}

void MainWindow::setupLayout() {
    auto mainWidget = new QWidget(this);
    setCentralWidget(mainWidget);
    
    auto mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Add the splitter directly to the main layout
    auto splitter = new QSplitter(Qt::Horizontal);
    
    // Left side - Channel list
    splitter->addWidget(channelList);
    
    // Middle - Chat area
    auto chatWidget = new QWidget;
    auto chatLayout = new QVBoxLayout(chatWidget);
    chatLayout->addWidget(channelTabs);
    chatLayout->addWidget(messageInput);
    splitter->addWidget(chatWidget);
    
    // Right side - User list
    splitter->addWidget(userList);
    
    // Set splitter sizes
    QList<int> sizes;
    sizes << 100 << width() - 200 << 100;  // Fixed widths for side panels
    splitter->setSizes(sizes);
    
    mainLayout->addWidget(splitter);
    
    // Connect the channel list click signal to the slot
    connect(channelList, &ChannelList::channelClicked, this, &MainWindow::switchToChannel);
    
    channelTabs->setMovable(true);
    channelTabs->setTabsClosable(true);  
    channelTabs->tabBar()->setTabButton(0, QTabBar::RightSide, nullptr);  // Disable close button for Server tab
    
    connect(channelTabs, &QTabWidget::tabCloseRequested, this, &MainWindow::handleTabClose);
    connect(channelList, &ChannelList::leaveChannelRequested, this, &MainWindow::leaveChannel);
}

void MainWindow::showConnectDialog() {
    auto dialog = new ConnectDialog(this);
    if (dialog->exec() == QDialog::Accepted) {
        // Disconnect old signal connections if any
        disconnect(client, &Client::connected, nullptr, nullptr);
        
        QString nickname = dialog->getNickname();
        QString username = dialog->getUsername();
        QString server = dialog->getServer();
        
        if (nickname.isEmpty() || server.isEmpty()) {
            QMessageBox::warning(this, "Invalid Input", 
                               "Please enter a nickname and select a server.");
            dialog->deleteLater();
            return;
        }
        
        // Set up client before connecting
        client->setNickname(nickname);
        client->setUsername(username);
        
        // Connect signals
        connect(client, &Client::connected, this, [this]() {
            qDebug() << "Successfully registered with server";
        });
        
        connect(client, &Client::error, this, [this](const QString& error) {
            QMessageBox::critical(this, "Connection Error", 
                                "Failed to connect: " + error);
        });
        
        // Connect to server
        qDebug() << "Connecting to server:" << server;
        client->connectToServer(server);
    }
    dialog->deleteLater();
}

void MainWindow::handleMessageReceived(const Message& message) {
    if (message.command == "PRIVMSG") {
        QString channel = message.params.split(" ").first();
        if (!channelDisplays.contains(channel)) {
            createChannelTab(channel);
        }
        
        auto display = channelDisplays[channel];
        if (display) {
            display->addMessage(message.nickname(), message.trailing);
        }
    }
    else if (message.command == "353") { // RPL_NAMREPLY
        // Format: <channel> :[[@|+]<nick> [@|+]<nick> ...]
        QStringList parts = message.params.split(" ");
        if (parts.size() >= 3) {
            QString channel = parts[2];
            QStringList users = message.trailing.split(" ", Qt::SkipEmptyParts);
            
            // Strip @ and + prefixes from nicks
            for (int i = 0; i < users.size(); i++) {
                if (users[i].startsWith("@") || users[i].startsWith("+")) {
                    users[i] = users[i].mid(1);
                }
            }
            
            userList->updateUsers(channel, users);
        }
    }
    else if (message.command == "NOTICE") {
        // Show notices in current active channel or create a server tab
        QString target = currentChannel;
        if (target.isEmpty()) {
            target = "Server";
            if (!channelDisplays.contains(target)) {
                createChannelTab(target);
            }
        }
        auto display = channelDisplays[target];
        if (display) {
            display->addSystemMessage(QString("NOTICE: %1").arg(message.trailing));
        }
    }
    else if (message.command == "ERROR") {
        // Show errors in all channels
        for (auto display : channelDisplays) {
            display->addSystemMessage(QString("ERROR: %1").arg(message.trailing));
        }
    }
    else if (message.command.toInt() > 0) {
        // Show numeric replies in current channel or server tab
        QString target = currentChannel;
        if (target.isEmpty()) {
            target = "Server";
            if (!channelDisplays.contains(target)) {
                createChannelTab(target);
            }
        }
        auto display = channelDisplays[target];
        if (display) {
            display->addSystemMessage(message.trailing);
        }
    }
}

void MainWindow::sendMessage() {
    if (messageInput->text().isEmpty()) {
        return;
    }
    
    QString message = messageInput->text();
    
    if (message.startsWith('/')) {
        handleCommand(message);
        messageInput->clear();
        return;
    }
    
    if (currentChannel.isEmpty()) {
        return;
    }
    
    client->sendMessage(currentChannel, message);
    
    // Show message in our own chat display
    if (auto display = channelDisplays[currentChannel]) {
        display->addMessage(client->nickname(), message);
    }
    
    messageInput->clear();
}

void MainWindow::handleCommand(const QString& command) {
    QStringList parts = command.split(' ', Qt::SkipEmptyParts);
    if (parts.isEmpty()) return;
    
    QString cmd = parts[0].toLower();
    
    if (cmd == "/join") {
        if (parts.size() < 2) {
            if (auto display = channelDisplays[currentChannel]) {
                display->addSystemMessage("Usage: /join #channel");
            }
            return;
        }
        
        QString channel = parts[1];
        if (!channel.startsWith('#')) {
            channel.prepend('#');
        }
        
        // Create the channel tab first
        createChannelTab(channel);
        
        // Then join the channel
        client->joinChannel(channel);
        
        handleChannelChanged(channel);
    }
    else {
        if (auto display = channelDisplays[currentChannel]) {
            display->addSystemMessage("Unknown command: " + cmd);
        }
    }
}

void MainWindow::createChannelTab(const QString& channel) {
    if (channelDisplays.contains(channel)) return;
    
    auto display = new ChatDisplay(this);
    channelDisplays[channel] = display;
    channelTabs->addTab(display, channel);
    
    bool isServer = (channel == "Server");
    channelList->addChannel(channel, isServer);
}

void MainWindow::handleChannelChanged(const QString& channel) {
    currentChannel = channel;
    userList->setCurrentChannel(channel);
    for (int i = 0; i < channelTabs->count(); i++) {
        if (channelTabs->tabText(i) == channel) {
            channelTabs->setCurrentIndex(i);
            break;
        }
    }
}

void MainWindow::about() {
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Bean Consumer says:"));
    msgBox.setText(tr("<b>ComSock IRC Client</b><br>"
                      "A simple primitive IRC client to suit your messaging needs<br>"
                      "Version: 0.0.5<br><br>"
                      "Licensed under MIT lisence"));

    QPixmap beanPixmap("src/ui/bean_consumer_by_creepercat1_dfte7u0-pre.jpg");
    msgBox.setIconPixmap(beanPixmap);

    QPushButton *thanksButton = msgBox.addButton(tr("Thanks Bean Consumer!"), QMessageBox::AcceptRole);
    msgBox.setDefaultButton(thanksButton);

    msgBox.exec();
}

void MainWindow::handleDisconnect() {
    client->disconnect();
    for (auto display : channelDisplays) {
        display->addSystemMessage("Disconnected from server");
    }
}

void MainWindow::handleUserJoined(const QString& channel, const QString& user) {
    QString cleanChannel = channel;
    if (cleanChannel.startsWith(':')) {
        cleanChannel = cleanChannel.mid(1);  
    }
    
    if (channelDisplays.contains(cleanChannel)) {
        channelDisplays[cleanChannel]->addSystemMessage(
            QString("%1 has joined").arg(user)
        );
    }
}

void MainWindow::handleUserLeft(const QString& channel, const QString& user, const QString& reason) {
    if (channelDisplays.contains(channel)) {
        if (!reason.isEmpty()) {
            channelDisplays[channel]->addSystemMessage(
                QString("%1 has quit (%2)").arg(user, reason)
            );
        } else {
            channelDisplays[channel]->addSystemMessage(
                QString("%1 has left %2").arg(user, channel)
            );
        }
    }
}

void MainWindow::handleTabChanged(int index) {
    if (index >= 0) {
        QString channel = channelTabs->tabText(index);
        handleChannelChanged(channel);
    }
}

void MainWindow::switchToChannel(const QString& channel) {
    if (channelDisplays.contains(channel)) {
        handleChannelChanged(channel);
    }
}

void MainWindow::handleTabClose(int index) {
    QString channel = channelTabs->tabText(index);
    if (channel != "Server") {  // Don't allow closing server tab
        leaveChannel(channel);
    }
}

void MainWindow::leaveChannel(const QString& channel) {
    if (channel == "Server") return;
    
    client->partChannel(channel);
    
    channelTabs->removeTab(channelTabs->indexOf(channelDisplays[channel]));
    delete channelDisplays[channel];
    channelDisplays.remove(channel);
    channelList->removeChannel(channel);
    
    if (currentChannel == channel) {
        handleChannelChanged("Server");
    }
}