#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFile>
#include <QStandardItem>


MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->splitter->setSizes({ 300, 1300 });

	resize(1600, 1000);
	move(0, 0);

	connect(ui->projectTreeView, &ProjectTreeView::SgnSelectPathChange,
	        ui->logTableView, &LogTableView::ChangeProPath);

	connect(ui->logTableView,    &LogTableView::SgnChangeSelectLog,
	        ui->fileTableView, &FileTableView::ChangeLog);
}

MainWindow::~MainWindow()
{
	delete ui;
}
