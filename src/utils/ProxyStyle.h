#ifndef QProxyStyle_H
#define QProxyStyle_H

#include <QProxyStyle>


class NoIconStyle : public QProxyStyle {
public:

	QSize sizeFromContents(
		ContentsType type, const QStyleOption* option,
		const QSize& size, const QWidget* widget) const override;

	void drawControl(
		ControlElement element, const QStyleOption* option,
		QPainter* painter, const QWidget* widget) const override;
};


#endif // PROXYSTYLE_H
