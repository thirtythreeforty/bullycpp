#include <QMap>

#include "bullycpp/PicDevice.h"
#include "QtPicBootloaderDriver.h"

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

void QtPicBootloaderDriver::programHexFile(const QString path)
{
	driver.programHexFile(path.toStdString());
}

void QtPicBootloaderDriver::parseDeviceFile(const QString path)
{
	driver.parseDeviceFile(path.toStdString());
}

void QtPicBootloaderDriver::setMCLR(bool mclr)
{
	driver.setMCLR(mclr);
}

bool QtPicBootloaderDriver::readDevice()
{
	const bullycpp::PicDevice* optionalDevice = driver.readDevice();
	if(optionalDevice) {
		emit deviceChanged(QString::fromStdString(optionalDevice->name));
		return true;
	}
	else return false;
}

void QtPicBootloaderDriver::onProgress(Status status, int percent)
{
	emit programmingStatusChanged(status, percent);
}
