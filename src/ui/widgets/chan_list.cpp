#include "chan_list.h"
#include <QListWidgetItem>
#include <QStyle>

ChannelList::ChannelList(QWidget* parent) : QListWidget(parent) {
    connect(this, &QListWidget::itemClicked, this, &ChannelList::handleItemClicked);
    
    channelIcon = style()->standardIcon(QStyle::SP_MessageBoxInformation);
    serverIcon = style()->standardIcon(QStyle::SP_ComputerIcon);
}

void ChannelList::addChannel(const QString& channel, bool isServer) {
    if (!findItems(channel, Qt::MatchExactly).isEmpty()) return;
    
    auto item = new QListWidgetItem(channel);
    item->setIcon(isServer ? serverIcon : channelIcon);
    addItem(item);
}

void ChannelList::removeChannel(const QString& channel) {
    auto items = findItems(channel, Qt::MatchExactly);
    for (auto item : items) {
        delete takeItem(row(item));
    }
}

QString ChannelList::currentChannel() const {
    return currentItem() ? currentItem()->text() : QString();
}

void ChannelList::handleItemClicked(QListWidgetItem* item) {
    emit channelClicked(item->text());
}