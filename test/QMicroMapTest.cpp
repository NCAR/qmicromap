/*
 * QMicroMapTest.cpp
 *
 *  Created on: May 3, 2011
 *      Author: martinc
 */
#include "QMicroMapTest.h"
#include <iostream>

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

	_mm = new QMicroMap(db, _xmin, _ymin, _xmax, _ymax);
	QVBoxLayout* vb = new QVBoxLayout(frame);
	vb->addWidget(_mm);

	connect(zoomIn, SIGNAL(released()), this, SLOT(zoomInSlot()));
	connect(zoomOut, SIGNAL(released()), this, SLOT(zoomOutSlot()));

}

QMicroMapTest::~QMicroMapTest() {

}

void QMicroMapTest::zoomInSlot() {
	_mm->scale(1.0+_zoomInc, 1.0+_zoomInc);
}


void QMicroMapTest::zoomOutSlot() {
	_mm->scale(1.0/(1.0+_zoomInc), 1.0/(1.0+_zoomInc));
}

void QMicroMapTest::zoom(double deltax, double deltay) {

	_xmin = _xmin+deltax;
	_xmax = _xmax-deltax;
	_ymin = _ymin+deltay;
	_ymax = _ymax-deltay;
	_mm->drawFeatures(_xmin, _ymin, _xmax, _ymax);
}
