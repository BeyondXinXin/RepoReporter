#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFile>
#include <QStandardItem>


MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->splitter->setSizes({ 400, 1000 });

	connect(ui->projectTreeView, &ProjectTreeView::SgnSelectPathChange,
	        ui->logTableView, &LogTableView::ChangeProPath);

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
