#pragma once

#ifndef VRSYS_HMD_SERVER
#define VRSYS_HMD_SERVER

#include <zmq.hpp>
#include <array>
#include "message.hpp"

struct Server {
	Server(std::string address)
	  : m_ctx(1)
	  , m_socket(m_ctx, ZMQ_PUB)
	{
		uint32_t hwm = 1;
		m_socket.setsockopt(ZMQ_SNDHWM, &hwm, sizeof(hwm));

		m_socket.bind(address.c_str());
	}

	bool send(Message msg) {

		// DEBUG
		/*
		std::cout << matrix[0] << " " << matrix[4] << " " << matrix[8] << " " << matrix[12] << std::endl;
		std::cout << matrix[1] << " " << matrix[5] << " " << matrix[9] << " " << matrix[13] << std::endl;
		std::cout << matrix[2] << " " << matrix[6] << " " << matrix[10] << " " << matrix[14] << std::endl;
		std::cout << matrix[3] << " " << matrix[7] << " " << matrix[11] << " " << matrix[15] << std::endl;
		*/
		size_t len = sizeof(Message);

		zmq::message_t message(len);
		memcpy(message.data(), msg.data(), len);
		m_socket.send(message);
		std::cout << ".";
		return 1;
	}
	zmq::context_t m_ctx;
	zmq::socket_t m_socket;
};

#endif
