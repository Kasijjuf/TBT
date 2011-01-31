/*  S-Lang console widgets
 *
 *  (C) Copyright 2004-2008 Denis Rojo <jaromil@dyne.org>
 *
 * This source code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License as published 
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * Please refer to the GNU Public License for more details.
 *
 * You should have received a copy of the GNU Public License along with
 * this source code; if not, write to:
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef __SLW_PROMPT_H__
#define __SLW_PROMPT_H__

#include <slw.h>

#include <slw_text.h>


class SLW_Prompt : public SLangWidget {

	public:
		SLW_Prompt();
		~SLW_Prompt();

		// pure virtual functions from parent class
		bool init();
		bool feed(int key);
		bool refresh();
		////////////////////////

	private:
  		SLW_TextConsole *textconsole;
		void append(const char *text);

};

#endif
