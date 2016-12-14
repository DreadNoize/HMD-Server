#ifndef VRSYS_HMD_SERVER
#define VRSYS_HMD_SERVER

#include <array>
#include <chrono>

#include "message.hpp"
#include "zmq.hpp"

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds milliseconds;

struct Server {
	Server(std::string address)
	  : m_ctx(1)
	  , m_socket(m_ctx, ZMQ_PUB)
	  , m_packets_since_last_print(0)
	  , m_last_print_time(Clock::now())
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
		++m_packets_since_last_print;

		Clock::time_point now = Clock::now();
		__int64 elapsed_ms(std::chrono::duration_cast<milliseconds>(now - m_last_print_time).count());

		if (elapsed_ms > 1000) {
			std::cout << "Sent " << m_packets_since_last_print << " packets in a second." << std::endl;
			m_packets_since_last_print = 0;
			m_last_print_time = Clock::now();
		}

		return 1;
	}

	zmq::context_t m_ctx;
	zmq::socket_t m_socket;
	int m_packets_since_last_print;
	Clock::time_point m_last_print_time;
};

#endif
