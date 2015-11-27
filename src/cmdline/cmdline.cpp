#include <iostream>

#include <QCoreApplication>
#include <QProcess>
#include <QString>

int main(int argc, char** argv)
{
	QCoreApplication app(argc, argv);
	QProcess child;
	QString childPath(QCoreApplication::applicationDirPath() + "/BullyCPPw");
	QStringList args;
	for(int i = 1; i < argc; ++i) {
		args.push_back({argv[i]});
	}

	// Windows doesn't forward GUI programs' standard output and error, on
	// purpose.  So, we cannot use the standard QProcess::ForwardChannels (or
	// even just QProcess::execute)... We have to shuffle it ourselves.  And
	// yes, this is ridiculous.  Currently we do not redirect standard input,
	// as the main executable does not need it.
	QObject::connect(&child, &QProcess::readyReadStandardOutput,
	                 [&]{ std::cout << child.readAllStandardOutput().toStdString(); });
	QObject::connect(&child, &QProcess::readyReadStandardError,
	                 [&]{ std::cerr << child.readAllStandardError().toStdString(); });

	child.start(childPath, args);
	child.waitForFinished(-1);
	int retval = child.exitCode();

	if(retval == -1) {
		std::cout << argv[0] << ": Subprocess BullyCPPw crashed!" << std::endl;
	}
	else if(retval == -2) {
		std::cout << argv[0] << ": Subprocess " << childPath.toStdString()
		          << " could not be started!" << std::endl;
	}

	return retval;
}
