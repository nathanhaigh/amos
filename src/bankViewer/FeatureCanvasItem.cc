#include "FeatureCanvasItem.hh"
#include "UIElements.hh"
#include <qpainter.h>
#include "foundation_AMOS.hh"

const int FeatureCanvasItem::RTTI = 132548;

FeatureCanvasItem::FeatureCanvasItem(int x, int y, int width, int height,
                                     AMOS::Feature_t & feat, QCanvas * canvas)
  : QCanvasRectangle(x, y, width, height, canvas), m_feat(feat)
{
  setPen(UIElements::getFeatureColor(feat.getType()));
  setBrush(UIElements::getFeatureColor(feat.getType()));
}

void FeatureCanvasItem::drawShape(QPainter & p)
{
  if (m_feat.getType() == 'B')
  {
    p.drawLine((int) x(),                 (int) y(),
               (int) (x() + width() - 1), (int) y());

    if (m_feat.getRange().isReverse())
    {
      p.drawRect((int) (x() + width() - width()/2),   (int) y(),
                 (int) (width()/2),                   (int) height());
    }
    else
    {
      p.drawRect((int) x(),                 (int) y(),
                 (int) (width()/2),         (int) height());
    }
  }
  else
  {
    p.drawRect((int) x(),      (int) y(),
               (int) width(),  (int) height());
  }
}


static const char * repeatstr = "Repeat";
static const char * unitigstr = "Unitig";
static const char * joinstr = "Join";
static const char * coveragestr = "Coverage";
static const char * orfstr = "Orf";
static const char * polymorphismstr = "Polymorphism";
static const char * breakstr = "Breakpoint";
static const char * unknownstr = "Unknown";

const char * FeatureCanvasItem::getFeatureTypeStr (AMOS::FeatureType_t type)
{
  switch (type)
    {
    case AMOS::Feature_t::REPEAT:       return repeatstr;
    case AMOS::Feature_t::UNITIG:       return unitigstr;
    case AMOS::Feature_t::JOIN:         return joinstr;
    case AMOS::Feature_t::COVERAGE:     return coveragestr;
    case AMOS::Feature_t::ORF:          return orfstr;
    case AMOS::Feature_t::POLYMORPHISM: return polymorphismstr;
    case AMOS::Feature_t::BREAKPOINT:   return breakstr;
    case AMOS::Feature_t::NULL_FEATURE:
    default:                            return unknownstr;
    }
}
