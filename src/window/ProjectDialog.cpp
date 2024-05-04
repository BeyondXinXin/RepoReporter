#include "ProjectDialog.h"
#include "ui_ProjectDialog.h"

#include "utils/FileUtil.h"

ProjectDialog::ProjectDialog(const Type& type, QWidget* parent) :
	QDialog(parent),
	m_UI(new Ui::ProjectDialog),
	m_Type(type)
{
	m_UI->setupUi(this);

	connect(m_UI->btnAccept, &QPushButton::clicked,
	        this, &ProjectDialog::SlotAcceptButtonClicked);
	connect(m_UI->btnReject, &QPushButton::clicked,
	        this, &ProjectDialog::SlotRejectButtonClicked);

	connect(m_UI->editPath,  &QLineEdit::textChanged,
	        this, &ProjectDialog::UpdateRepoType);

	if (m_Type == Add) {
		setWindowTitle(u8"添加项目");
	} else {
		setWindowTitle(u8"编辑项目");
	}

	setWindowIcon(QIcon(":/image/logo.png"));
	setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
}

ProjectDialog::~ProjectDialog()
{
	delete m_UI;
}

void ProjectDialog::SetProjectData(const VCRepoEntry & data)
{
	m_UI->editName->setText(data.name);
	m_UI->editPath->setText(data.path);
	m_UI->SvnRBtn->setEnabled(false);
	m_UI->GitRBtn->setEnabled(false);
	if (RepoType::Unknown == data.type) {
		m_UI->GitRBtn->setChecked(false);
		m_UI->SvnRBtn->setChecked(false);
	} else if (RepoType::Git == data.type) {
		m_UI->GitRBtn->setChecked(true);
		m_UI->SvnRBtn->setChecked(false);
	} else if (RepoType::Svn == data.type) {
		m_UI->GitRBtn->setChecked(false);
		m_UI->SvnRBtn->setChecked(true);
	}
}

VCRepoEntry  ProjectDialog::GetProjectPathFromInput() const
{
	VCRepoEntry  data(m_UI->editName->text(), m_UI->editPath->text());
	if (m_UI->GitRBtn->isChecked()) {
		data.type = RepoType::Git;
	} else if (m_UI->SvnRBtn->isChecked()) {
		data.type = RepoType::Svn;
	}
	return data;
}

bool ProjectDialog::ProjectPathFromInputIsEmpty() const
{
	return m_UI->editPath->text().isEmpty();
}

void ProjectDialog::SlotAcceptButtonClicked()
{
	if (m_UI->editName->text().isEmpty()) {
		reject();
		return;
	}
	accept();
}

void ProjectDialog::SlotRejectButtonClicked()
{
	reject();
}

void ProjectDialog::UpdateRepoType(const QString&)
{
	QString dir = m_UI->editPath->text();
	bool    gitRepo = FileUtil::FileExists(dir + "/.git");
	bool    svnRepo = FileUtil::FileExists(dir + "/.svn");

	m_UI->GitRBtn->setEnabled(gitRepo);
	m_UI->SvnRBtn->setEnabled(svnRepo);
	if (gitRepo) {
		m_UI->GitRBtn->setChecked(true);
		m_UI->SvnRBtn->setChecked(false);
	} else if (svnRepo) {
		m_UI->GitRBtn->setChecked(false);
		m_UI->SvnRBtn->setChecked(true);
	} else {
		m_UI->GitRBtn->setChecked(false);
		m_UI->SvnRBtn->setChecked(false);
	}
}
