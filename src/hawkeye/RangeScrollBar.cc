///////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 04/07/2006
//!
//! \brief A QScrollBar with an adjustable viewing range
//!
////////////////////////////////////////////////////////////////////////////////

//#include <QtGui>
//#include "QtUtilities.hh"

#include "RangeScrollBar.hh"
#include <qpainter.h>
#include <qcursor.h>
//Added by qt3to4:
#include <QTimerEvent>
#include <QWheelEvent>
#include <QPaintEvent>
#include <QPixmap>
#include <QMouseEvent>
#include <QHideEvent>
#include <QKeyEvent>
#include <QEvent>
#include <iostream>

#include "icons/rangebar_horiz.xpm"
#include "icons/rangebar_vert.xpm"
#include "icons/rangegroove_horiz.xpm"
#include "icons/rangegroove_vert.xpm"
#include "icons/rangearrow_left.xpm"
#include "icons/rangearrow_right.xpm"
#include "icons/rangearrow_up.xpm"
#include "icons/rangearrow_down.xpm"
#include "icons/rangegrip_left.xpm"
#include "icons/rangegrip_right.xpm"
#include "icons/rangegrip_up.xpm"
#include "icons/rangegrip_down.xpm"




//======================================================== RangeScrollBar_t ====
//-- Load in the xpms for the RangeScrollBar
const QImage RangeScrollBar_t::hBarImage_m   (rangebar_horiz);
const QImage RangeScrollBar_t::vBarImage_m   (rangebar_vert);
const QImage RangeScrollBar_t::hGrooveImage_m(rangegroove_horiz);
const QImage RangeScrollBar_t::vGrooveImage_m(rangegroove_vert);
const QImage RangeScrollBar_t::lArrowImage_m (rangearrow_left);
const QImage RangeScrollBar_t::rArrowImage_m (rangearrow_right);
const QImage RangeScrollBar_t::uArrowImage_m (rangearrow_up);
const QImage RangeScrollBar_t::dArrowImage_m (rangearrow_down);
const QImage RangeScrollBar_t::lGripImage_m  (rangegrip_left);
const QImage RangeScrollBar_t::rGripImage_m  (rangegrip_right);
const QImage RangeScrollBar_t::uGripImage_m  (rangegrip_up);
const QImage RangeScrollBar_t::dGripImage_m  (rangegrip_down);

//-- Shorthand
#define IS_HORIZ (orientation_m == Qt::Horizontal)
#define IS_VERT !IS_HORIZ


inline QRect Shrunk (const QRect & r, int v)
{ 
  QRect n(r);
  n.addCoords (v,v,-v,-v); 
  return n;
}



//-------------------------------------------------------- RangeScrollBar_t ----
RangeScrollBar_t::RangeScrollBar_t (QWidget * parent)
  : QWidget (parent)
{
  orientation_m = Qt::Horizontal;
  init();
}


//-------------------------------------------------------- RangeScrollBar_t ----
RangeScrollBar_t::RangeScrollBar_t
(Qt::Orientation orientation, QWidget * parent)
  : QWidget (parent)
{
  orientation_m = orientation;
  init();
}


//------------------------------------------------------- ~RangeScrollBar_t ----
RangeScrollBar_t::~RangeScrollBar_t()
{
}


//--------------------------------------------------------- activateControl ----
void RangeScrollBar_t::activateControl (int control, int threshold)
{
  SliderAction action = SliderNoAction;
  switch ( control )
    {
    case SC_AddPage:
      action = SliderPageStepAdd;
      break;
    case SC_SubPage:
      action = SliderPageStepSub;
      break;
    case SC_AddStep:
      action = SliderSingleStepAdd;
      break;
    case SC_SubStep:
      action = SliderSingleStepSub;
      break;
    case SC_FirstPage:
      action = SliderToMinimum;
      break;
    case SC_LastPage:
      action = SliderToMaximum;
      break;
    default:
      break;
    }

  if ( action )
    {
      triggerAction (action);
      setRepeatAction (action, threshold);
    }
}


//------------------------------------------------------------- changeEvent ----
void RangeScrollBar_t::changeEvent (QEvent * e)
{
  //-- Stop the repeat if widget becomes disabled
 // if ( e->type() == QEvent::EnabledChange && !isEnabled() )
  if ( e->type() == QEvent::EnabledChange && !isEnabled() )
    repeatActionTimer_m.stop();

  QWidget::changeEvent (e);
}


