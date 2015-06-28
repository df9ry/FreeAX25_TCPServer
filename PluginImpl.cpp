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

#include "PluginImpl.h"

#include <Environment.h>

#include <string>
#include <stdexcept>

using namespace std;
using namespace FreeAX25::Runtime;

static FreeAX25_TCPServer::PluginImpl* implementation{ nullptr };

void init(const Plugin& plugin_descriptor) {
	if (implementation) throw runtime_error(
			"FreeAX25_TCPServer::PluginImpl already instantiated");
	implementation = new FreeAX25_TCPServer::PluginImpl(plugin_descriptor);
	implementation->init();
}

void start() {
	if (!implementation) throw runtime_error(
			"FreeAX25_Zero::PluginImpl not instantiated");
	implementation->start();
}

namespace FreeAX25_TCPServer {

	PluginImpl::PluginImpl(const Plugin& plugin_descriptor):
			m_plugin_descriptor{ plugin_descriptor }
	{
		env().logInfo(
				"New FreeAX25_TCPServer::PluginImpl(" + m_plugin_descriptor.getName() + ")");
		for (UniquePointerDictConstIterator<Instance> iter = plugin_descriptor.instances.begin();
				iter != plugin_descriptor.instances.end(); ++iter)
		{
			m_instances.insertNew(iter->first, new InstanceImpl(*iter->second.get()));
		}
	}

	PluginImpl::~PluginImpl() {
		env().logInfo(
				"Del FreeAX25_TCPServer::PluginImpl(" + m_plugin_descriptor.getName() + ")");
		::implementation = nullptr;
	}

	void PluginImpl::init() {
		env().logInfo(
				"Init FreeAX25_TCPServer::PluginImpl(" + m_plugin_descriptor.getName() + ")");
		for (SharedPointerDictIterator<InstanceImpl> iter = m_instances.begin();
				iter != m_instances.end(); ++iter)
		{
			iter->second.get()->init();
		}
	}

	void PluginImpl::start() {
		env().logInfo(
				"Start FreeAX25_TCPServer::PluginImpl(" + m_plugin_descriptor.getName() + ")");
		for (SharedPointerDictIterator<InstanceImpl> iter = m_instances.begin();
				iter != m_instances.end(); ++iter)
		{
			iter->second.get()->start();
		}
	}

} /* namespace FreeAX25_TCPServer */
