#ifdef ENCRYPTION
#include "Rsa.h"
#include <QString>
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <stddef.h>


std::string Rsa::rsa_MSG_encryp(std::string& msg){



    QString cryptMsg="";
    std::string outMsg;
    BigInt crypted_msg, e, n;


    for (int i = 0; i < msg.length(); i++)
    {
        BigInt message(msg[i]);
        Rsa :: rsa_encryp(message, crypted_msg);
        cryptMsg += BigInt2Str(crypted_msg);
        cryptMsg += " ";
    }

    outMsg=cryptMsg.toLocal8Bit().constData();
    return outMsg;
}

std::string Rsa::rsa_MSG_decryp(std::string&encryptedMsg){

    std::string temp_saver,total;
    vector<string> saver;
    temp_saver.clear();

    for(int i=0; i<encryptedMsg.size();i++)
    {
        if (encryptedMsg[i]!=' ')
            temp_saver +=encryptedMsg[i];
        else
        {
            saver.push_back(temp_saver);
            temp_saver.clear();
        }
    }

    BigInt result;
    total.clear();

    for(int i=0; i<saver.size();i++)
    {
        BigInt message(saver[i]);
        Rsa :: rsa_decrypt(result, message);
        total += result.toInt();
    }

    return total;

}



BigInt::~BigInt()
{
    value.clear();
}

BigInt::BigInt(const BigInt& number){
    value = number.value;
    sign = number.sign;
}

BigInt::BigInt(int number){
	if (number == 0){
        value.push_back(0);
        sign = true;
	}
	else
	{
		if (number < 0)
		{
            sign = false;
			number *= (-1);
		}
		else
		{
            sign = true;
		}
		while (number != 0)
		{
            value.push_back(number % 10);
			number /= 10;
		}
	}
}

BigInt::BigInt(std::string x){
    int size = x.size();
    for(int i=size-1; i>=0; i--)
    {
        value.push_back(x[i]-'0');
    }

    sign=true;

}

QString BigInt2Str(BigInt number){

   QString temp;
  for(int i=number.value.size()-1; i>=0; i--)
  {
      temp+=QString::number(number.value[i],10);
  }
   return temp;
}

BigInt BigInt::operator=(const BigInt& number){
	if (this == &number)
		return *this; 
    value = number.value;
    sign = number.sign;
	return *this;
}

int BigInt::toInt()
{
    if (value.size()>8)
        return 0;
    else
    {
        int temp=0;
        for (int i=value.size()-1;i>=0;i--)
        {
            temp*=10;
            temp+=value[i];
        }
        return temp;
    }
}

#pragma region DATA

int cmp(const data_type &a, const data_type &b) {

	if (a.size() != b.size())
		return a.size() - b.size();
	for (int i = a.size()-1, j = b.size()-1; i >= 0; i--, j--)
	{
		if (a[i] != b[j])
			return a[i] - b[j];
	}

	return 0;
}

bool operator < (const data_type &a, const data_type&b) {
	return cmp(a, b) < 0;
}

bool operator > (const data_type &a, const data_type &b) {
	return cmp(a, b) > 0;
}

bool operator == (const data_type &a, const data_type &b) {
	return cmp(a, b) == 0;
}

bool operator != (const data_type &a, const data_type &b) {
	return cmp(a, b) != 0;
}
bool operator <= (const data_type &a, const data_type &b) {
	return cmp(a, b) <= 0;
}

bool operator >= (const data_type &a, const data_type &b) {

	return cmp(a, b) >= 0;

}

data_type operator+(const data_type& left, const  data_type& right){

    data_type temp;
	int left_size = left.size(),
		right_size = right.size();
	int i = 0;
	while (i < left_size || i < right_size)
	{
        temp.push_back((i < left_size ? left[i] : 0) + (i < right_size ? right[i] : 0));
		i++;
	}
    for (i = 0; i + 1 != temp.size(); i++) {
        temp[i + 1] += temp[i] / 10;
        temp[i] %= 10;
	}
    if (temp[i] >= 10) {
        temp.push_back(temp[i] / 10);
        temp[i] %= 10;
	}

    return temp;

}

data_type operator-(const data_type& left, const  data_type& right){

    data_type temp;
	if (left < right) {
		throw 1;
	}

	if (left == right)
	{

        temp.push_back(0);

        return temp;

	}
	for (int i = 0;  i< left.size();i++)
	{

        temp.push_back(left[i] - (i < right.size() ? right[i] : 0));

	}
	
    for ( int  i = 0;i + 1 < temp.size(); i++) {

        temp[i] += 10;

        temp[i + 1]-=1;

        temp[i + 1] += temp[i] / 10 ;

        temp[i] %= 10;

	}

    data_type::iterator i = temp.end() - 1;

    if (temp.size() > 1)

        while (temp.size() && *i == 0)
		{

            i = temp.erase(i) - 1;

		}

    return temp;

}

