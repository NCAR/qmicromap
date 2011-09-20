/*
 * QMicroMapTest.cpp
 *
 *  Created on: May 3, 2011
 *      Author: martinc
 */
#include "QMicroMapTest.h"
#include <iostream>
#include "QStationModelGraphicsItem.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
QMicroMapTest::QMicroMapTest(SpatiaLiteDB& db,
		double xmin,
		double ymin,
		double xmax,
		double ymax,
		std::string backgroundColor,
		QWidget* parent):
QDialog(parent),
_xmin(xmin),
_ymin(ymin),
_xmax(xmax),
_ymax(ymax),
_zoomInc(0.1)
{
	// setting up the Qt form
	setupUi(this);

	// we need a layout manager
	QVBoxLayout* vb = new QVBoxLayout(frame);

	// create the micromap and add to the layout
	_mm = new QMicroMap(db, _xmin, _ymin, _xmax, _ymax, backgroundColor);
	vb->addWidget(_mm);

	// collect a list of stations, which will e added to an item group
	QList<QGraphicsItem*> stationList;

	// create one leg of station models
	double wspd = 45;
	double wdir = 15;
	double lon = -86;
	double tdry = 23.0;
	double RH = 75.0;
	double presOrHeight = 1013;
	bool isPres = true;
	int hh = 17;
	int mm = 01;

	for (double lat = 24.0; lat < 29.0; lat += 0.8) {
		QStationModelGraphicsItem* sm
			= new QStationModelGraphicsItem(lon, lat, wspd, wdir, tdry, RH, presOrHeight, isPres, hh, mm, 60);
		wspd += 12;
		wdir += 13;
		lon -= 0.9;
		RH -= 4;
		mm += 3;
		tdry -= 1.3;
		presOrHeight -= 3;
		stationList.append(sm);
	}

	// create a seond leg of station models
	wspd = 55;
	wdir = 15;
	lon = -85;
	hh  -= 2;
	for (double lat = 27.0; lat >= 24.0; lat -= 0.51) {
		QStationModelGraphicsItem* sm
			= new QStationModelGraphicsItem(lon, lat, wspd, wdir, tdry, RH, presOrHeight, isPres, hh, mm, 60);
		wspd += 9;
		wdir += 13;
		lon -= 1.8;
		RH -= 4;
		mm -= 3;
		tdry += .8;
		presOrHeight -= 3;
		stationList.append(sm);
	}

	// create the item group of stations
	_stationGroup = _mm->scene()->createItemGroup(stationList);
	// Allow events to be propagated to the children. Note that the Qt documentation
	// for this is wrong; it states that the default is false, which is not true for
	// a QGraphicsItemGroup.
	_stationGroup->setHandlesChildEvents(false);

	// connect signals
	connect(labels,    SIGNAL(stateChanged(int)),                _mm,  SLOT(labels(int)));
	connect(grid,      SIGNAL(stateChanged(int)),                _mm,  SLOT(grid(int)));
	connect(obs,       SIGNAL(stateChanged(int)),                this, SLOT(obsSlot(int)));
	connect(mousePan,  SIGNAL(toggled(bool)),                    this, SLOT(mouseSlot(bool)));
	connect(mouseZoom, SIGNAL(toggled(bool)),                    this, SLOT(mouseSlot(bool)));
	connect(reset,     SIGNAL(released()),                       _mm,  SLOT(reset()));
	connect(_mm,       SIGNAL(mouseMode(QMicroMap::MOUSE_MODE)), this, SLOT(mouseModeSlot(QMicroMap::MOUSE_MODE)));

}


/////////////////////////////////////////////////////////////////////////////////////////////////
QMicroMapTest::~QMicroMapTest() {

}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QMicroMapTest::mouseSlot(bool b) {

	if (mousePan->isChecked()) {
		_mm->setMouseMode(QMicroMap::MOUSE_PAN);
	} else {
		if (mouseZoom->isChecked()) {
			_mm->setMouseMode(QMicroMap::MOUSE_ZOOM);
		} else {
			if (mouseSelect->isChecked()) {
				_mm->setMouseMode(QMicroMap::MOUSE_SELECT);
			}
		}
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QMicroMapTest::mouseModeSlot(QMicroMap::MOUSE_MODE mode) {

	switch (mode) {
	case QMicroMap::MOUSE_PAN:
		mousePan->setChecked(true);
		mouseZoom->setChecked(false);
		mouseSelect->setChecked(false);
		break;
	case QMicroMap::MOUSE_ZOOM:
		mousePan->setChecked(false);
		mouseZoom->setChecked(true);
		mouseSelect->setChecked(false);
		break;
	case QMicroMap::MOUSE_SELECT:
		mousePan->setChecked(false);
		mouseZoom->setChecked(false);
		mouseSelect->setChecked(true);
		break;
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QMicroMapTest::obsSlot(int on) {
	_stationGroup->setVisible(on);
}
