#include "Rsa.h"

int main(int argc, char *argv[])
{
    cout << "RSA algorithm start......." << endl;

    string plainMsg="Hello";

    if (argc > 1){

        plainMsg = argv[1];

    }

    Rsa rsaTester;

    string encryptedMsg=rsaTester.rsa_MSG_encryp(plainMsg);

    string decryptedMsg=rsaTester.rsa_MSG_decryp(encryptedMsg);

    BigInt e, n;

    rsaTester.publish_keys(e, n);

    cout << "exp: " << e << ", " << "key: " << n << endl;

    cout << "message:   " << plainMsg << endl;

    cout << "crypted:   " << encryptedMsg << endl;

    cout << "decrypted: " << decryptedMsg << endl;

    return 0;
}

