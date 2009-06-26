// cmd-network-receiver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;


void read_float(tcp::socket&_socket, boost::system::error_code &_error, float &_val)
{
	boost::array<char, 4> buf;
	std::size_t n = _socket.read_some(boost::asio::buffer(buf), _error);

	std::cout << "read " << n << " bytes (out of 4)" << std::endl;
	memcpy(&_val, buf.c_array(), 4);
}


int _tmain(int argc, _TCHAR* argv[])
{

	try
	{
		boost::asio::io_service io_service;

		tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 8888));

		boost::format fmt("(%.2f, %.2f, %.2f) [err : %d] \n");
		for (;;)
		{
			tcp::socket socket(io_service);
			std::cout << "waiting client" << std::endl;
			acceptor.accept(socket);

			std::cout << "got it" << std::endl;

			boost::system::error_code ignored_error;

			while (1)
			{
				boost::array<char, 4> buf;
				boost::system::error_code ec;

				float x, y, z;
				read_float(socket, ec, x);
				read_float(socket, ec, y);
				read_float(socket, ec, z);
				fmt % x % y  % z % ec;
				std::cout << fmt.str();

				if (ec)
					break;
			}
			std::cout << "socket error occured\n";
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}





	return 0;
}

