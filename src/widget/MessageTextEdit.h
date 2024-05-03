#ifndef MESSAGETEXTEDIT_H
#define MESSAGETEXTEDIT_H


#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QTextEdit>

class MessageTextEdit : public QTextEdit {
public:

	MessageTextEdit(QWidget* parent = nullptr);

protected:

	void contextMenuEvent(QContextMenuEvent* event) override;

private:

	void InitUI();
	void InitConnect();

private slots:

	void OnCopyAction();
	void OnCopyAllContentAction();

private:

	QAction* m_CopyAction;
	QAction* m_CopyAllContentAction;
};

#endif // MESSAGETEXTEDIT_H
