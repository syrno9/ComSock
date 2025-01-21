#include "chan_list.h"
#include <QListWidgetItem>
#include <QStyle>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

ChannelList::ChannelList(QWidget* parent) : QListWidget(parent) {
    connect(this, &QListWidget::itemClicked, this, &ChannelList::handleItemClicked);
    
    channelIcon = style()->standardIcon(QStyle::SP_MessageBoxInformation);
    serverIcon = style()->standardIcon(QStyle::SP_ComputerIcon);
    
    setStyleSheet(R"(
        QListWidget::item {
            padding-left: 5px;
        }
        QListWidget::item[isChannel="true"] {
            margin-left: 20px;
        }
    )");
}

void ChannelList::addChannel(const QString& channel, bool isServer) {
    if (!findItems(channel, Qt::MatchExactly).isEmpty()) return;
    
    auto item = new QListWidgetItem(channel);
    item->setIcon(isServer ? serverIcon : channelIcon);
    
    if (!isServer) {
        item->setData(Qt::UserRole, true);
        item->setData(Qt::UserRole + 1, "true");
    }
    
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

void ChannelList::contextMenuEvent(QContextMenuEvent* event) {
    QListWidgetItem* item = itemAt(event->pos());
    if (!item) return;
    
    QString channel = item->text();
    if (channel == "Server") return;  
    
    QMenu menu(this);
    QAction* leaveAction = menu.addAction("Leave Channel");
    
    connect(leaveAction, &QAction::triggered, this, [this, channel]() {
        emit leaveChannelRequested(channel);
    });
    
    menu.exec(event->globalPos());
}