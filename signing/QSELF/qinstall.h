#ifndef QINSTALL_H
#define QINSTALL_H

#include <QMainWindow>

namespace Ui {
class QInstall;
}

class QInstall : public QMainWindow
{
    Q_OBJECT

public:
    explicit QInstall(QWidget *parent = 0);
    ~QInstall();

private slots:
    void on_installbtn_clicked();

    void on_btnRemove_clicked();

    void on_btnAdd_clicked();

    void on_btnCreate_clicked();

    void on_btnOpen_clicked();

    void on_btnDecompress_clicked();

    void on_btnBrowse_clicked();

    void on_btnInstall_clicked();

 //   int getExecutablePath(char* szBuff, unsigned long ulBufferLen);
 //   int linuxextract(QString file);

private:
    Ui::QInstall *ui;
};

#endif // QINSTALL_H
