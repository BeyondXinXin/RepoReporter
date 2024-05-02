#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
	class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
	Q_OBJECT

public:

	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

protected:

	void showEvent(QShowEvent* event) override;
	void hideEvent(QHideEvent* event) override;

private:

	void InitUI();
	void InitConnect();

	void UpdateStateLab(const int& index, const int& num);

private:

	Ui::MainWindow* ui;
};
#endif // MAINWINDOW_H
