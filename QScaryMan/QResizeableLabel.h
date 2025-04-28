#pragma once
#include <QtWidgets>

class QResizeableLabel : public QLabel
{
	Q_OBJECT

public:
	QResizeableLabel(QWidget* pwgt = Q_NULLPTR);
	QResizeableLabel(const QPixmap& pix, QWidget* pwgt = Q_NULLPTR);
	void savePixmap(const QPixmap& pix);
	bool isPixmapPresent();

private:
	QPixmap pix;

protected:
	void resizeEvent(QResizeEvent* e);
};

