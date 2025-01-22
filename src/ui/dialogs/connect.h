#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>

class ConnectDialog : public QDialog {
    Q_OBJECT
public:
    explicit ConnectDialog(QWidget* parent = nullptr);
    
    QString getServer() const;
    QString getNickname() const;
    QString getUsername() const;
    QStringList getAlternativeNicks() const;

private slots:
    void addNewServer();
    void removeServer();
    void saveServers();

private:
    QListWidget* networkList;
    QLineEdit* nicknameInput;
    QLineEdit* nickname2Input;
    QLineEdit* nickname3Input;
    QLineEdit* usernameInput;
    QPushButton* addServerButton;
    QPushButton* removeServerButton;
    
    void setupDefaultServers();
    void setupServerButtons();
};