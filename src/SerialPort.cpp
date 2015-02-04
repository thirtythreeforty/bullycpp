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

#include "SerialPort.h"

#include <iostream>
#include <stdexcept>

#include <QString>

SerialPort::SerialPort(QObject *parent)
	: qserialport(parent)
{}

SerialPort::SerialPort(const std::string& name, QObject *parent)
	: qserialport(QString(name.c_str()), parent)
{}

SerialPort::SerialPort(const char* name, QObject *parent)
	: qserialport(QString(name), parent)
{}

SerialPort::SerialPort(const QString &name, QObject *parent)
	: qserialport(name, parent)
{}

SerialPort::~SerialPort()
{
	qserialport.close();
}

void SerialPort::setName(const std::string& name)
{
	qserialport.setPortName(QString(name.c_str()));
}

void SerialPort::setName(const char* name)
{
	qserialport.setPortName(QString(name));
}

void SerialPort::setSpeed(const unsigned int speed)
{
	qserialport.setBaudRate(speed);
}

void SerialPort::open()
{
	if(!qserialport.open(QIODevice::ReadWrite))
		throw std::runtime_error(std::string("Failed to open serial port: ").append(qserialport.errorString().toStdString()));
}

void SerialPort::close()
{
	qserialport.close();
}

void SerialPort::setDTR(bool b)
{
	qserialport.setDataTerminalReady(b);
}

void SerialPort::setRTS(bool b)
{
	qserialport.setRequestToSend(b);
}

void SerialPort::read(std::vector<uint8_t>& dest)
{
	read(&dest[0], dest.size());
}

std::vector<uint8_t> SerialPort::read(size_t size)
{
	std::vector<uint8_t> v(size);
	read(&v[0], size);
	return v;
}

void SerialPort::read(uint8_t* dest, size_t size)
{
	unsigned int read = 0;
	do {
		waitForReadable();
		read += qserialport.read(reinterpret_cast<char*>(dest) + read, size - read);
	} while(read != size);
}

unsigned char SerialPort::read()
{
	char c;
	waitForReadable();
	qserialport.getChar(&c);
	return c;
}

void SerialPort::write(const uint8_t* src, size_t size)
{
	unsigned int written = 0;
	do {
		written += qserialport.write(reinterpret_cast<const char*>(src), size);
		waitForWritten();
	} while(written != size);
}

void SerialPort::write(const std::vector<uint8_t>& src)
{
	write(&src[0], src.size());
}

void SerialPort::write(const unsigned char c)
{
	qserialport.putChar(c);
	waitForWritten();
}

void SerialPort::clear()
{
	qserialport.clear();
}

void SerialPort::waitForReadable()
{
	if(qserialport.bytesAvailable() == 0)
		if(!qserialport.waitForReadyRead(timeout))
			throw TimeoutException("Timeout while reading serial port");
}

void SerialPort::waitForWritten()
{
	if(qserialport.bytesToWrite() != 0)
		if(!qserialport.waitForBytesWritten(timeout))
			throw TimeoutException("Timeout while writing serial port");
}
