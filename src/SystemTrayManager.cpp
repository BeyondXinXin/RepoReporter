#include "SystemTrayManager.h"

#include <QStyle>
#include <QMutex>
#include <QApplication>
#include <QStyle>
#include <QMenu>

SystemTrayManager * SystemTrayManager::m_Self = nullptr;
SystemTrayManager * SystemTrayManager::Instance()
{
	if (m_Self == nullptr) {
		static QMutex mutex;
		QMutexLocker  locker(&mutex);
		m_Self = new SystemTrayManager();
	}

	return m_Self;
}

SystemTrayManager::SystemTrayManager(QObject* parent) : QSystemTrayIcon(parent)
{
	m_MainWidget = 0;
	m_Mmenu = new QMenu;

	connect(this, &SystemTrayManager::activated,
	        this, &SystemTrayManager::SlotIconIsActived);

	setIcon(QIcon(":/image/logo.png"));
}

void SystemTrayManager::SlotIconIsActived(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) {
	case QSystemTrayIcon::Trigger:
	case QSystemTrayIcon::DoubleClick: {
		OnShowMainWidget();
		break;
	}

	default:
		break;
	}
}

void SystemTrayManager::setMainWidget(QWidget* mainWidget)
{
	m_MainWidget = mainWidget;
	m_Mmenu->addAction(u8"主界面", this, &SystemTrayManager::OnShowMainWidget);
	m_Mmenu->addAction(u8"退出",  this, &SystemTrayManager::OnCloseAll);
	setContextMenu(m_Mmenu);
	setVisible(true);
}

void SystemTrayManager::OnCloseAll()
{
	hide();
	deleteLater();
	qApp->exit();
}

void SystemTrayManager::OnShowMainWidget()
{
	if (m_MainWidget) {
		m_MainWidget->showNormal();
		m_MainWidget->activateWindow();
	}
}
