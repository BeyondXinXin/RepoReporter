#include "CommandLineManager.h"

#include <QCommandLineParser>

CommandLineManager::CmdOption CommandLineManager::option = {
	false,
};

void CommandLineManager::Initial(const QApplication& app)
{
	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addVersionOption();
	parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

	QCommandLineOption debugOption("d", "debug mode");
	parser.addOption(debugOption);

	parser.process(app);


	if (parser.isSet(debugOption)) {
		CommandLineManager::option.debug = true;
	}
}
