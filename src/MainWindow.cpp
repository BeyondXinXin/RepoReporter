#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDebug>
#include <QFile>
#include <QStandardItem>

#include "ConfigManager.h"


MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

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

MainWindow::~MainWindow()
{
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
