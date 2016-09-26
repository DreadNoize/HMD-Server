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

	bool send(Message* msg) {
		size_t len = sizeof(Message);

		zmq::message_t message(len);
		memcpy(message.data(), msg, len);
		m_socket.send(message);
		std::cout << ".";
		return 1;
	}
	zmq::context_t m_ctx;
	zmq::socket_t m_socket;
};

#endif
