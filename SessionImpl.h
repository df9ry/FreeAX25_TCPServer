/*
 Project FreeAX25_TCPServer
 Copyright (C) 2015  tania@df9ry.de

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as
 published by the Free Software Foundation, either version 3 of the
 License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SESSIONIMPL_H_
#define SESSIONIMPL_H_

#include <Instance.h>
#include <SessionBase.h>
#include <Channel.h>
#include <ChannelProxy.h>
#include <JsonXValue.h>
#include <Socket.h>

#include <vector>
#include <thread>
#include <mutex>

namespace FreeAX25_TCPServer {

class InstanceImpl;

class SessionImpl: public FreeAX25::Runtime::SessionBase {
	friend class InstanceImpl;

public:
	/**
	 * Default constructor. Needed for dictionary support.
	 */
	SessionImpl();

	/**
	 * Constructor
	 * @param instance_impl The instance this session belongs to.
	 */
	SessionImpl(const InstanceImpl& instance_impl);

	/**
	 * Destructor.
	 */
	~SessionImpl();

	/**
	 * Start session.
	 * @param broker The broker server.
	 */
	void start(
			FreeAX25::Runtime::ChannelProxy broker,
			SocketIO::Socket&& socket);

private:
	void _run() noexcept;
	void _exit() noexcept;

	void onUplinkClose(std::unique_ptr<JsonX::Object>&&);
	void onUplinkReceive(std::unique_ptr<JsonX::Object>&&,
			FreeAX25::Runtime::MessagePriority);
	std::unique_ptr<JsonX::Object> onUplinkCtrl(std::unique_ptr<JsonX::Object>&&);

	const InstanceImpl&        m_instance;
	FreeAX25::Runtime::Channel m_uplink;
	SocketIO::Socket           m_socket{};
	std::vector<uint8_t>       m_buffer{};
	std::thread                m_thread{};
	bool                       m_exiting{false};
	std::mutex                 m_mutex{};
};

} /* namespace FreeAX25_TCPServer */
#endif /* SESSIONIMPL_H_ */

