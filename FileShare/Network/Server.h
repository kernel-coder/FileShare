#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>

class Connection;

class Server : public QTcpServer
{
Q_OBJECT
public:
    Server(QObject *parent = 0);
    ~Server();

signals:
    void newPeer(Connection *pConnection);

public slots:
protected:
    void incomingConnection(int socketDescriptor);

};

#endif // SERVER_H
