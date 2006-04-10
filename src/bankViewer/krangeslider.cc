/* tnagy : tnagy@eleve.emn.fr */

#include "krangeslider.hh"
#include <qstyle.h>
#include <qpainter.h>
#include <qsize.h>
#include <qrect.h>
#include <qpixmap.h>
#include <qwidget.h>
#include <qpointarray.h> 

#include <iostream>
using namespace std;


// this will be useful once converted to a library
class krangesliderdata
{
	public:
		krangesliderdata() : rightToLeft(false) {};

		// TODO: if we use the widget from the right to the left
		bool rightToLeft;

		QPointArray upArrow;
		QPointArray downArrow;
		QPointArray leftArrow;
		QPointArray rightArrow;

		intpair realbounds;
		intpair realrange;
		intpair tmprange;

		// TODO : minimum range size
		int m_minDist;

		// rangeslider dimensions
		int scrollLength;
		int scrollWidth;
		int arrowDim;
};


	krangeslider::krangeslider(QWidget *parent, const char* name)
: QWidget( parent, name, Qt::WStyle_NoBorder | Qt::WStyle_Customize)
{
	orient = Qt::Horizontal;
	init();
}

	krangeslider::krangeslider(Orientation o, QWidget *parent, const char* name)
: QWidget( parent, name, Qt::WStyle_NoBorder | Qt::WStyle_Customize ), orient( o )
{
	init();
}


krangeslider::~krangeslider()
{
	if (d)
		delete d;
}

int krangeslider::toSlider(int val)
{
	if (val > length() + d->arrowDim)
		return length() + d->arrowDim;

	if (val < d->arrowDim+1)
		return d->arrowDim+1;

	return val;
}

void krangeslider::initArrows()
{
	d->upArrow.resize(0);

	d->downArrow.putPoints(0,3,  
			d->scrollWidth/5, (d->arrowDim*4)/5-2, 
			(d->scrollWidth*4)/5-1, (d->arrowDim*4)/5-2, 
			d->scrollWidth/2-1, d->arrowDim/5 );

	d->upArrow.putPoints(0,3,  
			d->scrollWidth/5, d->arrowDim/5+1, 
			(d->scrollWidth*4)/5-1, d->arrowDim/5+1, 
			d->scrollWidth/2-1, (d->arrowDim*4)/5-1);

	d->rightArrow.putPoints(0,3,  
			(d->arrowDim*4)/5-2, d->scrollWidth/5,
			(d->arrowDim*4)/5-2, (d->scrollWidth*4)/5-1,
			d->arrowDim/5 ,    d->scrollWidth/2-1 );

	d->leftArrow.putPoints(0,3,  
			d->arrowDim/5+1, d->scrollWidth/5,
			d->arrowDim/5+1, (d->scrollWidth*4)/5-1,
			(d->arrowDim*4)/5-1, d->scrollWidth/2-1 );
}

void krangeslider::init()
{
	d = new krangesliderdata();

	d->arrowDim = 15;
	//this->setStyle( "SGI" );
	//this->setStyle( "windows" );
	//this->setStyle( "CDE" );

	pressedControl = r_None;

	d->scrollWidth = style().pixelMetric(QStyle::PM_ScrollBarExtent, NULL);
	d->scrollLength = style().pixelMetric(QStyle::PM_ScrollBarSliderMin, NULL);
	d->scrollLength *= 2;


	d->m_minDist = 50;
	d->realbounds.setValues(0, 100);
	d->realrange.setValuesInPair(22, 73, d->realbounds);

	setBackgroundColor(QColor(245,245,245));
	setFocusPolicy( NoFocus );

	update();
	updateGeometry();

	if ( Qt::Vertical == orient )
	{
		setFixedWidth(d->scrollWidth);
		setMinimumHeight(d->scrollLength);
	}
	else
	{
		setMinimumWidth(d->scrollLength);
		setFixedHeight(d->scrollWidth);
	}


	QSizePolicy sp( QSizePolicy::Minimum, QSizePolicy::Fixed );
	if ( Qt::Vertical == orient )
		sp.transpose();
	setSizePolicy( sp );

	updateSliderFromRange();

	initArrows();
}


