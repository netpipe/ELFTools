#include "qinstall.h"
#include "ui_qinstall.h"
#include <QFile>
#include <QFileDialog>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QCryptographicHash>
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"
#include "quazip/JlCompress.h"
#include <QProcess>

#include <string.h>
#include <QDebug>

#define LINUX

#include "linuxfiles.h" //https://www.strchr.com/creating_self-extracting_executables


#ifdef WINDOWS
#include <windows.h>
#endif

int ReadFromExeFile();

QByteArray fileChecksum(const QString &fileName);

void zip(QString filename , QString zip_filename)
{
   QFile infile(filename);
   QFile outfile(zip_filename);
   infile.open(QIODevice::ReadOnly);
   outfile.open(QIODevice::WriteOnly);
   QByteArray uncompressed_data = infile.readAll();
   QByteArray compressed_data = qCompress(uncompressed_data, 9);
   outfile.write(compressed_data);
   infile.close();
   outfile.close();
}

void unZip(QString zip_filename , QString filename)
{
   QFile infile(zip_filename);
   QFile outfile(filename);
   infile.open(QIODevice::ReadOnly);
   outfile.open(QIODevice::WriteOnly);
   QByteArray uncompressed_data = infile.readAll();
   QByteArray compressed_data = qUncompress(uncompressed_data);
   outfile.write(compressed_data);
   infile.close();
   outfile.close();
}


QInstall::QInstall(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QInstall)
{
    ui->setupUi(this);
}

QInstall::~QInstall()
{
    delete ui;
}

void QInstall::on_installbtn_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                 "",
                                 tr("Zip File (*.zip)"));
//    zip("C:/test.txt", "C:/test.zip");
//    unZip("C:/test.zip", "C:/test2.txt");
}

void QInstall::on_btnRemove_clicked()
{
    QList<QTableWidgetItem*> itmlist = ui->tblFileList->selectedItems();
    if (itmlist.size() <= 0)
        return;
    int nCurRow = itmlist[0]->row();

    ui->tblFileList->removeRow(nCurRow);
}

void QInstall::on_btnAdd_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(
                              this,
                              "Select one or more files to open",
                              "",
                              "All Files (*)");

    int nCurIndex = ui->tblFileList->rowCount();

    for(int i = 0 ; i < files.size() ; i++) {
        QTableWidgetItem *item = new QTableWidgetItem(files.at(i));
        ui->tblFileList->insertRow(i+nCurIndex);
        ui->tblFileList->setItem(i+nCurIndex,0,item);
        ui->tblFileList->setColumnWidth(0, ui->tblFileList->width());
    }
}

void QInstall::on_btnCreate_clicked()
{
    //make archive to append
    QString saveFile = "1.zip" ; //QFileDialog::getSaveFileName(this, "Select file to save","", "Zip File(*.zip)");
    QStringList list;
    for(int i = 0 ; i < ui->tblFileList->rowCount() ; i++) {
        QTableWidgetItem *item = ui->tblFileList->item(i,0);
        list.append(item->text());
    }

    if (saveFile.toLatin1() != ""){
    //get all the files from list
    //this should compress the archive with the md5 sum file inside of the archive.
    if(JlCompress::compressFiles(saveFile, list)){
        QMessageBox::information(this, "Notice", "Compression success.");
        QByteArray checksum = fileChecksum(saveFile);
     //   QString checkFile = saveFile.section(".",0,0) + ".md5";
      // QString checkFile = saveFile.section("md5sum.txt");

        QString checkFile = "md5sum.txt";
        if(ui->chkMd5->isChecked()) {
           // QFile file(checkFile);
            QFile file(checkFile);
            file.open(QIODevice::WriteOnly);
            file.write(checksum);
            file.close();
        }

        QStringList list;
        list.append(saveFile);
        list.append(checkFile);
        JlCompress::compressFiles("finalmd5.zip", list); // make double zip with included md5sum
    }

    //append to binary
#ifdef Q_OS_WIN
    if( ui->chkExe->isChecked() ) {
        QString exeFile = saveFile.section(".", 0,0) + ".exe";
        BYTE buff[4096];
        wchar_t wzcmd[4096] = {0};
        GetModuleFileNameA(NULL, (CHAR*)buff, sizeof(buff));
        QString cmd = QString("/c copy /b '%1'+'%2' '%3'").arg(QString::fromLocal8Bit((char*)buff)).arg(saveFile.replace("/","\\")).arg(exeFile.replace("/","\\"));
        cmd.replace("'", QString((char)34));
        cmd.toWCharArray(wzcmd);
        ShellExecuteA(NULL, "open", "cmd", cmd.toUtf8(), NULL, SW_HIDE);
    }
#endif


#ifdef Q_OS_UNIX
    //  QProcess process;
    //  cat main test.txt test.zip > combined
    //  process.start("cat", QStringList() << "1.zip > marker.txt >> QInstall > combined ");


    //make marker.txt file with "apple" or split string
    //QProcess::execute("echo apples > marker.txt");

        if( ui->chkExe->isChecked() ) {
            //copy the binary
        //    QString bname="Installer";
        }


   //     QString dir = QCoreApplication::applicationDirPath();

//    QProcess::execute( QString('cat finalmd5.zip >> marker.txt >> QInstall > combined') ); // might not be working yet
 //   QProcess::execute( "xterm -e 'cat finalmd5.zip >> marker.txt'" ); // might not be working yet
 //   QProcess::execute( "xterm -e 'cat marker.txt >> QInstall'" );
 //   QProcess::execute( "xterm -e 'cat QInstall > combined'" );
    QProcess::execute("bash", QStringList() << "-c" << "cp QInstall NEWInstaller; cat NEWInstaller marker.txt finalmd5.zip  > cInstall;chmod +x cInstall");
//rename binary after
#endif

        }
}

