#pragma once
#include <QSettings>
#include <QStringList>

class Settings {
public:
    static void saveServers(const QStringList& servers);
    static QStringList loadServers();
    static void saveNickname(const QString& nickname);
    static QString loadNickname();
    static void saveShowJoinDialog(bool show);
    static bool shouldShowJoinDialog();
    
private:
    static QSettings& instance();
    static QString generateGuestNickname();
}; 