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