QByteArray fileChecksum(const QString &fileName)
{
    QFile f(fileName);
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(QCryptographicHash::Md5);
        if (hash.addData(&f)) {
            return hash.result();
        }
    }
    return QByteArray();
}

void QInstall::on_btnOpen_clicked()
{
//    QString file = QFileDialog::getOpenFileName(this, "Select file to open","", "Zip File(*.zip)");
//    ui->editZipFilePath->setText(file);
}

void QInstall::on_btnDecompress_clicked()
{

    QString saveFile = QFileDialog::getSaveFileName(this, "Select file to save","", "Zip File(*.zip)");


    QString zipFile = ui->editZipFilePath->text();
    if(zipFile == "")
        return;

    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                      "",
                                                      QFileDialog::ShowDirsOnly
                                                      | QFileDialog::DontResolveSymlinks);



    if(dir == "")
        return;

    QStringList list = JlCompress::getFileList(zipFile);
    JlCompress::extractFiles(zipFile, list, dir);
    QMessageBox::information(this, "Notice", "Decompress success.");
}

void QInstall::on_btnBrowse_clicked()
{
    #ifdef Q_OS_WIN
        QString file = QFileDialog::getOpenFileName(this, "Select file to open","", "Exe File(*.*)");
        ReadFromExeFile(file);

    #endif

    #ifdef Q_OS_UNIX
        QString file = QFileDialog::getOpenFileName(this, "Select file to open","", "Exe File(*)");
        linuxextract(file);

    #endif



      QString dir = QCoreApplication::applicationDirPath();
//
           ui->editFile->setText(file);
      QString zipFile = dir.toLatin1() + "/finalmd5.zip";

      QStringList list = JlCompress::getFileList(zipFile); //verify md5sum is present to validate the install file

      JlCompress::extractFiles(zipFile, list, dir);

      QByteArray checksum = fileChecksum("1.zip");
        ui->editFile->setText(checksum);

      QString zipFile1 = dir.toLatin1() + "/1.zip";
      QStringList list1 = JlCompress::getFileList(zipFile1);
      JlCompress::extractFiles(zipFile1, list1, dir);
   //   QMessageBox::information(this, "Notice", "Install success.");
      //get md5 from zip archive and compare sums


    for(int i = 0 ; i < list1.size() ; i++) {
        QTableWidgetItem *item = new QTableWidgetItem(list1.at(i));
        ui->filelist->insertRow(i);
        ui->filelist->setItem(i,0,item);
        ui->filelist->setColumnWidth(0, ui->filelist->width());
    }

}

void QInstall::on_btnInstall_clicked()
{

    QString files = QFileDialog::getOpenFileName(
                              this,
                              "Select one or more files to open",
                              "",
                              "All Files (*)");

    QString exe = files.toLatin1(); //ui->editFile->text();

    if(exe == "")
        return;

    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                      "",
                                                      QFileDialog::ShowDirsOnly
                                                      | QFileDialog::DontResolveSymlinks);
    if(dir == "")
        return;

#ifdef Q_OS_WIN
    ReadFromExeFile(exe);
#endif

#ifdef Q_OS_UNIX
    linuxextract(exe);
#endif


  //  QString zipFile = QCoreApplication::applicationDirPath() + "./finalmd5.zip";
    QString zipFile = dir.toLatin1() + "/finalmd5.zip";

    QStringList list = JlCompress::getFileList(zipFile); //verify md5sum is present to validate the install file

    // QString md5sum;
    //while ( !list.end() ) {
    //   md5sum =   list.pop_front();
    //}

    JlCompress::extractFiles(zipFile, list, dir);


    //md5 finder
//    QFile MyFile(file.toLatin1());
//    MyFile.open(QIODevice::ReadWrite);
//    QTextStream in (&MyFile);
//    QString line;
//    QStringList list;
//    QStringList nums;
//    QRegExp rx("[:]");
//    do {
//        line = in.readLine();
//        if (line.contains(":")) {
//            list = line.split(rx);
//            nums.append(list.at(1).toLatin1());
//        }
//    } while (!line.isNull());
//    MyFile.close();

    QByteArray checksum = fileChecksum(dir.toLatin1() + "1.zip");
    //  QString checkFile = saveFile.section(".",0,0) + "1.zip";


    QString zipFile1 = QCoreApplication::applicationDirPath() + "/1.zip";
    QStringList list1 = JlCompress::getFileList(zipFile1);
    JlCompress::extractFiles(zipFile1, list1, dir);
    QMessageBox::information(this, "Notice", "Install success.");
    //get md5 from zip archive and compare sums

}

#include "winfiles.cpp"
