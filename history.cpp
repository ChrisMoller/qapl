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

#include <stdio.h>
#include <string.h>
#include <readline/history.h>

#include "history.h"

History::History ()
{
  using_history ();
}

void
History::insert (const char *string)
{
  if (string && *string) {
    HIST_ENTRY *ct = current_history ();
    if (!ct ||
	(ct && ct->line && *(ct->line)  && strcmp (string, ct->line)))
      add_history (string);
  }
}

char *
History::next ()
{
  HIST_ENTRY *ety = next_history ();
  return ety ? ety->line : nullptr;
}

char *
History::previous ()
{
  HIST_ENTRY *ety = current_history ();
  previous_history ();
  return ety ? ety->line : nullptr;
}

void
History::rebase ()
{
  if (history_length > 0)
    history_set_pos (history_length - 1);
}
