#ifndef AddProjectDialog_H
#define AddProjectDialog_H

#include <QDialog>

#include "data/VCSDataStructures.h"

namespace Ui
{
	class AddProjectDialog;
}

class AddProjectDialog : public QDialog {
	Q_OBJECT

public:

	explicit AddProjectDialog(QWidget* parent = nullptr);
	~AddProjectDialog();

	VCProjectPath GetProjectPathFromInput()const;
	bool ProjectPathFromInputIsEmpty()const;

private slots:

	void SlotAcceptButtonClicked();
	void SlotRejectButtonClicked();

private:

	Ui::AddProjectDialog* m_UI;
};

#endif // ifndef AddProjectDialog_H
