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

#ifndef IPROGRESSCALLBACK
#define IPROGRESSCALLBACK

namespace bullycpp
{

class IProgressCallback {
public:
	enum class Status {
		Idle,        // always 100%
		Busy,        // no % given
		Programming,
		Verifying,
		Error        // no % given
	};
	virtual void onProgress(Status, int) = 0;
};

} // bullycpp

#endif // IPROGRESSCALLBACK
