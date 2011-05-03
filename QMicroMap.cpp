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
Feature::Feature(std::string tableName, std::string baseColor):
_tableName(tableName),
_name(tableName),
_baseColor(baseColor){
}
Feature::~Feature() {}

PointFeature::PointFeature(std::string tableName, std::string baseColor, std::string edgeColor):
Feature(tableName, baseColor),
_edgeColor(edgeColor) {
}
PointFeature::~PointFeature() {}

LineFeature::LineFeature(std::string tableName, std::string baseColor):
Feature(tableName, baseColor) {
}
LineFeature::~LineFeature() {}

PolygonFeature::PolygonFeature(std::string tableName, std::string baseColor, std::string edgeColor):
Feature(tableName, baseColor),
_edgeColor(edgeColor) {
}
PolygonFeature::~PolygonFeature() {}

/////////////////////////////////////////////////////////////////////////////////////////////////
QMicroMap::QMicroMap(SpatiaLiteDB& db,
		double xmin,
		double ymin,
		double xmax,
		double ymax,
		std::string backgroundColor,
		QWidget* parent):
QGraphicsView(parent),
_db(db),
_xmin(xmin),
_ymin(ymin),
_xmax(xmax),
_ymax(ymax)
{

	// determine what features we will use from this database
	selectFeatures();

	setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

	// create the scene to hold our graphics items
	_scene = new QGraphicsScene(this);
	setScene(_scene);

	// set the background color
	setBackgroundBrush(QBrush(backgroundColor.c_str()));

    // set the scene rectangle to match our bounding box
	setSceneRect(_xmin, _ymin, _xmax-_xmin, _ymax-_ymin);

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
	all_features.push_back(new PolygonFeature("admin_0_countries","beige", "black"));
	all_features.push_back(new LineFeature("admin_1_states_provinces_lines_shp", "grey"));
	all_features.push_back(new PolygonFeature("lakes","lightblue", "blue"));
	all_features.push_back(new LineFeature("rivers_lake_centerlines", "blue"));
	all_features.push_back(new PointFeature("geography_regions_points", "yellow", "brown"));
	all_features.push_back(new PointFeature("geography_regions_elevation_points", "lightgreen", "green"));
	all_features.push_back(new PointFeature("populated_places","red", "black"));
	all_features.push_back(new LineFeature("geographic_lines", "black"));
	all_features.push_back(new LineFeature("coastline", "red"));

	// get the names of tables containing geometry
	std::vector<std::string> geo_tables;
	geo_tables = _db.geometryTables();

	// query the tables
	for (std::vector<Feature*>::iterator
			feature = all_features.begin();
			feature != all_features.end();
			feature++) {

		std::string table = (*feature)->_tableName;

		std::vector<std::string>::iterator result =
				std::find(geo_tables.begin(), geo_tables.end(), table);
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

	update();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QMicroMap::drawFeatures() {

	for (std::vector<Feature*>::iterator
			feature = _features.begin();
			feature != _features.end();
			feature++) {

		std::string table = (*feature)->_tableName;

		// query the table
		_db.queryGeometry(table, "Geometry", _xmin, _ymin, _xmax, _ymax);

		SpatiaLiteDB::PointList points = _db.points();
		SpatiaLiteDB::LinestringList linestrings = _db.linestrings();
		SpatiaLiteDB::PolygonList polygons = _db.polygons();

		for (int i = 0 ; i < points.size(); i++) {
			drawPoint(*feature, points[i]);
		}

		for (int i = 0 ; i < polygons.size(); i++) {
			drawPolygon(*feature, polygons[i]);
		}

		for (int i = 0 ; i < linestrings.size(); i++) {
			drawLinestring(*feature, linestrings[i]);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void QMicroMap::drawLinestring(Feature* feature, SpatiaLiteDB::Linestring& ls) {

	LineFeature* lfeature = dynamic_cast<LineFeature*>(feature);
	if (!lfeature) {
		std::cerr << "dynamic cast failed for line in " << feature->_tableName << std::endl;
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

	PointFeature* pfeature = dynamic_cast<PointFeature*>(feature);
	if (!pfeature) {
		std::cerr << "dynamic cast failed for line in " << feature->_tableName << std::endl;
		return;
	}

	QPen pen(pfeature->_edgeColor.c_str());
	QBrush brush(pfeature->_baseColor.c_str());

	double x = pt._x;
	double y = pt._y;
	double deltax = 0.2;
	double deltay = 0.2;
	QRectF rect(x,y,deltax, deltay);

	QGraphicsEllipseItem* item = new QGraphicsEllipseItem(rect);
	item->setPen(pen);
	item->setBrush(brush);

	_scene->addItem(item);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QMicroMap::drawPolygon(Feature* feature, SpatiaLiteDB::Polygon& pl) {

	PolygonFeature* pfeature = dynamic_cast<PolygonFeature*>(feature);
	if (!pfeature) {
		std::cerr << "dynamic cast failed for polygon in " << feature->_tableName << std::endl;
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

	//this->fitInView(_scene->sceneRect());
	QRectF rect(_xmin, _ymin, _xmax-_xmin, _ymax-_ymin);
	this->fitInView(rect);

	//Call the subclass resize so the scrollbars are updated correctly
    QGraphicsView::resizeEvent(event);

}
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////