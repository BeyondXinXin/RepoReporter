#include "AddProjectDialog.h"
#include "ui_AddProjectDialog.h"

AddProjectDialog::AddProjectDialog(QWidget* parent) :
	QDialog(parent),
	m_UI(new Ui::AddProjectDialog)
{
	m_UI->setupUi(this);

	connect(m_UI->btnAccept, &QPushButton::clicked, this, &AddProjectDialog::SlotAcceptButtonClicked);
	connect(m_UI->btnReject, &QPushButton::clicked, this, &AddProjectDialog::SlotRejectButtonClicked);
}

AddProjectDialog::~AddProjectDialog()
{
	delete m_UI;
}

VCProjectPath AddProjectDialog::GetProjectPathFromInput() const
{
	return VCProjectPath(m_UI->editName->text(), m_UI->editPath->text());
}

bool AddProjectDialog::ProjectPathFromInputIsEmpty() const
{
	return m_UI->editPath->text().isEmpty();
}

void AddProjectDialog::SlotAcceptButtonClicked()
{
	if (m_UI->editName->text().isEmpty()) {
		reject();
		return;
	}
	accept();
}

void AddProjectDialog::SlotRejectButtonClicked()
{
	reject();
}
