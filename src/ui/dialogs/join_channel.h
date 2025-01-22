#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QCheckBox>
#include <QPushButton>

class JoinChannelDialog : public QDialog {
    Q_OBJECT
public:
    explicit JoinChannelDialog(QWidget* parent = nullptr);
    QString getChannel() const;
    bool getDontShowAgain() const;
    void setChannelList(const QStringList& channels);

private:
    QListWidget* channelList;
    QLineEdit* channelInput;
    QCheckBox* dontShowAgain;
    QPushButton* joinButton;
    QPushButton* cancelButton;
    QPushButton* refreshButton;

signals:
    void refreshRequested();

private slots:
    void handleChannelSelected(QListWidgetItem* item);
    void validateInput();
}; 