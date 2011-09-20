/*
 * QStationModelItem.h
 *
 *  Created on: May 5, 2011
 *      Author: martinc
 */

#ifndef QSTATIONMODELITEM_H_
#define QSTATIONMODELITEM_H_

#include <QtGui>
#include <map>

/// @brief A QGraphicsItem representation of the meterological station model.
///
/// A wind barb and text elements are rendered.
///
/// There are four text elements that appear on the plot, depicting
/// temperature, rh, pres (or height) and observation time.
///
/// Because the wind barb can be pointing in any direction, an
/// algorithm attempts to locate the text elements so that they are
/// not overdrawn by the barb.
/// The model is divided into eight sectors of equal angular width (45 degrees),
/// numbered 0 through 7. The wind flag will appear in one of these sectors,
/// based on the wind direction. The text elements will be rendered in
/// four other sectors. A predefined map is used to specify, for a wind flag
/// in a given sector, the sector that each of the four text elements is
/// displayed in.
///
/// The QGraphicsItem::ItemIgnoresTransformations flag is set for
/// this graphics item, which means that the internal coordinates are
/// those of the viewport, which I believe are screen pixels. They
/// are referenced to the zero origin of the item however. setPos() is
/// used to set the position in the graphics scene.
class QStationModelGraphicsItem: public QGraphicsItem
{
	/// @brief TextSectors is a helper class which locates the texts so
	/// that they don't interfere with the wind barb. A sector scheme
	/// is used. The sector numbers start at zero, and increase by one for
	/// each 45 degree segment. Even though the wind direction is specified in
	/// meteorological coordinates, the sectors are numbered in an
	/// increasing cartessian sense, with 0-45 degrees being sector
	/// 0, 45-90 degrees is sector 1, etc.
	class TextSectors {
	public:
		enum TEXT_TYPE {TDRY, RH, PHT, TIME};
		enum TEXT_JUST {LEFT, RIGHT, TOP, BOTTOM};
		/// @param wdir The wind direction, in meteorlogical coordinates,
		/// magnetic, pointing into the wind.
		/// @param The offset of the text along the radial.
		TextSectors(double wdir, double offset);
		/// Destructor
		virtual ~TextSectors();
		/// The sector occupied by the wind barb.
		int _windSector;
		/// The x coordinate of the sector occupied by each text type.
		std::map<TEXT_TYPE, double> _x;
		/// The y coordinate of the sector occupied by each text type.
		std::map<TEXT_TYPE, double> _y;
		/// The horizontal justification for this text
		std::map<TEXT_TYPE, TEXT_JUST> _hjust;
		/// The vertical justification for this text
		std::map<TEXT_TYPE, TEXT_JUST> _vjust;
	protected:
		/// Create the x,y coordinates for each text type. Figure out
		/// how to avoid conflicting with the wind barb.
		void createCoordinates();
		/// The wind direction, in meteorlogical coordinates,
		/// pointing into the wind.
		double _wdir;
		/// The offset of the text elements, along the radial.
		double _offset;
		/// The sector that this text type falls into.
		std::map<TEXT_TYPE, int> _sector;
	};

public:
	/// Text location indicators
	enum TEXT_POS {N, NE, E, SE, S, SW, W, NW};

	/// Constructor
	/// @param x X location in the QGraphicsscene coordinate system, typically longitude.
	/// @param x Y location in the QGraphicsscene coordinate system, typically latitude.
	/// @param spdKnots Wind speed in knots.
	/// @param dirMet Meteorological wind direction, magnetic, pointing into the wind.
	/// @param tDryC Temperature in degC.
	/// @param RH Relative humidity, in percent.
	/// @param presOrHeight Pressure in mb, or height in meters. The isPres flag identifies which it is.
	/// @param isPres Set true if presOrHeight is a pressure value, false if it is a height value.
	/// @param hh The hour time of observation.
	/// @param mm The minute time of the observation.
	/// @param scale The graphical size of the station model, in viewport coordinates.
	QStationModelGraphicsItem(
			double x,
			double y,
			double spdKnots,
			double dirMet,
			double tDryC,
			double RH,
			double presOrHeight,
			bool isPres,
			int hh,
			int mm,
			double scale);
	/// Destructor
	virtual ~QStationModelGraphicsItem();
	/// Paint the station model.
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    /// Assign the text to a given sector.
    /// @param pos The sector position indicator.
    /// @param text The text which is assigned to this position.
    void setText(TEXT_POS pos, QString text);

protected:
	/// @return The station model bounding box.
    QRectF boundingRect() const;
    /// Handle the hover enter event. Currently, no action is taken.
    /// @param event The event.
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) ;
    /// Handle the hover leave event. Currently, no action is taken.
    /// @param event The event.
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) ;
    /// Handle the mouse press event. Currently, no action is taken.
    /// @param event The event.
    void mousePressEvent (QGraphicsSceneMouseEvent* event);
	/// Draw a standard meteorological wind barb, representing the speed and direction of the
	/// wind. The barb points towards the direction the wind is coming from. Flags on the barb cumulatively add
	/// to the wind speed: 1/2 line is 5, a whole line is 10, and a triangle is 50. No assumption is made about the units of the
	/// wind speed.
    /// @param painter The painter to draw with.
    void drawWindFlag(QPainter *painter);
    /// Draw the text fields: tdry, rh, press/ht and time. The TextSectors class
    /// is used to determine the location of each element.
    /// @param painter The painter to draw with.
    void drawTextFields(QPainter *painter);
    /// Render a single text field.
    /// @param painter THe device to be drawn upon.
    /// @param sectors
    /// @param typ The text type
    /// @param txt The text to be rendered.
    void drawTextField(QPainter* painter, TextSectors& sectors, TextSectors::TEXT_TYPE typ, QString txt);
    /// Calculate the position of an endpoint at a given length and direction from a starting
	/// point. The aspect ratio of the Rect is taken into consideration, so that the
	/// direction is validly portrayed.
	/// @param p The initial point.
	/// @param angle The line angle, in cartessian coordinate system.
	/// @param length The length of the line, in Rect units.
	/// @param  newP The calculated endpoint is returned here.
    void xyang(QPointF p, double angle, double length, QPointF& newP);
    /// The center x location , in scene coordinates
    double _x;
    /// The center y location, in scene coordinates
    double _y;
    /// The wind speed in knots
    double _spdKnots;
    /// The wind direction in meteorological coordinates:
    /// 0 is a wind from the north,
    /// 90 is a wind from the east, etc.
    double _dirMet;
    /// Temperature in degC
    double _tDryC;
    /// RH in percent.
    double _RH;
	/// Pressure in mb, or height in meters. The isPres flag identifies which it is.
    double _presOrHeight;
	/// Set true if presOrHeight is a pressure value, false if it is a height value.
    double _isPres;
	/// The hour time of observation.
    int _hh;
	/// The minute time of the observation.
    int _mm;
    /// This map identifies the sectors that the
    std::map<TEXT_POS, QString> _text;
    int _scale;
    /// The aspect ration (Y/X) of the current viewport. It allows us to
    /// present angles correctly.
    double _aspectRatio;

};

#endif /* QSTATIONMODELITEM_H_ */
