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
	Feature(std::string tableName, std::string baseColor, std::string nameColumn = "");
	virtual ~Feature();
	std::string _tableName;
	std::string _name;
	std::string _baseColor;
	std::string _nameColumn;
};

class PointFeature: public Feature {
public:
	PointFeature(std::string tableName, std::string baseColor, std::string edgeColor, std::string nameColumn = "");
	virtual ~PointFeature();
	std::string _edgeColor;
};

class LineFeature: public Feature {
public:
	LineFeature(std::string tableName, std::string baseColor, std::string nameColumn = "");
	virtual ~LineFeature();
};

class PolygonFeature: public Feature {
public:
	PolygonFeature(std::string tableName, std::string baseColor, std::string edgeColor, std::string nameColumn = "");
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
			std::string backGroundColor = "white",
			QWidget* parent = 0);
	virtual ~QMicroMap();

public slots:
	void drawFeatures(double xmin, double ymin, double xmax, double ymax);
	void labels(int on);

protected:
    virtual void resizeEvent(QResizeEvent* event);
    //Set the current centerpoint in the
    void setCenter(const QPointF& centerPoint);
    QPointF getCenter();
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    /// Create the features that are available in the database. These
    /// will be saved in _features.
    void selectFeatures();
    /// extract the features from the database and draw them.
    /// xmin, ymin, xmax, ymax specifies the bounding box.
    void drawFeatures();
    ///
    void drawPoint(Feature* feature, SpatiaLiteDB::Point& p, QGraphicsItemGroup* group = 0);
    ///
    void drawLinestring(Feature* feature, SpatiaLiteDB::Linestring& l);
    ///
    void drawPolygon(Feature* feature, SpatiaLiteDB::Polygon& p);
    ///
    void drawGrid();
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
	QPoint _lastPanPoint;
    //Holds the current centerpoint for the view, used for panning and zooming
    QPointF _currentCenterPoint;
    ///
    QGraphicsItemGroup* _pointsGroup;
    ///
    QGraphicsItemGroup* _gridGroup;

};

#endif /* QMICROMAP_H_ */
