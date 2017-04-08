#ifndef PEERCHATVIEW_H
#define PEERCHATVIEW_H

#include <QWidget>
#include <QScrollArea>

class QVBoxLayout;
class ScrollArea;
class Connection;
class QResizeEvent;
class ChatWnd;

class PeerChatView : public QWidget
{
Q_OBJECT
public:
    enum Label{Me,Peer};
    PeerChatView(ScrollArea *pContainer, Connection *pPeer,QWidget *parent = 0);
    void addText(QString text,QColor color,PeerChatView::Label lbl);
    Connection *peer();

signals:

public slots:

private:
    void setColorToLbl(PeerChatView::Label lbl,QColor color);

private:
    ScrollArea *mpContainer;
    Connection *mpPeer;
    QVBoxLayout *mpLayout;

    friend class ChatWnd;
};

class ScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    ScrollArea(QWidget *pParent = 0);
    PeerChatView *content();

protected:
    void resizeEvent(QResizeEvent *pEvent);
};

#endif // PEERCHATVIEW_H
