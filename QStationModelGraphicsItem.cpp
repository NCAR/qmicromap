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
		double scale) :
QGraphicsItem(),
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
	_text[N] = "";
	_text[NE] = "";
	_text[E] = "";
	_text[SE] = "";
	_text[S] = "";
	_text[SW] = "";
	_text[W] = "";
	_text[NW] = "";

	setPos(_x, _y);

	setAcceptedMouseButtons(Qt::LeftButton);

	// turn off transformations. The item will now draw with local
	// scale in terms of screen pixels.
	setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
	setFlag(QGraphicsItem::ItemIsFocusable, true);

	// accept hover events
	setAcceptHoverEvents(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
QStationModelGraphicsItem::~QStationModelGraphicsItem() {
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QStationModelGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent * event) {

	// Event handling is just stubbed out here, until we find
	// something useful to do with it.

	QGraphicsItem::mousePressEvent(event);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QStationModelGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event) {

	// Event handling is just stubbed out here, until we find
	// something useful to do with it.

	QGraphicsItem::hoverEnterEvent(event);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QStationModelGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event) {

	// Event handling is just stubbed out here, until we find
	// something useful to do with it.

	QGraphicsItem::hoverEnterEvent(event);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
QRectF QStationModelGraphicsItem::boundingRect() const {

	QRectF r(-_scale, -_scale, 2 * _scale, 2 * _scale);

	return r;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QStationModelGraphicsItem::paint(QPainter *painter,
		const QStyleOptionGraphicsItem *option, QWidget *widget) {

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
	TextSectors sectors(_dirMet, 11);

	// Draw each text field
	QString tdry = QString("%1").arg(_tDryC, 0, 'f', 1);

	QString rh = QString("%1").arg(_RH, 0, 'f', 1);

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
	QString pht = QString("%1").arg((int)round(presOrHeight), 3, 10, QLatin1Char('0'));

	int t = _hh * 100 + _mm;
	QString time = QString("%1").arg(t, 4, 10, QChar('0'));	// filled with leading 0's

	if (_tDryC != -999.0)
		drawTextField(painter, sectors, TextSectors::TDRY, tdry);
	if (_RH != -999.0)
		drawTextField(painter, sectors, TextSectors::RH, rh);
	if (_presOrHeight != -999.0)
		drawTextField(painter, sectors, TextSectors::PHT, pht);

	drawTextField(painter, sectors, TextSectors::TIME, time);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QStationModelGraphicsItem::drawTextField(QPainter* painter,
		TextSectors& sectors, TextSectors::TEXT_TYPE typ, QString txt) {

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
	painter->drawEllipse(-dotRadius, -dotRadius, 2 * dotRadius, 2 * dotRadius);
	if (_spdKnots == 0.0) {
		// calm winds, draw double circle
		painter->drawEllipse(-1.5 * dotRadius, -1.5 * dotRadius, 3 * dotRadius,
				3 * dotRadius);
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

	while (w >= delta) {
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

	while (w >= delta) {

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

	while (w >= delta) {

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
void QStationModelGraphicsItem::xyang(QPointF p, double angle, double length, QPointF& newP) {
	double d = angle * 3.14159 / 180.0;

	double deltaX = length * cos(d);

	double deltaY = length * sin(d) / _aspectRatio;

	newP = QPointF(p.x() + deltaX, p.y() - deltaY);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QStationModelGraphicsItem::setText(TEXT_POS pos, QString text) {
	_text[pos] = text;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
QStationModelGraphicsItem::TextSectors::TextSectors(double wdir, double offset) :
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
	_windSector = (int) (dir / 22.5);

	// identify the text sectors that will not interfere
	// with the wind sector.
	switch (_windSector) {
	case 0:
	case 7:
	case 8:
	case 15:
		assignSector(2, 5, 9, 14);
		setVjustification(BOTTOM, BOTTOM, TOP, TOP);
		break;
	case 1:
		assignSector(0, 7, 10, 13);
		setVjustification(TOP, BOTTOM, TOP, TOP);
		break;
	case 2:
		assignSector(1, 7, 9, 14);
		setVjustification(TOP, BOTTOM, TOP, TOP);
		break;
	case 3:
	case 4:
	case 11:
	case 12:
		assignSector(1, 6, 9, 14);
		setVjustification(BOTTOM, BOTTOM, TOP, TOP);
		break;
	case 5:
		assignSector(0, 6, 9, 14);
		setVjustification(BOTTOM, TOP, TOP, TOP);
		break;
	case 6:
		assignSector(0, 7, 10, 13);
		setVjustification(BOTTOM, TOP, TOP, TOP);
		break;
	case 9:
		assignSector(2, 5, 8, 15);
		setVjustification(BOTTOM, BOTTOM, BOTTOM, TOP);
		break;
	case 10:
		assignSector(1, 6, 9, 15);
		setVjustification(BOTTOM, BOTTOM, BOTTOM, TOP);
		break;
	case 13:
		assignSector(1, 6, 8, 14);
		setVjustification(BOTTOM, BOTTOM, TOP, BOTTOM);
		break;
	case 14:
		assignSector(2, 5, 8, 15);
		setVjustification(BOTTOM, BOTTOM, TOP, BOTTOM);
		break;
	}
	//std::cout << "wdir " << wdir << ", dir " << dir << ", wind " << _windSector << ", alt " << _sector[PHT];
	//std::cout << ", tdry " << _sector[TDRY] << ", rh " << _sector[RH] << ", time " << _sector[TIME] << std::endl;

	_hjust[TDRY] = RIGHT;
	_hjust[RH] = RIGHT;
	_hjust[PHT] = LEFT;
	_hjust[TIME] = LEFT;

	// compute the coordinates for each text item
	createCoordinates();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
QStationModelGraphicsItem::TextSectors::~TextSectors() {
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QStationModelGraphicsItem::TextSectors::assignSector(int phtSector,
		int tdrySector, int rhSector, int timeSector)
{
	_sector[PHT]  = phtSector;
	_sector[TDRY] = tdrySector;
	_sector[RH]   = rhSector;
	_sector[TIME] = timeSector;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QStationModelGraphicsItem::TextSectors::setVjustification(TEXT_JUST phtVjust,
		TEXT_JUST tdryVjust, TEXT_JUST rhVjust, TEXT_JUST timeVjust)
{
	_vjust[PHT]  = phtVjust;
	_vjust[TDRY] = tdryVjust;
	_vjust[RH]   = rhVjust;
	_vjust[TIME] = timeVjust;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QStationModelGraphicsItem::TextSectors::createCoordinates()
{
	std::vector<TEXT_TYPE> types;
	types.push_back(TDRY);
	types.push_back(RH);
	types.push_back(PHT);
	types.push_back(TIME);

	for (std::vector<TEXT_TYPE>::iterator i = types.begin(); i != types.end();
			i++) {
		double angle = 22.5 * (_sector[*i] + 0.5);
		angle = M_PI * angle / 180.0;

		_x[*i] = cos(angle) * _offset;

		// the Y coordinate is inverted, since Y runs from top of screen to bottom of screen
		_y[*i] = -sin(angle) * _offset;
	}
}
