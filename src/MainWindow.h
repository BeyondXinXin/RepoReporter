#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow() override;

private:
	void InitUi();
	void InitConnect();

private:
	Ui::MainWindow *m_UI;
};

#endif // MAINWINDOW_H
