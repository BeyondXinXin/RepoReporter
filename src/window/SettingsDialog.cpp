#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"

#include <QCoreApplication>

#include "utils/VersionControlManager.h"
#include "utils/FileUtil.h"
#include "utils/ConfigManager.h"
#include "utils/HotkeyManager.h"
#include "utils/AutoRunManager.h"

SettingsDialog::SettingsDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);

	connect(ui->TortoiseGitEdit, &QLineEdit::textChanged, this, &SettingsDialog::SlotCheckValidity);
	connect(ui->TortoiseSvnEdit, &QLineEdit::textChanged, this, &SettingsDialog::SlotCheckValidity);
	connect(ui->GitEdit,         &QLineEdit::textChanged, this, &SettingsDialog::SlotCheckValidity);
	connect(ui->SvnEdit,         &QLineEdit::textChanged, this, &SettingsDialog::SlotCheckValidity);
	connect(ui->TortoiseGitBtn,  &QPushButton::clicked,   this, &SettingsDialog::SlotSelectFile);
	connect(ui->TortoiseSvnBtn,  &QPushButton::clicked,   this, &SettingsDialog::SlotSelectFile);
	connect(ui->GitBtn,          &QPushButton::clicked,   this, &SettingsDialog::SlotSelectFile);
	connect(ui->SvnBtn,          &QPushButton::clicked,   this, &SettingsDialog::SlotSelectFile);
	connect(ui->AcceptBtn,       &QPushButton::clicked,   this, &SettingsDialog::SlotAcceptButtonClicked);
	connect(ui->RejectBtn,       &QPushButton::clicked,   this, &SettingsDialog::SlotRejectButtonClicked);

	setWindowTitle(u8"设置");
	setWindowIcon(QIcon(":/image/logo.png"));
	setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::showEvent(QShowEvent* event)
{
	ui->cbox1->setChecked(QCoreApplication::testAttribute(Qt::AA_DisableHighDpiScaling));
	ui->cbox2->setChecked(QCoreApplication::testAttribute(Qt::AA_UseHighDpiPixmaps));
	ui->cbox3->setChecked(QCoreApplication::testAttribute(Qt::AA_EnableHighDpiScaling));
	ui->TortoiseGitEdit->setText(VersionControlManager::TortoiseGitPath);
	ui->TortoiseSvnEdit->setText(VersionControlManager::TortoiseSvnPath);
	ui->GitEdit->setText(VersionControlManager::GitPath);
	ui->SvnEdit->setText(VersionControlManager::SvnPath);

	ui->hotkeyCheckbox_1->setChecked(HotkeyManager::Instance()->GetShowMainWindowRegistered());
	ui->hotkeySequenceEdit_1->setKeySequence(HotkeyManager::Instance()->GetShowMainWindowShortcut());

	ui->cboxAutoRun->setChecked(ConfigManager::GetInstance().ReadValue("AutoRun", true).toBool());

	SlotCheckValidity();
	QDialog::showEvent(event);

	HotkeyManager::Instance()->SetShowMainWindowRegistered(false);
}

void SettingsDialog::hideEvent(QHideEvent* event)
{
	QDialog::hideEvent(event);
}

void SettingsDialog::SlotCheckValidity()
{
	const bool tgitPass = FileUtil::FileExists(ui->TortoiseGitEdit->text());
	const bool tsvnPass = FileUtil::FileExists(ui->TortoiseSvnEdit->text());
	const bool gitPass = FileUtil::FileExists(ui->GitEdit->text());
	const bool svnPass = FileUtil::FileExists(ui->SvnEdit->text());
	SetTextColor(ui->TortoiseGitLab,  tgitPass);
	SetTextColor(ui->TortoiseSvnLab,  tsvnPass);
	SetTextColor(ui->GitLab,          gitPass);
	SetTextColor(ui->SvnLab,          svnPass);

	SetTextColor(ui->TortoiseGitEdit, tgitPass);
	SetTextColor(ui->TortoiseSvnEdit, tsvnPass);
	SetTextColor(ui->GitEdit,         gitPass);
	SetTextColor(ui->SvnEdit,         svnPass);
}

void SettingsDialog::SlotAcceptButtonClicked()
{
	VersionControlManager::TortoiseGitPath = ui->TortoiseGitEdit->text();
	VersionControlManager::TortoiseSvnPath = ui->TortoiseSvnEdit->text();
	VersionControlManager::GitPath = ui->GitEdit->text();
	VersionControlManager::SvnPath = ui->SvnEdit->text();
	ConfigManager::GetInstance().WriteValue("DisableHighDpiScaling", ui->cbox1->isChecked());
	ConfigManager::GetInstance().WriteValue("UseHighDpiPixmaps", ui->cbox2->isChecked());
	ConfigManager::GetInstance().WriteValue("EnableHighDpiScaling", ui->cbox3->isChecked());
	ConfigManager::GetInstance().WriteValue("AutoRun", ui->cboxAutoRun->isChecked());
	HotkeyManager::Instance()->SetShowMainWindowShortcut(ui->hotkeySequenceEdit_1->keySequence());
	HotkeyManager::Instance()->SetShowMainWindowRegistered(ui->hotkeyCheckbox_1->isChecked());
	HotkeyManager::Instance()->SaveKey();


	if (ui->cboxAutoRun->isChecked()) {
		AutoRunManager::SetAutoRun(
			QCoreApplication::applicationName(),
			QCoreApplication::applicationFilePath());
	} else {
		AutoRunManager::RemoveAutoRun(QCoreApplication::applicationName());
	}

	accept();
}

void SettingsDialog::SlotRejectButtonClicked()
{
	reject();
}

void SettingsDialog::SlotSelectFile()
{
	QObject* obj = sender();
	QPushButton* btn = qobject_cast<QPushButton *>(obj);
	if (!btn) {
		return;
	}
	QLineEdit* edit = ui->groupBox->findChild<QLineEdit *>(btn->objectName().remove("Btn") + "Edit");
	if (!edit) {
		return;
	}
	QString file = FileUtil::SelectFile("*.exe", edit->text());
	if (file.isEmpty()) {
		return;
	}
	edit->setText(file);
}

void SettingsDialog::SetTextColor(QWidget* wid, const bool& pass)
{
	wid->setStyleSheet(pass ? "color: black;" : "color: red;");
}
