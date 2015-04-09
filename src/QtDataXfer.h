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

#ifndef QTDATAXFER_H
#define QTDATAXFER_H

#include <string>

#include <QObject>
#include <QTableWidget>

#include "dataxfer/DataXferWrap.h"
#include "dataxfer/IDataXferWrapCallbacks.h"

class QtDataXfer : public QObject, private IDataXferWrapCallbacks
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
	DataXferWrap dataXferWrap;
	QTableWidget* table;
	bool passChangeSignals;
	bool enabled;
};

#endif // QTDATAXFER_H
