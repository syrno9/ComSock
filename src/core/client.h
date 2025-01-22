#pragma once
#include <QObject>
#include <QTcpSocket>
#include "message.h"

class Client : public QObject {
    Q_OBJECT
public:
    explicit Client(QObject* parent = nullptr);
    
    void connectToServer(const QString& host, quint16 port = 6667);
    void disconnect();
    void sendMessage(const QString& channel, const QString& message);
    void joinChannel(const QString& channel);
    void setNickname(const QString& nickname);
    void setUsername(const QString& username);
    QString nickname() const { return currentNickname; }
    void partChannel(const QString& channel);
    void requestChannelList();
    
signals:
    void connected();
    void disconnected();
    void messageReceived(const Message& message);
    void userJoined(const QString& channel, const QString& nickname);
    void userLeft(const QString& channel, const QString& nickname, const QString& reason);
    void topicChanged(const QString& channel, const QString& topic);
    void error(const QString& error);
    void channelListReceived(const QStringList& channels);
    
private slots:
    void handleConnected();
    void handleError(QAbstractSocket::SocketError error);
    void handleSocketData();
    
private:
    QTcpSocket* socket;
    QString currentNickname;
    QString currentUsername;
    bool registrationSent = false;
    QSet<QString> activeChannels;
    
    // helper methods
    void sendRegistration();
    void sendUserCommand();
    void reconnect();
};