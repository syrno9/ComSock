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
    setWindowFlags(Qt::FramelessWindowHint);
    resize(800, 600);
    
    // Title bar
    auto titleBar = new QWidget(this);
    titleBar->setFixedHeight(30);
    titleBar->setStyleSheet("background: #3c3c3c; border-bottom: 1px solid #555555;");
    
    // Title label
    auto titleLabel = new QLabel("ComSock", titleBar);
    titleLabel->setStyleSheet("QLabel { color: #f4f4f4; padding-left: 10px; }");
    
    // Window control buttons
    auto minimizeBtn = new QPushButton("-", titleBar);
    auto maximizeBtn = new QPushButton("□", titleBar);
    auto closeBtn = new QPushButton("×", titleBar);
    
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
    
    minimizeBtn->setStyleSheet(buttonStyle);
    maximizeBtn->setStyleSheet(buttonStyle);
    closeBtn->setStyleSheet(buttonStyle);
    closeBtn->setObjectName("closeButton");
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
    
    // Title bar
    auto titleBar = new QWidget(this);
    titleBar->setFixedHeight(30);
    titleBar->setStyleSheet("background: #3c3c3c; border-bottom: 1px solid #555555;");
    
    // Title label
    auto titleLabel = new QLabel("ComSock", titleBar);
    titleLabel->setStyleSheet("QLabel { color: #f4f4f4; padding-left: 10px; }");
    
    // Window control buttons
    auto minimizeBtn = new QPushButton("-", titleBar);
    auto maximizeBtn = new QPushButton("□", titleBar);
    auto closeBtn = new QPushButton("×", titleBar);
    
    // Layout for title bar
    auto titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(minimizeBtn);
    titleLayout->addWidget(maximizeBtn);
    titleLayout->addWidget(closeBtn);
    
    // Connect window control buttons
    connect(minimizeBtn, &QPushButton::clicked, this, &MainWindow::showMinimized);
    connect(maximizeBtn, &QPushButton::clicked, this, [this]() {
        if (isMaximized()) showNormal();
        else showMaximized();
    });
    connect(closeBtn, &QPushButton::clicked, this, &QWidget::close);
    
    // Enable window dragging
    titleBar->installEventFilter(this);
    
    // Add title bar to main layout
    mainLayout->addWidget(titleBar);
        
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
    }
    else {
        if (auto display = channelDisplays[currentChannel]) {
            display->addSystemMessage("Unknown command: " + cmd);
        }
    }
}

void MainWindow::createChannelTab(const QString& channel) {
    if (!channelDisplays.contains(channel)) {
        auto display = new ChatDisplay(this);
        channelDisplays[channel] = display;
        channelTabs->addTab(display, channel);
        channelList->addChannel(channel);
        display->addSystemMessage(QString("Joined channel %1").arg(channel));
    }
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
    QMessageBox::about(this, tr("About ComSock"),
        tr("ComSock IRC Client\n"
           "A simple IRC client written in Qt\n"
           "Licensed under GNU GPL v2"));
}

void MainWindow::handleDisconnect() {
    client->disconnect();
    for (auto display : channelDisplays) {
        display->addSystemMessage("Disconnected from server");
    }
}

void MainWindow::handleUserJoined(const QString& channel, const QString& user) {
    if (channelDisplays.contains(channel)) {
        channelDisplays[channel]->addSystemMessage(
            QString("%1 has joined %2").arg(user, channel)
        );
    }
}

void MainWindow::handleUserLeft(const QString& channel, const QString& user) {
    if (channelDisplays.contains(channel)) {
        channelDisplays[channel]->addSystemMessage(
            QString("%1 has left %2").arg(user, channel)
        );
    }
}

void MainWindow::handleTabChanged(int index) {
    if (index >= 0) {
        QString channel = channelTabs->tabText(index);
        handleChannelChanged(channel);
    }
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    static QPoint dragPos;
    
    if (event->type() == QEvent::MouseButtonPress) {
        auto mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            dragPos = mouseEvent->globalPos() - frameGeometry().topLeft();
            return true;
        }
    } else if (event->type() == QEvent::MouseMove) {
        auto mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->buttons() & Qt::LeftButton) {
            move(mouseEvent->globalPos() - dragPos);
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}