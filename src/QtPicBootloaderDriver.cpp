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

#include "QtPicBootloaderDriver.h"

#include <QFile>
#include <QMap>

#include "QStdStreamBuf.h"
#include "SerialPort.h"
#include "bullycpp/PicDevice.h"

QtPicBootloaderDriver::QtPicBootloaderDriver(bullycpp::ISerialPort& serialPort,QObject *parent)
	: QObject(parent)
	, driver(serialPort, this)
{}

QtPicBootloaderDriver::~QtPicBootloaderDriver()
{}

void QtPicBootloaderDriver::setConfigBitsEnabled(bool enabled)
{
	driver.setConfigBitsEnabled(enabled);
}

bool QtPicBootloaderDriver::configBitsEnabled()
{
	return driver.getConfigBitsEnabled();
}

void QtPicBootloaderDriver::programHexFile(const QString path, const bool rethrow)
{
	// Our ISerialPort implementation may throw its TimeoutException.
	// Or, the driver may throw a logic_error.
	try {
		driver.programHexFile(path.toStdString());
	}
	catch(SerialPort::TimeoutException&) {
		emit programmingStatusChanged(IProgressCallback::Status::Error, 0);
		// Serial console wants us to throw the error so it can display it.
		if(rethrow) throw;
	}
	catch(std::logic_error& e) {
		emit programmingStatusChanged(IProgressCallback::Status::Error, 0);
		if(rethrow) throw;
	}
}

void QtPicBootloaderDriver::parseDeviceFile(const QString path)
{
	// Special case for QRC resources, which begin with ":/"
	if(path.startsWith(QStringLiteral(":/"))) {
		QFile file(path);
		if(file.open(QIODevice::ReadOnly)) {
			QStdIStream stream(&file);
			driver.parseDeviceFile(stream);
		}
	}
	else
		driver.parseDeviceFile(path.toStdString());
}

void QtPicBootloaderDriver::setMCLR(bool mclr)
{
	driver.setMCLR(mclr);
}

bool QtPicBootloaderDriver::readDevice(const bool rethrow)
{
	const bullycpp::PicDevice* optionalDevice = nullptr;

	// Our ISerialPort implementation may throw its TimeoutException
	try {
		optionalDevice = driver.readDevice();
	}
	catch(SerialPort::TimeoutException&) {
		emit programmingStatusChanged(IProgressCallback::Status::Error, 0);
		if(rethrow) throw;
	}

	if(optionalDevice) {
		emit deviceChanged(QString::fromStdString(optionalDevice->name));
		// Serial console wants us to throw the error so it can display it.
		return true;
	}
	else return false;
}

void QtPicBootloaderDriver::onProgress(Status status, int percent)
{
	emit programmingStatusChanged(status, percent);
}
