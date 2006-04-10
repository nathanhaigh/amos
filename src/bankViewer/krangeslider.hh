/* tnagy : tnagy@eleve.emn.fr */

#ifndef _VBAR_
#define _VBAR_

#include "birangecontrol.hh"
#include <qwidget.h>

class krangesliderdata;


class krangeslider : public QWidget, private biRangeControl
{
	Q_OBJECT
	public:
		enum control{r_None, r_InfHandle, r_SupHandle, r_Slider};

		krangeslider(QWidget *parent, const char* name = 0);
		krangeslider(Orientation o, QWidget *parent, const char* name = 0 );
		~krangeslider();

		QSize sizeHint() const;
		QSize minimumSizeHint() const;

		void initRange(int, int);
		
		void styleChange( QStyle& );
	
		// min < inf < sup < max
		int range_min();
		int range_max();
		int range_inf();
		int range_sup();
		int range_minDist();

	signals:
		void rangeChanged(int, int);

	protected:
		void paintEvent( QPaintEvent * );
		void resizeEvent( QResizeEvent * );
		void hideEvent( QHideEvent* );

		void mousePressEvent( QMouseEvent * );
		void mouseReleaseEvent( QMouseEvent * );
		void mouseMoveEvent( QMouseEvent * );
		void wheelEvent( QWheelEvent * );
	
	private:
		Orientation orient;

		krangesliderdata *d;
		
		int toSlider(int);
		void updateRange();
		void updateSliderFromRange();

		int sliderToRange(int);
		int rangeToSlider(int);

		int length();
		void init();
		void initArrows();


		uint pressedControl;
		QPoint position;
};

#endif




