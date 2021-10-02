/***
    qvis Copyright (C) 2021  Dr. C. H. L. Moller

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

#ifndef APLEXEC_H
#define APLEXEC_H

#include <QtWidgets>
#include <apl/libapl.h>

#include "aplexec.h"

typedef enum {
  APL_OP_EXEC,
  APL_OP_COMMAND
} apl_op_e;

class AplExec
{
 public:
  static LIBAPL_error
  aplExec (apl_op_e apl_op, QString &cmd,
	   QString &outString, QString &errString);
};
#endif // APLEEXEC_H

