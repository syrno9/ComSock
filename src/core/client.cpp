#include "client.h"
#include <QDebug>
#include <QTimer>
#include <QRandomGenerator>

Client::Client(QObject* parent) : QObject(parent), socket(new QTcpSocket(this)) {
    connect(socket, &QTcpSocket::readyRead, this, &Client::handleSocketData);
    connect(socket, &QTcpSocket::connected, this, &Client::handleConnected);
    connect(socket, &QTcpSocket::disconnected, this, &Client::disconnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &Client::handleError);
}

void Client::connectToServer(const QString& host, quint16 port) {
    if (!socket || socket->state() == QAbstractSocket::ConnectedState || 
        socket->state() == QAbstractSocket::ConnectingState) return;

    qDebug() << "Connecting to" << host << ":" << port;
    socket->connectToHost(host, port);
    
    if (!socket->waitForConnected(5000)) {
        emit error("Could not connect to server.");
        return;
    }

    QString cmd = QString("NICK %1\r\nUSER %2 0 * :My IRC Client\r\n")
                 .arg(currentNickname)
                 .arg(currentUsername.isEmpty() ? currentNickname : currentUsername);
    
    qDebug() << "Sending registration:" << cmd.trimmed();
    socket->write(cmd.toUtf8());
    socket->flush();
}

void Client::disconnect() {
    if (socket && socket->state() == QTcpSocket::ConnectedState) {
        socket->disconnectFromHost();
    }
}

void Client::sendMessage(const QString& channel, const QString& message) {
    if (!socket || socket->state() != QTcpSocket::ConnectedState) {
        qDebug() << "Cannot send message: not connected";
        return;
    }
    QString cmd = QString("PRIVMSG %1 :%2\r\n").arg(channel, message);
    socket->write(cmd.toUtf8());
}

void Client::joinChannel(const QString& channel) {
    if (!socket || socket->state() != QTcpSocket::ConnectedState) {
        qDebug() << "Cannot join channel: not connected";
        return;
    }
    activeChannels.insert(channel);
    QString cmd = QString("JOIN %1\r\n").arg(channel);
    socket->write(cmd.toUtf8());
}

void Client::setNickname(const QString& nickname) {
    currentNickname = nickname;
    if (socket && socket->state() == QTcpSocket::ConnectedState) {
        qDebug() << "Setting nickname:" << nickname;
        QString cmd = QString("NICK %1\r\n").arg(nickname);
        socket->write(cmd.toUtf8());
    }
}

void Client::setUsername(const QString& username) {
    currentUsername = username;
    if (socket && socket->state() == QTcpSocket::ConnectedState) {
        qDebug() << "Setting username:" << username;
        QString cmd = QString("USER %1 0 * :%1\r\n").arg(username);
        socket->write(cmd.toUtf8());
    }
}

void Client::handleConnected() {
    qDebug() << "TCP Connected to server";
    registrationSent = false;
    
    // For Rizon, wait for the No Ident response before sending registration
    if (!socket->peerName().contains("rizon")) {
        sendRegistration();
    }
}

void Client::handleError(QAbstractSocket::SocketError socketError) {
    qDebug() << "Socket error:" << socketError << socket->errorString();
    emit error(socket->errorString());
}

void Client::handleSocketData() {
    while (socket->canReadLine()) {
        QString line = QString::fromUtf8(socket->readLine());
        line = line.trimmed();
        qDebug() << "Received:" << line;
        
        // PING handling
        if (line.startsWith("PING")) {
            QString response = QString("PONG %1\r\n").arg(line.mid(5));
            qDebug() << "Sending:" << response.trimmed();
            socket->write(response.toUtf8());
            socket->flush();
            continue;
        }
        
        Message msg = Message::parse(line);
        
        if (msg.command == "001") {  // RPL_WELCOME
            qDebug() << "Registration successful!";
            emit connected();
        }
        else if (msg.command == "PRIVMSG") {
            emit messageReceived(msg);
        }
        else if (msg.command == "JOIN") {
            emit userJoined(msg.params, msg.fullUserInfo());
        }
        else if (msg.command == "QUIT") {
            QString reason = msg.trailing;
            QString user = msg.fullUserInfo();
            for (const QString& channel : activeChannels) {
                emit userLeft(channel, user, reason);
            }
        }
        else if (msg.command == "PART") {
            emit userLeft(msg.params, msg.fullUserInfo(), QString());
        }
        else if (msg.command == "ERROR") {
            qDebug() << "Server error:" << msg.trailing;
            emit error(msg.trailing);
        }
        else if (msg.command.toInt() > 0 || msg.command == "NOTICE") {
            emit messageReceived(msg);
        }
    }
}

void Client::sendRegistration() {
    qDebug() << "Sending registration...";
    
    // Send NICK first
    QString nickCmd = QString("NICK %1\r\n").arg(currentNickname);
    qDebug() << "Sending:" << nickCmd.trimmed();
    socket->write(nickCmd.toUtf8());
    socket->flush();
    
    // Send USER command
    QString userCmd = QString("USER %1 0 * :%1\r\n").arg(currentUsername);
    qDebug() << "Sending:" << userCmd.trimmed();
    socket->write(userCmd.toUtf8());
    socket->flush();
    
    registrationSent = true;
}

void Client::reconnect() {
    socket->disconnectFromHost();
    if (socket->state() != QAbstractSocket::UnconnectedState) {
        socket->waitForDisconnected(1000);
    }
    QTimer::singleShot(2000, this, [this]() {
        connectToServer(socket->peerName(), socket->peerPort());
    });
}