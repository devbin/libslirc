/***************************************************************************
**  Copyright 2014-2014 by Simon "SlashLife" Stienen                      **
**  http://projects.slashlife.org/libslirc/                               **
**  libslirc@projects.slashlife.org                                       **
**                                                                        **
**  This file is part of libslIRC.                                        **
**                                                                        **
**  libslIRC is free software: you can redistribute it and/or modify      **
**  it under the terms of the GNU Lesser General Public License as        **
**  published by the Free Software Foundation, either version 3 of the    **
**  License, or (at your option) any later version.                       **
**                                                                        **
**  libslIRC is distributed in the hope that it will be useful,           **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  and the GNU Lesser General Public License along with libslIRC.        **
**  If not, see <http://www.gnu.org/licenses/>.                           **
***************************************************************************/

#include "client_to_server.hpp"

#include "../apis/connection.hpp"

#include <utility>

namespace arg = std::placeholders;

slirc::modules::client_to_server::client_to_server(slirc::irc &context)
: apis::protocol(context)
, parserconn(context.attach<apis::connection::raw_irc_line_event>(
	std::bind(&client_to_server::parser, this, arg::_1))) {
}

slirc::modules::client_to_server::~client_to_server() {
	parserconn.disconnect();
}

void slirc::modules::client_to_server::parser(event::pointer ep) {
	const std::string &line = ep->data.get<apis::connection::raw_irc_line>().line;

	parameters &prm = ep->data.set(parameters());
		prm.params = irc_split(line);

	if (prm.params.empty()) {
		return;
	}

	if (
		// !prm.params[0].empty() && // Check unnecessary: Only the last
		//   parameter can be empty if it is the literal extended parameter ":"
		//   The first parameter cannot be an extended parameter, though.
		prm.params[0][0] == ':'
	) {
		origin &org = ep->data.set(origin());
			org.origin_string = prm.params[0].substr(1);

		;
	}

	ep->queue_as<parsed_event>();
}
