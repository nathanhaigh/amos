#ifndef UIELEMENTS_HH_
#define UIELEMENTS_HH_ 1

#include <qpen.h>
#include <qcolor.h>


class UIElements
{
public:
  static void setBasePen(QPen & pen, char base);
  static const QColor & getBaseColor(char base);

  static QColor color_a;
  static QColor color_c;
  static QColor color_g;
  static QColor color_t;
  static QColor color_default;

  static QColor color_discrepancy;
};


#endif
