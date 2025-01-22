#include "settings.h"
#include <QDateTime>

QSettings& Settings::instance() {
    static QSettings settings("ComSock", "ComSock");
    return settings;
}

void Settings::saveServers(const QStringList& servers) {
    instance().setValue("servers", servers);
    instance().sync();
}

QStringList Settings::loadServers() {
    QStringList defaultServers = {
        "irc.libera.chat",
        "irc.freenode.net",
        "irc.rizon.net"
    };
    return instance().value("servers", defaultServers).toStringList();
}

void Settings::saveNickname(const QString& nickname) {
    if (!nickname.startsWith("Guest")) {  // Only save non-guest nicknames
        instance().setValue("nickname", nickname);
        instance().sync();
    }
}

QString Settings::loadNickname() {
    QString savedNick = instance().value("nickname").toString();
    if (savedNick.isEmpty()) {
        return generateGuestNickname();
    }
    return savedNick;
}

QString Settings::generateGuestNickname() {
    // Generate a random number based on current time
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
    int randomNum = timestamp % 10000;
    return QString("Guest%1").arg(randomNum, 4, 10, QChar('0'));
} 