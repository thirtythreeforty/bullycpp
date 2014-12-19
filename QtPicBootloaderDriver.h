#ifndef QTPICBOOTLOADERDRIVER_H
#define QTPICBOOTLOADERDRIVER_H

#include <QObject>
#include <QString>

#include "ISerialPort.h"
#include "PicBootloaderDriver.h"

class QtPicBootloaderDriver : public QObject
{
	Q_OBJECT
public:
	explicit QtPicBootloaderDriver(bullycpp::ISerialPort& serialPort, QObject *parent = 0);
	virtual ~QtPicBootloaderDriver();

	bool configBitsEnabled();

signals:
	void configBitsEnabledChanged(bool);
	void deviceChanged(QString);

public slots:
	void programHexFile(const QString path);
	void parseDeviceFile(const QString path);
	void setConfigBitsEnabled(bool enabled);
	void setMCLR(bool mclr);
	bool readDevice();

private:
	bullycpp::PicBootloaderDriver driver;
};

#endif // QTPICBOOTLOADERDRIVER_H
