#ifndef CommandLineManager_H
#define CommandLineManager_H

#include <QApplication>

class CommandLineManager : public QObject {
	Q_OBJECT

public:

	struct CmdOption {
		bool debug;
	};

public:

	static void Initial(const QApplication& app);
	static CmdOption option;
};

#endif // ifndef CommandLineManager_H
