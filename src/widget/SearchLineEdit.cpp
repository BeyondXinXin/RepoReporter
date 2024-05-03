#include "SearchLineEdit.h"

#include <QMenu>
#include <QActionGroup>
#include <QToolButton>
#include <QWidgetAction>
#include <QStyleOptionFrameV3>
#include <QPainter>

#include "utils/ConfigManager.h"

SearchLineEdit::SearchLineEdit(QWidget* parent)
	: QLineEdit(parent)
	, m_PatternGroup(new QActionGroup(this))
	, m_FilterGroup(new QActionGroup(this))
{
	setClearButtonEnabled(true);
	connect(this, &QLineEdit::textChanged,
	        this, &SearchLineEdit::SgnFilterChanged);

	QMenu  * menu = new QMenu(this);
	QAction* action;

	// add 过滤
	m_FilterGroup->setExclusive(false);
	auto funAddFilterAction =
		[&](QString name, int data){
			action = menu->addAction(name);
			action->setData(data);
			action->setCheckable(true);
			action->setChecked(true);
			m_FilterGroup->addAction(action);
		};
	funAddFilterAction(u8"版本", 0);
	funAddFilterAction(u8"作者", 2);
	funAddFilterAction(u8"日期", 3);
	funAddFilterAction(u8"信息", 4);
	connect(m_FilterGroup, &QActionGroup::triggered,
	        this, &SearchLineEdit::FilterChange);

	// add 大小写
	menu->addSeparator();
	m_CaseSensitivityAction = menu->addAction(u8"区分大小写 ");
	m_CaseSensitivityAction->setCheckable(true);
	m_CaseSensitivityAction->setChecked(true);
	connect(m_CaseSensitivityAction, &QAction::toggled,
	        this, &SearchLineEdit::SgnFilterChanged);

	// add 正则
	menu->addSeparator();
	m_PatternGroup->setExclusive(true);
	auto funAddPatternAction =
		[&](QString name, int data){
			action = menu->addAction(name);
			action->setData(data);
			action->setCheckable(true);
			action->setChecked(false);
			m_PatternGroup->addAction(action);
		};
	funAddPatternAction(u8"正常字符",  int(QRegExp::FixedString));
	funAddPatternAction(u8"正则表达式", int(QRegExp::RegExp2));
	funAddPatternAction(u8"通配符",   int(QRegExp::Wildcard));
	SetPatternSyntax(QRegExp::FixedString);
	connect(m_PatternGroup, &QActionGroup::triggered,
	        this, &SearchLineEdit::SgnFilterChanged);

	QToolButton* optionsButton = new QToolButton(this);
	optionsButton->setCursor(Qt::ArrowCursor);
	optionsButton->setFocusPolicy(Qt::NoFocus);
	optionsButton->setMenu(menu);
	optionsButton->setPopupMode(QToolButton::InstantPopup);
	optionsButton->setFixedSize(36, 36);

	optionsButton->setStyleSheet(
		"QToolButton { "
		"background-color: transparent;; border: none; "
		"}"
		"QToolButton::menu-indicator { width: 0; }");

	QWidgetAction* optionsAction = new QWidgetAction(this);
	optionsAction->setDefaultWidget(optionsButton);
	addAction(optionsAction, QLineEdit::LeadingPosition);

	setStyleSheet(
		"QLineEdit { "
		"background-image: url(:image/search.png);"
		"background-repeat: no-repeat;"
		"background-position: left;"
		"}");
}

SearchLineEdit::~SearchLineEdit()
{}

Qt::CaseSensitivity SearchLineEdit::GetCaseSensitivity() const
{
	return m_CaseSensitivityAction->isChecked() ?
	       Qt::CaseSensitive : Qt::CaseInsensitive;
}

void SearchLineEdit::SetCaseSensitivity(Qt::CaseSensitivity cs)
{
	m_CaseSensitivityAction->setChecked(cs == Qt::CaseSensitive);
}

static inline QRegExp::PatternSyntax patternSyntaxFromAction(const QAction* a)
{
	return static_cast<QRegExp::PatternSyntax>(a->data().toInt());
}

QRegExp::PatternSyntax SearchLineEdit::GetPatternSyntax() const
{
	return patternSyntaxFromAction(m_PatternGroup->checkedAction());
}

void SearchLineEdit::SetPatternSyntax(QRegExp::PatternSyntax s)
{
	foreach(QAction * a, m_PatternGroup->actions())
	{
		if (patternSyntaxFromAction(a) == s) {
			a->setChecked(true);
			break;
		}
	}
}

QList<int>SearchLineEdit::GetFilterList() const
{
	QList<int> filters;
	foreach(QAction * a, m_FilterGroup->actions())
	{
		if (a->isChecked()) {
			filters << a->data().toInt();
		}
	}
	return filters;
}

void SearchLineEdit::showEvent(QShowEvent* event)
{
	QLineEdit::showEvent(event);
	QList<bool> states =
		ConfigManager::GetInstance()
		.ReadList<bool>("SearchLineEdit", QList<bool>{ 1, 1, 1, 1, 0, 1, 0, 0 });
	if ((states.size() != 8) ||
	    (m_FilterGroup->actions().size() != 4) ||
	    (m_PatternGroup->actions().size() != 3)) {
		return;
	}
	m_FilterGroup->actions().at(0)->setChecked(states.at(0));
	m_FilterGroup->actions().at(1)->setChecked(states.at(1));
	m_FilterGroup->actions().at(2)->setChecked(states.at(2));
	m_FilterGroup->actions().at(3)->setChecked(states.at(3));
	m_CaseSensitivityAction->setChecked(states.at(4));
	m_PatternGroup->actions().at(0)->setChecked(states.at(5));
	m_PatternGroup->actions().at(1)->setChecked(states.at(6));
	m_PatternGroup->actions().at(2)->setChecked(states.at(7));

	UpdatePlaceholderText();
}

void SearchLineEdit::hideEvent(QHideEvent* event)
{
	QLineEdit::hideEvent(event);
	QList<bool> states;

	foreach(QAction * a, m_FilterGroup->actions())
	{
		states << a->isChecked();
	}
	states << m_CaseSensitivityAction->isChecked();
	foreach(QAction * a, m_PatternGroup->actions())
	{
		states << a->isChecked();
	}

	ConfigManager::GetInstance().WriteList<bool>("SearchLineEdit", states);
}

void SearchLineEdit::FilterChange()
{
	UpdatePlaceholderText();
	emit SearchLineEdit::SgnFilterChanged();
}

void SearchLineEdit::UpdatePlaceholderText()
{
	QStringList filters;
	foreach(QAction * a, m_FilterGroup->actions())
	{
		if (a->isChecked()) {
			filters << a->text();
		}
	}
	setPlaceholderText(filters.join(","));
}
