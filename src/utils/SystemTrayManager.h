#ifndef SystemTrayManager_H
#define SystemTrayManager_H


#include <QObject>
#include <QSystemTrayIcon>

class QMenu;

class SystemTrayManager : public QSystemTrayIcon {
	Q_OBJECT

public:

	static SystemTrayManager* Instance();

public:

	void setMainWidget(QWidget* mainWidget);

public Q_SLOTS:

	void OnCloseAll();
	void OnShowMainWidget();
	void OnShowSettings();

Q_SIGNALS:

	void trayIconExit();

private slots:

	void SlotIconIsActived(QSystemTrayIcon::ActivationReason reason);

private:

	SystemTrayManager(QObject* parent = nullptr);

private:

	static SystemTrayManager* m_Self;

	QWidget* m_MainWidget { nullptr };
	QWidget* m_SettingsWid{ nullptr };

	QMenu* m_Mmenu;
};

#endif // TRAYICON_H
