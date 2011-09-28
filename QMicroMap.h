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
/// @brief A property manager for features to be rendered on the map, and their pairing with
/// database elements.
/// The database table and column are specified, along with a feature name and its default rendering
/// color. Derived classes will add properties relevant to their type.
class Feature {
public:
	/// Constructor
	/// @param tableName The database table containing this feature.
	/// @param baseColor The default color for rendering this feature.
	/// @param geometryName The name of the column containing the geometry. It will
	/// almost always be named "Geometry".
	/// @param nameColumn If the database has a name associated with this feature, this is
	/// the column containing the name data.
	Feature(std::string tableName,
			std::string baseColor,
			std::string geometryName = "Geometry",
			std::string nameColumn = "");
	/// Destructor
	virtual ~Feature();
	/// The database table name.
	std::string _tableName;
	/// The default color for this feature.
	std::string _baseColor;
	/// The name of the column containing the geometry data.
	std::string _geometryName;
	/// The name of the column containing a name or identifier. Blank if none.
	std::string _nameColumn;
};

/// @brief A Point feature.
class PointFeature: public Feature {
public:
	/// Constructor
	/// @param tableName The database table containing this feature.
	/// @param baseColor The default internal color for rendering this feature.
	/// @param edgeColor The default edge color for rendering this feature.
	/// @param geometryName The name of the column containing the geometry. It will
	/// almost always be named "Geometry".
	/// @param nameColumn If the database has a name associated with this feature, this is
	/// the column containing the name data.
	PointFeature(
			std::string tableName,
			std::string baseColor,
			std::string edgeColor,
			std::string geometryName = "Geometry",
			std::string nameColumn = "");
	/// Destructor
	virtual ~PointFeature();
	/// The rendering color for the edge of the point.
	std::string _edgeColor;
};

class LineFeature: public Feature {
public:
	/// Constructor
	/// @param tableName The database table containing this feature.
	/// @param baseColor The default color for rendering this feature.
	/// @param edgeColor The default edge color for rendering this feature.
	/// @param geometryName The name of the column containing the geometry. It will
	/// almost always be named "Geometry".
	/// @param nameColumn If the database has a name associated with this feature, this is
	/// the column containing the name data.
	LineFeature(
			std::string tableName,
			std::string baseColor,
			std::string geometryName = "Geometry",
			std::string nameColumn = "");
	/// Destructor
	virtual ~LineFeature();
};

class PolygonFeature: public Feature {
public:
	/// Constructor
	/// @param tableName The database table containing this feature.
	/// @param baseColor The default color for rendering this feature.
	/// @param geometryName The name of the column containing the geometry. It will
	/// almost always be named "Geometry".
	/// @param nameColumn If the database has a name associated with this feature, this is
	/// the column containing the name data.
	PolygonFeature(
			std::string tableName,
			std::string baseColor,
			std::string edgeColor,
			std::string geometryName = "Geometry",
			std::string nameColumn = "");
	/// Destructor
	virtual ~PolygonFeature();
	/// The rendering color for the edge of the polygon.
	std::string _edgeColor;
};

/////////////////////////////////////////////////////////////////////
/// @brief Render a geographical database on a QGraphicsView.
/// Certain geographical features within that a specified bounding
/// box are extracted from the database and rendered on the QGraphicsView.
/// Zooming and panning are supported. A lat/lon grid overlay may be drawn.
/// Users may add their own additional elements, such as sounding station
/// models, to the view.
///
/// Note that panning is provided automatically by QGraphicsView, responding to
/// mouse move events and scroll wheel events. This is really nice.
///
/// Three modes are defined for left mouse button interaction: zooming, panning and selection.
/// In zooming and panning modes, the mouse is used to perform these activities.
/// In selection mode, the mouse events are passed on, allowing other scene elements
/// to interact with the mouse. A right click is used to step backwards the zoom history.
///
/// The display of feature names and the grid can be toggled.
///
/// A QGraphicsScene is created and contained within QMicroMap. It is used to
/// manage the graphics elements, in keeping with the Qt Graphics View framework.
/// All drawing is accomplished by creating QGraphicsItem derived classes (e.g.
/// QGraphicsPathItem), and adding them to the QGraphicsScene. Users can access
/// the scene via QMicroMap.scene().
///
/// The Feature is the basic element that is rendered. It corresponds to one geometric
/// feature type found in the database, such as an administrative boundary, a lake,
/// a coast line, etc.
///
/// Currently the features are hardwired to those found in the Natural Earth 1m to 50m
/// shape files. See selectFeatures() for the chosen ones.
/// @todo Make the feature definitions configurable, so that alternative databases
/// can be easily swapped in.
class QMicroMap: public QGraphicsView {
	Q_OBJECT

public:
	/// Behavioral modes for mouse interaction with QMicroMap.
	enum MOUSE_MODE  {
		/// The mouse is used for selecting objects.
		MOUSE_SELECT,
		/// The mouse is used for panning.
		MOUSE_PAN,
		/// The mouse is used for zooming.
		MOUSE_ZOOM};