void krangeslider::paintEvent( QPaintEvent * )
{
	QSize tmpSize = size();
	QPixmap pm( tmpSize );
	QPainter p;

	p.begin(&pm);

	QRect r0, r1, r2, r3, r4;
	int inf = infVal();
	int sup = supVal();
	uint sliderstyle = QStyle::Style_Enabled;

	if (r_Slider == pressedControl)
		sliderstyle |= QStyle::Style_Down;

	if (Qt::Horizontal == orient)
	{
		sliderstyle |= QStyle::Style_Horizontal;

		r0.setRect(0, 0, tmpSize.width(), tmpSize.height());
		r2.setRect(inf, 0, sup - inf, d->scrollWidth);
		r3.setRect(0, 0, d->arrowDim, d->scrollWidth-1);
	}
	else
	{
		r0.setRect(0, 0, tmpSize.width(), tmpSize.height());
		r2.setRect(0, inf, d->scrollWidth, sup - inf);
		r3.setRect(0, 0, d->scrollWidth-1, d->arrowDim);
	}

	// paint the background
	p.setPen(Qt::SolidLine);
    p.setPen(QColor(100,100,100));
	p.setBrush(QColor(225,225,225));
	p.drawRect(r0);

	p.setPen(Qt::NoPen);
	// paint the slider
	style().drawPrimitive(QStyle::PE_ScrollBarSlider,&p, r2, colorGroup(),
			sliderstyle, QStyleOption::Default);

	p.setPen(Qt::SolidLine);

	// up/left arrow
	p.save();

	if (Qt::Horizontal == orient)
		p.translate(inf - d->arrowDim, 0);
	else
		p.translate(0, inf - d->arrowDim);

	p.setBrush(QWidget::paletteBackgroundColor());
	p.drawRect(r3);
	p.setBrush(QWidget::paletteForegroundColor());	

	if (Qt::Horizontal == orient)
		p.drawConvexPolygon(d->leftArrow,0,-1);
	else
		p.drawConvexPolygon(d->upArrow,0,-1);

	p.restore();


	// right/down arrow
	p.save();

	if (Qt::Horizontal == orient)
		p.translate(sup, 0);
	else
		p.translate(0, sup );

	p.setBrush(QWidget::paletteBackgroundColor());
	p.drawRect(r3);
	p.setBrush(QWidget::paletteForegroundColor());

	if (Qt::Horizontal == orient)
		p.drawConvexPolygon(d->rightArrow,0,-1);
	else
		p.drawConvexPolygon(d->downArrow,0,-1);


	p.restore();

	p.end();

	bitBlt(this, 0, 0, &pm);
}


QSize krangeslider::sizeHint() const
{
	if ( orient == Qt::Horizontal ) 
		return QSize( d->scrollWidth, d->scrollLength );
	else 
		return QSize( d->scrollLength, d->scrollWidth );
};

QSize krangeslider::minimumSizeHint() const
{
	QSize s = sizeHint();
	if ( orient == Qt::Horizontal )
		s.setWidth( 2 * d->arrowDim );
	else
		s.setHeight( 2 * d->arrowDim );
	return s;
}

void krangeslider::resizeEvent( QResizeEvent* )
{
	setBounds( d->arrowDim+1, length()+d->arrowDim );
	updateSliderFromRange();
	updateGeometry();
}

void krangeslider::hideEvent( QHideEvent* )
{
	pressedControl = r_None;
}

