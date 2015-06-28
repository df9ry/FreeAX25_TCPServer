/*
    Project FreeAX25 TCPServer
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

#include "InstanceImpl.h"
#include "SessionImpl.h"

#include <Environment.h>
#include <ServerEndPoint.h>

#include <stdexcept>

using namespace std;
using namespace FreeAX25::Runtime;

namespace FreeAX25_TCPServer {

InstanceImpl::InstanceImpl():
			m_instance_descriptor{},
			m_broker{ m_pointer }
{
}

InstanceImpl::InstanceImpl(const Instance& instance_descriptor):
			SessionBase{"FreeAX25_TCPServer::" +
				instance_descriptor.getName()},
			m_instance_descriptor{ instance_descriptor },
			m_broker{ m_pointer }
{
	env().logInfo(
			"New FreeAX25_TCPServer::InstanceImpl(" + m_instance_descriptor.getName() + ")");
}

InstanceImpl::~InstanceImpl() {
	env().logInfo(
			"Del FreeAX25_TCPServer::InstanceImpl(" + m_instance_descriptor.getName() + ")");
}

void InstanceImpl::init() {
	env().logInfo(
			"Init FreeAX25_TCPServer::InstanceImpl(" + m_instance_descriptor.getName() + ")");
	m_ifc = Setting::asStringValue(m_instance_descriptor.settings, "ifc", "");
	m_port = Setting::asIntValue(m_instance_descriptor.settings, "port", -1);
	if (m_port == -1) throw invalid_argument("Missing mandatory parameter port");
	m_backof = Setting::asIntValue(m_instance_descriptor.settings, "backof", -1);
	if (m_backof == -1) throw invalid_argument("Missing mandatory parameter backof");
	int _bufsize = Setting::asIntValue(m_instance_descriptor.settings, "bufsize", 0);
	if (_bufsize <= 0) throw invalid_argument("Missing mandatory parameter bufsize");
	m_bufsize = static_cast<size_t>(_bufsize);
}

void InstanceImpl::start() {
	env().logInfo(
			"Start FreeAX25_TCPServer::InstanceImpl(" + m_instance_descriptor.getName() + ")");
	auto cep = m_instance_descriptor.clientEndPoints.findEntryConst("uplink");
	string url = cep.getUrl();
	auto broker = env().serverProxies.findEntry(url);
	if (!broker) throw runtime_error("Not found: \"" + url + "\"");
	setRemote(m_broker, broker);
	// Start thread:
	m_thread = thread{&InstanceImpl::_run, this};
}

void InstanceImpl::_run() noexcept {
	try {
		env().logInfo("Start listening thread " + m_id);
		m_socket.bind(m_port, m_ifc);
		env().logInfo("TCPServer: Listen on " + m_socket.toString());
		m_socket.listen(m_backof);
		while (true) {
			SocketIO::Socket rx{m_socket.accept()};
			const string peer{rx.toStringPeer()};
			env().logInfo(
					"Accepted connection from " + peer +
					" on " + m_socket.toString());
			try {
				SessionImpl* si = new SessionImpl(*this);
				si->start(m_broker.getRemoteProxy(), move(rx));
			}
			catch (const exception& e) {
				env().logError(peer + " exception: " + e.what());
			}
			catch (...) {
				env().logError(peer + " unknown exception");
			}
		} // end while //
	}
	catch (const exception& e) {
		env().logError(m_id + ": Exception: " + e.what());
	}
	catch (...) {
		env().logError(m_id + ": Unknown exception.");
	}
	env().logInfo("Exit listening thread \"" + m_id + "\"");

}

} /* namespace FreeAX25_TCPServer */