//------------------------------------------------------------------- event ----
bool RangeScrollBar_t::event (QEvent * e)
{
  switch( e->type() )
    {
      //-- Capture mouse hover events
    /*
    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
    case QEvent::HoverMove:
      if ( const QHoverEvent * he = static_cast<const QHoverEvent *>(e) )
        updateHover (he->pos());
      break;
      */
    default:
      break;
    }
  return QWidget::event (e);
}


//--------------------------------------------------------------- hideEvent ----
void RangeScrollBar_t::hideEvent (QHideEvent *)
{
  //-- Stop the repeat if widget becomes hidden
  if ( pressedControl_m )
    {
      pressedControl_m = SC_None;
      setRepeatAction (SliderNoAction);
    }
}


//----------------------------------------------------------- highlightRect ----
QRect RangeScrollBar_t::highlightRect (int control)
{
  //-- Return the highlight rect for a given control
  switch ( control )
    {
    case SC_RangeGrip:
    case SC_LowGrip:
    case SC_HighGrip:
      return Shrunk (sliderRect_m, 1);

    case SC_AddStep:
      return Shrunk (addRect_m, 1);

    case SC_SubStep:
      return Shrunk (subRect_m, 1);

    default:
      return QRect();
    }
}


//-------------------------------------------------------------------- init ----
void RangeScrollBar_t::init()
{
  setWindowFlags(Qt::WRepaintNoErase | Qt::WResizeNoErase);
  setBackgroundMode(Qt::NoBackground); // for some reason this is necessary on qt 3.3.6

  //-- Default pixel size
  controlPix_m = 15;
  gripPix_m = 5;

  //-- Default values
  min_m = 0;
  max_m = 99;
  low_m = 0;
  high_m = 10;
  singleStep_m = 1;
  pageStep_m = 10;

  pressedControl_m = SC_None;
  hoveredControl_m = SC_None;

  repeatAction_m = SliderNoAction;

  setMinimumSize (sizeHint());
  setMouseTracking(true);

  if ( IS_HORIZ )
    setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Fixed);
  else
    setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Minimum);

  //setAttribute (Qt::WA_Hover, true);
  //setFocusPolicy (Qt::NoFocus);
}


//----------------------------------------------------------- keyPressEvent ----
void RangeScrollBar_t::keyPressEvent (QKeyEvent * e)
{
  SliderAction action = SliderNoAction;

  //-- Tie key presses to their actions
  switch ( e->key() )
    {
    case Qt::Key_Left:
    case Qt::Key_Up:
      action = SliderSingleStepSub;
      break;

    case Qt::Key_Right:
    case Qt::Key_Down:
      action = SliderSingleStepAdd;
      break;

    case Qt::Key_PageUp:
      action = SliderPageStepSub;
      break;

    case Qt::Key_PageDown:
      action = SliderPageStepAdd;
      break;

    case Qt::Key_Home:
      action = SliderToMinimum;
      break;

    case Qt::Key_End:
      action = SliderToMaximum;
      break;

    default:
      e->ignore();
      break;
    }

  if ( action )
    triggerAction (action);
}


//---------------------------------------------------------- mouseMoveEvent ----
void RangeScrollBar_t::mouseMoveEvent (QMouseEvent * e)
{
  updateHover (e->pos());

  if ( !pressedControl_m)
    return;

  int pos = IS_HORIZ ? e->pos().x() : e->pos().y();

  //-- Range adjustment via mouse dragging
  switch ( pressedControl_m )
    {
    case SC_RangeGrip:
      setValue (pix2val (pos - clickOffset_m));
      break;
    case SC_LowGrip:
      setRangeLow (pix2val (pos - clickOffset_m));
      break;
    case SC_HighGrip:
      setRangeHigh (pix2val (pos - clickOffset_m));
      break;
    }

  repaint(); // can't call update, or the repaint events will just queue up 
}


