#include "connect.h"
#include "../../utils/settings.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>

ConnectDialog::ConnectDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Connect to Server");
    
    auto layout = new QVBoxLayout(this);
    
    auto serverSection = new QVBoxLayout;
    serverSection->addWidget(new QLabel("Server:"));
    
    networkList = new QListWidget(this);
    serverSection->addWidget(networkList);
    
    auto buttonLayout = new QHBoxLayout;
    addServerButton = new QPushButton("Add Server", this);
    removeServerButton = new QPushButton("Remove Server", this);
    buttonLayout->addWidget(addServerButton);
    buttonLayout->addWidget(removeServerButton);
    serverSection->addLayout(buttonLayout);
    
    layout->addLayout(serverSection);
    
    // Nickname inputs
    auto nickLayout = new QVBoxLayout;
    nicknameInput = new QLineEdit(this);
    nicknameInput->setText(Settings::loadNickname());  // Load saved or guest nickname
    nickname2Input = new QLineEdit(this);
    nickname3Input = new QLineEdit(this);
    
    nickLayout->addWidget(new QLabel("Nickname:"));
    nickLayout->addWidget(nicknameInput);
    nickLayout->addWidget(new QLabel("Alternative nicknames:"));
    nickLayout->addWidget(nickname2Input);
    nickLayout->addWidget(nickname3Input);
    
    // Username input
    usernameInput = new QLineEdit(this);
    usernameInput->setText(nicknameInput->text());  
    nickLayout->addWidget(new QLabel("Username:"));
    nickLayout->addWidget(usernameInput);
    
    layout->addLayout(nickLayout);
    
    connect(nicknameInput, &QLineEdit::textChanged, this, [this](const QString& text) {
        if (usernameInput->text().isEmpty() || 
            usernameInput->text().startsWith("Guest")) {
            usernameInput->setText(text);
        }
    });
    
    auto buttonBox = new QHBoxLayout;
    auto connectButton = new QPushButton("Connect", this);
    auto cancelButton = new QPushButton("Cancel", this);
    
    connect(connectButton, &QPushButton::clicked, this, [this]() {
        Settings::saveNickname(nicknameInput->text());
        accept();
    });
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
    buttonBox->addWidget(connectButton);
    buttonBox->addWidget(cancelButton);
    layout->addLayout(buttonBox);
    
    setupDefaultServers();
    setupServerButtons();
}

QString ConnectDialog::getServer() const {
    return networkList->currentItem() ? networkList->currentItem()->text() : QString();
}

QString ConnectDialog::getNickname() const {
    return nicknameInput->text();
}

QString ConnectDialog::getUsername() const {
    return usernameInput->text();
}

QStringList ConnectDialog::getAlternativeNicks() const {
    QStringList nicks;
    if (!nickname2Input->text().isEmpty()) nicks << nickname2Input->text();
    if (!nickname3Input->text().isEmpty()) nicks << nickname3Input->text();
    return nicks;
}

void ConnectDialog::setupDefaultServers() {
    networkList->clear();
    networkList->addItems(Settings::loadServers());
    networkList->setCurrentRow(0);
}

void ConnectDialog::setupServerButtons() {
    connect(addServerButton, &QPushButton::clicked, this, &ConnectDialog::addNewServer);
    connect(removeServerButton, &QPushButton::clicked, this, &ConnectDialog::removeServer);
}

void ConnectDialog::addNewServer() {
    bool ok;
    QString server = QInputDialog::getText(this, "Add Server",
                                         "Enter server address:",
                                         QLineEdit::Normal,
                                         QString(), &ok);
    
    if (ok && !server.isEmpty()) {
        if (!networkList->findItems(server, Qt::MatchExactly).isEmpty()) {
            QMessageBox::warning(this, "Duplicate Server",
                               "This server is already in the list.");
            return;
        }
        
        networkList->addItem(server);
        saveServers();
    }
}

void ConnectDialog::removeServer() {
    QListWidgetItem* current = networkList->currentItem();
    if (!current) return;
    
    if (QMessageBox::question(this, "Remove Server",
                            "Are you sure you want to remove this server?") 
        == QMessageBox::Yes) {
        delete networkList->takeItem(networkList->row(current));
        saveServers();
    }
}

void ConnectDialog::saveServers() {
    QStringList servers;
    for (int i = 0; i < networkList->count(); i++) {
        servers << networkList->item(i)->text();
    }
    Settings::saveServers(servers);
}