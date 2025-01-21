#pragma once
#include <QString>
#include <QDateTime>

struct Message {
    QString prefix;
    QString command;
    QString params;
    QString trailing;
    QString raw;
    QDateTime timestamp;
    
    static Message parse(const QString& raw);
    QString nickname() const;
    QString fullUserInfo() const { return prefix; }
};