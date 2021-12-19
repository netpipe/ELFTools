#ifndef SSLSERVER_H
#define SSLSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QSslKey>
#include <QSslCertificate>
#include <QSslSocket>
#include <QVector>
#include <QDir>
#include "define.h"

class CSslSocket;
class SSLServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit SSLServer(QObject *parent = nullptr);

private:
    QVector<QSslSocket*> sockets;
protected:
    void incomingConnection(qintptr socketDescriptor) override;
private slots:
    void disconnectedSlot();
    void errorSlot(QAbstractSocket::SocketError);
    void stateChangedSlot(QAbstractSocket::SocketState);
    void sslErrorsSlot(const QList<QSslError> &error_list);
    void receiveMessage();
    bool onSendMessage(QString message);
signals:
    void appendToLog(const QString&);
    void updateClientList(const QVector<QSslSocket*>&);
    void receiveString(QString);
};

#endif // SSLSERVER_H
