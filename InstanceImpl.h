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

#ifndef INSTANCEIMPL_H_
#define INSTANCEIMPL_H_

#include <Instance.h>
#include <Channel.h>
#include <ChannelProxy.h>
#include <SessionBase.h>
#include <JsonXValue.h>
#include <Socket.h>

#include <memory>
#include <thread>

namespace FreeAX25_TCPServer {

class InstanceImpl: public FreeAX25::Runtime::SessionBase {
public:
	/**
	 * Default constructor, needed for dictionary support.
	 */
	InstanceImpl();

	/**
	 * Constructor.
	 * @param instance_descriptor Instance descriptor
	 */
	InstanceImpl(const FreeAX25::Runtime::Instance& instance_descriptor);

	/**
	 * Destructor.
	 */
	~InstanceImpl();

	/**
	 * Initialize this instance.
	 */
	void init();

	/**
	 * Start this instance.
	 */
	void start();

	/**
	 * Get the interface parameter.
	 * @return Interface parameter.
	 */
	const std::string& ifc() const { return m_ifc; }

	/**
	 * Get the port parameter.
	 * @return Port parameter.
	 */
	int port() const { return m_port; }

	/**
	 * Get the backof parameter.
	 * @return Backof parameter.
	 */
	int backof() const { return m_backof; }

	/**
	 * Get the bufsize parameter.
	 * @return Bufsize parameter.
	 */
	size_t bufsize() const { return m_bufsize; }

private:
	void _run() noexcept;

	const FreeAX25::Runtime::Instance& m_instance_descriptor;
	FreeAX25::Runtime::Channel         m_broker;
	std::string                        m_ifc{};
	int                                m_port{-1};
	int                                m_backof{-1};
	size_t                             m_bufsize{0};
	std::thread                        m_thread{};
	SocketIO::Socket                   m_socket{};
};

} /* namespace FreeAX25_TCPServer */

#endif /* INSTANCEIMPL_H_ */
