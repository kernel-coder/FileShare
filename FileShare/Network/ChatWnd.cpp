#include "ChatWnd.h"
#include <QLineEdit>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCloseEvent>
#include <QLabel>
#include <QPushButton>
#include "PeerChatView.h"
#include "Connection.h"
#include "Messages/ChatMsg.h"
#include "Messages/PeerViewInfoMsg.h"
#include "../Settings/GameSettings.h"

ChatWnd::ChatWnd(QWidget *parent) :
    QDialog(parent)
{
    mpPeerTab = new QTabWidget();
    mpPeerTab->setTabsClosable(true);
    mpPeerTab->setTabPosition(QTabWidget::South);

    QVBoxLayout *pMainLayout = new QVBoxLayout();
    pMainLayout->addWidget(mpPeerTab);

    mpMsgEdit = new QLineEdit();
    mpBtnSend = new QPushButton("Send");
    QHBoxLayout *pSendChatLayout = new QHBoxLayout();
    pSendChatLayout->addWidget(mpMsgEdit);
    pSendChatLayout->addSpacing(5);
    pSendChatLayout->addWidget(mpBtnSend);
    mpBtnPlay = new QPushButton(tr("Play"));
    pSendChatLayout->addSpacing(5);
    pSendChatLayout->addWidget(mpBtnPlay);

    pMainLayout->addLayout(pSendChatLayout);
    this->setLayout(pMainLayout);

    enableSendEquipment();

    connect(mpPeerTab,SIGNAL(tabCloseRequested(int)),SLOT(tabCloseWanted(int)));
    connect(mpBtnSend,SIGNAL(clicked()),SLOT(sendMsg()));
    connect(mpMsgEdit,SIGNAL(returnPressed()),SLOT(sendMsg()));
    connect(mpMsgEdit,SIGNAL(textChanged(QString)),SLOT(msgEditing(QString)));
    connect(mpBtnPlay,SIGNAL(clicked()),SLOT(askForPlay()));
    connect(GameSettings::me(),SIGNAL(playerColorChanged(Game::Player)),SLOT(myColorChanged(Game::Player)));
}

ScrollArea *ChatWnd::addOrFindTab(Connection *pConn)
{
    ScrollArea *pContainer = peerContainer(pConn);

    if(!pContainer){
        msgEditing(mpMsgEdit->text().trimmed());
        pContainer = new ScrollArea();
        PeerChatView *pPCV = new PeerChatView(pContainer,pConn);
        pContainer->setWidget(pPCV);
        pContainer->setWidgetResizable(true);
        mpPeerTab->addTab(pContainer,pConn->peerViewInfo()->name());
        enableSendEquipment();
    }

    mpPeerTab->setCurrentWidget(pContainer);

    return pContainer;
}

void ChatWnd::newMsg(Connection *pConn, ChatMsg *pMsg)
{
    ScrollArea *pContainer = addOrFindTab(pConn);
    pContainer->content()->addText(pConn->peerViewInfo()->name() + QString(": ") + pMsg->string(),pConn->peerViewInfo()->color(),PeerChatView::Peer);
}

void ChatWnd::sendMsg()
{
    QString txt = mpMsgEdit->text().trimmed();

    if(txt.isEmpty()){
        return;
    }

    if(txt.startsWith(QChar('/'))){
        txt = txt.remove(QChar('/'));
    }

    ScrollArea *pCurrPeer = currentPeerContainer();

    if(pCurrPeer){
        pCurrPeer->content()->addText(QString("Me: ") + txt,GameSettings::me()->playerColor(Game::Player1),PeerChatView::Me);
        ChatMsg *pMsg = new ChatMsg(txt);
        pCurrPeer->content()->peer()->sendMessage(pMsg);
        mpMsgEdit->setText("");
    }
}

void ChatWnd::tabCloseWanted(int nIndex)
{
    removeATab(nIndex);
}

void ChatWnd::removeATab(int nIndex)
{
    if(nIndex >= 0){
        ScrollArea *pScroll = qobject_cast<ScrollArea*>(mpPeerTab->widget(nIndex));
        mpPeerTab->removeTab(nIndex);
        delete pScroll;
    }

    enableSendEquipment();
}

void ChatWnd::closeTab(Connection *pConn)
{
    ScrollArea *pScroll = peerContainer(pConn);

    if(pScroll){
        removeATab(mpPeerTab->indexOf(pScroll));
    }
}

void ChatWnd::enableSendEquipment()
{
    bool bEnable = mpPeerTab->count() != 0;
    mpMsgEdit->setEnabled(bEnable);
    mpBtnPlay->setEnabled(bEnable);
}

ScrollArea *ChatWnd::currentPeerContainer()
{
    int nIndex = mpPeerTab->currentIndex();

    if(nIndex >= 0){
        return qobject_cast<ScrollArea*>(mpPeerTab->widget(nIndex));
    }

    return NULL;
}

ScrollArea *ChatWnd::peerContainer(Connection *pPeer)
{
    int nCount = mpPeerTab->count();

    for(int i = 0; i < nCount; i++){
        ScrollArea *pScroll = qobject_cast<ScrollArea*>(mpPeerTab->widget(i));

        if(pScroll && pScroll->content()->peer() == pPeer){
            return pScroll;
        }
    }

    return NULL;
}

void ChatWnd::closeEvent(QCloseEvent *pEvent)
{
    pEvent->ignore();
    this->hide();
}

void ChatWnd::msgEditing(const QString &text)
{
    mpBtnSend->setEnabled(text.trimmed().length() > 0);
}

void ChatWnd::askForPlay()
{
    ScrollArea *pTab = currentPeerContainer();

    if(pTab){
        emit askForPlaying(pTab->content()->peer());
    }
}

void ChatWnd::peerViewInfoChanged()
{
    Connection *pConn = reinterpret_cast<Connection*>(sender());

    if(pConn){
        ScrollArea *pContainer = peerContainer(pConn);

        if(pContainer){
            pContainer->content()->setColorToLbl(PeerChatView::Peer,pConn->peerViewInfo()->color());
        }
    }
}


void ChatWnd::myColorChanged(Game::Player player)
{
    QColor color = GameSettings::me()->playerColor(Game::Player1);

    if(Game::Player1 == player){
        int nCount = mpPeerTab->count();

        for(int i = 0; i < nCount; i++){
            ScrollArea *pScroll = qobject_cast<ScrollArea*>(mpPeerTab->widget(i));
            pScroll->content()->setColorToLbl(PeerChatView::Me,color);
            }
        }    
}
