#include "join_channel.h"
#include "../../utils/settings.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

JoinChannelDialog::JoinChannelDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Join Channel");
    resize(400, 500);

    auto layout = new QVBoxLayout(this);

    // Channel list
    auto listLabel = new QLabel("Available Channels:", this);
    layout->addWidget(listLabel);

    channelList = new QListWidget(this);
    layout->addWidget(channelList);

    // Manual channel input
    auto inputLayout = new QHBoxLayout;
    channelInput = new QLineEdit(this);
    channelInput->setPlaceholderText("#channel");
    inputLayout->addWidget(new QLabel("Channel:"));
    inputLayout->addWidget(channelInput);
    layout->addLayout(inputLayout);

    // Refresh button
    refreshButton = new QPushButton("Refresh Channel List", this);
    layout->addWidget(refreshButton);

    // Don't show again checkbox
    dontShowAgain = new QCheckBox("Don't show this dialog again", this);
    layout->addWidget(dontShowAgain);

    // Buttons
    auto buttonLayout = new QHBoxLayout;
    joinButton = new QPushButton("Join", this);
    cancelButton = new QPushButton("Cancel", this);
    buttonLayout->addWidget(joinButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);

    // Connect signals
    connect(joinButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(refreshButton, &QPushButton::clicked, this, &JoinChannelDialog::refreshRequested);
    connect(channelList, &QListWidget::itemClicked, this, &JoinChannelDialog::handleChannelSelected);
    connect(channelInput, &QLineEdit::textChanged, this, &JoinChannelDialog::validateInput);

    joinButton->setEnabled(false);
}

QString JoinChannelDialog::getChannel() const {
    QString channel = channelInput->text();
    if (!channel.startsWith('#')) {
        channel.prepend('#');
    }
    return channel;
}

bool JoinChannelDialog::getDontShowAgain() const {
    return dontShowAgain->isChecked();
}

void JoinChannelDialog::setChannelList(const QStringList& channels) {
    channelList->clear();
    for (const QString& channel : channels) {
        // Only add if it starts with # and isn't already in the list
        if (channel.startsWith("#") && 
            channelList->findItems(channel, Qt::MatchExactly).isEmpty()) {
            channelList->addItem(channel);
        }
    }
}

void JoinChannelDialog::handleChannelSelected(QListWidgetItem* item) {
    if (item) {
        channelInput->setText(item->text());
        validateInput();
    }
}

void JoinChannelDialog::validateInput() {
    joinButton->setEnabled(!channelInput->text().isEmpty());
} 