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

#include "QtPicDriver.h"

#include <stdexcept>

QtPicDriver::QtPicDriver(const QStringList& deviceFiles, bool rethrow, QObject *parent)
	: QObject(parent)
	, serialPort(this)
	, bootloaderDriver(serialPort)
	, forwardData(true)
	, mclrOnProgram(false)
	, rethrow(rethrow)
{
	for(const auto& deviceFile: deviceFiles)
		bootloaderDriver.parseDeviceFile(deviceFile);
	connect(&serialPort.getQSerialPort(), &QIODevice::readyRead, this, &QtPicDriver::onReadyRead);
	connect(&bootloaderDriver, &QtPicBootloaderDriver::deviceChanged, this, &QtPicDriver::onDeviceChanged);
	connect(&bootloaderDriver, &QtPicBootloaderDriver::programmingStatusChanged, this, &QtPicDriver::onProgrammingStatusChanged);
}

QtPicDriver::~QtPicDriver()
{
	serialPort.close();
}

void QtPicDriver::sendSerialData(const QByteArray data)
{
	if(!forwardData)
		return;
	serialPort.getQSerialPort().write(data);
}

void QtPicDriver::programHexFile(const QString path)
{
	forwardData = false;

	emit programmingStateChanged(true);

	if(mclrOnProgram) {
		setMCLR(true);
		QThread::msleep(50);
	}
	setMCLR(false);
	emit mclrChanged(false);
	QThread::msleep(50);

	serialPort.clear();

	if(bootloaderDriver.readDevice(rethrow))
		bootloaderDriver.programHexFile(path, rethrow);

	emit programmingStateChanged(false);

	forwardData = true;
}

void QtPicDriver::setSerialPort(const QString name)
{
	serialPort.close();
	serialPort.setName(name.toStdString());
	openSerialPort();
}

void QtPicDriver::setBaudRate(const QString baud)
{
	bool ok;

	serialPort.close();

	unsigned int intBaud = baud.toUInt(&ok);
	if(ok) {
		try {
			serialPort.setSpeed(intBaud);
		}
		catch(std::runtime_error& e) {
			emit serialPortStatusChanged(false);
			emit serialPortErrorChanged(QString(e.what()));
			if(rethrow) throw;
			return;
		}
		openSerialPort();
	}
	else {
		emit serialPortStatusChanged(false);
		emit serialPortErrorChanged("Invalid baud rate");
	}
}

void QtPicDriver::setMCLR(bool mclr)
{
	bootloaderDriver.setMCLR(mclr);
}

void QtPicDriver::setMCLROnProgram(bool set)
{
	mclrOnProgram = set;
}

void QtPicDriver::setConfigBitsEnabled(bool set)
{
	bootloaderDriver.setConfigBitsEnabled(set);
}

void QtPicDriver::openSerialPort()
{
	try {
		serialPort.open();
		emit serialPortStatusChanged(true);
		emit serialPortErrorChanged("");
	}
	catch(std::runtime_error& e) {
		emit serialPortStatusChanged(false);
		emit serialPortErrorChanged(e.what());
		if(rethrow) throw;
	}
}

void QtPicDriver::onReadyRead()
{
	if(!forwardData)
		return;
	QByteArray bytes = serialPort.getQSerialPort().readAll();
	emit serialDataReceived(bytes);
}

void QtPicDriver::onDeviceChanged(const QString device)
{
	emit deviceChanged(device);
}

void QtPicDriver::onProgrammingStatusChanged(bullycpp::IProgressCallback::Status status, int percent, QString error)
{
	typedef bullycpp::IProgressCallback::Status Status;
	const static QMap<Status, QString> map = {
		{Status::Idle,        QStringLiteral("")},
		{Status::Busy,        QStringLiteral("Working...")},
		{Status::Programming, QStringLiteral("Programming (%1%)")},
		{Status::Verifying,   QStringLiteral("Verifying (%1%)")},
		{Status::Error,       QStringLiteral("Error: %1")}
	};

	QString strStatus = map[status];
	int scaledPercent = 0;

	switch(status) {
	case Status::Idle:
		emit programmingStateChanged(false);
		emit programmingErrorChanged(false);
		break;
	case Status::Busy:
		emit programmingStateChanged(true);
		emit programmingErrorChanged(false);
		break;
	case Status::Programming:
		emit programmingStateChanged(true);
		emit programmingErrorChanged(false);
		strStatus = strStatus.arg(percent);
		scaledPercent = percent / 2;
		break;
	case Status::Verifying:
		emit programmingStateChanged(true);
		emit programmingErrorChanged(false);
		strStatus = strStatus.arg(percent);
		scaledPercent = percent / 2 + 50;
		break;
	case Status::Error:
		emit programmingStateChanged(false);
		emit programmingErrorChanged(true);
		strStatus = strStatus.arg(error);
		break;
	}
	emit programmingProgressChanged(strStatus, scaledPercent);
}
