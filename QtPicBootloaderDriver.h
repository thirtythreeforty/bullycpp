#ifndef QTPICBOOTLOADERDRIVER_H
#define QTPICBOOTLOADERDRIVER_H

#include <QObject>
#include <QString>

#include "bullycpp/IProgressCallback.h"
#include "bullycpp/ISerialPort.h"
#include "bullycpp/PicBootloaderDriver.h"

class QtPicBootloaderDriver : public QObject, public bullycpp::IProgressCallback
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
	void programHexFile(const QString path);
	void parseDeviceFile(const QString path);
	void setConfigBitsEnabled(bool enabled);
	void setMCLR(bool mclr);
	bool readDevice();

private:
	bullycpp::PicBootloaderDriver driver;
};

#endif // QTPICBOOTLOADERDRIVER_H
