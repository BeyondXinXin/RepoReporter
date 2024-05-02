#include "MessageTextEdit.h"


MessageTextEdit::MessageTextEdit(QWidget* parent) : QTextEdit(parent)
{
	InitUI();
	InitConnect();
}

void MessageTextEdit::contextMenuEvent(QContextMenuEvent* event)
{
	QMenu menu(this);

	menu.addAction(m_CopyAction);
	menu.addAction(m_CopyAllContentAction);
	menu.addAction(m_EditCommentAction);
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
	fun(m_EditCommentAction,    "", u8"编辑注释",   &MessageTextEdit::OnEditCommentAction);
}

void MessageTextEdit::OnCopyAction()
{
}

void MessageTextEdit::OnCopyAllContentAction()
{
}

void MessageTextEdit::OnEditCommentAction()
{
}
