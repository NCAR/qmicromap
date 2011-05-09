/*
 * QStationModelItem.cpp
 *
 *  Created on: May 5, 2011
 *      Author: martinc
 */

#include "QStationModelGraphicsItem.h"
#include <iostream>

/////////////////////////////////////////////////////////////////////////////////////////////////
QStationModelGraphicsItem::QStationModelGraphicsItem(double x, double y,
		double spdKnots, double dirMet,
		QGraphicsItem* parent):
QGraphicsItem(parent),
_x(x),
_y(y),
_spdKnots(spdKnots),
_dirMet(dirMet),
_textOffset(40),
_aspectRatio(1.0)
{
	_text[N ] = "";
	_text[NE] = "";
	_text[E ] = "";
	_text[SE] = "";
	_text[S ] = "";
	_text[SW] = "";
	_text[W ] = "";
	_text[NW] = "";

	setPos(_x, _y);

	// turn off transformations. The item will now draw with local
	// scale in terms of screen pixels.
	setFlag(QGraphicsItem::ItemIgnoresTransformations, true);

}

/////////////////////////////////////////////////////////////////////////////////////////////////
QStationModelGraphicsItem::~QStationModelGraphicsItem() {

}

/////////////////////////////////////////////////////////////////////////////////////////////////
QRectF QStationModelGraphicsItem::boundingRect() const
 {
     qreal penWidth = 1;
     return QRectF(-10 - penWidth / 2, -10 - penWidth / 2,
                   20 + penWidth, 20 + penWidth);
 }

/////////////////////////////////////////////////////////////////////////////////////////////////
void QStationModelGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
            QWidget *widget)
 {
     modelText(painter, _text[ N], N );
     modelText(painter, _text[NE], NE);
     modelText(painter, _text[ E], E );
     modelText(painter, _text[SE], SE);
     modelText(painter, _text[ S], S );
     modelText(painter, _text[SW], SW);
     modelText(painter, _text[ W], W );
     modelText(painter, _text[NW], NW);

     windFlag(painter);
 }

/////////////////////////////////////////////////////////////////////////////////////////////////
void QStationModelGraphicsItem::modelText(QPainter *painter, QString txt, TEXT_POS position) {

	double x;
	double y;
    // The text will be placed around the perimeter of rect
	QRect rect(-_textOffset, _textOffset, 2*_textOffset, -2*_textOffset);

	// find the size of the text bounding box
    QRect textBox = painter->fontMetrics().boundingRect(txt);

	// determine the start coordinates for the text. Arrange for justification
    // to rect.
    switch (position) {
	case N:
	    x = -textBox.width()/2;
	    y = rect.bottom();
		break;
	case NE:
	    x = rect.right();
	    y = rect.bottom();
		break;
	case E:
	    x = rect.right();
	    y = textBox.height()/2;
		break;
	case SE:
	    x = rect.right();
	    y = rect.top() + textBox.height();
		break;
	case S:
	    x = -textBox.width()/2;
	    y = rect.top() + textBox.height();
		break;
	case SW:
	    x = rect.left() - textBox.width();
	    y = rect.top() + textBox.height();
		break;
	case W:
	    x = rect.topLeft().x() - textBox.width();
	    y = textBox.height()/2;
		break;
	case NW:
	    x = rect.left() - textBox.width();
	    y = rect.bottom();
		break;
	}

    // render the text.
    QPointF point(x,y);
    painter->drawText(point, txt);

}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QStationModelGraphicsItem::windFlag(QPainter *painter) {

	// draw the dot at the center of the flag
	double dotRadius = _textOffset/10;
	painter->drawEllipse(-dotRadius, -dotRadius, 2*dotRadius, 2*dotRadius);

	double barbLen = _textOffset;
	double symScale = 0.20 * barbLen;

	QPointF p1(0.0, 0.0);
	QPointF p2;

	double d;
	d = 450 - _dirMet;
	if (d < 0)
		d = d + 360;
	if (d >= 360)
		d = d - 360;

	xyang(p1, d, barbLen, p2);
	painter->drawLine(p1, p2);
	p1 = p2;

	//painter->symbol(QPointF(_x, _y), SKEWT_BLUE, Rect::SmallDot);

	//	convert from m/s to knots.
	//double w = _wspd*1.94;
	double w = _spdKnots;

	double triLength = symScale / sin(60 * 3.14159 / 180);

	double delta;

	// plot the 50 symbols, which will be equilateral triangles

	bool did50 = 0;

	delta = 50;

	while (w > delta) {
		xyang(p1, d - 120, triLength, p2);
		painter->drawLine(p1, p2);
		p1 = p2;
		xyang(p1, d + 120, triLength, p2);
		painter->drawLine(p1, p2);
		p1 = p2;

		w = w - delta;
		did50 = 1;
	}

	if (did50) {
		xyang(p1, d + 180, symScale / 3, p2); // move in along the barb
		painter->drawLine(p1, p2);
		p1 = p2;
	}

	// plot the 10 symbols, which will be full length flags

	delta = 10;

	while (w > delta) {

		xyang(p1, d - 90, symScale, p2);
		painter->drawLine(p1, p2);
		p1 = p2;

		xyang(p1, d + 90, symScale, p2);
		painter->drawLine(p1, p2);
		p1 = p2;

		xyang(p1, d + 180, symScale / 2, p2);
		p1 = p2;

		w = w - delta;
	}

	// plot the 5 symbols, which will be half length flags

	delta = 5;

	while (w > delta) {

		xyang(p1, d - 90, symScale / 2, p2);
		painter->drawLine(p1, p2);
		p1 = p2;

		xyang(p1, d + 90, symScale / 2, p2);
		painter->drawLine(p1, p2);
		p1 = p2;

		xyang(p1, d + 180, symScale / 2, p2);
		p1 = p2;

		w = w - delta;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QStationModelGraphicsItem::xyang(QPointF p, double angle, double length, QPointF& newP)
  {
  double d = angle * 3.14159 / 180.0;

  double deltaX = length * cos(d);

  double deltaY = length * sin(d)/_aspectRatio;

  newP = QPointF(p.x() + deltaX, p.y() - deltaY);

  }

/////////////////////////////////////////////////////////////////////////////////////////////////
void QStationModelGraphicsItem::setText(TEXT_POS pos, QString text) {
	_text[pos] = text;
}
