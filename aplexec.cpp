/***
    qvis edif Copyright (C) 2021  Dr. C. H. L. Moller

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***/

#include <QtWidgets>

#include <iostream>
#include <sstream>

#include <apl/libapl.h>

#include "aplexec.h"

LIBAPL_error
AplExec::aplExec (apl_op_e apl_op, QString &cmd,
	   QString &outString, QString &errString)
{
  LIBAPL_error execerr = LAE_NO_ERROR;

  switch(apl_op) {
  case APL_OP_EXEC:
    {
      std::stringstream outbuffer;
      std::streambuf *coutbuf = std::cout.rdbuf();
      std::cout.rdbuf(outbuffer.rdbuf());
      std::stringstream errbuffer;
      std::streambuf *cerrbuf = std::cerr.rdbuf();
      std::cerr.rdbuf(errbuffer.rdbuf());
      execerr = apl_exec (cmd.toStdString ().c_str ());
      std::cout.rdbuf(coutbuf);
      std::cerr.rdbuf(cerrbuf);
      outString = QString (outbuffer.str ().c_str ());
      errString = QString (errbuffer.str ().c_str ());
    }
    break;
  case APL_OP_COMMAND:
    {
      std::stringstream errbuffer;
      std::streambuf *cerrbuf = std::cerr.rdbuf();
      std::cerr.rdbuf(errbuffer.rdbuf());
      const char *res = apl_command (cmd.toStdString ().c_str ());
      if (res) {
	outString = QString (res);
	free ((void *)res);
      }
      std::cerr.rdbuf(cerrbuf);
      errString = QString (errbuffer.str ().c_str ());
    }
    break;
  }
  
  return execerr;
}

