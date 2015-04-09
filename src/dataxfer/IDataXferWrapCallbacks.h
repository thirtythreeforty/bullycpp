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

#ifndef IDATAXFERWRAPCALLBACKS_H
#define IDATAXFERWRAPCALLBACKS_H

#include <string>

class IDataXferWrapCallbacks {
public:
	virtual void sendRawData(const std::string& bytes) = 0;
	virtual void displayRawData(const std::string& bytes) = 0;
	virtual void variableUpdated(const unsigned int index,
	                             const std::string& name,
	                             const std::string& description,
	                             const std::string& value,
	                             const bool modifiable) = 0;
};

#endif
