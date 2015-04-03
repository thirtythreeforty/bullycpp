#ifndef QTDATAXFER_H
#define QTDATAXFER_H

#include <string>

#include <QObject>
#include <QTableWidget>

#include "dataxfer/DataXfer.h"
#include "dataxfer/IDataXferCallbacks.h"

class QtDataXfer : public QObject, private IDataXferCallbacks
{
	Q_OBJECT
public:
	explicit QtDataXfer(QObject* parent = 0);
	virtual ~QtDataXfer() = default;

	// Table must be non-null.
	// Call this very first.  It is this way so that this object's
	// owner can have a chance to initialize their widget (via
	// ui->setupUi(this), for example).
	void setTableWidget(QTableWidget* table);

private:
	virtual void sendRawData(const std::string& bytes);
	virtual void displayRawData(const std::string& bytes);
	virtual void variableUpdated(const unsigned int index,
	                             const std::string& name,
	                             const std::string& description,
	                             const std::string& value,
	                             const bool modifiable);

signals:
	void inboundBytesReady(QByteArray);
	void sendRawBytes(QByteArray);

	void enabledChanged(bool);

public slots:
	void processOutboundBytes(QByteArray outbound);
	void processInboundBytes(QByteArray inbound);

	void enable(bool enable);

private slots:
	void updateItemVariable(QTableWidgetItem* item);

private:
	DataXfer dataXfer;
	QTableWidget* table;
	bool enabled;
};

#endif // QTDATAXFER_H