void krangeslider::mousePressEvent( QMouseEvent *e )
{
	//cout<<"press event"<<endl;
	if ( e->button() != LeftButton  )
		return;

	int range = ( Qt::Horizontal == orient)?e->x():e->y();
	if (range < infVal() - d->arrowDim || range > supVal() + d->arrowDim)
		return;

	if ( range < infVal() )
	{
		pressedControl = r_InfHandle;
	}
	else if ( range < supVal() )
	{
		pressedControl = r_Slider;
	}
	else
	{
		pressedControl = r_SupHandle;
	}

	position = e->pos();
	d->tmprange.setValues(infVal(), supVal());
	repaint(false);
}


void krangeslider::mouseReleaseEvent( QMouseEvent *e )
{
	//cout<<"release event"<<endl;
	if ( !pressedControl )
		return;

	mouseMoveEvent( e );  // Might have moved since last mouse move event.
	pressedControl = r_None;
	repaint(false);
}

void krangeslider::mouseMoveEvent( QMouseEvent *e )
{
	//cout<<"move event"<<endl;
	if ( !isVisible() ) 
	{
		pressedControl = r_None;
		return;
	}

	int newInf, newSup;
	int diff=0;

	if ( Qt::Horizontal == orient )
		diff = e->x() - position.x();
	else
		diff = e->y() - position.y();

	switch (pressedControl)
	{
		case r_InfHandle:	
			newInf = toSlider( d->tmprange.inf() + diff );
			setRange(toSlider(newInf), supVal());
			//cout<<"moving inf handle"<<endl;
			break;
		case r_SupHandle:
			newSup = toSlider( d->tmprange.sup() + diff );
			setRange(infVal(), toSlider(newSup));
			//cout<<"moving sup handle"<<endl;
			break;
		case r_Slider:
			setRange( d->tmprange.inf(), d->tmprange.sup() );
			translate( diff );
			//cout<<"moving slider"<<endl;
			break;
		default:
			break;
	}

	updateRange();
	emit rangeChanged( d->realrange.inf(), d->realrange.sup());
	repaint(false);
}

void krangeslider::updateRange()
{
	d->realrange.setValues( sliderToRange(infVal()), sliderToRange(supVal()) );
}

void krangeslider::updateSliderFromRange()
{
	setRange( rangeToSlider( d->realrange.inf() ), rangeToSlider( d->realrange.sup() ) );
}

void krangeslider::wheelEvent(QWheelEvent* e)
{
	e->ignore();
}

int krangeslider::length()
{
	int l = ( Qt::Horizontal == orient)?width():height();
	l = l - 2 * d->arrowDim;
	return (l>0)?l:1;
}

int krangeslider::sliderToRange(int val)
{
	if ( Qt::Vertical == orient && d->rightToLeft)
	{
		return d->realbounds.sup() - 
			((val - d->arrowDim) * (d->realbounds.sup() - d->realbounds.inf())) / length();
	}
	else
	{	
		return ((val - d->arrowDim) * (d->realbounds.sup() - d->realbounds.inf())) / length();
	}
}


int krangeslider::rangeToSlider(int val)
{
	if ( Qt::Vertical == orient && d->rightToLeft )
	{
		return height() - 
			((val * length()) / (d->realbounds.sup() - d->realbounds.inf()) + d->arrowDim);
	}
	else
	{
		return (val * length()) / (d->realbounds.sup() - d->realbounds.inf()) + d->arrowDim; 
	}
}

int krangeslider::range_min()
{
	return d->realbounds.inf();	
}

int krangeslider::range_max()
{
	return d->realbounds.sup();
}

int krangeslider::range_inf()
{
	return d->realrange.inf();
}

int krangeslider::range_sup()
{
	return d->realrange.inf();
}

int krangeslider::range_minDist()
{
	return d->m_minDist;
}

void krangeslider::initRange(int min, int max)
{
	d->realbounds.setValues(min, max);
	emit rangeChanged( d->realrange.inf(), d->realrange.sup());
}

void krangeslider::styleChange(QStyle &old)
{
	updateSliderFromRange();
	QWidget::styleChange( old );
	repaint(false);
}

