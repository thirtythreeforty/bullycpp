#include "QtPicDriver.h"

#include <stdexcept>

QtPicDriver::QtPicDriver(const QStringList& deviceFiles, QObject *parent)
	: QObject(parent)
	, serialPort(this)
	, bootloaderDriver(serialPort)
	, forwardData(true)
	, mclrOnProgram(false)
{
	openSerialPort();
	for(const auto& deviceFile: deviceFiles)
		bootloaderDriver.parseDeviceFile(deviceFile);
	connect(&serialPort.getQSerialPort(), SIGNAL(readyRead()), SLOT(onReadyRead()));
	connect(&bootloaderDriver, SIGNAL(deviceChanged(QString)), SLOT(onDeviceChanged(QString)));
	connect(&bootloaderDriver, SIGNAL(programmingStatusChanged(bullycpp::IProgressCallback::Status,int)), SLOT(onProgrammingStatusChanged(bullycpp::IProgressCallback::Status,int)));
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
		QThread::msleep(1);
	}
	setMCLR(false);
	emit mclrChanged(false);
	QThread::msleep(10);

	if(bootloaderDriver.readDevice())
		bootloaderDriver.programHexFile(path);

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
	serialPort.close();
	unsigned int intBaud = baud.toInt();
	if(intBaud) {
		serialPort.setSpeed(intBaud);
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

void QtPicDriver::onProgrammingStatusChanged(bullycpp::IProgressCallback::Status status, int percent)
{
	typedef bullycpp::IProgressCallback::Status Status;
	const static QMap<Status, QString> map = {
		{Status::Idle,        QStringLiteral("")},
		{Status::Busy,        QStringLiteral("Working...")},
		{Status::Programming, QStringLiteral("Programming (%1%)")},
		{Status::Verifying,   QStringLiteral("Verifying (%1%)")},
		{Status::Error,       QStringLiteral("An error occured")}
	};

	QString strStatus = map[status];
	int scaledPercent = 0;

	switch(status) {
	case Status::Idle:
		emit programmingStateChanged(false);
		emit programmingErrorChanged(false);
		scaledPercent = 100;
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
		break;
	}
	emit programmingProgressChanged(strStatus, scaledPercent);
}
