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

#ifndef INTERCEPTQPLAINTEXTEDIT_H
#define INTERCEPTQPLAINTEXTEDIT_H

#include <QFile>
#include <QMimeData>
#include <QPlainTextEdit>

class InterceptQPlainTextEdit : public QPlainTextEdit
{
	Q_OBJECT

public:
	InterceptQPlainTextEdit(QWidget* parent = 0)
		: QPlainTextEdit(parent)
	{}
	virtual ~InterceptQPlainTextEdit() = default;

signals:
	void keyPressed(QString);

protected:
	virtual void keyPressEvent(QKeyEvent *e) {
		// Do not forward this upward (we don't want local echo)
		emit keyPressed(e->text());
	}
	virtual void insertFromMimeData(const QMimeData *source) {
		if(source->hasUrls()) {
			for(const auto& url: source->urls()) {
				QFile file(url.toLocalFile());
				if(file.open(QIODevice::ReadOnly))
					emit keyPressed(file.readAll());
			}
		}
		else
			emit keyPressed(source->text());
	}
};

#endif // INTERCEPTQPLAINTEXTEDIT_H
