#ifndef SearchLineEdit_H
#define SearchLineEdit_H

#include <QLineEdit>

class QAction;
class QActionGroup;

Q_DECLARE_METATYPE(QRegExp::PatternSyntax)

class SearchLineEdit : public QLineEdit {
	Q_OBJECT

public:

	explicit SearchLineEdit(QWidget* parent = nullptr);
	~SearchLineEdit();

	Qt::CaseSensitivity GetCaseSensitivity() const;
	void SetCaseSensitivity(Qt::CaseSensitivity);
	QRegExp::PatternSyntax GetPatternSyntax() const;
	void SetPatternSyntax(QRegExp::PatternSyntax);

	QList<int>GetFilterList()const;

protected:

	void showEvent(QShowEvent* event) override;
	void hideEvent(QHideEvent* event) override;

signals:

	void SgnFilterChanged();

private:

	void FilterChange();
	void UpdatePlaceholderText();

private:

	QAction* m_CaseSensitivityAction;
	QActionGroup* m_PatternGroup;
	QActionGroup* m_FilterGroup;
};

#endif // ifndef SearchLineEdit_H
