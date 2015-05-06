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

#include <QMenu>
#include <QToolButton>

/**
 * This class repurposes a QToolButton to serve as a locking button.
 * The toggled() signal is emitted when the button is pressed, released, locked, or unlocked.
 * The only signal that clients should care about is the toggled() signal.
 */
class StickyQButton : public QToolButton
{
	Q_OBJECT
public:
	explicit StickyQButton(QWidget * parent=0)
		: QToolButton(parent)
	{
		// We need to have a menu in order for the arrow button to click correctly.
		setMenu(&signalingQMenu);

		// Emit the toggled signal whenever the button is pressed or released.
		// This will cause a double signal whenever the button is clicked normally after being held down with
		// the arrow, but this is OK for our purposes.
		connect(this, &QAbstractButton::pressed, [=]{ emit toggled(true); });
		connect(this, &QAbstractButton::released, [=]{ emit toggled(false); });

		connect(this, &QAbstractButton::released, this, &StickyQButton::uncheck);

		connect(&signalingQMenu, &QMenu::aboutToShow, this, &StickyQButton::toggleChecked);

		// We use a queued connection for this because we want the QMenu to appear before our close signal reaches it
		connect(&signalingQMenu, &QMenu::aboutToShow, &signalingQMenu, &QMenu::close, Qt::QueuedConnection);
	}
	virtual ~StickyQButton() = default;

public slots:
	void toggleChecked() {
		if(isCheckable()) {
			setChecked(false);
			setCheckable(false);
		}
		else {
			setCheckable(true);
			setChecked(true);
		}
	}

	void uncheck() {
		if(isCheckable()) {
			setChecked(false);
			setCheckable(false);
		}
	}

private:
	QMenu signalingQMenu;
};

#endif // STICKYQBUTTON

