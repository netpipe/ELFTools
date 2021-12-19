#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QThread>
#include <QProcess>
#include <QDateTime>
#include <QSslCertificate>
#include <QSslKey>
#include <QFile>
#include "csslsocket.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

void MainWindow::createSocket()
{
    socket = new CSslSocket(this);
    socket->addCaCertificates(SERVER_CA_PEM);
    socket->setPeerVerifyMode(QSslSocket::VerifyNone);
    socket->ignoreSslErrors({QSslError::SelfSignedCertificate});

    connect(socket, SIGNAL(connected()),                                this,   SLOT(connectedSlot()));
    connect(socket, SIGNAL(disconnected()),                             this,   SLOT(disconnectedSlot()));
    connect(socket, SIGNAL(encrypted()),                                this,   SLOT(encryptedSlot()));
    connect(socket, SIGNAL(readyRead()),                                this,   SLOT(onReceiveMessage()), Qt::DirectConnection);
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this,   SLOT(stateChangedSlot(QAbstractSocket::SocketState)));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),        this,   SLOT(errorSlot(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(sslErrors(const QList<QSslError> &)),        this,   SLOT(sslErrorsSlot(const QList<QSslError> &)));

    ui->statusbar->showMessage("Unconnected");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_connect_clicked()
{
    createSocket();

    socket->connectToHostEncrypted(ui->ip->text(), ui->port->text().toUInt());

    if (socket->waitForEncrypted()) {
        socket->write("Authentication Suceeded");
    }
    else {
        qDebug("Unable to connect to server");
    }

}

void MainWindow::connectedSlot()
{
    ui->log->append("Connected successfully [socket descriptor: " + QString::number(socket->socketDescriptor()) + "]");
    ui->statusbar->showMessage("Connected");
}

void MainWindow::disconnectedSlot()
{
    ui->log->append("Disconnected");
    ui->statusbar->showMessage("Disconnected");
}

void MainWindow::errorSlot(QAbstractSocket::SocketError e)
{
    ui->log->append("Error [" + QString::number(e) + "]: " + this->socket->errorString());
    qDebug() << "Client_Socket_Error:" << e;
}

void MainWindow::stateChangedSlot(QAbstractSocket::SocketState s)
{
    ui->log->append("State: " + QString::number(s));
    qDebug() << "Client_Socket_State:" << s;
}

void MainWindow::encryptedSlot()
{
    ui->log->append("Connection is encrypted");
}

void MainWindow::sslErrorsSlot(const QList<QSslError> &errors)
{

    for(const QSslError& e : errors){
        ui->log->append("SSL Error: " + e.errorString());
    }

    qDebug() << "Client_SSL_Error:" << errors;
}

void MainWindow::on_button_send_clicked()
{
    qDebug() << socket->isEncrypted() << "Encrypted";
    QByteArray byte_array = ui->textEdit->toPlainText().toLatin1();
    qint64 bytes = socket->write((const char*)byte_array);
    socket->flush();

    qDebug() << "Message sent : " << bytes << "bytes";
}

void MainWindow::onReceiveMessage()
{
    QByteArray array = socket->readAll();
    ui->edt_receive_data->append(QString(array));
}
