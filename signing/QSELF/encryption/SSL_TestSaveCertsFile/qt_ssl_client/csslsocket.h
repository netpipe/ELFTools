#ifndef CSSLSOCKET_H
#define CSSLSOCKET_H

#include <QObject>
#include <QSslSocket>


class CSslSocket : public QSslSocket
{
    Q_OBJECT
public:
    CSslSocket(QObject* parent = nullptr);
};

#endif // CSSLSOCKET_H
