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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCommandLineParser>
#include <QFileDialog>
#include <QMainWindow>
#include <QThread>
#include <QSettings>

#include "GitHubUpdateChecker.h"
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
	void onProgrammingProgressChanged(QString progress, int percent);
	void onSaveSerial(QString path);
	void onSerialTextSend(QString text);
	void onSerialTextReceived(QByteArray data);
	void onHexFileTextChanged(QString);
	void onUpdateAvailable(QString version, QString url);
	void tryEnableProgramButton();
	void showAbout();

private:
	Ui::MainWindow *ui;
	QThread thread;
	QtPicDriver *picDriver;
	QFileDialog hexFileDialog;
	QFileDialog saveLogDialog;
	GitHubUpdateChecker checker;
	QSettings settings;
};

#endif // MAINWINDOW_H
