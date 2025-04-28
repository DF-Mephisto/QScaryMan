#include "QResizeableLabel.h"

QResizeableLabel::QResizeableLabel(QWidget* pwgt) :
	QLabel(pwgt)
{

}

QResizeableLabel::QResizeableLabel(const QPixmap& pix, QWidget* pwgt) :
	QLabel(pwgt), pix(pix)
{
	setPixmap(this->pix.scaled(size()));
}

void QResizeableLabel::savePixmap(const QPixmap& pix)
{
	this->pix = pix;
	setPixmap(this->pix.scaled(size()));
}

void QResizeableLabel::resizeEvent(QResizeEvent* e)
{
	if (!pix.isNull())
	{
		setPixmap(pix.scaled(size()));
	}
}

bool QResizeableLabel::isPixmapPresent()
{
	return !pix.isNull();
}