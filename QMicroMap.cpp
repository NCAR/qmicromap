/*
 * QMicroMap.cpp
 *
 *  Created on: May 2, 2011
 *      Author: martinc
 */
#include "QMicroMap.h"
#include <iostream>
#include <algorithm>

/////////////////////////////////////////////////////////////////////////////////////////////////
Feature::Feature(std::string tableName, std::string baseColor) :
	_tableName(tableName), _name(tableName), _baseColor(baseColor) {
}
Feature::~Feature() {
}

PointFeature::PointFeature(std::string tableName, std::string baseColor,
		std::string edgeColor) :
	Feature(tableName, baseColor), _edgeColor(edgeColor) {
}
PointFeature::~PointFeature() {
}

LineFeature::LineFeature(std::string tableName, std::string baseColor) :
	Feature(tableName, baseColor) {
}
LineFeature::~LineFeature() {
}

PolygonFeature::PolygonFeature(std::string tableName, std::string baseColor,
		std::string edgeColor) :
	Feature(tableName, baseColor), _edgeColor(edgeColor) {
}
PolygonFeature::~PolygonFeature() {
}

/////////////////////////////////////////////////////////////////////////////////////////////////
QMicroMap::QMicroMap(SpatiaLiteDB& db, double xmin, double ymin, double xmax,
		double ymax, std::string backgroundColor, QWidget* parent) :
	QGraphicsView(parent), _db(db), _xmin(xmin), _ymin(ymin), _xmax(xmax),
			_ymax(ymax) {

	// determine what features we will use from this database
	selectFeatures();

	setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

	// create the scene to hold our graphics items
	_scene = new QGraphicsScene(this);
	setScene(_scene);

	// set the background color
	setBackgroundBrush(QBrush(backgroundColor.c_str()));

	// set the scene rectangle to match our bounding box
	setSceneRect(_xmin, _ymin, _xmax - _xmin, _ymax - _ymin);

	// disable the scrollbars
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	// Invert the y axis
	QMatrix m(1.0, 0.0, 0.0, -1.0, 0.0, 0.0);
	QGraphicsView::setMatrix(m);

	// Lend me a hand
	setCursor(Qt::OpenHandCursor);

	// draw the features
	drawFeatures();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
QMicroMap::~QMicroMap() {
	for (int i = 0; i < _features.size(); i++) {
		delete _features[i];
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QMicroMap::selectFeatures() {

	std::vector<Feature*> all_features;
	all_features.push_back(new PolygonFeature("admin_0_countries", "beige",
			"black"));
	all_features.push_back(new LineFeature(
			"admin_1_states_provinces_lines_shp", "grey"));
	all_features.push_back(new PolygonFeature("lakes", "lightblue", "blue"));
	all_features.push_back(new LineFeature("rivers_lake_centerlines", "blue"));
	all_features.push_back(new PointFeature("geography_regions_points",
			"yellow", "brown"));
	all_features.push_back(new PointFeature(
			"geography_regions_elevation_points", "lightgreen", "green"));
	all_features.push_back(new PointFeature("populated_places", "red", "black"));
	all_features.push_back(new LineFeature("geographic_lines", "black"));
	all_features.push_back(new LineFeature("coastline", "red"));

	// get the names of tables containing geometry
	std::vector < std::string > geo_tables;
	geo_tables = _db.geometryTables();

	// query the tables
	for (std::vector<Feature*>::iterator feature = all_features.begin(); feature
			!= all_features.end(); feature++) {

		std::string table = (*feature)->_tableName;

		std::vector<std::string>::iterator result = std::find(
				geo_tables.begin(), geo_tables.end(), table);
		if (result == geo_tables.end()) {
			// selected table not found in the existing tables
			std::cerr << table << " not found" << std::endl;
			delete *feature;
			continue;
		}
		_features.push_back(*feature);
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QMicroMap::drawFeatures(double xmin, double ymin, double xmax, double ymax) {

	_xmin = xmin;
	_ymin = ymin;
	_xmax = xmax;
	_ymax = ymax;

	drawFeatures();

}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QMicroMap::drawFeatures() {

	_scene->clear();

	std::cout << "drawing features " << _xmin << " " << _ymin << " " << _xmax
			<< " " << _ymax << std::endl;

	for (std::vector<Feature*>::iterator feature = _features.begin(); feature
			!= _features.end(); feature++) {

		std::string table = (*feature)->_tableName;

		// query the table
		_db.queryGeometry(table, "Geometry", _xmin, _ymin, _xmax, _ymax);

		SpatiaLiteDB::PointList points = _db.points();
		SpatiaLiteDB::LinestringList linestrings = _db.linestrings();
		SpatiaLiteDB::PolygonList polygons = _db.polygons();

		for (int i = 0; i < points.size(); i++) {
			drawPoint(*feature, points[i]);
		}

		for (int i = 0; i < polygons.size(); i++) {
			drawPolygon(*feature, polygons[i]);
		}

		for (int i = 0; i < linestrings.size(); i++) {
			drawLinestring(*feature, linestrings[i]);
		}
	}

	QRectF rect(_xmin, _ymin, _xmax - _xmin, _ymax - _ymin);
	fitInView(rect);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QMicroMap::drawLinestring(Feature* feature, SpatiaLiteDB::Linestring& ls) {

	LineFeature* lfeature = dynamic_cast<LineFeature*> (feature);
	if (!lfeature) {
		std::cerr << "dynamic cast failed for line in " << feature->_tableName
				<< std::endl;
		return;
	}

	if (ls.size() < 2) {
		return;
	}

	QPen pen(lfeature->_baseColor.c_str());
	QPainterPath path;
	path.moveTo(ls[0]._x, ls[0]._y);
	for (int i = 1; i < ls.size(); i++) {
		path.lineTo(ls[i]._x, ls[i]._y);
	}

	QGraphicsPathItem* item = new QGraphicsPathItem(path);
	item->setPen(pen);

	_scene->addItem(item);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QMicroMap::drawPoint(Feature* feature, SpatiaLiteDB::Point& pt) {

	PointFeature* pfeature = dynamic_cast<PointFeature*> (feature);
	if (!pfeature) {
		std::cerr << "dynamic cast failed for line in " << feature->_tableName
				<< std::endl;
		return;
	}

	QPen pen(pfeature->_edgeColor.c_str());
	QBrush brush(pfeature->_baseColor.c_str());

	double x = pt._x;
	double y = pt._y;
	double deltax = 0.2;
	double deltay = 0.2;
	QRectF rect(x, y, deltax, deltay);

	QGraphicsEllipseItem* item = new QGraphicsEllipseItem(rect);
	item->setPen(pen);
	item->setBrush(brush);

	_scene->addItem(item);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QMicroMap::drawPolygon(Feature* feature, SpatiaLiteDB::Polygon& pl) {

	PolygonFeature* pfeature = dynamic_cast<PolygonFeature*> (feature);
	if (!pfeature) {
		std::cerr << "dynamic cast failed for polygon in "
				<< feature->_tableName << std::endl;
		return;
	}

	QPen pen(pfeature->_edgeColor.c_str());
	QBrush brush(pfeature->_baseColor.c_str());

	SpatiaLiteDB::Ring extRing = pl.extRing();
	QPolygonF poly;
	for (int i = 0; i < extRing.size(); i++) {
		poly << QPointF(extRing[i]._x, extRing[i]._y);
	}

	QGraphicsPolygonItem* item = new QGraphicsPolygonItem(poly);
	item->setPen(pen);
	item->setBrush(brush);

	_scene->addItem(item);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QMicroMap::resizeEvent(QResizeEvent* event) {

	//QRectF rect(_xmin, _ymin, _xmax - _xmin, _ymax - _ymin);
	//fitInView(rect);
    //Get the rectangle of the visible area in scene coords
    QRectF visibleArea = mapToScene(rect()).boundingRect();
    setCenter(visibleArea.center());

	//Call the subclass resize so the scrollbars are updated correctly
	QGraphicsView::resizeEvent(event);

}

void QMicroMap::mousePressEvent(QMouseEvent* event) {
	std::cout << "mousePressEvent" << std::endl;
	//For panning the view
	_lastPanPoint = event->pos();
	setCursor(Qt::ClosedHandCursor);
}

void QMicroMap::mouseReleaseEvent(QMouseEvent* event) {
	std::cout << "mouseReleaseEvent" << std::endl;
	setCursor(Qt::OpenHandCursor);
	_lastPanPoint = QPoint();
}

void QMicroMap::mouseMoveEvent(QMouseEvent* event) {
	std::cout << "mouseMoveEvent" << std::endl;
	if (!_lastPanPoint.isNull()) {
		//Get how much we panned
		QPointF delta = mapToScene(_lastPanPoint) - mapToScene(event->pos());
		_lastPanPoint = event->pos();

		//Update the center ie. do the pan
		setCenter(getCenter() + delta);
	}
}
/**
 * Sets the current centerpoint.  Also updates the scene's center point.
 * Unlike centerOn, which has no way of getting the floating point center
 * back, SetCenter() stores the center point.  It also handles the special
 * sidebar case.  This function will claim the centerPoint to sceneRec ie.
 * the centerPoint must be within the sceneRec.
 */
//Set the current centerpoint in the
void QMicroMap::setCenter(const QPointF& centerPoint) {
	//Get the rectangle of the visible area in scene coords
	QRectF visibleArea = mapToScene(rect()).boundingRect();

	//Get the scene area
	QRectF sceneBounds = sceneRect();

	double boundX = visibleArea.width() / 2.0;
	double boundY = visibleArea.height() / 2.0;
	double boundWidth = sceneBounds.width() - 2.0 * boundX;
	double boundHeight = sceneBounds.height() - 2.0 * boundY;

	//The max boundary that the centerPoint can be to
	QRectF bounds(boundX, boundY, boundWidth, boundHeight);

	if (bounds.contains(centerPoint)) {
		//We are within the bounds
		_currentCenterPoint = centerPoint;
	} else {
		//We need to clamp or use the center of the screen
		if (visibleArea.contains(sceneBounds)) {
			//Use the center of scene ie. we can see the whole scene
			_currentCenterPoint = sceneBounds.center();
		} else {

			_currentCenterPoint = centerPoint;

			//We need to clamp the center. The centerPoint is too large
			if (centerPoint.x() > bounds.x() + bounds.width()) {
				_currentCenterPoint.setX(bounds.x() + bounds.width());
			} else if (centerPoint.x() < bounds.x()) {
				_currentCenterPoint.setX(bounds.x());
			}

			if (centerPoint.y() > bounds.y() + bounds.height()) {
				_currentCenterPoint.setY(bounds.y() + bounds.height());
			} else if (centerPoint.y() < bounds.y()) {
				_currentCenterPoint.setY(bounds.y());
			}

		}
	}

	//Update the scrollbars
	centerOn(_currentCenterPoint);
}

QPointF QMicroMap::getCenter() {
	return _currentCenterPoint;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
