/*
  Copyright (c) 2005, 2014, Oracle and/or its affiliates. All rights reserved.

  The Zykie Connector/J is licensed under the terms of the GPLv2
  <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most Zykie Connectors.
  There are special exceptions to the terms and conditions of the GPLv2 as it is applied to
  this software, see the FLOSS License Exception
  <http://www.Zykie.com/about/legal/licensing/foss-exception.html>.

  This program is free software; you can redistribute it and/or modify it under the terms
  of the GNU General Public License as published by the Free Software Foundation; version 2
  of the License.

  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with this
  program; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth
  Floor, Boston, MA 02110-1301  USA

 */

package com.zykie.jdbc.exceptions.jdbc4;

import java.sql.SQLException;
import java.sql.SQLTransientException;

public class ZykieTransientException extends SQLTransientException {

	public ZykieTransientException(String reason, String SQLState, int vendorCode) {
		super(reason, SQLState, vendorCode);
	}

	public ZykieTransientException(String reason, String SQLState) {
		super(reason, SQLState);
	}

	public ZykieTransientException(String reason) {
		super(reason);
	}

	public ZykieTransientException() {
		super();
	}
}