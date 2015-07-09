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

#ifndef POPUPALERTQCOMBOBOX_H
#define POPUPALERTQCOMBOBOX_H

#include <QComboBox>

class PopupAlertQComboBox : public QComboBox {
	Q_OBJECT
public:
	PopupAlertQComboBox(QWidget* parent) : QComboBox(parent) {}

	inline virtual void showPopup() {
		emit popupShown();
		QComboBox::showPopup();
	}

	inline virtual void hidePopup() {
		emit popupHidden();
		QComboBox::hidePopup();
	}

signals:
	void popupShown();
	void popupHidden();
};

#endif // POPUPALERTQCOMBOBOX_H

