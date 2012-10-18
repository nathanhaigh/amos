////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 04/07/2006
//!
//! \brief A QScrollBar with an adjustable viewing range
//!
////////////////////////////////////////////////////////////////////////////////


#ifndef RANGESCROLLBAR_HH
#define RANGESCROLLBAR_HH

#include <qwidget.h>
#include <qtimer.h>
#include <qstyle.h>
#include <qimage.h>
//Added by qt3to4:
#include <QTimerEvent>
#include <QWheelEvent>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QHideEvent>
#include <QKeyEvent>
#include <QEvent>
//#include <qstyleoptionslider.h>

#include <algorithm>


//======================================================== RangeScrollBar_t ====
class RangeScrollBar_t : public QWidget
{

  Q_OBJECT

public:

  enum SliderAction {
    SliderNoAction,       //!< no action
    SliderSingleStepAdd,  //!< add single step action
    SliderSingleStepSub,  //!< subtract single step action
    SliderPageStepAdd,    //!< add page action
    SliderPageStepSub,    //!< subtract page action
    SliderToMinimum,      //!< goto minimum action
    SliderToMaximum       //!< goto maximum action
  };
  
  RangeScrollBar_t (QWidget * parent = NULL);
  RangeScrollBar_t (Qt::Orientation, QWidget * parent = NULL);
  ~RangeScrollBar_t();

  bool event (QEvent * event);
  int maximum() const;
  int minimum() const;
  Qt::Orientation orientation() const;
  int pageStep() const;
  void setMaximum (int max);
  void setMinimum (int min);
  void setPageStep (int page);
  void setSingleStep (int step);
  int singleStep() const;
  QSize sizeHint() const;
  void triggerAction (SliderAction action);
  int value() const;
  int length() const;
  int rangeLow() const;
  int rangeHigh() const;

  int controlPix_m;       //!< thickness of groove
  int gripPix_m;          //!< thickness of grip


public slots:

  void setMaxRange (int min, int max);
  void setOrientation (Qt::Orientation);
  void setRangeLow (int low);
  void setRangeHigh (int high);
  void setRange (int low, int high);
  void setValue (int value);


signals:

  void actionTriggered (int action);
  void maxRangeChanged (int min, int max);
  void rangeChanged (int low, int high);
  void valueChanged (int value);


protected:

  QRect highlightRect (int control);
  void setRepeatAction
    (SliderAction action,
     int thresholdTime = 500,
     int repeatTime = 50);
  void updateHover (const QPoint & pos);
  void updatePress (const QPoint & pos);
  void updateRects();
  int whichControl (const QPoint & pos);

  void changeEvent (QEvent *);
  void hideEvent (QHideEvent *);
  void keyPressEvent (QKeyEvent *);
  void mouseMoveEvent (QMouseEvent *);
  void mousePressEvent (QMouseEvent *);
  void mouseReleaseEvent (QMouseEvent *);
  void paintEvent (QPaintEvent *);
  void timerEvent (QTimerEvent *);
  void wheelEvent (QWheelEvent *);


private:

  void activateControl (int control, int threshold = 500);
  void init();
  int pix2val (int pos) const;
  int val2pix (int pos) const;


  QRect grooveRect_m;     //!< slider groove
  QRect subRect_m;        //!< subtract single step button
  QRect addRect_m;        //!< add single step button
  QRect lowRect_m;        //!< low range grip
  QRect highRect_m;       //!< high range grip
  QRect sliderRect_m;     //!< low grip + range grip + high grip
  QRect rangeRect_m;      //!< range grip (between low and high)


  int min_m;              //!< minimum value
  int max_m;              //!< maximum value
  int minPix_m;           //!< minimum pixel value
  int maxPix_m;           //!< maximum pixel value
  int low_m;              //!< low range value
  int high_m;             //!< high range value
  int singleStep_m;       //!< single step
  int pageStep_m;         //!< page step
  int pressedControl_m;   //!< currently pressed control
  int hoveredControl_m;   //!< currently hovered control
  int clickOffset_m;      //!< click offset from low range value

  Qt::Orientation orientation_m;

  QTimer repeatActionTimer_m;
  SliderAction repeatAction_m;
  int repeatActionTime_m;

  enum Control {
    SC_None,              //!< no control
    SC_AddPage,           //!< page add control
    SC_SubPage,           //!< page subtract control
    SC_AddStep,           //!< step add control
    SC_SubStep,           //!< step subtract control
    SC_FirstPage,         //!< goto first page control
    SC_LastPage,          //!< goto last page control
    SC_LowGrip,           //!< low range grip control
    SC_RangeGrip,         //!< range grip control
    SC_HighGrip           //!< high range grip control
  };

  static const QImage hBarImage_m;      //!< horizontal range grip image
  static const QImage vBarImage_m;      //!< vertical range grip image
  static const QImage hGrooveImage_m;   //!< horizontal groove image
  static const QImage vGrooveImage_m;   //!< vertical groove image
  static const QImage lArrowImage_m;    //!< left arrow image
  static const QImage rArrowImage_m;    //!< right arrow image
  static const QImage uArrowImage_m;    //!< up arrow image
  static const QImage dArrowImage_m;    //!< down arrow image
  static const QImage lGripImage_m;     //!< left grip image
  static const QImage rGripImage_m;     //!< right grip image
  static const QImage uGripImage_m;     //!< up grip image
  static const QImage dGripImage_m;     //!< down grip image
};



//================================================================= Inlines ====
//-------------------------------------------------------- RangeScrollBar_t ----
inline int RangeScrollBar_t::maximum() const
{ return max_m; }

inline int RangeScrollBar_t::minimum() const
{ return min_m; }

inline Qt::Orientation RangeScrollBar_t::orientation() const
{ return orientation_m; }

inline int RangeScrollBar_t::pageStep() const
{ return pageStep_m; }

inline void RangeScrollBar_t::setMaximum (int max)
{ setMaxRange (std::min (min_m, max), max); }

inline void RangeScrollBar_t::setMinimum (int min)
{ setMaxRange (min, std::max (max_m, min)); }

inline void RangeScrollBar_t::setPageStep (int page)
{ pageStep_m = abs (page); update(); }

inline void RangeScrollBar_t::setSingleStep (int step)
{ singleStep_m = abs (step); update(); }

inline void RangeScrollBar_t::setRangeLow (int low)
{ if ( low > high_m ) low = high_m; setRange (low, high_m); }

inline void RangeScrollBar_t::setRangeHigh (int high)
{ if ( high < low_m ) high = low_m; setRange (low_m, high); }

inline int RangeScrollBar_t::singleStep() const
{ return singleStep_m; }

inline int RangeScrollBar_t::value() const
{ return low_m; }

inline int RangeScrollBar_t::length() const
{ return high_m - low_m; }

inline int RangeScrollBar_t::rangeLow() const
{ return low_m; }

inline int RangeScrollBar_t::rangeHigh() const
{ return high_m; }

#endif // RANGESCROLLBAR_HH
