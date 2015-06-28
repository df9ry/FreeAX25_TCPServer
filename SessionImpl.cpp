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

#include <Environment.h>
#include <ServerEndPoint.h>
#include <UUID.h>

#include "SessionImpl.h"
#include "InstanceImpl.h"

#include <string>

using namespace std;
using namespace FreeAX25::Runtime;

namespace FreeAX25_TCPServer {

SessionImpl::SessionImpl():
		m_instance{},
		m_uplink{ SessionBase::m_pointer }
{
}

SessionImpl::SessionImpl(const InstanceImpl& instance_impl):
		SessionBase{ instance_impl.id() + "::" + newUUID() },
		m_instance{ instance_impl },
		m_uplink{ SessionBase::m_pointer }
{
	env().logInfo(
			"New FreeAX25_TCPServer::SessionImpl(" + m_id + ")");
}

SessionImpl::~SessionImpl() {
	env().logInfo(
			"Del FreeAX25_TCPServer::SessionImpl(" + m_id + ")");
}

void SessionImpl::start(ChannelProxy broker, SocketIO::Socket&& socket) {
	ChannelProxy uplink = broker.connect(m_uplink.getLocalProxy());
	m_uplink.closeFunction = [this](std::unique_ptr<JsonX::Object>&& parameter)
		{ onUplinkClose(move(parameter)); };
	m_uplink.receiveFunction = [this](unique_ptr<JsonX::Object>&& parameter,
			MessagePriority priority)
		{ onUplinkReceive(move(parameter), priority); };
	m_uplink.ctrlFunction = [this](unique_ptr<JsonX::Object>&& parameter)
		{ return onUplinkCtrl(move(parameter)); };
	setRemote(m_uplink, uplink);
	m_socket = move(socket);
	m_buffer.resize(m_instance.bufsize());
	// Start thread:
	m_thread = thread{&SessionImpl::_run, this};
}

void SessionImpl::_run() noexcept {
	env().logInfo("Enter thread " + m_id);
	try {
		m_uplink.open();
		while (true) {
			// Read from socket:
			size_t l = m_socket.read(&m_buffer[0], m_buffer.size());
			if (l <= 0) {
				env().logInfo("Regular close session " + m_id);
				_exit();
				return;
			}
			// Write to uplink:
			JsonX::BlobPtr data{JsonX::Blob::make(&m_buffer[0], l)};
			JsonX::ObjectPtr request{JsonX::Object::make()};
			request.get()->add("data", move(data));
			m_uplink.send(move(request));
		} // end while //
	}
	catch (const exception& ex) {
		env().logError(
				"Exception in thread " + m_id + ": " + ex.what());
		_exit();
	}
	catch (...) {
		env().logError(
				"Unknown exception in thread " + m_id);
		_exit();
	}
	env().logInfo("Exit thread " + m_id);
}

void SessionImpl::_exit() noexcept {
	{
		lock_guard<mutex> lock(m_mutex);
		if (m_exiting) return;
		m_exiting = true;
		m_uplink.receiveFunction = nullptr;
		m_uplink.closeFunction = nullptr;
		m_uplink.ctrlFunction = nullptr;
	}
	try { if (m_socket) m_socket.close(); } catch (...) {}
	try { m_uplink.close(); } catch (...) {}
	try { m_uplink.reset(); } catch (...) {}
	try { m_thread.detach(); } catch (...) {}
	try { reset(); } catch (...) {}
}

void SessionImpl::onUplinkClose(std::unique_ptr<JsonX::Object>&& parameter) {
	env().logInfo(
			"FreeAX25_TCPServer::onUplinkClose(" + m_id + ", " +
				parameter.get()->toJsonString() +
			")");
	_exit();
}

void SessionImpl::onUplinkReceive(std::unique_ptr<JsonX::Object>&& message,
		FreeAX25::Runtime::MessagePriority priority)
{
	/*
	env().logDebug(
			"FreeAX25_TCPServer::onUplinkReceive(" + m_id + ", " +
				message.get()->toJsonString() + ", " +
				((priority == MessagePriority::PRIORITY) ? "PRIORITY" : "ROUTINE" ) +
			")");
	*/
	JsonX::ValuePtr vp{message.get()->extract("data")};
	if (!vp.get()->isBlob()) {
		env().logWarning("Value is not a Blob. Ignored");
		return;
	}
	const JsonX::Blob& blob{(*vp.get()).asBlob()};
	const JsonX::BlobValue& value{blob.get()};
	const uint8_t* pb = &value[0];
	size_t cb = value.size();
	while (cb > 0) {
		int w = m_socket.write(pb, cb);
		if (w <= 0) {
			env().logWarning("Write error on socket");
			_exit();
			return;
		}
		cb -= w;
		pb += w;
	}
}

std::unique_ptr<JsonX::Object> SessionImpl::onUplinkCtrl(std::unique_ptr<JsonX::Object>&& request) {
	env().logInfo(
			"FreeAX25_TCPServer::onUplinkCtrl(" + m_id + ", " +
				request.get()->toJsonString() + ", " +
			")");
	return JsonX::Object::make();
}

} /* namespace FreeAX25_TCPServer */
