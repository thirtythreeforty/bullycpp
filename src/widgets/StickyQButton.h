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

#ifndef STICKYQBUTTON_H
#define STICKYQBUTTON_H

#include <QWidget>

namespace Ui {
class StickyQButton;
}

class StickyQButton : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString title READ title WRITE setTitle)

public:
	explicit StickyQButton(QWidget *parent = 0);
	~StickyQButton();

	void setTitle(const QString& title);
	QString title() const;

signals:
	void toggled(bool checked) const;
	void pressed() const;

public slots:
	void setChecked(bool pressed);

private slots:
	void onLockToggle(bool checked);
	void onPushToggle(bool checked);

private:
	Ui::StickyQButton *ui;
};

#endif // STICKYQBUTTON_H