//--------------------------------------------------------- mousePressEvent ----
void RangeScrollBar_t::mousePressEvent (QMouseEvent * e)
{
  if ( min_m == max_m
       || (e->button() & (~e->button()))
       || !(e->button() == Qt::LeftButton || e->button() == Qt::MidButton) )
    return;

  updatePress (e->pos());
  int pos = IS_HORIZ ? e->pos().x() : e->pos().y();

  //-- Middle button goto
  if ( e->button() == Qt::MidButton &&
       (pressedControl_m == SC_RangeGrip
        || pressedControl_m == SC_SubPage
        || pressedControl_m == SC_AddPage) )
    {
      int half = (IS_HORIZ ? rangeRect_m.width() : rangeRect_m.height()) / 2;
      setValue (pix2val (pos - half));
      pressedControl_m = SC_RangeGrip;
      clickOffset_m = half;
    }
  //-- Range adjustments
  else if ( pressedControl_m == SC_RangeGrip )
    clickOffset_m =
      IS_HORIZ ? pos - rangeRect_m.x() : pos - rangeRect_m.y();
  else if ( pressedControl_m == SC_LowGrip )
    clickOffset_m =
      IS_HORIZ ? pos - rangeRect_m.x() : pos - rangeRect_m.y();
  else if ( pressedControl_m == SC_HighGrip )
    clickOffset_m =
      IS_HORIZ ? pos - rangeRect_m.right() : pos - rangeRect_m.bottom();

  activateControl (pressedControl_m);
}


//------------------------------------------------------- mouseReleaseEvent ----
void RangeScrollBar_t::mouseReleaseEvent (QMouseEvent * e)
{
//  if ( !pressedControl_m
//       || e->button() && (~e->button()) )
//    return;

  setRepeatAction (SliderNoAction);
  pressedControl_m = SC_None;
  updateHover (e->pos());
}

//-------------------------------------------------------------- paintEvent ----
void RangeScrollBar_t::paintEvent (QPaintEvent *)
{
  QPixmap pix(width(), height());
  pix.fill(this,0,0);
  QPainter p (&pix);

  updateRects(); // update the control boundaries

  if ( IS_HORIZ )
    {
      p.drawImage (grooveRect_m, hGrooveImage_m);
      p.drawImage (subRect_m, lArrowImage_m);
      p.drawImage (addRect_m, rArrowImage_m);
      p.drawImage (lowRect_m, lGripImage_m);
      p.drawImage (rangeRect_m, hBarImage_m);
      p.drawImage (highRect_m, rGripImage_m);
    }
  else
    {
      p.drawImage (grooveRect_m, vGrooveImage_m);
      p.drawImage (lowRect_m, uGripImage_m);
      p.drawImage (rangeRect_m, vBarImage_m);
      p.drawImage (highRect_m, dGripImage_m);
      p.drawImage (subRect_m, uArrowImage_m);
      p.drawImage (addRect_m, dArrowImage_m);
    }

  /*
  //-- Highlight the hovered control
  if ( hoveredControl_m )
    {
      p.setPen (Qt::NoPen);
      p.setBrush (QColor (0, 0, 0)); //, 50));
      p.drawRect (highlightRect (hoveredControl_m));
    }
    */

  p.end();

  p.begin(this);
  p.drawPixmap(0,0,pix);
  p.end();
}


//---------------------------------------------------------- setOrientation ----
void RangeScrollBar_t::setOrientation (Qt::Orientation orientation)
{
  if ( orientation_m == orientation )
    return;

  orientation_m = orientation;
  QSizePolicy sp = sizePolicy();
  sp.transpose();
  setSizePolicy (sp);

  update();
  updateGeometry();
}


//------------------------------------------------------------- setMaxRange ----
void RangeScrollBar_t::setMaxRange (int min, int max)
{
  if ( min > max )
    std::swap (min, max);

  if ( min == min_m && max == max_m )
    return;

  min_m = min;
  max_m = max;

  emit maxRangeChanged (min_m, max_m);
  setRange (low_m, high_m);  // re-bound

  update();
}


//---------------------------------------------------------------- setRange ----
void RangeScrollBar_t::setRange (int low, int high)
{
  //-- Keep the range within bounds
  low = std::max (min_m, std::min (max_m, low));
  high = std::max (min_m, std::min (max_m, high));

  if ( high < low )
    std::swap (low, high);

  if ( low == low_m && high == high_m )
    return;

  if (low == high)
   return;

  low_m = low;
  high_m = high;

  emit rangeChanged (low_m, high_m);
  QRect oldSlider (sliderRect_m);
  updateRects();
  update (sliderRect_m | oldSlider);
}


