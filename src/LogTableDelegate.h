#ifndef LOGTABLEDELEGATE_H
#define LOGTABLEDELEGATE_H

#include <QStyledItemDelegate>

class LogTableDelegate : public QStyledItemDelegate {
public:

	LogTableDelegate(QObject* parent = nullptr);

	void paint(
		QPainter* painter,
		const QStyleOptionViewItem& option,
		const QModelIndex& index) const override;

	void SetCurrentVersionRow(int row);

private:

	int m_CurVerRow = -1;
};

#endif // LOGTABLEDELEGATE_H
