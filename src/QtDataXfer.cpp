#include "QtDataXfer.h"

#include <QCheckBox>
#include <QDateTime>
#include <QHBoxLayout>
#include <QTableWidgetItem>
#include <QWidget>

QtDataXfer::QtDataXfer(QObject *parent)
	: QObject(parent)
    , dataXferWrap(this)
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
    QTableWidgetItem* createWidgetItem(const std::string& text, bool editable) {
		QTableWidgetItem* item = new QTableWidgetItem(QString::fromStdString(text));
		if(!editable)
			item->setFlags(item->flags() & ~Qt::ItemIsEditable);
		return item;
	}
}

void QtDataXfer::variableUpdated(const unsigned int index,
                                 const std::string& name,
                                 const std::string& description,
                                 const std::string& value,
                                 const bool modifiable)
{
    // Grow the table this this index if past the last row.
    if (index >= table->rowCount()) {
        table->setRowCount(index + 1);
    }

	// Name
    table->setItem(index, 0, createWidgetItem(name, false));

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
    table->setCellWidget(index, 1, pWidget);

	// Value
    table->setItem(index, 2, createWidgetItem(value, modifiable));

	// Description
    table->setItem(index, 3, createWidgetItem(description, false));
}

void QtDataXfer::processOutboundBytes(QByteArray outbound)
{
    if(enabled) {
        std::string s = dataXferWrap.escapeDataOut(outbound.toStdString());
        emit sendRawBytes(QByteArray::fromStdString(s));
    } else
		emit sendRawBytes(outbound);
}

void QtDataXfer::processInboundBytes(QByteArray inbound)
{
	if(enabled)
        dataXferWrap.onDataIn(inbound.toStdString(), QDateTime::currentMSecsSinceEpoch());
	else
		emit inboundBytesReady(inbound);
}

void QtDataXfer::enable(bool enable)
{
	enabled = enable;
	emit enabledChanged(enable);
}

void QtDataXfer::updateItemVariable(QTableWidgetItem *item)
{
    const unsigned int index = item->row();
    dataXferWrap.variableEdited(index, item->text().toStdString());
}
