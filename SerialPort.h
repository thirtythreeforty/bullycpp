#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <array>
#include <vector>

#include <QObject>
#include <QtSerialPort/QtSerialPort>

#include "ISerialPort.h"

class SerialPort : public bullycpp::ISerialPort {
public:
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
	QSerialPort qserialport;
};

#endif
