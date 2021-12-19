#include "csslsocket.h"

#include <QSslCertificate>
#include <QSslKey>
#include <openssl/conf.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <QDebug>
#include <QFile>
#include "define.h"

void save_certificate(const QString &filename, const QSslCertificate &crt)
{
    QFile k(filename);
    k.open(QIODevice::WriteOnly);
    k.write(crt.toPem());
    k.close();
}

void save_key(const QString &filename, const QSslKey &key)
{
    QFile k(filename);
    k.open(QIODevice::WriteOnly);
    k.write(key.toPem());
    k.close();
}

CSslSocket::CSslSocket(QObject *parent) : QSslSocket(parent)
{
    EVP_PKEY * pkey = nullptr;
    RSA * rsa = nullptr;
    X509 * x509 = nullptr;
    X509_NAME * name = nullptr;
    BIO * bp_public = nullptr, * bp_private = nullptr;
    const char * buffer = nullptr;
    long size;

    pkey = EVP_PKEY_new();

    q_check_ptr(pkey);

    // Generate RAS key
    rsa = RSA_generate_key(2048, RSA_F4, nullptr, nullptr);

    q_check_ptr(rsa);

    EVP_PKEY_assign_RSA(pkey, rsa);

    x509 = X509_new();

    q_check_ptr(x509);

    ASN1_INTEGER_set(X509_get_serialNumber(x509), 1);

    X509_gmtime_adj(X509_get_notBefore(x509), 0); // not before current time
    X509_gmtime_adj(X509_get_notAfter(x509), 31536000L); // not after a year from this point
    X509_set_pubkey(x509, pkey);

    name = X509_get_subject_name(x509);

    q_check_ptr(name);

    X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (unsigned char *)"US", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, (unsigned char *)"My Organization", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char *)"My Common Name", -1, -1, 0);
    X509_set_issuer_name(x509, name);
    X509_sign(x509, pkey, EVP_sha1());

    // Save private key
    bp_private = BIO_new(BIO_s_mem());

    q_check_ptr(bp_private);

    if(PEM_write_bio_PrivateKey(bp_private, pkey, nullptr, nullptr, 0, nullptr, nullptr) != 1)
    {
        EVP_PKEY_free(pkey);
        X509_free(x509);
        BIO_free_all(bp_private);
        qFatal("PEM_write_bio_PrivateKey");
    }

    // Save public key
    bp_public = BIO_new(BIO_s_mem());
    q_check_ptr(bp_public);

    if(PEM_write_bio_X509(bp_public, x509) != 1)
    {
        EVP_PKEY_free(pkey);
        X509_free(x509);
        BIO_free_all(bp_public);
        BIO_free_all(bp_private);
        qFatal("PEM_write_bio_PrivateKey");
    }
    size = BIO_get_mem_data(bp_public, &buffer);
    //qDebug() << "@@ Public key @@" << buffer;

    q_check_ptr(buffer);

    QSslCertificate cert = QSslCertificate(QByteArray(buffer, size));
    setLocalCertificate(cert);
    //save_certificate(CLIENT_PEM, cert);
    save_certificate(CLIENT_CA_PEM, cert);

    //qDebug() << "@@ Certificate @@" << buffer;

    if(localCertificate().isNull())
    {
        qFatal("Failed to generate a random client certificate");
    }
    size = BIO_get_mem_data(bp_private, &buffer);

    q_check_ptr(buffer);
    QSslKey private_key = QSslKey(QByteArray(buffer, size), QSsl::Rsa);
    setPrivateKey(private_key);
    //save_key(CLIENT_KEY, private_key);
    //qDebug() << "@@ Private key @@" << buffer;

    if(privateKey().isNull())
    {
        qFatal("Failed to generate a random private key");
    }

    EVP_PKEY_free(pkey); // this will also free the rsa key
    X509_free(x509);
    BIO_free_all(bp_public);
    BIO_free_all(bp_private);
}

