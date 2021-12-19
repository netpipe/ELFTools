//#include <QCoreApplication>
//#include <QTest>
//#include "unit_test/aestest.h"
#include "qaesencryption.h"
#include <QCryptographicHash>
#include <QDebug>
#include <rsa/Rsa.h>
#include <stdio.h>


int main(int argc, char *argv[])
{
   // QCoreApplication a(argc, argv);

  //  AesTest test1;
   // QTest::qExec(&test1);


    cout << "RSA algorithm start......." << endl;
    string plainMsg="Hello combined aes and rsa encryption";

    if (argc > 1){
        plainMsg = argv[1];
    }

    Rsa rsaTester;
    string encryptedMsg=rsaTester.rsa_MSG_encryp(plainMsg);
  //  string decryptedMsg=rsaTester.rsa_MSG_decryp(encryptedMsg);
    BigInt e, n;

    rsaTester.publish_keys(e, n);

//    qInfo() << "exp: " << e << ", " << "key: " << n << endl;
//    qInfo() << "message:   " << plainMsg << endl;
//    qInfo() << "crypted:   " << encryptedMsg << endl;
//    qInfo() << "decrypted: " << decryptedMsg << endl;
    cout << "exp: " << e << ", " << "key: " << n << endl;
    cout << "message:   " << plainMsg << endl;
    cout << "crypted:   " << encryptedMsg << endl;
 //   cout << "decrypted: " << decryptedMsg << endl;


    QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);

    // QString inputStr("The Advanced Encryption Standard (AES), also known by its original name Rijndael " "is a specification forthe encryption of electronic data established by the U.S. " "National Institute of Standards and Technology (NIST) in 2001");
    QString inputStr(QString::fromStdString(encryptedMsg));
    QString key("123456789");
    QString iv("your-IV-vector");
    QByteArray hashKey = QCryptographicHash::hash(key.toLocal8Bit(), QCryptographicHash::Sha256);
    QByteArray hashIV =    QCryptographicHash::hash(iv.toLocal8Bit(), QCryptographicHash::Md5);

    QByteArray encrypted = QAESEncryption::Crypt(QAESEncryption::AES_256, QAESEncryption::CBC,inputStr.toLocal8Bit(), hashKey, hashIV); //... // Removal of Padding via Static function QString decodedString =

    QByteArray decodeText = encryption.decode(encrypted, hashKey, hashIV);
   // QString decodedString = QString(QAESEncryption::removePadding(decodeText));
    QString decodedString = QString(encryption.removePadding(decodeText));

   //decodedString == inputStr !! ```
    string decryptedMsg=rsaTester.rsa_MSG_decryp(encryptedMsg);

    qInfo() << "encrypted string" << encrypted;
    qInfo() << "decoded string" << decodedString;
    string finalstring = decodedString.toStdString();

   // QString finaldecode = QString::fromStdString( (string) rsaTester.rsa_MSG_decryp( decodedString.toStdString() ) );



    qInfo() << "decoded string" << QString::fromStdString(decryptedMsg);



    return 0;
}


