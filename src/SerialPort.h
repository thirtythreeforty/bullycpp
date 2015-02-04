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

#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <array>
#include <vector>

#include <QObject>
#include <QException>
#include <QtSerialPort>

#include "bullycpp/ISerialPort.h"

class SerialPort : public bullycpp::ISerialPort {
public:
	class TimeoutException : public QException {
	public:
		void raise() const { throw *this; }
		QException* clone() const { return new TimeoutException(*this); }
	};

	explicit SerialPort(QObject *parent = Q_NULLPTR);
	explicit SerialPort(const char* name, QObject *parent = Q_NULLPTR);
	explicit SerialPort(const std::string& name, QObject *parent = Q_NULLPTR);
	explicit SerialPort(const QString& name, QObject *parent = Q_NULLPTR);
	virtual ~SerialPort();

	virtual void setName(const std::string& name);
	virtual void setName(const char* name);

	virtual void setSpeed(const unsigned int speed);

	virtual void open();
	virtual void close();

	virtual void setDTR(bool b);
	virtual void setRTS(bool b);

	virtual std::vector<uint8_t> read(size_t size);
	virtual void read(uint8_t* dest, size_t size);
	virtual void read(std::vector<uint8_t>& dest);
	virtual unsigned char read();

	virtual void write(const uint8_t* src, size_t size);
	virtual void write(const std::vector<uint8_t>& src);
	virtual void write(const unsigned char c);

	virtual void clear();

	QSerialPort& getQSerialPort() { return qserialport; }
private:
	// These functions throw on timeout (or return normally).
	void waitForReadable();
	void waitForWritten();

	QSerialPort qserialport;

	// Timeout in milliseconds
	const int timeout = 2000;
};

#endif
