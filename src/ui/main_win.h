#pragma once
#include <QMainWindow>
#include <QLineEdit>
#include <QMenuBar>
#include <QTabWidget>
#include <QLabel>
#include <QTimer>
#include <QPushButton>
#include <QMouseEvent>
#include <QHBoxLayout>
#include "widgets/chan_list.h"
#include "widgets/usr_list.h"
#include "widgets/msg_display.h"
#include "../core/client.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void showConnectDialog();
    void handleDisconnect();
    void handleMessageReceived(const Message& message);
    void handleUserJoined(const QString& channel, const QString& user);
    void handleUserLeft(const QString& channel, const QString& user, const QString& reason);
    void handleChannelChanged(const QString& channel);
    void handleTabChanged(int index);
    void sendMessage();
    void about();
    void switchToChannel(const QString& channel);
    void handleTabClose(int index);
    void leaveChannel(const QString& channel);
    void showJoinChannelDialog();

private:
    Client* client;
    ChannelList* channelList;
    UserList* userList;
    QTabWidget* channelTabs;
    QLineEdit* messageInput;
    
    // channel management
    QString currentChannel;
    QMap<QString, ChatDisplay*> channelDisplays;
    QStringList channelListBuffer;
    
    // helper methods
    void setupMenuBar();
    void setupLayout();
    void createChannelTab(const QString& channel);
    void handleCommand(const QString& command);
};