/** This file is part of BullyCPP.
 **
 **     BullyCPP is free software: you can redistribute it and/or modify
 **     it under the terms of the GNU General Public License as published by
 **     the Free Software Foundation, either version 3 of the License, or
 **     (at your option) any later version.
 **
 **     BullyCPP is distributed in the hope that it will be useful,
 **     but WITHOUT ANY WARRANTY; without even the implied warranty of
 **     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **     GNU General Public License for more details.
 **
 **     You should have received a copy of the GNU General Public License
 **     along with BullyCPP.  If not, see <http://www.gnu.org/licenses/>.
 **/

#ifndef DATAXFER_H
#define DATAXFER_H

#include <string>

#include "IDataXferCallbacks.h"

class DataXfer {
public:
	DataXfer(IDataXferCallbacks*);
	void onDataIn(const std::string& bytes, const unsigned int currentMilliseconds);
	void onDataOut(const std::string& typed);
	void variableEdited(const unsigned int index, const std::string& newValue);
};

#endif
