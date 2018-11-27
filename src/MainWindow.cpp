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

MainWindow::MainWindow(const QCommandLineParser& parser, QWidget* parent) :
	QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, appIcon(":/bullycpp.png")
	, picDriver(new QtPicDriver(parser.values("piclist")))
#ifndef NO_UPDATE_CHECK
	, checker("thirtythreeforty", "bullycpp", VERSION_STRING)
#endif
{
	ui->setupUi(this);
	setWindowIcon(appIcon);

	ui->progressWidget->hide();
	ui->programmingErrorLabel->hide();

	ui->dataXferTable->hide();
	qtDataXfer.setTableWidget(ui->dataXferTable);

	thread.start();
	picDriver->moveToThread(&thread);

	connect(ui->programButton, &QAbstractButton::clicked, this, &MainWindow::onProgramButtonClicked);
	connect(ui->hexFileNameEdit, &QLineEdit::textChanged, this, &MainWindow::onHexFileTextChanged);
	connect(ui->hexFileNameEdit, &QLineEdit::textChanged, this, &MainWindow::saveHexFilePref);
	connect(this, &MainWindow::programHexFile,
	        picDriver, static_cast<void (QtPicDriver::*)(QString)>(&QtPicDriver::programHexFile));
	connect(picDriver, &QtPicDriver::deviceChanged, ui->deviceInfoLabel, &QLabel::setText);
	connect(picDriver, &QtPicDriver::programmingStateChanged, ui->progressWidget, &QWidget::setVisible);
	connect(picDriver, &QtPicDriver::programmingStateChanged, ui->programmingWidget, &QWidget::setHidden);

	connect(ui->chooseHexFileButton, &QAbstractButton::clicked, this, &MainWindow::onChooseHexFileClicked);

	connect(ui->mclrCheckBox, &QAbstractButton::toggled, picDriver, &QtPicDriver::setMCLROnProgram);
	connect(ui->mclrCheckBox, &QAbstractButton::toggled, this, &MainWindow::saveMclrPref);

	connect(ui->configBitCheckBox, &QAbstractButton::toggled, picDriver, &QtPicDriver::setConfigBitsEnabled);
	connect(ui->configBitCheckBox, &QAbstractButton::toggled, this, &MainWindow::saveConfigBitsPref);

	connect(ui->mclrButton, &StickyQButton::toggled, picDriver, &QtPicDriver::setMCLR);
	connect(picDriver, &QtPicDriver::mclrChanged, ui->mclrButton, &StickyQButton::setChecked);

	connect(ui->serialText, &InterceptQPlainTextEdit::keyPressed, [=](QString s){ qtDataXfer.processOutboundBytes(s.toLocal8Bit()); });
	connect(&qtDataXfer, &QtDataXfer::sendRawBytes, picDriver, &QtPicDriver::sendSerialData);
	connect(picDriver, &QtPicDriver::serialDataReceived, &qtDataXfer, &QtDataXfer::processInboundBytes);
	connect(&qtDataXfer, &QtDataXfer::inboundBytesReady, this, &MainWindow::onSerialTextReceived);

	connect(ui->clearSerialButton, &QAbstractButton::clicked, this, &MainWindow::onClearSerialClicked);
	connect(ui->saveSerialButton, &QAbstractButton::clicked, this, &MainWindow::onSaveSerialClicked);

	connect(ui->aboutButton, &QAbstractButton::clicked, this, &MainWindow::showAbout);

	connectSerialPortComboBox();
	connect(ui->serialPortComboBox, &PopupAlertQComboBox::popupShown, this, &MainWindow::refreshSerialPortsKeepCurrent);

	connect(ui->baudComboBox, &QComboBox::currentTextChanged, picDriver, &QtPicDriver::setBaudRate);
	connect(picDriver, &QtPicDriver::serialPortStatusChanged, ui->serialText, &QWidget::setEnabled);
	connect(picDriver, &QtPicDriver::serialPortStatusChanged, ui->serialErrorWidget, &QWidget::setHidden);
	connect(picDriver, &QtPicDriver::serialPortStatusChanged, ui->mclrButton, &QWidget::setEnabled);
	connect(picDriver, &QtPicDriver::serialPortErrorChanged, ui->serialStatusLabel, &QLabel::setText);
	connect(picDriver, &QtPicDriver::serialPortErrorChanged, this, &MainWindow::tryEnableProgramButton);
	connect(ui->retrySerialButton, &QAbstractButton::clicked, picDriver, &QtPicDriver::openSerialPort);

	connect(picDriver, &QtPicDriver::programmingStateChanged, ui->progressWidget, &QWidget::setVisible);
	connect(picDriver, &QtPicDriver::programmingStateChanged, ui->programmingWidget, &QWidget::setHidden);
	connect(picDriver, &QtPicDriver::programmingErrorChanged, ui->programmingErrorLabel, &QWidget::setVisible);
	connect(picDriver, &QtPicDriver::programmingProgressChanged, this, &MainWindow::onProgrammingProgressChanged);

	connect(ui->useDataXferCheckBox, &QAbstractButton::toggled, ui->dataXferTable, &QWidget::setVisible);
	connect(ui->useDataXferCheckBox, &QAbstractButton::toggled, &qtDataXfer, &QtDataXfer::enable);
	connect(ui->mclrButton, &StickyQButton::pressed, [=]{ ui->dataXferTable->setRowCount(0); });

	connect(ui->sendMsg_TextBox, &QLineEdit::returnPressed, this, &MainWindow::sendMsgEnterPressed);
	connect(ui->sendMsg_Button, &QAbstractButton::clicked, this, &MainWindow::sendMsgButtonClicked);

#ifndef NO_UPDATE_CHECK
	connect(&checker, &GitHubUpdateChecker::updateAvailable, this, &MainWindow::onUpdateAvailable);
#endif

	// Set the serial window's font to be a monospace font.
	// The "Monospace" suggestion won't work on Windows, which is what the StyleHint is for.
	QFont monoFont("Monospace");
	monoFont.setStyleHint(QFont::TypeWriter);
	ui->serialText->setFont(monoFont);

	// Set up the serial port combo box.
	// We would like to refresh every 5 seconds (or so).
	refreshSerialPorts(settings.value(SERIAL_PORT_NAME_KEY).toString(), true);

	using std::begin; using std::end;
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

#ifndef NO_UPDATE_CHECK
	if(settings.value(AUTO_UPDATE_KEY, true).toBool())
		checker.checkForUpdate();
#endif

	// Set the monospace font for the send message TextBox
	ui->sendMsg_TextBox->setFont(monoFont);
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

void MainWindow::refreshSerialPortsKeepCurrent()
{
	// Don't dis/reconnect to the same serial port every time the user opens the list!
	disconnectSerialPortComboBox();
	refreshSerialPorts(ui->serialPortComboBox->currentText(), false);
	connectSerialPortComboBox();
}

void MainWindow::showAbout()
{
	QMessageBox aboutBox(QMessageBox::NoIcon,
	                     QStringLiteral("About BullyCPP"),
	                     "<p align='center'>"
	                     "<h2>BullyCPP " VERSION_STRING "</h2><br>"
	                     "Copyright &#0169; 2014 - 2015 George Hilliard (\"thirtythreeforty\") and contributors"
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

#ifndef NO_UPDATE_CHECK
	QCheckBox* cb = new QCheckBox("Check for updates automatically");
	cb->setChecked(settings.value(AUTO_UPDATE_KEY, true).toBool());
	aboutBox.setCheckBox(cb);
#endif

	aboutBox.exec();

#ifndef NO_UPDATE_CHECK
	settings.setValue(AUTO_UPDATE_KEY, cb->isChecked());
	settings.sync();
#endif
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

void MainWindow::refreshSerialPorts(const QString& current, bool startingUp)
{
	using std::begin; using std::end;
	QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
	QStringList portStrings;
	std::transform(begin(ports), end(ports), std::back_inserter(portStrings),
	               [](QSerialPortInfo& i){ return i.portName(); });
	int chosenSerialPortIndex = -1;

	if(startingUp && settings.contains(SERIAL_PORT_NAME_KEY))
		chosenSerialPortIndex = getPositionIfPresent(portStrings, settings.value(SERIAL_PORT_NAME_KEY).toString(), chosenSerialPortIndex);
	if(!current.isEmpty())
		chosenSerialPortIndex = addIfNotPresent(portStrings, current);
	ui->serialPortComboBox->clear();
	ui->serialPortComboBox->addItems(portStrings);
	if(chosenSerialPortIndex != -1)
		ui->serialPortComboBox->setCurrentIndex(chosenSerialPortIndex);
}

void MainWindow::connectSerialPortComboBox() {
	connect(ui->serialPortComboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged),
	        picDriver, &QtPicDriver::setSerialPort);
	connect(ui->serialPortComboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged),
	        this, &MainWindow::saveSerialPortPref);
}

void MainWindow::disconnectSerialPortComboBox() {
	disconnect(ui->serialPortComboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged),
	           picDriver, &QtPicDriver::setSerialPort);
	disconnect(ui->serialPortComboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged),
	           this, &MainWindow::saveSerialPortPref);
}

void MainWindow::sendMsgButtonClicked() {
	// grab the text from the message box
	QString text = ui->sendMsg_TextBox->text();

	// if control key is not pressed, insert newline
	if (!QGuiApplication::queryKeyboardModifiers().testFlag(Qt::ControlModifier)) {
		text.append('\n');
	}

	// actually send the data
	qtDataXfer.processOutboundBytes(text.toLocal8Bit());

	// clear the box if told to
	if (ui->clearOnSendCheckBox->isChecked()) {
		ui->sendMsg_TextBox->clear();
		ui->sendMsg_TextBox->setFocus();
	}
}

void MainWindow::sendMsgEnterPressed() {
	// if checked, click the button
	// otherwise, insert a newline character
	if (ui->sendOnReturnCheckBox->isChecked()) {
		ui->sendMsg_Button->click();
	} else {
		ui->sendMsg_TextBox->insert("\n");
	}
}
