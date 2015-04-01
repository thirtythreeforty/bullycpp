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

#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <algorithm>

#include <QCheckBox>
#include <QFile>
#include <QList>
#include <QIntValidator>
#include <QMessageBox>
#include <QSerialPortInfo>

#define AUTO_UPDATE_KEY       QStringLiteral("autoUpdate")
#define SERIAL_PORT_NAME_KEY  QStringLiteral("serialPortName")
#define MCLR_ON_PROGRAM_KEY   QStringLiteral("mclrOnProgram")
#define PROGRAM_CONFIG_KEY    QStringLiteral("programConfigBits")
#define HEX_FILE_NAME_KEY     QStringLiteral("hexFileName")

#define VERSION_STRING "v0.7"

MainWindow::MainWindow(const QCommandLineParser& parser, QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	appIcon(":/bullycpp.png"),
	picDriver(new QtPicDriver(parser.values("piclist"))),
	checker("thirtythreeforty", "bullycpp", VERSION_STRING)
{
	ui->setupUi(this);
	setWindowIcon(appIcon);

	ui->progressWidget->hide();
	ui->programmingErrorLabel->hide();

	thread.start();
	picDriver->moveToThread(&thread);

	connect(ui->programButton, SIGNAL(clicked()), SLOT(onProgramButtonClicked()));
	connect(ui->hexFileNameEdit, SIGNAL(textChanged(QString)), SLOT(onHexFileTextChanged(QString)));
	connect(ui->hexFileNameEdit, SIGNAL(textChanged(QString)), SLOT(saveHexFilePref(QString)));
	connect(this, SIGNAL(programHexFile(QString)), picDriver, SLOT(programHexFile(QString)));
	connect(picDriver, SIGNAL(deviceChanged(QString)), ui->deviceInfoLabel, SLOT(setText(QString)));
	connect(picDriver, SIGNAL(programmingStateChanged(bool)), ui->progressWidget, SLOT(setVisible(bool)));
	connect(picDriver, SIGNAL(programmingStateChanged(bool)), ui->programmingWidget, SLOT(setHidden(bool)));

	connect(ui->chooseHexFileButton, SIGNAL(clicked()), SLOT(onChooseHexFileClicked()));

	connect(ui->mclrCheckBox, SIGNAL(toggled(bool)), picDriver, SLOT(setMCLROnProgram(bool)));
	connect(ui->mclrCheckBox, SIGNAL(toggled(bool)), SLOT(saveMclrPref(bool)));

	connect(ui->configBitCheckBox, SIGNAL(toggled(bool)), picDriver, SLOT(setConfigBitsEnabled(bool)));
	connect(ui->configBitCheckBox, SIGNAL(toggled(bool)), SLOT(saveConfigBitsPref(bool)));

	connect(ui->mclrButton, SIGNAL(clicked(bool)), picDriver, SLOT(setMCLR(bool)));
	connect(picDriver, SIGNAL(mclrChanged(bool)), ui->mclrButton, SLOT(setChecked(bool)));

	connect(ui->serialText, SIGNAL(keyPressed(QString)), SLOT(onSerialTextSend(QString)));
	connect(picDriver, SIGNAL(serialDataReceived(QByteArray)), SLOT(onSerialTextReceived(QByteArray)));
	connect(this, SIGNAL(sendSerialData(QByteArray)), picDriver, SLOT(sendSerialData(QByteArray)));

	connect(ui->clearSerialButton, SIGNAL(clicked()), SLOT(onClearSerialClicked()));
	connect(ui->saveSerialButton, SIGNAL(clicked()), SLOT(onSaveSerialClicked()));

	connect(ui->aboutButton, SIGNAL(clicked()), SLOT(showAbout()));

	connect(ui->serialPortComboBox, SIGNAL(currentIndexChanged(QString)), picDriver, SLOT(setSerialPort(QString)));
	connect(ui->serialPortComboBox, SIGNAL(currentIndexChanged(QString)), SLOT(saveSerialPortPref(QString)));
	connect(ui->baudComboBox, SIGNAL(currentTextChanged(QString)), picDriver, SLOT(setBaudRate(QString)));
	connect(picDriver, SIGNAL(serialPortStatusChanged(bool)), ui->serialText, SLOT(setEnabled(bool)));
	connect(picDriver, SIGNAL(serialPortStatusChanged(bool)), ui->serialErrorWidget, SLOT(setHidden(bool)));
	connect(picDriver, SIGNAL(serialPortStatusChanged(bool)), ui->mclrButton, SLOT(setEnabled(bool)));
	connect(picDriver, SIGNAL(serialPortErrorChanged(QString)), ui->serialStatusLabel, SLOT(setText(QString)));
	connect(picDriver, SIGNAL(serialPortErrorChanged(QString)), SLOT(tryEnableProgramButton()));
	connect(ui->retrySerialButton, SIGNAL(clicked()), picDriver, SLOT(openSerialPort()));

	connect(picDriver, SIGNAL(programmingStateChanged(bool)), ui->progressWidget, SLOT(setVisible(bool)));
	connect(picDriver, SIGNAL(programmingStateChanged(bool)), ui->programmingWidget, SLOT(setHidden(bool)));
	connect(picDriver, SIGNAL(programmingErrorChanged(bool)), ui->programmingErrorLabel, SLOT(setVisible(bool)));
	connect(picDriver, SIGNAL(programmingProgressChanged(QString,int)), SLOT(onProgrammingProgressChanged(QString,int)));

	connect(&checker, SIGNAL(updateAvailable(QString,QString)), SLOT(onUpdateAvailable(QString,QString)));

	// Set the serial window's font to be a monospace font.
	// The "Monospace" suggestion won't work on Windows, which is what the StyleHint is for.
	QFont monoFont("Monospace");
	monoFont.setStyleHint(QFont::TypeWriter);
	ui->serialText->setFont(std::move(monoFont));

	using std::begin; using std::end;
	QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
	QStringList portStrings;
	std::transform(begin(ports), end(ports), std::back_inserter(portStrings),
	               [](QSerialPortInfo& i){ return i.portName(); });
	int chosenSerialPortIndex = -1;
	if(settings.contains(SERIAL_PORT_NAME_KEY))
		chosenSerialPortIndex = getPositionIfPresent(portStrings, settings.value(SERIAL_PORT_NAME_KEY).toString(), chosenSerialPortIndex);
	if(parser.isSet("device"))
		chosenSerialPortIndex = addIfNotPresent(portStrings, parser.value("device"));
	ui->serialPortComboBox->addItems(portStrings);
	if(chosenSerialPortIndex != -1)
		ui->serialPortComboBox->setCurrentIndex(chosenSerialPortIndex);

	QList<qint32> bauds = QSerialPortInfo::standardBaudRates();
	QStringList baudStrings;
	std::transform(begin(bauds), end(bauds), std::back_inserter(baudStrings),
	               [](qint32 baud){ QString s; s.setNum(baud); return s; });
	// Baud is always set in parser (default option is present).
	// Because of this fact, it is difficult to determine the "most wanted"
	// value, so we don't save this one.
	int chosenBaudIndex = addIfNotPresent(baudStrings, parser.value("baud"));
	ui->baudComboBox->addItems(baudStrings);
	ui->baudComboBox->setCurrentIndex(chosenBaudIndex);
	ui->baudComboBox->setValidator(new QIntValidator(ui->baudComboBox));

	if(parser.isSet("mclr") || settings.value(MCLR_ON_PROGRAM_KEY, false).toBool())
		ui->mclrCheckBox->setChecked(true);

	if(parser.isSet("configbits") || settings.value(PROGRAM_CONFIG_KEY, false).toBool())
		ui->configBitCheckBox->setChecked(true);

	QString savedHexFileName;
	if(parser.positionalArguments().size())
		ui->hexFileNameEdit->setText(parser.positionalArguments()[0]);
	else if((savedHexFileName = settings.value(HEX_FILE_NAME_KEY, "").toString()).size())
		ui->hexFileNameEdit->setText(savedHexFileName);

	if(settings.value(AUTO_UPDATE_KEY, true).toBool())
		checker.checkForUpdate();
}

MainWindow::~MainWindow()
{
	picDriver->deleteLater();
	thread.exit();
	thread.wait();

	delete ui;
}

void MainWindow::onClearSerialClicked()
{
	ui->serialText->clear();
	rawSerialBuffer.clear();
}

void MainWindow::onChooseHexFileClicked()
{
	const QString fileName = QFileDialog::getOpenFileName(this,
		"Choose Hex File",                       // caption
		QString(),                               // dir (default)
		"Intel Hex files (*.hex);;All files (*)" // filter
	);
	if(!fileName.isNull())
		ui->hexFileNameEdit->setText(fileName);
}

void MainWindow::onProgramButtonClicked()
{
	emit programHexFile(ui->hexFileNameEdit->text());
}

void MainWindow::onSaveSerialClicked()
{
	const QString path = QFileDialog::getSaveFileName(this,
		QString(),           // caption (default)
		QString(),           // dir (default)
		"Text files (*.txt)" // filter
	);

	if(!path.isNull()) {
		QFile file(path);
		if(file.open(QFile::WriteOnly)) {
			file.write(rawSerialBuffer);
			file.close();
		}
		else {
			QMessageBox::critical(this, "Error", "Could not write log file!");
		}
	}
}

void MainWindow::onSerialTextSend(QString text)
{
	emit sendSerialData(text.toLocal8Bit());
}

void MainWindow::onSerialTextReceived(QByteArray data)
{
	// Keep a copy of the data for saving.
	rawSerialBuffer.append(data);

	// Strip null characters from the data we display.
	data.replace('\0', "");

	ui->serialText->moveCursor(QTextCursor::End);
	ui->serialText->insertPlainText(data);
	ui->serialText->moveCursor(QTextCursor::End);
}

void MainWindow::onHexFileTextChanged(QString)
{
	tryEnableProgramButton();
}

void MainWindow::onProgrammingProgressChanged(QString progress, int percent)
{
	ui->programmingProgressBar->setFormat(progress);
	ui->programmingProgressBar->setValue(percent);
	ui->programmingErrorLabel->setText(progress);
}

void MainWindow::onUpdateAvailable(QString version, QString url)
{
	const QString& newReleaseMessage = QStringLiteral("A new release, %1, is available.  <a href=\"%2\">Click here</a> to download it.<p>You can turn off update checking in the About screen.");
	QMessageBox::information(this, "New Release Available", newReleaseMessage.arg(version, url));
}

void MainWindow::tryEnableProgramButton()
{
	// Only makes sense to enable the programButton if serial is enabled
	// and a filename is in the box
	ui->programButton->setEnabled(
		ui->hexFileNameEdit->text().size() != 0 && ui->serialText->isEnabled()
	);
}

void MainWindow::showAbout()
{
	QMessageBox aboutBox(QMessageBox::NoIcon,
	                     QStringLiteral("About BullyCPP"),
	                     "<p align='center'>"
	                     "<h2>BullyCPP " VERSION_STRING "</h2><br>"
	                     "Copyright &#0169; 2014 - 2015 George Hilliard (\"thirtythreeforty\")"
	                     "<p align='center'>"
	                     "See <a href='https://www.github.com/thirtythreeforty/bullycpp'>www.github.com/thirtythreeforty/bullycpp</a> "
	                     "for updates and source code."
	                     "<p>"
	                     "BullyCPP is a serial console and a driver for the open source Bully Bootloader for the PIC24 and dsPIC33.  "
	                     "See <a href='http://www.reesemicro.com/'>www.reesemicro.com</a> for the microcontroller firmware."
	                     "<p>"
	                     "This program is free software; you can redistribute it and/or modify it under the terms of "
	                     "the GNU General Public License v3 or later, as published by the Free Software Foundation.",
	                     QMessageBox::Ok,
	                     this
	);

	aboutBox.setIconPixmap(appIcon.pixmap(128, 128));

	QCheckBox* cb = new QCheckBox("Check for updates automatically");

	cb->setChecked(settings.value(AUTO_UPDATE_KEY, true).toBool());

	aboutBox.setCheckBox(cb);
	aboutBox.exec();

	settings.setValue(AUTO_UPDATE_KEY, cb->isChecked());
	settings.sync();
}

void MainWindow::saveSerialPortPref(QString name)
{
	settings.setValue(SERIAL_PORT_NAME_KEY, name);
}

void MainWindow::saveHexFilePref(QString name)
{
	settings.setValue(HEX_FILE_NAME_KEY, name);
}

void MainWindow::saveMclrPref(bool newState)
{
	settings.setValue(MCLR_ON_PROGRAM_KEY, newState);
}

void MainWindow::saveConfigBitsPref(bool newState)
{
	settings.setValue(PROGRAM_CONFIG_KEY, newState);
}

int MainWindow::getPositionIfPresent(QStringList& list, const QString& value, int oldPosition)
{
	using std::begin; using std::end;
	auto it = std::find(begin(list), end(list), value);
	if(it != end(list))
		return it - begin(list);
	return oldPosition;
}

int MainWindow::addIfNotPresent(QStringList& list, const QString& value)
{
	using std::begin; using std::end;
	auto it = std::find(begin(list), end(list), value);
	if(it == end(list)) {
		int position = list.size();
		list.append(value);
		return position;
	}
	else
		return it - begin(list);
}
