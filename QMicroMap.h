/*
 * QMicroMap.h
 *
 *  Created on: May 2, 2011
 *      Author: martinc
 */

#ifndef QMICROMAP_H_
#define QMICROMAP_H_

#include <QtGui>
#include <stack>
#include <vector>
#include <string>
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
	enum MOUSE_MODE  {MOUSE_PAN, MOUSE_ZOOM};

	QMicroMap(SpatiaLiteDB& db,
			double xmin,
			double ymin,
			double xmax,
			double ymax,
			std::string backGroundColor = "white",
			QWidget* parent = 0);
	virtual ~QMicroMap();
	void setMouseMode(MOUSE_MODE mode);
    /// Override the scale() function so that we can redraw the
    /// the grid when zooming is effected through scaling.
    virtual void scale(qreal sx, qreal sy);

public slots:
	void labels(int on);
	void grid(int on);
	void reset();

protected:
    virtual void resizeEvent(QResizeEvent* event);
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
	std::stack<QRectF> _zoomRectStack;
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
    ///
    QGraphicsItemGroup* _pointsGroup;
    ///
    bool _gridOn;
    ///
    QGraphicsItemGroup* _gridGroup;
    /// The current grid spacing, in degrees.
    double _gridDelta;
    MOUSE_MODE _mouseMode;
    QRubberBand* _rubberBand;
    QPoint _rbOrigin;
};

#endif /* QMICROMAP_H_ */
