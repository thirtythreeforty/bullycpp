#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <algorithm>

#include <QFile>
#include <QList>
#include <QIntValidator>
#include <QMessageBox>
#include <QSerialPortInfo>

MainWindow::MainWindow(const QCommandLineParser& parser, QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	picDriver(new QtPicDriver(parser.values("piclist")))
{
	ui->setupUi(this);

	ui->progressWidget->hide();

	thread.start();
	picDriver->moveToThread(&thread);

	connect(ui->programButton, SIGNAL(clicked()), SLOT(onProgramButtonClicked()));
	connect(ui->hexFileNameEdit, SIGNAL(textChanged(QString)), SLOT(onHexFileTextChanged(QString)));
	connect(this, SIGNAL(programHexFile(QString)), picDriver, SLOT(programHexFile(QString)));
	connect(picDriver, SIGNAL(deviceChanged(QString)), ui->deviceInfoLabel, SLOT(setText(QString)));
	connect(picDriver, SIGNAL(programmingStateChanged(bool)), ui->progressWidget, SLOT(setVisible(bool)));
	connect(picDriver, SIGNAL(programmingStateChanged(bool)), ui->programmingWidget, SLOT(setHidden(bool)));

	connect(ui->chooseHexFileButton, SIGNAL(clicked()), &hexFileDialog, SLOT(open()));
	connect(&hexFileDialog, SIGNAL(fileSelected(QString)), ui->hexFileNameEdit, SLOT(setText(QString)));

	connect(ui->mclrCheckBox, SIGNAL(toggled(bool)), picDriver, SLOT(setMCLROnProgram(bool)));

	connect(ui->configBitCheckBox, SIGNAL(toggled(bool)), picDriver, SLOT(setConfigBitsEnabled(bool)));

	connect(ui->mclrButton, SIGNAL(clicked(bool)), picDriver, SLOT(setMCLR(bool)));
	connect(picDriver, SIGNAL(mclrChanged(bool)), ui->mclrButton, SLOT(setChecked(bool)));

	connect(ui->serialText, SIGNAL(keyPressed(QString)), SLOT(onSerialTextSend(QString)));
	connect(picDriver, SIGNAL(serialDataReceived(QByteArray)), SLOT(onSerialTextReceived(QByteArray)));
	connect(this, SIGNAL(sendSerialData(QByteArray)), picDriver, SLOT(sendSerialData(QByteArray)));

	connect(ui->clearSerialButton, SIGNAL(clicked()), ui->serialText, SLOT(clear()));

	connect(ui->saveSerialButton, SIGNAL(clicked()), &saveLogDialog, SLOT(open()));
	connect(&saveLogDialog, SIGNAL(fileSelected(QString)), SLOT(onSaveSerial(QString)));

	connect(ui->aboutButton, SIGNAL(clicked()), SLOT(showAbout()));

	connect(ui->serialPortComboBox, SIGNAL(currentIndexChanged(QString)), picDriver, SLOT(setSerialPort(QString)));
	connect(ui->baudComboBox, SIGNAL(currentTextChanged(QString)), picDriver, SLOT(setBaudRate(QString)));
	connect(picDriver, SIGNAL(serialPortStatusChanged(bool)), ui->serialText, SLOT(setEnabled(bool)));
	connect(picDriver, SIGNAL(serialPortStatusChanged(bool)), ui->serialErrorWidget, SLOT(setHidden(bool)));
	connect(picDriver, SIGNAL(serialPortStatusChanged(bool)), ui->mclrButton, SLOT(setEnabled(bool)));
	connect(picDriver, SIGNAL(serialPortErrorChanged(QString)), ui->serialStatusLabel, SLOT(setText(QString)));
	connect(picDriver, SIGNAL(serialPortErrorChanged(QString)), SLOT(tryEnableProgramButton()));
	connect(ui->retrySerialButton, SIGNAL(clicked()), picDriver, SLOT(openSerialPort()));

	connect(picDriver, SIGNAL(programmingStateChanged(bool)), ui->progressWidget, SLOT(setVisible(bool)));
	connect(picDriver, SIGNAL(programmingStateChanged(bool)), ui->programmingWidget, SLOT(setHidden(bool)));
	connect(picDriver, SIGNAL(programmingProgressChanged(QString,int)), SLOT(onProgrammingProgressChanged(QString,int)));

	using std::begin; using std::end;
	QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
	QStringList portStrings;
	std::transform(begin(ports), end(ports), std::back_inserter(portStrings),
	               [](QSerialPortInfo& i){ return i.portName(); });
	ui->serialPortComboBox->addItems(portStrings);
	if(parser.isSet("device")) {
		auto it = std::find(begin(portStrings), end(portStrings), parser.value("device"));
		if(it == end(portStrings)) {
			ui->serialPortComboBox->addItem(parser.value("device"));
			// Custom value inserted at end of list
			ui->serialPortComboBox->setCurrentIndex(portStrings.size());
		}
		else
			ui->serialPortComboBox->setCurrentIndex(it - begin(portStrings));
	}

	QList<qint32> bauds = QSerialPortInfo::standardBaudRates();
	QStringList baudStrings;
	std::transform(begin(bauds), end(bauds), std::back_inserter(baudStrings),
	               [](qint32 baud){ QString s; s.setNum(baud); return s; });
	ui->baudComboBox->addItems(baudStrings);
	// baud is always set (default option is present)
	auto it = std::find(begin(baudStrings), end(baudStrings), parser.value("baud"));
	if(it == end(baudStrings)) {
		ui->baudComboBox->addItem(parser.value("baud"));
		ui->baudComboBox->setCurrentIndex(baudStrings.size());
	}
	else
		ui->baudComboBox->setCurrentIndex(it - begin(baudStrings));
	ui->baudComboBox->setValidator(new QIntValidator(ui->baudComboBox));

	ui->mclrCheckBox->setChecked(parser.isSet("mclr"));

	ui->configBitCheckBox->setChecked(parser.isSet("configbits"));

	if(parser.positionalArguments().size())
		ui->hexFileNameEdit->setText(parser.positionalArguments()[0]);

	hexFileDialog.setNameFilters({"Intel Hex files (*.hex)", "All files (*)"});
	hexFileDialog.setFileMode(QFileDialog::ExistingFile);

	saveLogDialog.setNameFilter("Text files (*.txt)");
	saveLogDialog.setDefaultSuffix("txt");
}

MainWindow::~MainWindow()
{
	picDriver->deleteLater();
	thread.exit();
	thread.wait();

	delete ui;
}

void MainWindow::onProgramButtonClicked()
{
	emit programHexFile(ui->hexFileNameEdit->text());
}

void MainWindow::onSaveSerial(QString path)
{
	QFile file(path);
	if(file.open(QFile::WriteOnly)) {
		file.write(ui->serialText->toPlainText().toLatin1());
		file.close();
	}
	else {
		QMessageBox::critical(this, "Error", "Could not write log file!");
	}
}

void MainWindow::onSerialTextSend(QString text)
{
	emit sendSerialData(text.toLocal8Bit());
}

void MainWindow::onSerialTextReceived(QByteArray data)
{
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
	QMessageBox::about(this, QStringLiteral("About BullyCPP"),
	                   "<p align='center'>"
	                   "<h2>BullyCPP v0.2</h2><br>"
	                   "Copyright &#0169; 2014 George Hilliard (\"thirtythreeforty\")"
	                   "<p align='center'>"
	                   "See <a href='https://www.github.com/thirtythreeforty/bullycpp'>www.github.com/thirtythreeforty/bullycpp</a> "
	                   "for updates and source code."
	                   "<p>"
	                   "BullyCPP is a serial console and a driver for the open source Bully Bootloader for the PIC24 and dsPIC33.  "
	                   "See <a href='http://www.reesemicro.com/'>www.reesemicro.com</a> for the microcontroller firmware."
	                   "<p>"
	                   "This program is free software; you can redistribute it and/or modify it under the terms of "
	                   "the GNU General Public License v3 or later, as published by the Free Software Foundation."
	);
}
