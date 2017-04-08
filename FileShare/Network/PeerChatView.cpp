#include "PeerChatView.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QScrollBar>
#include "Connection.h"
#include "Messages/PeerViewInfoMsg.h"
#include <QResizeEvent>
#include "../Utils/Tag.h"

PeerChatView::PeerChatView(ScrollArea *pContainer,Connection *pPeer,QWidget *parent) :
    QWidget(parent),mpContainer(pContainer), mpPeer(pPeer)
{
    mpLayout = new QVBoxLayout();
    mpLayout->setDirection(QBoxLayout::TopToBottom);
    this->setLayout(mpLayout);

    QPalette pal;
    pal.setColor(this->backgroundRole(),QColor(121,121,121));
    this->setPalette(pal);
}

void PeerChatView::addText(QString text,QColor color,PeerChatView::Label lbl)
{
    QLabel *pLbl = new QLabel(text);
    UserIntData *pData = new UserIntData(int(lbl));
    pLbl->setUserData(ROLE_TAG, pData);
    pLbl->setWordWrap(true);
    pLbl->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    pLbl->setAutoFillBackground(true);
    pLbl->setAlignment(Qt::AlignLeft);
    QPalette pal;
    pal.setColor(pLbl->backgroundRole(),color);
    pLbl->setPalette(pal);
    mpLayout->addWidget(pLbl);
    mpContainer->verticalScrollBar()->setValue(mpContainer->verticalScrollBar()->maximum());
}

Connection *PeerChatView::peer(){return mpPeer;}

void PeerChatView::setColorToLbl(PeerChatView::Label lbl, QColor color)
{
    int nCount = mpLayout->count();

    for(int i = 0; i < nCount; i++){
        QLayoutItem *pItem = mpLayout->itemAt(i);

        if(pItem){
            QWidget *pWidget = pItem->widget();

            if(pWidget){
                QLabel *pLbl = qobject_cast<QLabel*>(pWidget);

                if(pLbl){
                    UserIntData *pUsrData = dynamic_cast<UserIntData*>(pLbl->userData(ROLE_TAG));

                    if(pUsrData->data() == int(lbl)){
                        QPalette pal;
                        pal.setColor(pLbl->backgroundRole(),color);
                        pLbl->setPalette(pal);
                        pLbl->setPalette(pal);
                    }
                }
            }
        }
    }
}

ScrollArea::ScrollArea(QWidget *pParent):QScrollArea(pParent)
{
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

PeerChatView *ScrollArea::content(){return qobject_cast<PeerChatView*>(widget());}

void ScrollArea::resizeEvent(QResizeEvent *pEvent)
{
    QScrollArea::resizeEvent(pEvent);
}

