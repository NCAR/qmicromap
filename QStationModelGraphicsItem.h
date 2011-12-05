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
#include <set>
#include <bitset>

/// @brief A QGraphicsItem representation of the meteorological station model.
///
/// A wind barb and text elements are rendered.
///
/// There are four text elements that appear on the plot, depicting
/// temperature, rh, pres (or height) and observation time.
///
/// Because the wind barb can be pointing in any direction, an
/// algorithm attempts to locate the text elements so that they are
/// not overdrawn by the barb.
/// The model is divided into sixteen sectors of equal angular width (22.5
/// degrees), numbered 0 through 15. The wind flag will appear in one of
/// these sectors, based on the wind direction. The text elements will be
/// rendered in four other sectors. A predefined map is used to specify,
/// for a wind flag in a given sector, the sector that each of the four
/// text elements is displayed in.
///
/// The QGraphicsItem::ItemIgnoresTransformations flag is set for
/// this graphics item, which means that the internal coordinates are
/// those of the viewport, which I believe are screen pixels. They
/// are referenced to the zero origin of the item however. setPos() is
/// used to set the position in the graphics scene.
class QStationModelGraphicsItem: public QGraphicsItem
{
	/// @brief TextSectors is a helper class which locates the texts so that
	/// they don't interfere with the wind barb. A sector scheme is used. The
	/// sector numbers start at zero, and increase by one for each 22.5 degree
	/// segment. Even though the wind direction is specified in meteorological
	/// coordinates, the sectors are numbered in an increasing cartessian sense,
	/// with 0-22.5 degrees being sector 0, 22.5-45 degrees is sector 1, etc.
	class TextSectors {
	public:
		enum TEXT_TYPE { TDRY, RH, PHT, TIME };
		enum TEXT_JUST { LEFT, RIGHT, TOP, BOTTOM };
		/// @param wdir The wind direction, in meteorological coordinates,
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
		/// Assign sectors for each text type
		/// @param phtSector The sector assigned to PHT text
		/// @param tdrySector The sector assigned to TDRY text
		/// @param rhSector The sector assigned to RH text
		/// @param timeSector The sector assigned to TIME text
		void assignSector(int phtSector, int tdrySector, int rhSector, int timeSector);
		/// Set vertical justification for each text type
		/// @param phtVjust The vertical justification for PHT text
		/// @param tdryVjust The vertical justification for TDRY text
		/// @param rhVjust The vertical justification for RH text
		/// @param timeVjust The vertical justification for TIME text
		void setVjustification(TEXT_JUST phtVjust, TEXT_JUST tdryVjust, TEXT_JUST rhVjust, TEXT_JUST timeVjust);
		/// Create the x,y coordinates for each text type. Figure out
		/// how to avoid conflicting with the wind barb.
		void createCoordinates();
		/// The wind direction, in meteorological coordinates,
		/// pointing into the wind.
		double _wdir;
		/// The offset of the text elements, along the radial.
		double _offset;
		/// The sector that this text type falls into.
		std::map<TEXT_TYPE, int> _sector;
	};

public:
	std::bitset<16> _parts;
	/// Keys for each of the station model elements
	enum MODEL_PART {
		MODEL_WIND   = 1,
		MODEL_TDRY   = 2,
		MODEL_DP     = 4,
		MODEL_PRESHT = 8,
		MODEL_TIME   = 16,
		MODEL_ALL    = 0xFF
	};
	enum {
		MODEL_WIND_BIT = 0,
		MODEL_TDRY_BIT = 1,
		MODEL_DP_BIT = 2,
		MODEL_PRESHT_BIT = 3,
		MODEL_TIME_BIT = 4
	};

	/// Constructor
	/// @param x X location in the QGraphicsscene coordinate system, typically longitude.
	/// @param y Y location in the QGraphicsscene coordinate system, typically latitude.
	/// @param spdKnots Wind speed in knots.
	/// @param dirMet Meteorological wind direction, magnetic, pointing into the wind.
	/// @param tDryC Temperature in degC.
	/// @param DP Dew point, in degC.
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
			double DP,
			double presOrHeight,
			bool isPres,
			int hh,
			int mm,
			double scale,
			ulong parts = MODEL_ALL);
	/// Destructor
	virtual ~QStationModelGraphicsItem();
	/// Paint the station model.
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    /// Enable the painting of a model part. Use bitwise combinations of MODEL_PART
    /// constants to specify a bit mask to select specific parts.
    /// @param part The part to show.
	virtual void showpart(ulong part);
    /// Disable the painting of a model part. Use bitwise combinations of MODEL_PART
    /// constants to specify a bit mask to select specific parts.
    /// @param part The part to hide.
	virtual void hidepart(ulong part);

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
	/// Draw a standard meteorological wind barb, representing the speed and direction of
    /// the wind. The barb points towards the direction the wind is coming from. Flags on
    /// the barb cumulatively add to the wind speed: 1/2 line is 5, a whole line is 10,
    /// and a triangle is 50. No assumption is made about the units of the wind speed.
    /// @param painter The painter to draw with.
    void drawWindFlag(QPainter *painter);
    /// Draw the text fields: tdry, rh, press/ht and time. The TextSectors class
    /// is used to determine the location of each element.
    /// @param painter The painter to draw with.
    void drawTextFields(QPainter *painter);
    /// Render a single text field.
    /// @param painter The device to be drawn upon.
    /// @param sectors The sector to draw the text field
    /// @param typ The text type
    /// @param txt The text to be rendered.
    void drawTextField(QPainter* painter, TextSectors& sectors, TextSectors::TEXT_TYPE typ, QString txt);
    /// Calculate the position of an endpoint at a given length and direction from a starting
	/// point. The aspect ratio of the Rect is taken into consideration, so that the
	/// direction is validly portrayed.
	/// @param p The initial point.
	/// @param angle The line angle, in cartessian coordinate system.
	/// @param length The length of the line, in Rect units.
	/// @param newP The calculated endpoint is returned here.
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
    double _DP;
	/// Pressure in mb, or height in meters. The isPres flag identifies which it is.
    double _presOrHeight;
	/// Set true if presOrHeight is a pressure value, false if it is a height value.
    double _isPres;
	/// The hour time of observation.
    int _hh;
	/// The minute time of the observation.
    int _mm;
    int _scale;
    /// The aspect ration (Y/X) of the current viewport. It allows us to
    /// present angles correctly.
    double _aspectRatio;

};

#endif /* QSTATIONMODELITEM_H_ */
