#pragma once
#ifdef ENCRYPTION
#include <QString>
#include <vector>
#include <iostream>

using namespace std;

typedef vector<unsigned char>  data_type;

class BigInt
{

public:

    data_type value;

    BigInt()
    {
        sign = true;
    }

    ~BigInt();

    BigInt(const BigInt&);

    BigInt(int);

    friend QString BigInt2Str(BigInt number);

    BigInt operator=(const BigInt&);

    BigInt(std::string x);

	int toInt();

    friend ostream& operator<<(ostream&, const BigInt&);

    friend bool operator==(const BigInt&, const BigInt&);

    friend bool operator!=(const BigInt&, const BigInt&);

    friend bool operator>(const BigInt&, const BigInt&);

    friend bool operator<(const BigInt&, const BigInt&);

    friend BigInt operator+(const BigInt&, const BigInt&);

    friend BigInt operator-(const BigInt&, const BigInt&);

    friend BigInt operator*(const BigInt&, const BigInt&);

    friend BigInt operator/(const BigInt&, const BigInt&);

    friend BigInt operator%(const BigInt&, const BigInt&);

    friend BigInt modul_mult(BigInt&, BigInt&, BigInt&);

    friend BigInt fast_power(BigInt&, BigInt&, BigInt&);

protected:

    bool sign;
};

const BigInt zero(0);

const BigInt one(1);

const BigInt two(2);

const int length(8);

const int probability(5);

class Rsa
{

public:

    Rsa();

    Rsa(BigInt p, BigInt q);

    ~Rsa();

    std::string rsa_MSG_encryp(std::string&);

    std::string rsa_MSG_decryp(std::string&);

    void rsa_encryp(BigInt&, BigInt&);

    void rsa_encryp(BigInt&, BigInt&,BigInt & exp,BigInt & mod);

    void rsa_decrypt(BigInt&, BigInt&);

    void publish_keys(BigInt&, BigInt&);

private:

    BigInt euclid(const BigInt&, const BigInt&);

    void randomiser(BigInt &, int);

    void range_randomiser(BigInt &, const BigInt &);

    bool is_prime(BigInt &);

    void generat_prime(BigInt &);
	
    BigInt e;

    BigInt d;

    BigInt n;

};

#endif
