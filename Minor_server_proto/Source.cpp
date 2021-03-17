#include <iostream>
#include <WS2tcpip.h>
#include <string>
//#include <boost/multiprecision/cpp_int.hpp>
//#include <boost/lexical_cast.hpp>

#pragma comment (lib, "WS2_32.lib")

using namespace std;
//using namespace boost::multiprecision;

// to do power shit
/*
uint512_t power_of(uint512_t user_1, int user_2)
{
	uint512_t power_b = 1;
	for (int i = 1; i <= user_2; i++)
	{
		power_b = power_b * user_1;
		//cout << "The power is now: " << power_b << endl;
	}

	return power_b;
}
*/


void main()
{
	// Initialize winsock
	WSADATA ws_data;
	WORD ver = MAKEWORD(2, 2);
	char e_test[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	char mod_test[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int e = 0;
	int mod = 0;

	const int ws_ok = WSAStartup(ver, &ws_data);
	if (ws_ok != 0)
	{
		cerr << "Can't Initialize winsock! Quiting" << endl;
		return;
	}

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);

	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl;
		return;
	}

	// Bind the socket to an ip address and port number
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell Winsock socket is for listening
	listen(listening, SOMAXCONN);

	// set master and slave
	fd_set master;

	FD_ZERO(&master);

	FD_SET(listening, &master);

	cout << "The server is setted up and works fully \n";

	while (true)
	{
		fd_set copy = master;

		const int socket_count = select(0, &copy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socket_count; i++)
		{
			SOCKET sock = copy.fd_array[i];

			if (sock == listening)
			{
				// accept a new connection
				SOCKET client = accept(listening, nullptr, nullptr);

				// add the new connection to the list of connected clients
				FD_SET(client, &master);

				cout << "Client is connected on #" << client << endl;
			}

			//accept a new message
			else
			{
				char buf[4096];
				ZeroMemory(buf, 4096);
				const int bytes_in = recv(sock, buf, 4096, 0);
				const char P[] = { "P" };
				const char K[] = { "K" };
				const char seperation[] = { "," };


				if (bytes_in <= 0)
				{
					// drop the client cause nothing
					closesocket(sock);
					FD_CLR(sock, &master);
					cout << "No connection with the client (client left) \n";
				}

				else
				{
					cout << "The output from sock #" << sock << " =" << buf << endl; // buf is the string send


					////////////////////////////////////// Find the e and the mod

					if (buf[0] == P[0] && buf[1] == K[0])
					{
						size_t found = string(buf).find(seperation);
						size_t found_2 = string(buf).find(seperation, found + 1);

						int k = 0;
						int n = 0;

						for (int j = 2; j <= found - 1; j++)
						{
							e_test[k] = buf[j];
							k++;
						}
						for (int j = found + 1; j <= found_2 - 1; j++)
						{
							mod_test[n] = buf[j];
							n++;
						}
						
						e = atoi(e_test);
						mod = atoi(mod_test);


						const char send_this[] = "";

						for (int i = 0; i <= master.fd_count; i++)
						{
							SOCKET out_sock = master.fd_array[i];
							if (out_sock != listening && out_sock != sock)
							{
								//send(out_sock, send_this, 4096, 0);
								send(sock, send_this, 4096, 0);
							}
						}

					}
					////////////////////////////////////// end find the e and the mod


					if(buf[0] != P[0] && buf[1] != K[0])
					{
					// send message to other clients

						for (int i = 0; i <= master.fd_count; i++)
						{
							SOCKET out_sock = master.fd_array[i];
							if (out_sock != listening && out_sock != sock)
							{
								/// ascii try
								string encrypted_str;

								for (int i = 0; i <= 25; i++)
								{
									int be = int(buf[i]);
									unsigned __int64 powe = pow(be, e);
									int enc = (powe % mod);
									cout << "enc = " << enc << endl;
									encrypted_str += toascii(enc);
								}
								cout << "try ascii stuff = " << encrypted_str << endl;

								//send(out_sock, encrypted_str.c_str(), encrypted_str.size(), 0);
								send(sock, encrypted_str.c_str(), encrypted_str.size(), 0);

								/*
								const int client = atoi(buf);
								const unsigned __int64 power_e = pow(client, e);
								//uint512_t power_e = power_of(client, e);

								const int encryption = (power_e % mod);
								const string str = to_string(encryption);
								//string str = boost::lexical_cast<string>(encryption);
								cout << "The encrypted sending = " << str << endl;

								send(out_sock, str.c_str(), str.size(), 0);
								send(sock, str.c_str(), str.size(), 0);
								*/
							}
						}
					}
				}
			}
		}

	}

	// Shutdown winsock
	WSACleanup();
}


