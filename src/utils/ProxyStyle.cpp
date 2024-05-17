#include "ProxyStyle.h"

#include <QStyleOptionMenuItem>

QSize NoIconStyle::sizeFromContents(
	ContentsType type, const QStyleOption* option,
	const QSize& size, const QWidget* widget) const
{
	QSize newSize = QProxyStyle::sizeFromContents(type, option, size, widget);
	if (type == CT_MenuItem) {
		const QStyleOptionMenuItem* menuItemOption = qstyleoption_cast<const QStyleOptionMenuItem *>(option);
		if (menuItemOption && (menuItemOption->menuItemType == QStyleOptionMenuItem::Normal)) {
			int iconSize = 24;
			newSize.setWidth(newSize.width() - iconSize);
		}
	}
	return newSize;
}

void NoIconStyle::drawControl(
	ControlElement element, const QStyleOption* option,
	QPainter* painter, const QWidget* widget) const
{
	if (element == CE_MenuItem) {
		QStyleOptionMenuItem myOption(*qstyleoption_cast<const QStyleOptionMenuItem *>(option));
		myOption.icon = QIcon();
		int iconSize = 24;
		myOption.rect.adjust(-iconSize, 0, 0, 0);

		QProxyStyle::drawControl(element, &myOption, painter, widget);
	} else {
		QProxyStyle::drawControl(element, option, painter, widget);
	}
}
