#ifndef INTERCEPTQPLAINTEXTEDIT_H
#define INTERCEPTQPLAINTEXTEDIT_H

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
};

#endif // INTERCEPTQPLAINTEXTEDIT_H
