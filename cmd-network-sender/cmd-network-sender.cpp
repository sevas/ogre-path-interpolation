// cmd-network-sender.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <iostream>
#include <string>

#include <cmath>

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>

using boost::asio::ip::udp;

void write_float(udp::socket &_udpSocket, udp::endpoint &_udpReceiverPoint
				 , boost::system::error_code& _error, float _val)
{
	char arr[4];
	memcpy(arr, &_val, sizeof(_val));

	std::cout << "sending FLOAT " << _val << std::endl;
	//boost::asio::write(_socket, boost::asio::buffer(arr, sizeof(_val)), boost::asio::transfer_all(),  _error);

	_udpSocket.send_to(boost::asio::buffer(arr, sizeof(_val))
				     , _udpReceiverPoint
					 , 0
					 , _error);
}

int main(int argc, char* argv[])
{
    srand(time(NULL));
	try
	{
		boost::asio::io_service io_service;
		udp::resolver resolver(io_service);

		std::string ip(argv[1]);
		udp::resolver::query query(udp::v4(), ip, "8888");

		//udp::resolver::iterator iterator = resolver.resolve(query);

		udp::endpoint receiverEndpoint = *resolver.resolve(query);
		
		udp::socket udpSocket(io_service);
		udpSocket.open(udp::v4());

		boost::system::error_code error = boost::asio::error::host_not_found;
		

		std::cout << "connected" << std::endl;
		int i=0; float f = 0.0f;

		int pos;

		char c = 'a';
		std::stringstream s;

		s << "#";

		for (;;)
		{        
			write_float(udpSocket, receiverEndpoint, error, 50*cos(f));
			write_float(udpSocket, receiverEndpoint, error, 50*sin(f));
			write_float(udpSocket, receiverEndpoint, error, 0);

            if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error.

			int speed = 4;
			int time_split = 33;

            Sleep(time_split);


			f+= (1.0/(speed*time_split)) * (2*3.1416) ;
			if(f>2*3.1416)
            {
				f=0.0;
                //Sleep(5000);
            }
			


			

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

