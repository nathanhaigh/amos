#ifndef UIELEMENTS_HH_
#define UIELEMENTS_HH_ 1

#include <qpen.h>
#include <qcolor.h>

#include "Insert.hh"


class UIElements
{
public:
  static void setBasePen(QPen & pen, char base);
  static QColor & getBaseColor(char base);

  static QColor color_tiling;
  static QColor color_tilingoffset;

  static QColor color_a;
  static QColor color_c;
  static QColor color_g;
  static QColor color_t;
  static QColor color_default;

  static QColor color_discrepancy;

  static QColor & getInsertColor(Insert::MateState state);
  static QColor color_Happy;
  static QColor color_Unknown;
  static QColor color_SizeViolation;
  static QColor color_LinkingMate;
  static QColor color_OrientationViolation;
  static QColor color_MissingMate;
  static QColor color_NoMate;
};


#endif
