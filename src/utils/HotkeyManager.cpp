#include "HotkeyManager.h"

#include <QMutex>

#include "utils/ConfigManager.h"

HotkeyManager * HotkeyManager::m_Self = nullptr;
HotkeyManager * HotkeyManager::Instance()
{
	if (m_Self == nullptr) {
		static QMutex mutex;
		QMutexLocker  locker(&mutex);
		m_Self = new HotkeyManager();
	}

	return m_Self;
}

void HotkeyManager::Binding()
{
	QKeySequence(Qt::Key_P, Qt::ShiftModifier | Qt::ControlModifier).toString();

	bool registered;
	QString shortcutStr;

	// ShowMainWindow
	registered = ConfigManager::GetInstance().ReadValue(
		"ShowMainWindowRegistered", true).toBool();
	shortcutStr = ConfigManager::GetInstance().ReadValue(
		"ShowMainWindowShortcut", QKeySequence("Ctrl+Shift+P")).toString();

	SetShowMainWindowShortcut(QKeySequence(shortcutStr));
	SetShowMainWindowRegistered(registered);
}

void HotkeyManager::SaveKey()
{
	ConfigManager::GetInstance().WriteValue(
		"ShowMainWindowRegistered", GetShowMainWindowRegistered());
	ConfigManager::GetInstance().WriteValue(
		"ShowMainWindowShortcut", GetShowMainWindowShortcut().toString());
}

void HotkeyManager::SetShowMainWindowRegistered(const bool& registered)
{
	m_KeyShowMainWindow->setRegistered(registered);
}

void HotkeyManager::SetShowMainWindowShortcut(const QKeySequence& sequence)
{
	m_KeyShowMainWindow->setShortcut(sequence);
}

bool HotkeyManager::GetShowMainWindowRegistered() const
{
	return m_KeyShowMainWindow->isRegistered();
}

QKeySequence HotkeyManager::GetShowMainWindowShortcut() const
{
	return m_KeyShowMainWindow->shortcut();
}

HotkeyManager::HotkeyManager(QObject* parent) : QObject(parent)
{
	m_KeyShowMainWindow = new QHotkey(this);
	connect(m_KeyShowMainWindow, &QHotkey::activated,
	        this, &HotkeyManager::SgnShowMainWindow);
}
