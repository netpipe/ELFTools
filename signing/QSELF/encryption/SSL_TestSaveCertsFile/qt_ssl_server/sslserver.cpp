#include "sslserver.h"
#include <QBuffer>
#include <QTcpSocket>
#include <QFile>
#include <QMessageBox>
#include <QSslConfiguration>
#include "csslsocket.h"

SSLServer::SSLServer(QObject *parent) : QTcpServer(parent)
{
//    QFileInfo server_ca(SERVER_CA_PEM);
//    if(!server_ca.exists())
//    {
        CSslSocket* socket = new CSslSocket(this);
        if(socket)
        {
            delete socket;
        }
//    }
}

void SSLServer::incomingConnection(qintptr socketDescriptor)
{
    QSslSocket* socket = new QSslSocket(this);

    connect(socket, SIGNAL(disconnected()),                             this, SLOT(disconnectedSlot()));
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChangedSlot(QAbstractSocket::SocketState)));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),        this, SLOT(errorSlot(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(sslErrors(const QList<QSslError> &)),        this, SLOT(sslErrorsSlot(const QList<QSslError> &)));
    connect(socket, SIGNAL(readyRead()),                                this, SLOT(receiveMessage()), Qt::DirectConnection);

   if(socket->setSocketDescriptor(socketDescriptor))
   {
       socket->setPrivateKey(SERVER_KEY);
       socket->setLocalCertificate(SERVER_PEM);
       addPendingConnection(socket);
       socket->startServerEncryption();
       socket->addCaCertificates(CLIENT_CA_PEM);
   }else {
        delete socket;
   }
    socket->setPeerVerifyMode(QSslSocket::VerifyNone);
    socket->ignoreSslErrors ({QSslError::SelfSignedCertificate});

    qDebug() << "LocalCertificated:" <<socket->localCertificate().isNull();

    this->sockets.push_back(socket);
    emit appendToLog("Server received a new connection [socket descriptor: " + QString::number(socketDescriptor) + "]");
    emit updateClientList(sockets);
}

bool SSLServer::onSendMessage(QString message)
{
    foreach (QSslSocket* socket, sockets) {
        QByteArray data = message.toLatin1();
        qint64 bytes = socket->write((const char*)data);
        qDebug() << "Send data bytes:" << bytes;
    }
    return true;
}

void SSLServer::disconnectedSlot()
{
    QSslSocket* socket = qobject_cast<QSslSocket*>(sender());
    this->sockets.removeOne(socket);

    emit appendToLog("Client disconnected " + socket->peerAddress().toString() + ":" + QString::number(socket->peerPort()));
    emit updateClientList(this->sockets);
}

void SSLServer::errorSlot(QAbstractSocket::SocketError e)
{
    emit appendToLog("Socket Error: " + QString::number(e));
    qDebug() << "Server_Socket_Error:" << e;
}

void SSLServer::stateChangedSlot(QAbstractSocket::SocketState s)
{
    emit appendToLog("Socket State: " + QString::number(s));
    qDebug() << "Server_Socket_State:" << s;
}

void SSLServer::sslErrorsSlot(const QList<QSslError> & error_list)
{
    emit appendToLog("SSL Error");
    qDebug() << "SSL_Error" << error_list;
}

void SSLServer::receiveMessage()
{
    QSslSocket* socket = static_cast<QSslSocket*>(sender());
    QBuffer* buffer = new QBuffer(this);
    // missing some checks for returns values for the sake of simplicity
    buffer->open(QIODevice::ReadWrite);
    QByteArray array = socket->readAll();
    emit receiveString(QString(array));
}
