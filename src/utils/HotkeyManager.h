#ifndef HOTKEYMANAGER_H
#define HOTKEYMANAGER_H

#include <QHotkey>

class HotkeyManager : public QObject {
	Q_OBJECT

public:

	static HotkeyManager* Instance();

	void Binding();
	void SaveKey();

	void SetShowMainWindowRegistered(const bool& registered);
	void SetShowMainWindowShortcut(const QKeySequence& sequence);
	bool GetShowMainWindowRegistered()const;
	QKeySequence GetShowMainWindowShortcut()const;

Q_SIGNALS:

	void SgnShowMainWindow();

private:

	HotkeyManager(QObject* parent = nullptr);

private:

	static HotkeyManager* m_Self;

	QHotkey* m_KeyShowMainWindow;
};

#endif // HOTKEYMANAGER_H
