#include "MessageTextEdit.h"

#include <QClipboard>
#include <QApplication>

MessageTextEdit::MessageTextEdit(QWidget* parent) : QTextEdit(parent)
{
	InitUI();
	InitConnect();
}

void MessageTextEdit::contextMenuEvent(QContextMenuEvent* event)
{
	m_CopyAction->setEnabled(!textCursor().selectedText().isEmpty());

	QMenu menu(this);
	menu.addAction(m_CopyAction);
	menu.addAction(m_CopyAllContentAction);
	menu.exec(event->globalPos() - QPoint(20, 10));
}

void MessageTextEdit::InitUI()
{
}

void MessageTextEdit::InitConnect()
{
	auto fun = [&](QAction *& action, const QString& iconPath, const QString& name,
	               void (MessageTextEdit::*SlogAction)())
		   {
			   action = new QAction(QIcon(iconPath), name, this);
			   connect(action, &QAction::triggered, this, SlogAction);
		   };

	fun(m_CopyAction,           "", u8"复制",     &MessageTextEdit::OnCopyAction);
	fun(m_CopyAllContentAction, "", u8"复制所有内容", &MessageTextEdit::OnCopyAllContentAction);
}

void MessageTextEdit::OnCopyAction()
{
	QString selectedText = textCursor().selectedText();
	if (!selectedText.isEmpty()) {
		QChar c = 0x2029;
		selectedText.replace(c, '\n');
		QClipboard* clipboard = QApplication::clipboard();
		clipboard->setText(selectedText);
	}
}

void MessageTextEdit::OnCopyAllContentAction()
{
	QClipboard* clipboard = QApplication::clipboard();
	clipboard->setText(toPlainText());
}
