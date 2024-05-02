#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDebug>
#include <QFile>
#include <QStandardItem>
#include <QScreen>

#include "ConfigManager.h"
#include "SystemTrayManager.h"


MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	InitUI();
	InitConnect();

	SystemTrayManager::Instance()->setMainWidget(this);
	qInfo() << u8"软件启动。";
}

MainWindow::~MainWindow()
{
	qInfo() << u8"软件关闭。";
	ConfigManager::GetInstance().WriteValue("LastWindowSize", size());
	ConfigManager::GetInstance().WriteValue("LastWindowPosition", pos());
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
	QSize lastSize = ConfigManager::GetInstance()
	                 .ReadValue("LastWindowSize").toSize();
	if (!lastSize.isValid()) {
		QScreen* screen = QGuiApplication::screenAt(QCursor::pos());
		QRect    screenRect = screen->geometry();
		QSize    screenSize = screenRect.size();
		QSize    windowSize = screenSize / 2;
		lastSize = windowSize;
	}
	resize(lastSize);

	QPoint lastPosition = ConfigManager::GetInstance()
	                      .ReadValue("LastWindowPosition").toPoint();
	if (lastPosition.isNull()) {
		QScreen* screen = QGuiApplication::screenAt(QCursor::pos());
		QRect    screenRect = screen->geometry();
		lastPosition = screenRect.center() - rect().center();
	}
	move(lastPosition);
}

void MainWindow::InitConnect()
{
	connect(ui->projectTreeView, &ProjectTreeView::SgnSelectPathChange,
	        ui->logTableView, &LogTableView::ChangeProPath);

	connect(ui->projectTreeView, &ProjectTreeView::SgnSelectPathChange,
	        ui->fileTableView, &FileTableView::ChangeProPath);

	connect(ui->logTableView,    &LogTableView::SgnChangeSelectLog,
	        ui->fileTableView, &FileTableView::ChangeLog);

	connect(ui->logTableView,    &LogTableView::SgnUpdateDescription,
	        this, [&](const QString& str){
		ui->editMessage->setText(str);
	});
}
