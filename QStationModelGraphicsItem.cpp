/*
 * QStationModelItem.cpp
 *
 *  Created on: May 5, 2011
 *      Author: martinc
 */

#include "QStationModelGraphicsItem.h"
#include <iostream>

/////////////////////////////////////////////////////////////////////////////////////////////////
QStationModelGraphicsItem::QStationModelGraphicsItem(
		double x,
		double y,
		double spdKnots,
		double dirMet,
		double tDryC,
		double RH,
		double presOrHeight,
		bool isPres,
		int hh,
		int mm,
		double scale,
		QGraphicsItem* parent):
QGraphicsItem(parent),
_x(x),
_y(y),
_spdKnots(spdKnots),
_dirMet(dirMet),
_tDryC(tDryC),
_RH(RH),
_presOrHeight(presOrHeight),
_isPres(isPres),
_hh(hh),
_mm(mm),
_scale(scale),
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
     return QRectF(-_scale, -_scale, 2*_scale, 2*_scale);
 }

/////////////////////////////////////////////////////////////////////////////////////////////////
void QStationModelGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
            QWidget *widget)
 {

	 QPen oldPen = painter->pen();

	 painter->setPen(QPen("black"));
     drawWindFlag(painter);

     painter->setPen(QPen("blue"));
     drawTextFields(painter);

     painter->setPen(oldPen);

 }

/////////////////////////////////////////////////////////////////////////////////////////////////
void QStationModelGraphicsItem::drawTextFields(QPainter* painter) {

	// get the sector and coordinate assignments for this wind direction
	TextSectors sectors(_dirMet, 10);

	// Draw each text field
	QString tdry = QString("%1").arg(_tDryC,        0,'f',1);

	QString rh   = QString("%1").arg(_RH,           0,'f',0);

	double presOrHeight = _presOrHeight;
	if (_isPres) {
		if (_presOrHeight >= 1000.0) {
			presOrHeight = _presOrHeight - 1000.0;
		} else {
			if (_presOrHeight >= 900.0) {
				presOrHeight = _presOrHeight - 900.0;
			}
		}
	}
	QString pht  = QString("%1").arg(presOrHeight, 0,'f',0);

	int t = _hh*100 + _mm;
	QString time = QString("%1").arg(t, 4);

	drawTextField(painter, sectors, TextSectors::TDRY, tdry);
	drawTextField(painter, sectors, TextSectors::RH,   rh);
	drawTextField(painter, sectors, TextSectors::PHT,  pht);
	drawTextField(painter, sectors, TextSectors::TIME, time);

}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QStationModelGraphicsItem::drawTextField(QPainter* painter, TextSectors& sectors, TextSectors::TEXT_TYPE typ, QString txt) {



	QRect textBox = painter->fontMetrics().boundingRect(txt);

    double xoffset = 0;
    if (sectors._hjust[typ] == TextSectors::RIGHT) {
    	xoffset = -textBox.width();
    }

    double yoffset = 0;
    if (sectors._vjust[typ] == TextSectors::TOP) {
    	yoffset = textBox.height();
    }

	double x = sectors._x[typ] + xoffset;
	double y = sectors._y[typ] + yoffset;

    painter->drawText(x, y, txt);


}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QStationModelGraphicsItem::drawWindFlag(QPainter *painter) {

	// draw the dot at the center of the flag
	double dotRadius = 3;
	painter->drawEllipse(-dotRadius, -dotRadius, 2*dotRadius, 2*dotRadius);
	if (_spdKnots < 0) {
		// winds are missing; draw double circle
		painter->drawEllipse(-1.5*dotRadius, -1.5*dotRadius, 3*dotRadius, 3*dotRadius);
	}

	if (_spdKnots < 0.1) {
		// Don't try to draw a flag when wind speed is missing or zero.
		return;
	}

	double barbLen = _scale;
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

/////////////////////////////////////////////////////////////////////////////////////////////////
QStationModelGraphicsItem::TextSectors::TextSectors(double wdir, double offset):
	_wdir(wdir),
	_offset(offset)
{

	double dir = 450 - wdir;

	// make sure that the direction is between 0 and 360
	while (dir < 0) {
		dir += 360.0;
	}
	while (dir >= 360) {
		dir -= 360.0;
	}

	// determine the wind flag sector
	_windSector = (int)(dir/45.0);

	// identify the sectors
	switch (_windSector) {
	case 1:
	case 2:
	case 5:
	case 6:
		_sector [TDRY] = 3;
		_sector [RH]   = 4;
		_sector [PHT]  = 0;
		_sector [TIME] = 7;
		break;
	case 0:
		_sector [TDRY] = 3;
		_sector [RH]   = 4;
		_sector [PHT]  = 1;
		_sector [TIME] = 7;
		break;
	case 3:
		_sector [TDRY] = 2;
		_sector [RH]   = 4;
		_sector [PHT]  = 0;
		_sector [TIME] = 7;
		break;
	case 4:
		_sector [TDRY] = 3;
		_sector [RH]   = 5;
		_sector [PHT]  = 0;
		_sector [TIME] = 7;
		break;
	case 7:
		_sector [TDRY] = 3;
		_sector [RH]   = 4;
		_sector [PHT]  = 0;
		_sector [TIME] = 6;
		break;
	}

	_hjust[TDRY] = RIGHT;
	_hjust[RH]   = RIGHT;
	_hjust[PHT]  = LEFT;
	_hjust[TIME] = LEFT;

	_vjust[TDRY] = BOTTOM;
	_vjust[RH]   = TOP;
	_vjust[PHT]  = BOTTOM;
	_vjust[TIME] = TOP;

	// compute the coordinates
	createCoordinates();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
QStationModelGraphicsItem::TextSectors::~TextSectors() {

}
/////////////////////////////////////////////////////////////////////////////////////////////////
void QStationModelGraphicsItem::TextSectors::createCoordinates() {

	std::vector<TEXT_TYPE> types;
	types.push_back(TDRY);
	types.push_back(RH);
	types.push_back(PHT);
	types.push_back(TIME);


	for (std::vector<TEXT_TYPE>::iterator i = types.begin(); i != types.end(); i++) {
		double angle = 45.0*(_sector[*i] + 0.5);
		angle = M_PI*angle/180.0;

		_x[*i] =  cos(angle)*_offset;

		// the Y coordinate is inverted, since Y runs from top of screen to bottom of screen
		_y[*i] = -sin(angle)*_offset;

		//std::cout << "wdir:" << _wdir << " wind sector:" << _windSector << " type:" << *i << " sector:" << _sector[*i] << " x:" << _x[*i] << "   y:" << _y[*i] << std::endl;
	}
}