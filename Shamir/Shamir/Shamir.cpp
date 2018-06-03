#include "stdafx.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define _uint unsigned int

using namespace std;

// Расшириный Алгоритм Евклида
void extended_euclid(long a, long b, long *x, long *y, long *d)
{
	// calculates a * *x + b * *y = gcd(a, b) = *d 

	long q, r, x1, x2, y1, y2;

	if (b == 0)
	{
		*d = a, *x = 1, *y = 0;
		return;
	}

	x2 = 1, x1 = 0, y2 = 0, y1 = 1;

	while (b > 0)
	{
		q = a / b, r = a - q * b;
		*x = x2 - q * x1, *y = y2 - q * y1;
		a = b, b = r;
		x2 = x1, x1 = *x, y2 = y1, y1 = *y;
	}

	*d = a, *x = x2, *y = y2;
}

// Поиск Обратного элемента по умножению для функции взятия по модулю
long inverse(int element, _uint field_char)
/* computes the inverse of a modulo n */
{
	long a, n, d, x, y;
	a = (long) element;
	n = (long) field_char;

	extended_euclid(a, n, &x, &y, &d);

	if (d == 1)
	{
		return (int) x;
	}

	return -1;
}

// Определение простого числа
bool prime(_uint n) {
	long long i;
	n = (long long) n;
	for (i = 2; i <= sqrt(n); i++)
	{
		if (n % i == 0)
		{
			return false;
		}
	}
	return true;
}

// Перемешивание Фишера - Йетса модифицированное
void shuffle(_uint *array, int N)
{
	_uint i;
	_uint j;
	_uint tmp;

	srand(time(NULL));

	for (i = N - 1; i > 1; i--)
	{
		do
		{
			j = rand() % (i + 1);;
		}
		while (j == 0);

		tmp = array[j];
		array[j] = array[i];
		array[i] = tmp;
	}
}

// Операция взятия по модулю
int field(_uint field_char, int n, int parametr)
{
	// param = 0 => field_char << n 
	// param = 1 => -5
	// param = 2 => 2^(-1)
	
	if (parametr < 0 && parametr > 2)
	{
		cout << endl << "Wrong parametr for mod N operation";
		return -1;
	}
	else
	{
		if (parametr == 0)
		{
			n = n % field_char;
		}

		if (parametr == 1 && n < 0)
		{
			do
			{
				n = n + field_char;
			} 
			while (n < 0);
		}
		if (parametr == 2)
		{
			n = inverse(n, field_char);

			if (n < 0)
			{
				n = field(field_char, n, 1);
			}
		}
	}
	return n;
}

// Генерация коэффициентов для многочлена в переменную polynom
int generate_coef_polyniom(_uint field_char, _uint min_number_of_members, _uint *polynom) 
{
	_uint i;
	srand(time(NULL));

	for (i = 0; i < min_number_of_members; i++)
	{
		polynom[i] = rand() % field_char;
		cout << "polynom[" << i << "] = " << polynom[i] << endl;
	}
	return polynom[0];
}

// Вычисление секретов для участников в переменную calculated_values
void find_min_number_of_members_and_1_values(_uint field_char, _uint min_number_of_members, _uint *polynom, _uint number_of_secter_parts, _uint *calculated_values) 
{
	_uint i;
	_uint j;
	cout << endl;
	for (j = 1; j < (number_of_secter_parts + 1); j ++)
	{
		calculated_values[j] = 0;
		for (i = 0; i < min_number_of_members; i++)
		{
			calculated_values[j] = (calculated_values[j] + (polynom[i] * (int) pow(j, i))) % field_char;
		}
		cout << "f(" << j << ") = " << calculated_values[j] << endl;
	}
}

// Генерация случайных участников в переменную members
void generate_users(_uint field_char, _uint min_number_of_members, _uint number_of_secter_parts, _uint *members)
{
	_uint i;
	_uint *users_for_recover = NULL;
	users_for_recover = new _uint[number_of_secter_parts + 1];

	for ( i = 1; i < (number_of_secter_parts + 1); i++) 
	{
		users_for_recover[i] = i;
	}

	cout << endl << "List of users" << endl;

	shuffle(users_for_recover, number_of_secter_parts + 1);

	for (i = 1; i < (min_number_of_members + 1); i++)
	{
		members[i] = users_for_recover[i];
		cout << "member number: " << members[i] << endl;
	}
	delete[] users_for_recover;
}

// Восстановление секрета - мультипликативная часть
int recover_multiplicative_part_2(_uint field_char, _uint min_number_of_members, _uint *members, _uint j)
{
	int x = 1;
	_uint i;

	for (i = 1; i < (min_number_of_members + 1); i++)
	{
		if (i != j)
		{
			//xi = xi [*, i!=j] (rj / (rj - ri)  % mod N)
			x =  (x * (members[i] * field(field_char,(int)(members[i] - members[j]), 2))) % field_char;
		}
	}

	return x;
}

// Восстановление секрета - аддитивная часть
int recover_additive_part_1(_uint field_char, _uint min_number_of_members, _uint *members, _uint *calculated_values)
{
	_uint i;
	_uint x = 0;


	for (i = 1; i < (min_number_of_members + 1); i++)
	{
		x = (x + (calculated_values[members[i]] * recover_multiplicative_part_2(field_char, min_number_of_members, members, i))) % field_char;
	}

	return x;
}

int main()
{
	_uint field_char;					// Характеристика поля
	_uint min_number_of_members;		// Минимальное количество участников разделения секрета
	_uint number_of_secter_parts;		// Количество частей секрета
	_uint secret;						// Сгенерированный секрет
	_uint recovered_secret;				// Выщитанный секрет 
	_uint *polynom = NULL;				// Сгенерированные коэффициенты многочлена
	_uint *calculated_values = NULL;	// Вычесленные значения многочлена от всех участников
	_uint *members = NULL;				// Список участников для восстановления секрета
	bool p;								// Логическая проверка на простоту

	cout << endl << "Enter PRIME field characteristic = ";
	cin >> field_char;
	p = prime(field_char);

	// Проверка на простоту характеристики поля
	if (p == true)
	{
		cout << "Enter min number of members = ";
		cin >> min_number_of_members;
		cout << "Enter number of secret parts  = ";
		cin >> number_of_secter_parts;
		cout << endl;

		//Проверка входных параметров
		if (number_of_secter_parts > min_number_of_members)
		{
			polynom = new _uint[min_number_of_members];
			calculated_values = new _uint[number_of_secter_parts + 1];
			members = new _uint[number_of_secter_parts];

			secret = generate_coef_polyniom(field_char, min_number_of_members, polynom);
			find_min_number_of_members_and_1_values(field_char, min_number_of_members, polynom, number_of_secter_parts, calculated_values);
			generate_users(field_char, min_number_of_members, number_of_secter_parts, members);
			recovered_secret = recover_additive_part_1(field_char, min_number_of_members, members, calculated_values);
			cout << endl << "recovered secret = " << recovered_secret;
			cout << endl << "secret = " << secret << endl;

		}
		else
		{
			cout << endl << "ERROR! you have entered wrong values " << endl;
		}
	}
	else
	{
		cout << endl << "ERROR! you have entered not prime field parameter " << endl;
	}

	delete[] polynom;
	delete[] calculated_values;
	delete[] members;

	system("PAUSE");
	return 0;
}