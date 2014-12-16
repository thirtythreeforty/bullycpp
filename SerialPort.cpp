#include "SerialPort.h"

#include <iostream>
#include <stdexcept>

#include <QString>

SerialPort::SerialPort()
{}

SerialPort::SerialPort(const std::string& name)
	: qserialport(QString(name.c_str()))
{}

SerialPort::SerialPort(const char* name)
	: qserialport(QString(name))
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
		if(!qserialport.bytesAvailable())
			do {} while(!qserialport.waitForReadyRead(-1));
		read += qserialport.read(reinterpret_cast<char*>(dest) + read, size - read);
	} while(read != size);
}

unsigned char SerialPort::read()
{
	char c;
	if(!qserialport.bytesAvailable())
		do {} while(!qserialport.waitForReadyRead(-1));
	qserialport.getChar(&c);
	return c;
}

void SerialPort::write(const uint8_t* src, size_t size)
{
	unsigned int written = 0;
	do {
		written += qserialport.write(reinterpret_cast<const char*>(src), size);
		qserialport.waitForBytesWritten(-1);
	} while(written != size);
}

void SerialPort::write(const std::vector<uint8_t>& src)
{
	write(&src[0], src.size());
}

void SerialPort::write(const unsigned char c)
{
	qserialport.putChar(c);
	qserialport.waitForBytesWritten(-1);
}