data_type operator*(const data_type& left, const  data_type& right){

    data_type temp;

    temp.resize(left.size() + right.size() + 1, 0);

	int j = 0;

	for (int i = 0;	i != left.size(); i++) 
	{
		j = i;
		for ( int g = 0;	g != right.size(); g++, j++) 
		{
            temp[j] += left[i] * right[g];
            temp[j + 1] += temp[j] / 10;
            temp[j] %= 10;
		}
	}

    data_type::iterator i = temp.end() - 1;

    while (temp.size() > 1 && *i == 0) {
        i = temp.erase(i) - 1;
	}

    return temp;
}

data_type operator/(const data_type& left, const  data_type& right){

    data_type temp;
	if (left < right)
	{
        temp.push_back(0);
        return temp;
	}
	if (!right.size())
		throw 1;
	data_type a = left, b = right;

	int x = 0;

	while (a >=b) 
	{
        b.insert(b.begin(), 0);
		x++;
	}

    b.erase(b.begin());
	data_type edinica;
	edinica.push_back(1);

	while (x--) {
		while (a >= b) {
			a = a - b;
            temp = temp + edinica;
		}
        temp.insert(temp.begin(),0);
        b.erase(b.begin());
	}

    temp.erase(temp.begin());
    if (temp.size() == 0)
        temp.push_back(0);
    return temp;
}

data_type operator%(const data_type& left, const  data_type& right){

	if (left < right)
		return left;
	int x = 0;

	data_type a = left, b = right;

	while (a >= b) {
        b.insert(b.begin(), 0);
		x++;
	}

    b.erase(b.begin());

    while (x--) {
		while (a >= b) {
			a = a - b;
		}
        b.erase(b.begin());
	}
	return a;
}


#pragma endregion


bool operator==(const BigInt& left, const BigInt& right)
{
    if (left.sign != right.sign)
		return false;
    if (left.value==right.value)
        return true;
    else
		return false;
}

bool operator!=(const BigInt& left, const BigInt& right)
{
	return !(left == right);
}

bool operator<(const BigInt& left, const BigInt& right)
{
    if (left.sign != right.sign)
	{
        if (left.sign == true)
			return false;
		else
			return true;
	}

    if (left.sign == true)
	{
        if (left.value < right.value)
			return true;
		else

			return false;
	}

    if (left.sign == false)
	{
        if (left.value>right.value)
			return true;
		else
			return false;
	}
}

bool operator>(const BigInt& left, const BigInt& right)
{
	return (right < left);
}

ostream& operator<<(ostream& out, const BigInt& number)
{
    if (!number.sign)
		out << "-";
    for (int i = number.value.size() - 1; i >= 0; i--)
        out <<(int) number.value[i];
	return out;
}


BigInt operator+(const BigInt& left, const BigInt& right){

    BigInt temp;

    if (left.sign != right.sign)
	{
        if (left.sign)
		{
            if (right.value == left.value)
			{
				return zero;
			}
            if (right.value < left.value)
			{
                temp.sign = true;

                temp.value = left.value - right.value;

                return temp;
			}
			else
			{
                temp.sign = false;
                temp.value = right.value - left.value;
                return temp;
			}
		}
		else
		{
            if (right.value == left.value)
			{
				return zero;
			}
            if (right.value > left.value)
			{
                temp.sign = true;
                temp.value = right.value - left.value;
                return temp;
			}
			else
			{
                temp.sign = false;
                temp.value = left.value - right.value;
                return temp;
			}
		}
	}
	else
	{
        if (!left.sign)
		{
            temp.sign = false;
            temp.value = left.value + right.value;
            return temp;
		}
        if (left.sign)
		{
            temp.sign = true;
            temp.value = left.value + right.value;
            return temp;
		}
	}
	
}
BigInt operator-(const BigInt& left, const BigInt& right){

    BigInt temp;

    if (left.sign == right.sign)
	{
        if (left.value == right.value)
		{
			return zero;
		}
        if (left.sign)
		{
            if (left.value > right.value)
			{
                temp.sign = true;
                temp.value = left.value - right.value;
                return temp;
			}
			else
			{
                temp.sign = false;
                temp.value = right.value - left.value;
                return temp;
			}
		}
		else
		{
            if (left.value > right.value)
			{
                temp.sign = false;
                temp.value = left.value - right.value;
                return temp;
			}
			else
			{
                temp.sign = true;
                temp.value = right.value - left.value;
                return temp;
			}
		}
	}
	else
	{
        if (left.sign)
		{
            temp.sign = true;
            temp.value = left.value + right.value;
            return temp;
		}
		else
		{
            temp.sign = false;
            temp.value = left.value + right.value;
            return temp;
		}
	}
}
BigInt operator*(const BigInt& left, const BigInt& right)
{
    BigInt temp;
    if (left.sign != right.sign)
        temp.sign = false;
    temp.value = left.value * right.value;
    return temp;
}

