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

protected:

};

#endif /* QMICROMAPTEST_H_ */
