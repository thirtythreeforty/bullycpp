#ifndef COLLAPSINGQTABWIDGET
#define COLLAPSINGQTABWIDGET

#include <QTabWidget>

class CollapsingQTabWidget : public QTabWidget {
	Q_OBJECT
public:
	explicit CollapsingQTabWidget(QWidget* parent = 0)
		: QTabWidget(parent)
	{
		connect(this, &QTabWidget::currentChanged, this, &CollapsingQTabWidget::changeSizePolicies);
	}
	virtual ~CollapsingQTabWidget() = default;

	void changeSizePolicies(int nowCurrent) {
		for(int i = 0; i < count(); ++i) {
			QWidget* w = widget(i);
			if(i == nowCurrent)
				w->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
			else
				w->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
		}
	}
};

#endif // COLLAPSINGQTABWIDGET
