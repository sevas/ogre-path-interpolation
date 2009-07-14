// cmd-network-receiver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

#include <boost/asio.hpp>

using boost::asio::ip::udp;


void read_float(udp::socket&_socket, udp::endpoint &_udpRemotePoint
				, boost::system::error_code &_error, float &_val)
{
	boost::array<char, 4> buf;
	std::size_t n = _socket.receive_from( boost::asio::buffer(buf)
										, _udpRemotePoint, 0, _error);

	std::cout << "read " << n << " bytes (out of 4)" << std::endl;
	memcpy(&_val, buf.c_array(), 4);
}


int _tmain(int argc, _TCHAR* argv[])
{

	try
	{
		boost::asio::io_service io_service;

		udp::endpoint udpRemotePoint = udp::endpoint(udp::v4(), 8888);
		udp::socket  udpSocket = udp::socket(io_service, udpRemotePoint);



		boost::format fmt_err("(%.2f, %.2f, %.2f) [err : %d] \n");
		boost::format fmt("(%.2f, %.2f, %.2f)\n");
		for (;;)
		{
			boost::system::error_code ignored_error;

			while (1)
			{
				boost::array<char, 4> buf;
				boost::system::error_code ec;

				float x, y, z;
				read_float(udpSocket, udpRemotePoint, ec, x);
				read_float(udpSocket, udpRemotePoint, ec, y);
				read_float(udpSocket, udpRemotePoint, ec, z);

				if (ec)
				{
					fmt_err % x % y  % z % ec;
					std::cout << fmt_err.str();
				}
				else
				{
					fmt % x % y  % z;
					std::cout << fmt.str();
				}
				

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

