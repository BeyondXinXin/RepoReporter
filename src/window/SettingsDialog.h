#ifndef SettingsDialog_H
#define SettingsDialog_H

#include <QDialog>

namespace Ui
{
	class SettingsDialog;
}

class SettingsDialog : public QDialog {
	Q_OBJECT

public:

	explicit SettingsDialog(QWidget* parent = nullptr);
	~SettingsDialog();

protected:

	void showEvent(QShowEvent* event) override;
	void hideEvent(QHideEvent* event) override;

private:

	void SlotCheckValidity();
	void SlotAcceptButtonClicked();
	void SlotRejectButtonClicked();
	void SlotSelectFile();

	void SetTextColor(QWidget* wid, const bool& pass);

private:

	Ui::SettingsDialog* ui;
};

#endif // SETTINGSWIDGET_H
