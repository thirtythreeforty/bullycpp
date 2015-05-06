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

#ifndef QTPICBOOTLOADERDRIVER_H
#define QTPICBOOTLOADERDRIVER_H

#include <QObject>
#include <QString>

#include "bullycpp/IProgressCallback.h"
#include "bullycpp/ISerialPort.h"
#include "bullycpp/PicBootloaderDriver.h"

class QtPicBootloaderDriver : public QObject, private bullycpp::IProgressCallback
{
	Q_OBJECT
public:
	explicit QtPicBootloaderDriver(bullycpp::ISerialPort& serialPort, QObject *parent = 0);
	virtual ~QtPicBootloaderDriver();

	bool configBitsEnabled();

	virtual void onProgress(Status status, int percent);

signals:
	void configBitsEnabledChanged(bool);
	void deviceChanged(QString);
	void programmingStatusChanged(bullycpp::IProgressCallback::Status, int);

public slots:
	void programHexFile(const QString path, const bool rethrow);
	void parseDeviceFile(const QString path);
	void setConfigBitsEnabled(bool enabled);
	void setMCLR(bool mclr);
	bool readDevice(const bool rethrow);

private:
	bullycpp::PicBootloaderDriver driver;
};

#endif // QTPICBOOTLOADERDRIVER_H
