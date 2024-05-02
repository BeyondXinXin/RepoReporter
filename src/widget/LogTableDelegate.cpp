#include "LogTableDelegate.h"

#include <QPainter>
#include <QDebug>
#include <QPen>

LogTableDelegate::LogTableDelegate(QObject* parent) : QStyledItemDelegate(parent)
{}

void LogTableDelegate::paint(
	QPainter* painter,
	const QStyleOptionViewItem& option,
	const QModelIndex& index) const
{
	QStyleOptionViewItem newOption(option);
	if (index.row() == m_CurVerRow) {
		newOption.font.setBold(true);
	}
	QStyledItemDelegate::paint(painter, newOption, index);
}

void LogTableDelegate::SetCurrentVersionRow(int row)
{
	m_CurVerRow = row;
}
