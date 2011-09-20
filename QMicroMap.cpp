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
void printTransform(QTransform t) {
	std::cout << "m11:" << t.m11() << " m22:" << t.m22() << std::endl;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
Feature::Feature(
		std::string tableName,
		std::string baseColor,
		std::string geometryName,
		std::string nameColumn) :
	_tableName(tableName),
	_baseColor(baseColor),
	_geometryName(geometryName),
	_nameColumn(nameColumn) {
}

/////////////////////////////////////////////////////////////////////////////////////////////////
Feature::~Feature() {
}

/////////////////////////////////////////////////////////////////////////////////////////////////
PointFeature::PointFeature(
		std::string tableName,
		std::string baseColor,
		std::string edgeColor,
		std::string geometryName,
		std::string nameColumn) :
	Feature(tableName, baseColor, geometryName, nameColumn), _edgeColor(edgeColor) {
}

/////////////////////////////////////////////////////////////////////////////////////////////////
PointFeature::~PointFeature() {
}

/////////////////////////////////////////////////////////////////////////////////////////////////
LineFeature::LineFeature(
		std::string tableName,
		std::string baseColor,
		std::string geometryName,
		std::string nameColumn) :
	Feature(tableName, baseColor, geometryName, nameColumn) {
}

/////////////////////////////////////////////////////////////////////////////////////////////////
LineFeature::~LineFeature() {
}

/////////////////////////////////////////////////////////////////////////////////////////////////
PolygonFeature::PolygonFeature(
		std::string tableName,
		std::string baseColor,
		std::string edgeColor,
		std::string geometryName,
		std::string nameColumn) :
	Feature(tableName, baseColor, geometryName, nameColumn), _edgeColor(edgeColor) {
}

/////////////////////////////////////////////////////////////////////////////////////////////////
PolygonFeature::~PolygonFeature() {
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
QMicroMap::QMicroMap(SpatiaLiteDB& db, double xmin, double ymin, double xmax,
		double ymax, std::string backgroundColor, QWidget* parent) :
	QGraphicsView(parent), _db(db), _xmin(xmin), _ymin(ymin), _xmax(xmax),
			_ymax(ymax), _pointsGroup(0), _gridOn(true), _gridGroup(0),
			_gridDelta(0), _mouseMode(MOUSE_ZOOM), _rubberBand(0), _rbOrigin(100,100) {

	// determine what features we will use from this database
	selectFeatures();

	// Some say that antaliasing is a performance hit, and that it doesn't improver
	// the rendering. Neither was verified by my testing.
	setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

	// Allows the mouse events to pan the display
	setDragMode(QGraphicsView::NoDrag);

	setMouseMode(MOUSE_ZOOM);

	QRectF scene_rect = QRectF(_xmin, _ymin, _xmax - _xmin, _ymax - _ymin);

	// create the scene to hold our graphics items
	_scene = new QGraphicsScene(this);
	setScene(_scene);

	// set the background color
	setBackgroundBrush(QBrush(backgroundColor.c_str()));

	// disable the scrollbars
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	// Invert the y axis
	QMatrix m(1.0, 0.0, 0.0, -1.0, 0.0, 0.0);
	QGraphicsView::setMatrix(m);

	_pointsGroup = new QGraphicsItemGroup;

	// draw the features
	drawFeatures();

	_scene->addItem(_pointsGroup);
	_pointsGroup->hide();

	_gridGroup = new QGraphicsItemGroup;
	_scene->addItem(_gridGroup);

	_scene->setSceneRect(_scene->itemsBoundingRect());

	fitInView(scene_rect);

	_zoomRectStack.push(scene_rect);

	_scene->setSceneRect(_xmin, _ymin, _xmax - _xmin, _ymax - _ymin);

	viewport()->setMouseTracking(true);

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

	// define the features that we hope are in the database. There is a good chance that
	// they are not all avaiable.
	all_features.push_back(
			new PolygonFeature("admin_0_countries",               "beige",        "black"));
	all_features.push_back(
			new LineFeature("admin_1_states_provinces_lines_shp", "grey"));
	all_features.push_back(
			new PolygonFeature("lakes",                            "lightblue",   "blue"));
	all_features.push_back(
			new LineFeature("rivers_lake_centerlines",             "blue"));
	all_features.push_back(
			new PointFeature("geography_regions_points",            "yellow",     "brown", "Geometry", "Name"));
	all_features.push_back(
			new PointFeature("geography_regions_elevation_points",  "lightgreen", "green", "Geometry", "Name"));
	all_features.push_back(
			new PointFeature("populated_places",                    "red",        "black", "Geometry", "Name"));
	all_features.push_back(
			new LineFeature("geographic_lines",                     "yellow"));
	all_features.push_back(
			new LineFeature("coastline",                            "red"));

	// get the names of tables containing geometry
	std::vector < std::string > geo_tables;
	geo_tables = _db.geometryTables();

	// query the tables, to verify that the feature is avaiable
	for (std::vector<Feature*>::iterator feature = all_features.begin();
			feature != all_features.end(); feature++) {

		std::string table = (*feature)->_tableName;

		// Make sure that the requested geometry feature exists in the database.
		std::vector<std::string>::iterator result =
				std::find(geo_tables.begin(), geo_tables.end(), table);
		if (result == geo_tables.end()) {
			// selected table not found in the existing tables
			std::cerr << table << " not found" << std::endl;
			delete *feature;
			continue;
		}
		// Okay, it passed the test, so save it as one of the vetted features.
		_features.push_back(*feature);
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QMicroMap::labels(int on) {
	if (_pointsGroup) {
		_pointsGroup->setVisible(on);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QMicroMap::grid(int on) {
	_gridOn = on;
	if (_gridGroup) {
		_gridGroup->setVisible(on);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QMicroMap::scale(qreal sx, qreal sy) {

	// perform the scaling
	QGraphicsView::scale(sx, sy);

	// redraw the grid if necessary
	drawGrid();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QMicroMap::drawFeatures() {

	// Remove all existing items from the scene
	_scene->clear();

	for (std::vector<Feature*>::iterator feature = _features.begin(); feature
			!= _features.end(); feature++) {

		std::string table = (*feature)->_tableName;
		std::string geometryColumn = (*feature)->_geometryName;
		std::string nameColumn = (*feature)->_nameColumn;

		// query the table
		try {
			_db.queryGeometry(
					table,
					geometryColumn,
					_xmin, _ymin, _xmax, _ymax,
					nameColumn);
		} catch (std::string error) {
			std::cout << error << std::endl;
		}

		SpatiaLiteDB::PointList points = _db.points();
		SpatiaLiteDB::LinestringList linestrings = _db.linestrings();
		SpatiaLiteDB::PolygonList polygons = _db.polygons();

		for (int i = 0; i < points.size(); i++) {
			drawPoint(*feature, points[i], _pointsGroup);
		}

		for (int i = 0; i < polygons.size(); i++) {
			drawPolygon(*feature, polygons[i]);
		}

		for (int i = 0; i < linestrings.size(); i++) {
			drawLinestring(*feature, linestrings[i]);
		}
	}
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
void QMicroMap::drawPoint(Feature* feature, SpatiaLiteDB::Point& pt,
		QGraphicsItemGroup* group) {

	PointFeature* pfeature = dynamic_cast<PointFeature*> (feature);
	if (!pfeature) {
		std::cerr << "dynamic cast failed for line in " << feature->_tableName
				<< std::endl;
		return;
	}

	QPen pen(pfeature->_edgeColor.c_str());
	QBrush brush(pfeature->_baseColor.c_str());

	QRectF rect(-3, -3, 6, 6);

	QGraphicsEllipseItem* eitem = new QGraphicsEllipseItem(rect);
	eitem->setPos(pt._x, pt._y);
	eitem->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);

	eitem->setPen(pen);
	eitem->setBrush(brush);
	if (group) {
		group->addToGroup(eitem);
	} else {
		_scene->addItem(eitem);
	}

	std::string label = pt._label;
	if (label.size()) {
		QGraphicsSimpleTextItem* litem = new QGraphicsSimpleTextItem(
				label.c_str(), group);
		litem->setPos(pt._x, pt._y);
		litem->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
		litem->setFont(QFont("Helvetica", 12));
		litem->setPen(pen);
		litem->setBrush(brush);
		if (group) {
			group->addToGroup(litem);
		}
	}
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
void QMicroMap::drawGrid() {

	// get the current height of the viewport, in degrees.
	QRectF rect = mapToScene(viewport()->geometry()).boundingRect();
	double h = rect.height();

	// determine the grid spacing. It will be 1, 2, 5, 10 or 15 degrees latitude.
	// try for approx. 5 segments in latitude.
	double delta = h / 5.0;
	if (delta < 1.0) {
		delta = 1.0;
	} else {
		if (delta < 2) {
			delta = 2.0;
		} else {
			if (delta < 5.0) {
				delta = 5.0;
			} else {
				if (delta < 10.0) {
					delta = 10.0;
				} else {
					delta = 15.0;
				}
			}
		}
	}

	// if the grid spacing is different than the current grid, remove the
	// current grid and redraw.
	if (delta != _gridDelta) {
		_gridDelta = delta;

		// destroy the current grid
		QList<QGraphicsItem*> items = _gridGroup->childItems();
		for (int i = 0; i < items.size(); i++) {
			_gridGroup->removeFromGroup(items[i]);
			delete items[i];
		}

		// create the new grid
		QPen pen("grey");
		for (double x = _xmin; x <= _xmax; x += _gridDelta) {
			QGraphicsLineItem* line = new QGraphicsLineItem(QLineF(QPointF(x,
					_ymin), QPointF(x, _ymax)));
			line->setPen(pen);
			_gridGroup->addToGroup(line);
		}
		for (double y = _ymin; y <= _ymax; y += _gridDelta) {
			QGraphicsLineItem* line = new QGraphicsLineItem(QLineF(QPointF(
					_xmin, y), QPointF(_xmax, y)));
			line->setPen(pen);
			_gridGroup->addToGroup(line);
		}

		// make it visible based on the current choice.
		if (_gridOn) {
			_gridGroup->show();
		} else {
			_gridGroup->hide();
		}
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QMicroMap::resizeEvent(QResizeEvent* event) {

	// Call the subclass resize
	QGraphicsView::resizeEvent(event);

	// draw the grid
	drawGrid();

}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QMicroMap::mousePressEvent(QMouseEvent *event) {

	if (_mouseMode == MOUSE_SELECT) {
		QGraphicsView::mousePressEvent(event);
		return;
	}

	if (_mouseMode == MOUSE_PAN) {
		// mouse press on panning.
		QGraphicsView::mousePressEvent(event);
	} else {
		_rbOrigin = event->pos();
		if (!_rubberBand) {
			_rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
		}
		_rubberBand->setGeometry(QRect(_rbOrigin, QSize()));
		_rubberBand->show();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QMicroMap::mouseMoveEvent(QMouseEvent *event) {

	switch (_mouseMode) {
	case MOUSE_ZOOM:
		if (!_rubberBand) {
			_rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
		}
		_rubberBand->setGeometry(QRect(_rbOrigin, event->pos()).normalized());
		event->accept();
		break;

	case MOUSE_PAN:
	case MOUSE_SELECT:
		QGraphicsView::mouseMoveEvent(event);
		break;
	}

	return;

}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QMicroMap::mouseReleaseEvent(QMouseEvent *event) {

	if (event->button() == Qt::RightButton) {
		if (_zoomRectStack.size() > 1) {
			_zoomRectStack.pop();
			QRectF scenerect = _zoomRectStack.top();
			fitInView(scenerect);
		}
		return;
	}

	if (event->button() == Qt::LeftButton) {
		switch (_mouseMode) {
		case MOUSE_ZOOM: {
			// mouse release on zooming
			if (!_rubberBand) {
				_rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
			}
			_rubberBand->hide();
			QRect bandrect = _rubberBand->geometry();
			double bandh = bandrect.height();
			double bandw = bandrect.width();
			double viewh = viewport()->height();
			double vieww = viewport()->width();

			// make sure that the user has selected at least 5 percent of the scene
			// in both x and y directions.
			if ((bandh / viewh > 0.05) && (bandw / vieww > 0.05)) {
				QRectF scenerect = mapToScene(bandrect).boundingRect();
				fitInView(scenerect);
				_zoomRectStack.push(scenerect);
			}
			break;
		}
		case MOUSE_PAN:
		case MOUSE_SELECT:
			QGraphicsView::mouseReleaseEvent(event);
			break;
		}
	}

	return;

}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QMicroMap::setMouseMode(MOUSE_MODE mode) {

	_mouseMode = mode;

	switch (_mouseMode) {
	case MOUSE_PAN:
		setDragMode(QGraphicsView::ScrollHandDrag);
		setCursor(Qt::OpenHandCursor);
		break;
	case MOUSE_ZOOM:
		setDragMode(QGraphicsView::NoDrag);
		setCursor(Qt::CrossCursor);
		break;
	case MOUSE_SELECT:
		setDragMode(QGraphicsView::NoDrag);
		setCursor(Qt::ArrowCursor);
		break;
	default:
		break;
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QMicroMap::mouseDoubleClickEvent (QMouseEvent * event) {

	switch (_mouseMode) {
	case MOUSE_SELECT:
		setMouseMode(MOUSE_ZOOM);
	break;
	case MOUSE_PAN:
		setMouseMode(MOUSE_SELECT);
		break;
	case MOUSE_ZOOM:
		setMouseMode(MOUSE_PAN);
		break;
	default:
		break;
	}

	emit mouseMode(_mouseMode);

}

/////////////////////////////////////////////////////////////////////////////////////////////////
void QMicroMap::reset() {
	while (_zoomRectStack.size() > 1) {
		_zoomRectStack.pop();
	}

	QRectF scenerect = _zoomRectStack.top();
	fitInView(scenerect);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
