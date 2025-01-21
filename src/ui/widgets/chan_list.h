#pragma once
#include <QListWidget>
#include <QIcon>

class ChannelList : public QListWidget {
    Q_OBJECT
public:
    explicit ChannelList(QWidget* parent = nullptr);
    
    void addChannel(const QString& channel, bool isServer = false);
    void removeChannel(const QString& channel);
    QString currentChannel() const;

signals:
    void channelClicked(const QString& channel);

private:
    QIcon channelIcon;
    QIcon serverIcon;
    
private slots:
    void handleItemClicked(QListWidgetItem* item);
};