//--------------------------------------------------------- setRepeatAction ----
void RangeScrollBar_t::setRepeatAction
  (SliderAction action,
   int thresholdTime,
   int repeatTime)
{
  //-- Set up a repeating action
  repeatAction_m = action;
  if ( action == SliderNoAction )
    repeatActionTimer_m.stop();
  else
    {
      repeatActionTime_m = repeatTime;
      repeatActionTimer_m.start (thresholdTime, this);
    }
}


//---------------------------------------------------------------- setValue ----
void RangeScrollBar_t::setValue (int low)
{
  //-- Keep the range within bounds
  int width = high_m - low_m;
  if ( low < min_m )
    low = min_m;
  if ( low + width > max_m )
    low = max_m - width;

  if ( low == low_m )
    return;

  low_m = low;
  high_m = low + width;

  emit valueChanged (low_m);
  QRect oldSlider (sliderRect_m);
  updateRects();
  update (sliderRect_m | oldSlider);
}


//---------------------------------------------------------------- sizeHint ----
QSize RangeScrollBar_t::sizeHint() const
{
    //ensurePolished();

    //-- Two page step controls and two range grips
    QSize size (controlPix_m * 2 + gripPix_m * 2, controlPix_m);

    if ( orientation_m == Qt::Vertical )
      size.transpose();

    return size;
}


//-------------------------------------------------------------- timerEvent ----
void RangeScrollBar_t::timerEvent (QTimerEvent * e)
{
  if ( e->timerId() == repeatActionTimer_m.timerId() )
    {
      if ( repeatActionTime_m )
        {
          repeatActionTimer_m.start (repeatActionTime_m, this);
          repeatActionTime_m = 0;
        }
      triggerAction (repeatAction_m);
    }
}


//----------------------------------------------------------- triggerAction ----
void RangeScrollBar_t::triggerAction (SliderAction action)
{
  switch ( action )
    {
    case SliderSingleStepAdd:
      setValue (low_m + singleStep_m);
      break;
    case SliderSingleStepSub:
      setValue (low_m - singleStep_m);
      break;
    case SliderPageStepAdd:
      setValue (low_m + pageStep_m);
      break;
    case SliderPageStepSub:
      setValue (low_m - pageStep_m);
      break;
    case SliderToMinimum:
      setValue (min_m);
      break;
    case SliderToMaximum:
      setValue (max_m);
    case SliderNoAction:
      break;
    };

    emit actionTriggered (action);
}


//------------------------------------------------------------- updateHover ----
void RangeScrollBar_t::updateHover (const QPoint & pos)
{
  //-- The mouse is at pos, update hover info

  if ( pressedControl_m )
    return;
  
  QRect old (highlightRect (hoveredControl_m));

  hoveredControl_m = whichControl (pos);

  if ( hoveredControl_m == SC_LowGrip || hoveredControl_m == SC_HighGrip )
    {
      if ( IS_HORIZ ) setCursor (QCursor(Qt::SizeHorCursor));
      else            setCursor (QCursor(Qt::SizeVerCursor));
    }
  else
    unsetCursor();

  //update (old | highlightRect (hoveredControl_m));
}


//------------------------------------------------------------- updatePress ----
void RangeScrollBar_t::updatePress (const QPoint & pos)
{
  //-- The mouse was pressed at pos, update press info

  pressedControl_m = whichControl (pos);
}


