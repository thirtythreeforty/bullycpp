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

#include "StickyQButton.h"
#include "ui_StickyQButton.h"

StickyQButton::StickyQButton(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::StickyQButton)
{
	ui->setupUi(this);

	connect(ui->pushButton, &QPushButton::pressed, [this]{ emit toggled(true); });
	connect(ui->pushButton, &QPushButton::released, [this]{ emit toggled(false); });
	connect(ui->pushButton, &QPushButton::released, [this]{ ui->lockButton->setChecked(false); });

	connect(ui->lockButton, &QPushButton::toggled, this, &StickyQButton::onLockToggle);
}

StickyQButton::~StickyQButton()
{
	delete ui;
}

void StickyQButton::setTitle(const QString& title)
{
	ui->pushButton->setText(title);
}

QString StickyQButton::title() const
{
	return ui->pushButton->text();
}

void StickyQButton::onLockToggle(bool checked)
{
	emit toggled(checked);
	if(checked) {
		ui->pushButton->setCheckable(true);
		ui->pushButton->setChecked(true);
	}
	else {
		ui->pushButton->setChecked(false);
		ui->pushButton->setCheckable(false);
	}
}

void StickyQButton::setChecked(bool checked)
{
	onLockToggle(checked);
}
