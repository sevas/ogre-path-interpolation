// cmd-network-sender.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <iostream>

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>

using boost::asio::ip::tcp;

void write_float(tcp::socket &_socket, boost::system::error_code& _error, float _val)
{
	char arr[4];
	memcpy(arr, &_val, sizeof(_val));

	std::cout << "sending FLOAT " << _val << std::endl;
	boost::asio::write(_socket, boost::asio::buffer(arr, sizeof(_val)), boost::asio::transfer_all(),  _error);
}

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		boost::asio::io_service io_service;
		tcp::resolver resolver(io_service);


		tcp::resolver::query query("127.0.0.1", "8888");

		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		tcp::resolver::iterator end;

		tcp::socket socket(io_service);
		boost::system::error_code error = boost::asio::error::host_not_found;
		while (error && endpoint_iterator != end)
		{
			std::cout << "trying to connect " << std::endl;
			socket.close();
			socket.connect(*endpoint_iterator++, error);
		}
		if (error)
			throw boost::system::system_error(error);

		std::cout << "connected" << std::endl;
		int i=0; float f = 0.0f;
		char c = 'a';
		std::stringstream s;

		s << "#";

		for (;;)
		{        

			write_float(socket, error, f*10);
			write_float(socket, error, f*10);
			write_float(socket, error, f*10);


			if (error == boost::asio::error::eof)
				break; // Connection closed cleanly by peer.
			else if (error)
				throw boost::system::system_error(error); // Some other error.

			Sleep(1000);

			//    std::cout.write(buf.data(), len);
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	std::cout << "leaving ";
	int a;
	std::cin >> a;

	return 0;
}