//------------------------------------------------------------- updateRects ----
void RangeScrollBar_t::updateRects()
{
  //-- Update the control boundaries
  grooveRect_m =
    addRect_m =
    subRect_m =
    lowRect_m =
    highRect_m =
    rangeRect_m = rect();

  if ( IS_HORIZ )
    {
      //-- Groove is everything but the page step controls
     // grooveRect_m.adjust (controlPix_m - 1, 0, - controlPix_m + 1, 0);
      grooveRect_m.addCoords (controlPix_m - 1, 0, - controlPix_m + 1, 0);

      //-- Minimum and maximum pixel values, groove - grips
      minPix_m = grooveRect_m.left() + gripPix_m;
      maxPix_m = grooveRect_m.right() - gripPix_m;

      //-- Range bar, scroll bar between the range grips
      rangeRect_m.setLeft (val2pix (low_m));
      rangeRect_m.setRight (val2pix (high_m));

      //-- Low value range grip
      lowRect_m.setLeft (rangeRect_m.left() - gripPix_m);
      lowRect_m.setRight (rangeRect_m.left() - 1);

      //-- High value range grip
      highRect_m.setRight (rangeRect_m.right() + gripPix_m);
      highRect_m.setLeft (rangeRect_m.right() + 1);

      //-- Page step controls
      addRect_m.setLeft (grooveRect_m.right());
      subRect_m.setRight (grooveRect_m.left());
    }
  else
    {
      //-- Same as above, but left/right replaced with top/bottom
      //grooveRect_m.adjust (0, controlPix_m - 1, 0, - controlPix_m + 1);
      grooveRect_m.addCoords (0, controlPix_m - 1, 0, - controlPix_m + 1);

      minPix_m = grooveRect_m.top() + gripPix_m;
      maxPix_m = grooveRect_m.bottom() - gripPix_m;
      rangeRect_m.setTop (val2pix (low_m));
      rangeRect_m.setBottom (val2pix (high_m));
      lowRect_m.setTop (rangeRect_m.top() - gripPix_m);
      lowRect_m.setBottom (rangeRect_m.top() - 1);
      highRect_m.setBottom (rangeRect_m.bottom() + gripPix_m);
      highRect_m.setTop (rangeRect_m.bottom() + 1);
      addRect_m.setTop (grooveRect_m.bottom());
      subRect_m.setBottom (grooveRect_m.top());
    }

  //-- Slider rect is the union of both range grips and the range bar
  sliderRect_m = lowRect_m | rangeRect_m | highRect_m;
}


//----------------------------------------------------------------- pix2val ----
int RangeScrollBar_t::pix2val (int pos) const
{
  //-- Convert from a pixel value to a scroll value
  double frac = (double)(pos - minPix_m) / (maxPix_m - minPix_m);
  return qRound (frac * (max_m - min_m)) + min_m;
}


//----------------------------------------------------------------- val2pix ----
int RangeScrollBar_t::val2pix (int pos) const
{
  //-- Convert from a scroll value to a pixel value
  double frac = (double)(pos - min_m) / (max_m - min_m);
  return qRound (frac * (maxPix_m - minPix_m) + minPix_m);
}


//-------------------------------------------------------------- wheelEvent ----
void RangeScrollBar_t::wheelEvent (QWheelEvent * e)
{
  //-- Turn a mouse wheel event into a scroll event
  if ( e->orientation() != orientation_m && !rect().contains(e->pos()) )
    return;

  static double offset = 0;
  static RangeScrollBar_t * offset_owner = NULL;

  if ( offset_owner != this )
    {
      offset_owner = this;
      offset = 0;
    }
  
  //int step = std::min (QApplication::wheelScrollLines() * singleStep_m, pageStep_m);
  int step = std::min (6 * singleStep_m, pageStep_m);

  /*
  if ( (e->modifiers() & Qt::ControlModifier) ||
       (e->modifiers() & Qt::ShiftModifier) )
    step = pageStep_m;
    */

  offset += e->delta() * step / 120;
  offset = -offset;

  if ( offset > -1 && offset < 1 )
    return;

  setValue (low_m + (int)offset);
  offset -= int(offset);
  e->accept();
}


//------------------------------------------------------------ whichControl ----
int RangeScrollBar_t::whichControl (const QPoint & pos)
{
  //-- Return the control containing the point
  if ( rangeRect_m.contains (pos) )
    return SC_RangeGrip;
  else if ( lowRect_m.contains (pos) )
    return SC_LowGrip;
  else if ( highRect_m.contains (pos) )
    return SC_HighGrip;
  else if ( subRect_m.contains (pos) )
    return SC_SubStep;
  else if ( addRect_m.contains (pos) )
    return SC_AddStep;
  else if ( grooveRect_m.contains (pos) )
    return IS_HORIZ ?
      ( pos.x() < sliderRect_m.x() ? SC_SubPage : SC_AddPage ) :
      ( pos.y() < sliderRect_m.y() ? SC_SubPage : SC_AddPage );
  else
    return SC_None;
}
