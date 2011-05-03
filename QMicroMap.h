/*
 * QMicroMap.h
 *
 *  Created on: May 2, 2011
 *      Author: martinc
 */

#ifndef QMICROMAP_H_
#define QMICROMAP_H_

#include <QtGui>
#include "SpatiaLiteDB.h"

/////////////////////////////////////////////////////////////////////
class Feature {
public:
	Feature(std::string tableName, std::string baseColor);
	virtual ~Feature();
	std::string _tableName;
	std::string _name;
	std::string _baseColor;
};

class PointFeature: public Feature {
public:
	PointFeature(std::string tableName, std::string baseColor, std::string edgeColor);
	virtual ~PointFeature();
	std::string _edgeColor;
};

class LineFeature: public Feature {
public:
	LineFeature(std::string tableName, std::string baseColor);
	virtual ~LineFeature();
};

class PolygonFeature: public Feature {
public:
	PolygonFeature(std::string tableName, std::string baseColor, std::string edgeColor);
	virtual ~PolygonFeature();
	std::string _edgeColor;
};

/////////////////////////////////////////////////////////////////////
class QMicroMap: public QGraphicsView {
	Q_OBJECT
public:
	QMicroMap(SpatiaLiteDB& db,
			double xmin,
			double ymin,
			double xmax,
			double ymax,
			QWidget* parent = 0);
	virtual ~QMicroMap();

protected:
    virtual void resizeEvent(QResizeEvent* event);
    /// Create the features that are available in the database. These
    /// will be saved in _features.
    void selectFeatures();
    /// extract the features from the database and draw them.
    /// xmin, ymin, xmax, ymax specifies the bounding box.
    void drawFeatures();
    ///
    void drawPoint(Feature* feature, SpatiaLiteDB::Point& p);
    ///
    void drawLinestring(Feature* feature, SpatiaLiteDB::Linestring& l);
    ///
    void drawPolygon(Feature* feature, SpatiaLiteDB::Polygon& p);
    ///
	SpatiaLiteDB& _db;
	///
	QGraphicsScene* _scene;
	///
	double _xmin;
	///
	double _ymin;
	///
	double _xmax;
	///
	double _ymax;
	///
	std::vector<Feature*> _features;


};

#endif /* QMICROMAP_H_ */
