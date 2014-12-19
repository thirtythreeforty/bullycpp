#include "QtPicDriver.h"

#include <stdexcept>

QtPicDriver::QtPicDriver(QObject *parent)
	: QObject(parent)
	, serialPort(this)
	, bootloaderDriver(serialPort)
	, forwardData(true)
	, mclrOnProgram(false)
{
	serialPort.setSpeed(230400);
	openSerialPort();
	bootloaderDriver.parseDeviceFile("devices.txt");
	connect(&serialPort.getQSerialPort(), SIGNAL(readyRead()), SLOT(onReadyRead()));
	connect(&bootloaderDriver, SIGNAL(deviceChanged(QString)), SLOT(onDeviceChanged(QString)));
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

	if(mclrOnProgram) {
		setMCLR(true);
		QThread::msleep(1);
	}
	setMCLR(false);
	emit mclrChanged(false);
	QThread::msleep(10);

	if(bootloaderDriver.readDevice())
		bootloaderDriver.programHexFile(path);

	forwardData = true;
}

void QtPicDriver::setSerialPort(const QString name)
{
	serialPort.close();
	serialPort.setName(name.toStdString());
	openSerialPort();
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
