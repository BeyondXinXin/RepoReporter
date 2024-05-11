#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDebug>
#include <QFile>
#include <QStandardItem>
#include <QScreen>

#include "utils/ConfigManager.h"
#include "utils/SystemTrayManager.h"
#include "utils/VersionControlManager.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	InitUI();
	InitConnect();

	SystemTrayManager::Instance()->setMainWidget(this);
	VersionControlManager::CheckAndSetQuotepath();
	qInfo() << u8"软件启动。";
}

MainWindow::~MainWindow()
{
	qInfo() << u8"软件关闭。";

	if (isMaximized()) {
		ConfigManager::GetInstance().WriteValue("LastNormalWindowGeometry", normalGeometry());
	} else {
		ConfigManager::GetInstance().WriteValue("LastNormalWindowGeometry", geometry());
	}

	delete ui;
}

void MainWindow::showEvent(QShowEvent* event)
{
	QList<int> size1 = ConfigManager::GetInstance().ReadList<int>(
		"VerticalSplitterSize", QList<int>{ 400, 1000 });
	QList<int> size2 = ConfigManager::GetInstance().ReadList<int>(
		"LevelSplitterSize", QList<int>{ 400, 400, 400 });
	ui->verticalSplitter->setSizes(size1);
	ui->levelSplitter->setSizes(size2);

	QMainWindow::showEvent(event);
}

void MainWindow::hideEvent(QHideEvent* event)
{
	ConfigManager::GetInstance().WriteList<int>(
		"VerticalSplitterSize", ui->verticalSplitter->sizes());
	ConfigManager::GetInstance().WriteList<int>(
		"LevelSplitterSize", ui->levelSplitter->sizes());
	QMainWindow::hideEvent(event);
}

void MainWindow::InitUI()
{
	QRect normalGeometry =
		ConfigManager::GetInstance().ReadValue("LastNormalWindowGeometry").toRect();
	if ((normalGeometry.width() > 400) && (normalGeometry.height() > 400)) {
		setGeometry(normalGeometry);
	} else {
		QScreen* screen = QGuiApplication::screenAt(QCursor::pos());
		QRect    screenRect = screen->geometry();
		QSize    screenSize = screenRect.size();
		QSize    windowSize = screenSize / 2;
		resize(windowSize);
		move(screenRect.center() - rect().center());
	}

	setWindowTitle(u8"RepoReporter");
	setWindowIcon(QIcon(":/image/logo.png"));

	m_KeyShowWidget =
		new QHotkey(Qt::Key_P, Qt::ShiftModifier | Qt::ControlModifier, true, this);
	connect(m_KeyShowWidget, &QHotkey::activated,
	        this, [&](){
		if (isVisible()) {
			if (isMaximized() || isMinimized()) {
				showNormal();
			} else {
				close();
			}
		} else {
			showNormal();
		}
	});
}

void MainWindow::InitConnect()
{
	connect(ui->projectTreeView, &ProjectTreeView::SgnSelectPathChange,
	        this, &MainWindow::ChangeRepo);

	connect(ui->logTableView,    &LogTableView::SgnChangeSelectLog,
	        ui->fileTableView, &FileTableView::ChangeLog);

	connect(ui->logTableView,    &LogTableView::SgnUpdateDescription,
	        this, [&](const QString& str){
		ui->editMessage->setText(str);
	});

	connect(ui->logTableView,  &LogTableView::SgnStateLabChange,
	        this, &MainWindow::UpdateStateLab);

	connect(ui->fileTableView, &FileTableView::SgnStateLabChange,
	        this, &MainWindow::UpdateStateLab);

	connect(ui->searchEdit,    &SearchLineEdit::SgnFilterChanged,
	        this, &MainWindow::LogTableTextFilterChanged);

	connect(ui->searchEdit,    &SearchLineEdit::textChanged,
	        this, &MainWindow::LogTableTextFilterChanged);

	connect(ui->RefreshBtn,    &QPushButton::clicked,
	        this, &MainWindow::RefreshRepoLog);

	connect(ui->AllbranchCbox, &QCheckBox::clicked,
	        this, &MainWindow::RefreshRepoLog);
}

void MainWindow::ChangeRepo(const QString& path)
{
	m_CurPaht = path;

	ui->AllbranchCbox->setChecked(false);
	ui->searchEdit->clear();

	QString version;
	QHash<QString, QMap<QString, VCFileEntry> > repoFileDictionary;
	QList<VCLogEntry> logs =
		VersionControlManager::FetchLog(path, version, repoFileDictionary, false);

	ui->fileTableView->ChangeRepo(path, repoFileDictionary);
	ui->logTableView->ChangeRepo(logs, version);

	ui->branchBtn->setText(VersionControlManager::GetCurrentBranch(path));
	ui->branchBtn->setVisible(RepoType::Git == VersionControlManager::CurrentRepoType);
	ui->AllbranchCbox->setVisible(RepoType::Git == VersionControlManager::CurrentRepoType);
}

void MainWindow::UpdateStateLab(const int& index, const int& num)
{
	static int num1 = 0, num2 = 0, num3 = 0;
	switch (index) {
	case 0: {
		num1 = num;
		break;
	}

	case 1: {
		num2 = num;
		break;
	}

	case 2: {
		num3 = num;
		break;
	}

	default: {
		num1 = num;
		num2 = num;
		num3 = num;
		break;
	}
	}

	ui->labState->setText(QString(u8"正在显示%1个版本，已选择%2个版本，已选择%3个文件。")
	                      .arg(num1).arg(num2).arg(num3));
}

void MainWindow::LogTableTextFilterChanged()
{
	QRegExp regExp;
	regExp = QRegExp(ui->searchEdit->text(),
	                 ui->searchEdit->GetCaseSensitivity(),
	                 ui->searchEdit->GetPatternSyntax());
	ui->logTableView->setFilterRegExp(regExp, ui->searchEdit->GetFilterList());
}

void MainWindow::RefreshRepoLog()
{
	bool allBracch = ui->AllbranchCbox->isChecked();
	ui->searchEdit->clear();
	ui->logTableView->Clear();

	QString version;
	QHash<QString, QMap<QString, VCFileEntry> > repoFileDictionary;
	QList<VCLogEntry> logs =
		VersionControlManager::FetchLog(m_CurPaht, version, repoFileDictionary, allBracch);

	ui->fileTableView->ChangeRepo(m_CurPaht, repoFileDictionary);
	ui->logTableView->ChangeRepo(logs, version);

	if (allBracch) {
		ui->branchBtn->setText(u8"所有分支");
	} else {
		ui->branchBtn->setText(VersionControlManager::GetCurrentBranch(m_CurPaht));
	}
	ui->branchBtn->setVisible(RepoType::Git == VersionControlManager::CurrentRepoType);
	ui->AllbranchCbox->setVisible(RepoType::Git == VersionControlManager::CurrentRepoType);
}
