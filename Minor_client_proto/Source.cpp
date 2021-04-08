/// <README>
/// The client part of encryption minor
/// 
/// You can use the cmd to access to run the program
/// 
/// type in the following for the cmd
/// (path)\Minor_client_proto.exe, (path)sending_txt.txt, (path)recevieng_txt.txt, IPadress (of the server), port number (same as server)
/// </>

#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <WS2tcpip.h>
#include <istream>
#include <string>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <fstream>
#include <tuple>

#pragma comment(lib, "ws2_32.lib")

using namespace std;
using namespace boost::multiprecision;

int find_coprime(int coprime_1, int product_1)
{
	// also the encrypt number has to be 1 < encrypt < coprime_1
	int j = 0;
	int coprime_e[100] ;
	const int prime_num[] = { 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 169, 167, 173, 179, 181, 191, 193, 197, 199, 211, 
		223,227,229,233,239,241,251,257,263,269,271,277,281,283 ,293,307,311,313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,409 };
	int array_2[100] ;
	int array_3[100];

	for (int i = 0; prime_num[i] <= coprime_1 - 1; i++)
	{
		if (product_1 % prime_num[i] != 0)
		{
			array_2[i] = prime_num[i];
		}
		if (coprime_1 % prime_num[i] != 0)
		{
			array_3[i] = prime_num[i];
		}

		if (array_2[i] == array_3[i])
		{
			coprime_e[j] = array_2[i];
			j++;
		}
	}

	int e = coprime_e[0];
	if (e == 0)
	{
		e = coprime_e[1];
	}

	return e;
}

int func_product(int prime_1, int prime_2)
{
	// product of p_1 & p_2 that is the modules
	const int product_1 = prime_1 * prime_2;
	return product_1;
}

int rsa_encryption(int prime_1, int prime_2)
{
	// product of p_1 & p_2 that is the modules
	const int product_1 = { func_product(prime_1, prime_2) };

	// phi function
	// check the coprime with the product
	const int coprime_1 = (prime_1 - 1) * (prime_2 - 1);

	const int e = find_coprime(coprime_1, product_1);

	return e;
}

uint1024_t power_of(int user_1, int user_2)
{
	uint1024_t power_b = 1;
	uint1024_t en = user_1;
	for (int i = 1; i <= user_2; i++)
	{
		power_b = power_b * en;
	}

	return power_b;
}

void rsa_decryption(int prime_1, int prime_2, int encrypt, int product, char *encryption, int bytes_recv, string &encription1, string &decrypt_str)
{
	int k = 0;
	int d_array[200];

	const int coprime_1 = (prime_1 - 1) * (prime_2 - 1);

	// also the encrypt number has to be 1 < encrypt < coprime_1
	for (int i = 1; i <= coprime_1; i++)
	{

		double d_1 = (i * coprime_1) + 1;
		double d_2 = d_1 / encrypt;

		if (fmod(d_2, 1.0) == 0)
		{
			d_array[k] = d_2;
			k++;
		}
	}

	const int d = d_array[0];
	int l = 0;

	for(int i = 0; i <= bytes_recv; i++)
	{
		int num = 0;
		int exp = 1;

		for (int j = 0; j <= 7; j++)
		{
			if (encryption[l] == '1')
			{
				num += exp;
			}
			exp = exp * 2;
			l++;
		}

		encription1 += toascii(num);
		const int enc = num;
		const uint1024_t power_d = power_of(enc, d);
		const uint1024_t decryption = power_d % product;
		const string test_str = boost::lexical_cast<string>(decryption);
		decrypt_str += toascii(stoi(test_str));
	}
}

void write_txt(string encrypt, string decrypt, char* argv[])
{
	ofstream myfile;
	myfile.open(argv[2]);
	myfile << "The encrypted code = " + encrypt + "\n" + "The decryption =" + decrypt << endl;
	myfile.close();
}

void main(int argc, char* argv[])
{

	///////////// RSA encryption ////////////
	int prime_1 = 0;
	int prime_2 = 0;

	// pick two prime numbers
	cout << "Fill in two prime numbers, that are not the same! \n \n";
	cin >> prime_1 >> prime_2;

	const int product = { func_product(prime_1, prime_2) };
	const int encrypt = {rsa_encryption(prime_1, prime_2)};

	// this needs to be send towards the client or server
	cout << "[LOG] The public key = [" << encrypt << ", " << product << "] \n";

	///////////// RSA encryption ////////////

	string ip_address = argv[3];	// IP Adress
	const int port = stoi(argv[4]);	// listening port # on the server

	// Initialize winsock
	WSADATA data;
	WORD ver = MAKEWORD(2, 2);
	const int ws_result = WSAStartup(ver, &data);
	if (ws_result != 0)
	{
		cerr << "[LOG] Can't start Winsock, Err #" << ws_result << endl;
		return;
	}

	// create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cerr << "[LOG] Can't create socket, error #" << WSAGetLastError()<< endl;
		WSACleanup();
		return;
	}

	// Fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ip_address.c_str(), &hint.sin_addr);

	// connect to server
	const int conn_result = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (conn_result == SOCKET_ERROR)
	{
		cerr << "[LOG] Can't connect to server, error #" << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return;
	}

	// do_while looop to send and receive data
	char buf[4096];
	char *buffer;
	string user_input;
	int i = 0;
	int k = 0;
	string public_key = "PK" + to_string(encrypt) + "," + to_string(product) + ",";
	FILE* file;

	do
	{
		// prompt the user for some text
		if (i <= 0)
		{
			user_input = public_key;
			i++;
			const int send_result = send(sock, user_input.c_str(), user_input.size() + 1, 0);
			if (send_result != 0)
			{
				cerr << "[LOG] The public key did send! \n";
			}
		}
		
		// sending .txt file
		file = fopen(argv[1], "r");

		if (!file)
		{
			cerr << "[LOG] The File doesn't exist \n" << WSAGetLastError() << endl;
			closesocket(sock);
			WSACleanup();
			return;
		}

		cout << "[LOG] The file opened correctly! \n";

		fseek(file, 0, SEEK_END);
		const int size = ftell(file);
		fseek(file, 0, SEEK_SET);

		buffer = (char*)malloc(size + 1);
		fread(buffer, size, 1, file);
		fclose(file);

		const int send_result = send(sock, buffer, size, 0);
		free(buffer);

		// send the text

		if (send_result != SOCKET_ERROR)
		{
			cout << "[LOG] Sending is completed! \n";

			//wait for response
			ZeroMemory(buf, 4096);
			const int bytes_received = recv(sock, buf, 4096, 0);
			if (bytes_received > 0)
			{
				// echo response to console
				if (*buf != 0)
				{
					string encryption1, decryption1;
					rsa_decryption(prime_1, prime_2, encrypt, product, buf, bytes_received, encryption1, decryption1);
					write_txt(encryption1, decryption1, argv);
					break;
				}
			}
		}
		k++;
	}while(user_input.size() >= 0);

	
	// Clean up the stuff
	closesocket(sock);
	WSACleanup();
}