#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFile>
#include <QStandardItem>


MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->splitter->setSizes({ 250, 1000 });

	move(0, 0);
	
	connect(ui->projectTreeView,&ProjectTreeView::SgnSelectPathChange,
			ui->logTableView,&LogTableView::ChangeProPath);
}

MainWindow::~MainWindow()
{
	delete ui;
}
