#include "QtDataXfer.h"

#include <QCheckBox>
#include <QDateTime>
#include <QHBoxLayout>
#include <QTableWidgetItem>
#include <QWidget>

QtDataXfer::QtDataXfer(QObject *parent)
	: QObject(parent)
	, dataXfer(this)
	, passChangeSignals(true)
	, enabled(false)
{
}

void QtDataXfer::setTableWidget(QTableWidget *table)
{
	this->table = table;
	connect(table, &QTableWidget::itemChanged, this, &QtDataXfer::updateItemVariable);
}

void QtDataXfer::sendRawData(const std::string &bytes)
{
	emit sendRawBytes(QByteArray::fromStdString(bytes));
}

void QtDataXfer::displayRawData(const std::string &bytes)
{
	emit inboundBytesReady(QByteArray::fromStdString(bytes));
}

namespace {
	QTableWidgetItem* createWidgetItem(const std::string& text, bool editable, int index) {
		QTableWidgetItem* item = new QTableWidgetItem(QString::fromStdString(text));
		if(!editable)
			item->setFlags(item->flags() & ~Qt::ItemIsEditable);
		item->setData(Qt::UserRole, QVariant(index));
		return item;
	}
}

void QtDataXfer::variableUpdated(const unsigned int index,
                                 const std::string& name,
                                 const std::string& description,
                                 const std::string& value,
                                 const bool modifiable)
{
	// Don't pass itemChanged signals generated from the following code.
	passChangeSignals = false;

	int row = table->rowCount();
	table->insertRow(row);

	// Name
	table->setItem(row, 0, createWidgetItem(name, false, index));

	// Mutable (checkbox)
	// So this is irritating.  There's no good way to have an enabled, non-user-editable,
	// centered checkbox in a table item.  So we create our own layout.
	QWidget *pWidget = new QWidget();
	QCheckBox *pCheckBox = new QCheckBox();
	QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
	pLayout->addWidget(pCheckBox);
	pLayout->setAlignment(Qt::AlignCenter);
	pLayout->setContentsMargins(0, 0, 0, 0);
	pWidget->setLayout(pLayout);
	pCheckBox->setChecked(modifiable);
	pCheckBox->setEnabled(false);
	table->setCellWidget(row, 1, pWidget);

	// Value
	table->setItem(row, 2, createWidgetItem(value, modifiable, index));

	// Description
	table->setItem(row, 3, createWidgetItem(description, false, index));

	passChangeSignals = true;
}

void QtDataXfer::processOutboundBytes(QByteArray outbound)
{
	if(enabled)
		dataXfer.onDataOut(outbound.toStdString());
	else
		emit sendRawBytes(outbound);
}

void QtDataXfer::processInboundBytes(QByteArray inbound)
{
	if(enabled)
		dataXfer.onDataIn(inbound.toStdString(), QDateTime::currentMSecsSinceEpoch());
	else
		emit inboundBytesReady(inbound);
}

void QtDataXfer::enable(bool enable)
{
	if(enable != enabled)
		emit enabledChanged(enable);
	enabled = enable;
}

void QtDataXfer::updateItemVariable(QTableWidgetItem *item)
{
	if(!passChangeSignals)
		return;

	const unsigned int index = item->data(Qt::UserRole).toUInt();
	dataXfer.variableEdited(index, item->text().toStdString());
}
