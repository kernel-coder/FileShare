#ifndef CHATWND_H
#define CHATWND_H

#include <QDialog>
#include "../DataStructure/Enums.h"

class QLineEdit;
class QTabWidget;
class QPushButton;
class Connection;
class ScrollArea;
class ChatMsg;
class QCloseEvent;

class ChatWnd : public QDialog
{
Q_OBJECT
public:
    explicit ChatWnd(QWidget *parent = 0);
    ScrollArea *peerContainer(Connection *pPeer);
    ScrollArea *currentPeerContainer();
    void newMsg(Connection *pConn,ChatMsg *pMsg);
    ScrollArea *addOrFindTab(Connection *pConn);
    void closeTab(Connection *pConn);

signals:
    void askForPlaying(Connection *pPeer);

protected:
    void closeEvent(QCloseEvent *pEvent);

private slots:
    void tabCloseWanted(int nIndex);
    void sendMsg();
    void msgEditing(const QString & text);
    void removeATab(int nIndex);
    void askForPlay();
    void peerViewInfoChanged();
    void myColorChanged(Game::Player player);

private:

    void enableSendEquipment();

private:
    QLineEdit *mpMsgEdit;
    QPushButton *mpBtnSend,*mpBtnPlay;
    QTabWidget *mpPeerTab;
};

#endif // CHATWND_H
