/*
 * QMicroMapTest.cpp
 *
 *  Created on: May 3, 2011
 *      Author: martinc
 */
#include "QMicroMapTest.h"

QMicroMapTest::QMicroMapTest(SpatiaLiteDB& db,
		double xmin,
		double ymin,
		double xmax,
		double ymax,
		std::string backgroundColor,
		QWidget* parent):
QDialog(parent)
{
	// setting up the Qt form
	setupUi(this);

	QMicroMap* mm = new QMicroMap(db, xmin, ymin, xmax, ymax);
	QVBoxLayout* vb = new QVBoxLayout(this);
	frame->setLayout(vb);
	vb->addWidget(mm);

}

QMicroMapTest::~QMicroMapTest() {

}

