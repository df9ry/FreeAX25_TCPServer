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

#ifndef PLUGINIMPL_H_
#define PLUGINIMPL_H_

#include "InstanceImpl.h"

#include <Plugin.h>
#include <SharedPointerDict.h>

extern "C" {
	void init(const FreeAX25::Runtime::Plugin& plugin_descriptor);
	void start();
}

namespace FreeAX25_TCPServer {

class PluginImpl {
public:
	/**
	 * Constructor.
	 * @param plugin_descriptor Instance descriptor.
	 */
	PluginImpl(const FreeAX25::Runtime::Plugin& plugin_descriptor);

	/**
	 * Destructor.
	 */
	~PluginImpl();

	/**
	 * Init the plugin.
	 */
	void init();

	/**
	 * Start the plugin.
	 */
	void start();

private:
	const FreeAX25::Runtime::Plugin&                   m_plugin_descriptor;
	FreeAX25::Runtime::SharedPointerDict<InstanceImpl> m_instances{};
};

} /* namespace FreeAX25_TCPServer */

#endif /* PLUGINIMPL_H_ */
