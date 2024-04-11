#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QWidget(parent)
	, m_UI(new Ui::MainWindow)
{
	m_UI->setupUi(this);
	InitUi();
	InitConnect();
}

MainWindow::~MainWindow()
{
	delete m_UI;
}

void MainWindow::InitUi() {}

void MainWindow::InitConnect() {}
