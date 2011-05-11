/*
 * QMicroMapTest.h
 *
 *  Created on: May 3, 2011
 *      Author: martinc
 */

#ifndef QMICROMAPTEST_H_
#define QMICROMAPTEST_H_

#include <QtGui>
#include "ui_QMicroMapTest.h"
#include "QMicroMap.h"

class QMicroMapTest: public QDialog, public Ui::QMicroMapTest
{
	Q_OBJECT
public:
	QMicroMapTest(SpatiaLiteDB& db,
			double xmin,
			double ymin,
			double xmax,
			double ymax,
			std::string backGroundColor = "white",
			QWidget* parent = 0);
	virtual ~QMicroMapTest();

public slots:
	void zoomInSlot();
	void zoomOutSlot();
	void obsSlot(int);

protected:
	void zoom(double deltax, double deltay);
	QMicroMap* _mm;
	QGraphicsItemGroup* _stationGroup;
	double _xmin;
	double _ymin;
	double _xmax;
	double _ymax;
	double _zoomInc;

};

#endif /* QMICROMAPTEST_H_ */
