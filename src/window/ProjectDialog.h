#ifndef ProjectDialog_H
#define ProjectDialog_H

#include <QDialog>

#include "data/VCSDataStructures.h"

namespace Ui
{
	class ProjectDialog;
}

class ProjectDialog : public QDialog {
	Q_OBJECT

public:

	enum Type { Add, Edit };

public:

	explicit ProjectDialog(const Type& type, QWidget* parent = nullptr);
	~ProjectDialog();

	void SetProjectData(const VCProjectPath& data);

	VCProjectPath GetProjectPathFromInput()const;
	bool ProjectPathFromInputIsEmpty()const;

private slots:

	void SlotAcceptButtonClicked();
	void SlotRejectButtonClicked();

	void UpdateRepoType(const QString&);

private:

	Ui::ProjectDialog* m_UI;
	Type m_Type;
};

#endif // ifndef AddProjectDialog_H
