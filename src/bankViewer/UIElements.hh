#ifndef UIELEMENTS_HH_
#define UIELEMENTS_HH_ 1

#include <qpen.h>
#include <qcolor.h>


class UIElements
{
public:
  static void setBasePen(QPen & pen, char base);
  static QColor & getBaseColor(char base);

  static QColor & getInsertColor(char state);

  static QColor color_a;
  static QColor color_c;
  static QColor color_g;
  static QColor color_t;
  static QColor color_default;

  static QColor color_discrepancy;

  static QColor color_happy;
  static QColor color_sad;
  static QColor color_unknown;
  static QColor color_link;
  static QColor color_orientation;
};


#endif