	/// Constructor
	/// @param db The geographic database.
	/// @param xmin The bounding box minimum longitude, in decimal degrees.
	/// @param xmax The bounding box maximum longitude, in decimal degrees.
	/// @param ymin The bounding box minimum latitude, in decimal degrees.
	/// @param ymax The bounding box maximum latitude, in decimal degrees.
	/// @param backGroundColor The background color of the map.
	/// @param parent The parent widget.
	QMicroMap(SpatiaLiteDB& db,
			double xmin,
			double ymin,
			double xmax,
			double ymax,
			std::string backGroundColor = "white",
			QWidget* parent = 0);
	/// Destructor
	virtual ~QMicroMap();
	/// Set the mouse interaction mode.
	/// @param mode The mouse mode.
	void setMouseMode(MOUSE_MODE mode);
    /// Override the scale() function so that we can redraw the
    /// the grid when zooming is effected through scaling.
    virtual void scale(qreal sx, qreal sy);

public slots:
	/// Turn the feature labels on and off.
	/// @param on True if the labels should be displayed, false otherwise.
	void labels(int on);
	/// Turn the grid display on and off.
	/// @param on True if the grid should be displayed, false otherwise.
	void grid(int on);
	/// Reset the display to the minimum display level.
	void reset();

signals:
	/// Emmit this signal to inform others that the mouse mode has changed.
	void mouseMode(QMicroMap::MOUSE_MODE);

protected:
	/// Override the resize event, so that the grid may be redrawn.
	/// @param event The event.
    virtual void resizeEvent(QResizeEvent* event);
    /// Respond to a mouse press, depending upon the current mouse
    /// mode.  See the code for details!
	/// @param event The event.
    virtual void mousePressEvent(QMouseEvent* event);
    /// Respond to a mouse release, depending upon the current mouse
    /// mode. See the code for details!
	/// @param event The event.
    virtual void mouseReleaseEvent(QMouseEvent* event);
    /// Respond to a mouse move, depending upon the current mouse
    /// mode. See the code for details!
	/// @param event The event.
    virtual void mouseMoveEvent(QMouseEvent* event);
    /// Catch the mouse double click event. A double click is used to
    /// cycle through the different mouse modes. A mouseMode() signal
    /// will be emitted.
	/// @param event The event.
    virtual void mouseDoubleClickEvent (QMouseEvent * event);
    /// Create the features that are available in the database. These
    /// will be saved in _features. There is a possibility that some desired
    /// features do not exist in the database.
    void selectFeatures();
    /// Extract the features from the database and draw them.
    /// xmin, ymin, xmax, ymax specifies the bounding box.
    void drawFeatures();
    /// Draw a point, with the properties provided in feature.
    /// @param feature Use these properties for the rendering.
    /// @param p The point to be drawn.
    /// @param group If group is specified, add the point to this group. This will
    /// allow a collection of points, such as place identifiers, to be set visible together.
    void drawPoint(Feature* feature, SpatiaLiteDB::Point& p, QGraphicsItemGroup* group = 0);
    /// Draw a linestring, with the properties provided in feature.
    /// @param feature Use these properties for the rendering.
    /// @param l The linestring to be drawn.
    void drawLinestring(Feature* feature, SpatiaLiteDB::Linestring& l);
    /// Draw a polygon, with the properties provided in feature.
    /// @param feature Use these properties for the rendering.
    /// @param p The polygon to be drawn.
    void drawPolygon(Feature* feature, SpatiaLiteDB::Polygon& p);
    /// Draw the grid. A heuristic determines the grid spacing, based
    /// on the current span of the viewport.
    void drawGrid();
    /// The geometric database.
	SpatiaLiteDB& _db;
	/// The graphics scene which manages our drawing elements.
	QGraphicsScene* _scene;
	/// The zoom stack, containing the history of zoom levels.
	std::stack<QRectF> _zoomRectStack;
	/// Minimum longitude in the scene.
	double _xmin;
	/// Maximum longitude in the scene.
	double _ymin;
	/// Minimum latitude in the scene.
	double _xmax;
	/// Maximum latitude in the scene.
	double _ymax;
	/// The collection of features that were vetted and verified to be in the database.
	std::vector<Feature*> _features;
    /// The group of points. Points are used just for labels, and
	/// grouped so that they can be toggled on and off.
    QGraphicsItemGroup* _pointsGroup;
    /// True if the grid hould be drawn.
    bool _gridOn;
    /// The collection of grid lines.
    QGraphicsItemGroup* _gridGroup;
    /// The current grid spacing, in degrees.
    double _gridDelta;
    /// The current mousemode.
    MOUSE_MODE _mouseMode;
    /// The rubberband box used for zooming.
    QRubberBand* _rubberBand;
    /// The rubberband origin.
    QPoint _rbOrigin;
};

#endif /* QMICROMAP_H_ */
