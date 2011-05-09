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

/// The internal coordinates are in pixels
class QStationModelGraphicsItem: public QGraphicsItem
{
public:
	/// Location indicators
	enum TEXT_POS {N, NE, E, SE, S, SW, W, NW};

	QStationModelGraphicsItem(double x, double y,
			double spdKnots, double dirMag, QGraphicsItem* parent=0);
	virtual ~QStationModelGraphicsItem();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setText(TEXT_POS pos, QString text);

protected:
    void modelText(QPainter *painter, QString txt, TEXT_POS position);
	/// Draw a standard meteorlogical wind barb, representing the speed and direction of the
	/// wind. The barb points towards the direction the wind is coming from. Flags on the barb cumulatively add
	/// to the wind speed: 1/2 line is 5, a whole line is 10, and a triangle is 50. No assumption is made about the units of the
	/// wind speed.
    /// @param painter The painter to draw with.
    void windFlag(QPainter *painter);
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
    /// The wind direction in metorological coordinates:
    /// 0 is a wind from the north,
    /// 90 is a wind from the east, etc.
    double _dirMet;
    std::map<TEXT_POS, QString> _text;
    int _textOffset;
    double _aspectRatio;

};

#endif /* QSTATIONMODELITEM_H_ */
