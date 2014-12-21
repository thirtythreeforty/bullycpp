#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCommandLineParser>
#include <QFileDialog>
#include <QMainWindow>
#include <QThread>

#include "InterceptQPlainTextEdit.h"
#include "QtPicDriver.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(const QCommandLineParser& parser, QWidget* parent = 0);
	virtual ~MainWindow();

signals:
	void sendSerialData(QByteArray);
	void programHexFile(QString);

private slots:
	void onProgramButtonClicked();
	void onSaveSerial(QString path);
	void onSerialTextSend(QString text);
	void onSerialTextReceived(QByteArray data);
	void onHexFileTextChanged(QString);
	void tryEnableProgramButton();

private:
	Ui::MainWindow *ui;
	QThread thread;
	QtPicDriver *picDriver;
	QFileDialog hexFileDialog;
	QFileDialog saveLogDialog;
};

#endif // MAINWINDOW_H
