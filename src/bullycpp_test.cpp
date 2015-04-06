#include <iostream>
#include <string>
#include <thread>

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "MainWindow.h"
#include "QtPicDriver.h"

extern "C" void runAllTests();

int main(int argc, char** argv)
{
    // Testing only
    runAllTests();

	return 0;
}
