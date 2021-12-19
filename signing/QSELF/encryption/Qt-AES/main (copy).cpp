#include <QCoreApplication>
#include <QTest>
#include "unit_test/aestest.h"
#include "qaesencryption.h"
#include <QCryptographicHash>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

  //  AesTest test1;
   // QTest::qExec(&test1);
    QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);

    QString inputStr("The Advanced Encryption Standard (AES), also known by its original name Rijndael " "is a specification forthe encryption of electronic data established by the U.S. " "National Institute of Standards and Technology (NIST) in 2001");
    QString key("123456789");
    QString iv("your-IV-vector");
    QByteArray hashKey = QCryptographicHash::hash(key.toLocal8Bit(), QCryptographicHash::Sha256);
    QByteArray hashIV =    QCryptographicHash::hash(iv.toLocal8Bit(), QCryptographicHash::Md5);

    QByteArray encrypted = QAESEncryption::Crypt(QAESEncryption::AES_256, QAESEncryption::CBC,inputStr.toLocal8Bit(), hashKey, hashIV); //... // Removal of Padding via Static function QString decodedString =

    QByteArray decodeText = encryption.decode(encrypted, hashKey, hashIV);
   // QString decodedString = QString(QAESEncryption::removePadding(decodeText));
    QString decodedString = QString(encryption.removePadding(decodeText));

   //decodedString == inputStr !! ```

    qInfo() << "encrypted string" << encrypted;
    qInfo() << "decoded string" << decodedString;



    return 0;
}