BigInt operator/(const BigInt& left, const BigInt& right){

    BigInt temp;
    if (left.sign != right.sign)
        temp.sign = false;
    temp.value = left.value / right.value;
    return temp;
}

BigInt operator%(const BigInt& left, const BigInt& right){

    BigInt temp;
    temp.value = left.value % right.value;
    return temp;
}

BigInt modul_mult(BigInt &a, BigInt &b, BigInt &modul){

	if (b == zero)
		return one;
	if (b == one)
		return a;
    BigInt trans, supp;
	trans = b % two;
	if (trans == zero)
	{
		supp = b / two;
		trans = modul_mult(a, supp, modul);
		supp = trans*two;
		supp = supp%modul;
		return supp;
	}

	supp = b - one;
	trans = modul_mult(a, supp, modul);
    supp = trans + a;
	supp = supp%modul;
	return supp;
}

BigInt fast_power(BigInt &num, BigInt &power, BigInt &modulus){

	if (power == zero)
		return one;
	if (power == one)
		return num;

    BigInt trans, supp;
	trans = power % two;
	if (trans == zero)
	{
		supp = power / two;
		trans = fast_power(num, supp, modulus);
		supp = modul_mult(trans, trans, modulus);
		supp = supp%modulus;

		return supp;
	}

	supp = power - one;
	trans = fast_power(num, supp, modulus);
	supp = modul_mult(trans, num, modulus);
	supp = supp%modulus;
	return supp;
}
Rsa::Rsa()
{
    BigInt p, q, fi;
    generat_prime(p);
    generat_prime(q);
	n = p*q;
	fi = (p - one)*(q - one);
    e = (BigInt)65537;
	d = euclid(e, fi);
}

Rsa::Rsa(BigInt dd, BigInt nn)
{
    d = dd;
    n = nn;
}

Rsa::~Rsa()
{

}

BigInt Rsa::euclid(const BigInt& exp, const BigInt& mod)
{
    BigInt a(exp), b(mod);
    BigInt x1(zero), x2(one), y1(one), y2(zero);
    BigInt q, r, x, y;
	while (b != zero){
		q = a / b;
		r = a - q*b;
		x = x2 - q*x1;
		y = y2 - q*y1;
		a = b;
		b = r;
		x2 = x1;
		x1 = x;
		y2 = y1;
		y1 = y;
	}
	
	if (x2 < zero)

		x2 = x2 + mod;

	return x2;

}

void Rsa::randomiser(BigInt &num, int Digits){

	num = one;
	int new_rand;
    int a=time(NULL)%40;
    for(int i=0;i<a;i++){
        rand();
    }

	for (int i = 0; i < Digits; i++){
		new_rand = rand() % 9 + 1;
		num = num + num + num*new_rand;
	}

	num = num + num;
	num = num + one;
}

void Rsa::range_randomiser(BigInt &num, const BigInt &top){

    int Digit = (rand() % (length-1)) + 1;
    randomiser(num, Digit);

}

bool Rsa::is_prime(BigInt &num){

    BigInt num_minus_one;
	num_minus_one = num - one;
    BigInt temp, trans, random, mod, supp;
	trans = num_minus_one;
	temp = num_minus_one%two;

	while (temp == zero)
	{
		trans = trans / two;
		temp = trans%two;
	}

    for (int i = 0; i < probability; i++)
	{
		temp = trans;
        range_randomiser(random, num_minus_one);
		mod = fast_power(random, temp, num);

		while (temp != num_minus_one && mod != one && mod != num_minus_one)
		{
			mod = modul_mult(mod, mod, num);
			temp = temp*two;
		}
		supp = temp%two;

		if (mod != num_minus_one && supp == zero)
		{
			return false;
		}

	}
	return true;
}


void Rsa::generat_prime(BigInt &num){

	bool supp = false;
	while (!supp){
        randomiser(num, length);
        supp = is_prime(num);
	}
}

void Rsa::rsa_encryp(BigInt & in_msg, BigInt& encrypted_msg){
    encrypted_msg = fast_power(in_msg, e, n);

}

void Rsa::rsa_encryp(BigInt &  in_msg, BigInt & encrypted_msg,BigInt & exp,BigInt & mod){
    encrypted_msg = fast_power(in_msg, exp, mod);

}


void Rsa::rsa_decrypt(BigInt& out_msg, BigInt& encrypted_msg){
    out_msg = fast_power(encrypted_msg, d, n);

}

void Rsa::publish_keys(BigInt& a, BigInt& b){
    a = d;
	b = n;
}

#endif
