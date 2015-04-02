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

#ifndef QTPICDRIVER_H
#define QTPICDRIVER_H

#include <atomic>

#include <QObject>

#include "bullycpp/IProgressCallback.h"
#include "SerialPort.h"
#include "QtPicBootloaderDriver.h"

class QtPicDriver : public QObject
{
	Q_OBJECT
public:
	explicit QtPicDriver(const QStringList& deviceFiles, QObject *parent = 0);
	virtual ~QtPicDriver();

signals:
	void configBitsEnabledChanged(bool);
	void deviceChanged(QString);
	void serialDataReceived(QByteArray);
	void mclrChanged(bool);
	void serialPortStatusChanged(bool);
	void serialPortErrorChanged(QString);
	void programmingStateChanged(bool);
	void programmingErrorChanged(bool);
	void programmingProgressChanged(QString, int percent);

public slots:
	void sendSerialData(const QByteArray data);
	void programHexFile(const QString path) { programHexFile(path, false); }
	void programHexFile(const QString path, const bool rethrow);
	void setSerialPort(const QString name);
	void setBaudRate(const QString baud);
	void setMCLR(bool mclr);
	void setMCLROnProgram(bool set);
	void setConfigBitsEnabled(bool set);
	void openSerialPort();

private slots:
	void onReadyRead();
	void onDeviceChanged(const QString device);
	void onProgrammingStatusChanged(bullycpp::IProgressCallback::Status status, int percent);

private:
	SerialPort serialPort;
	QtPicBootloaderDriver bootloaderDriver;
	std::atomic<bool> forwardData;
	bool mclrOnProgram;
};

#endif // QTPICDRIVER_H
