#include "UIElements.hh"

QColor UIElements::color_a(Qt::darkGreen);
QColor UIElements::color_c(Qt::blue);
QColor UIElements::color_g(Qt::yellow);
QColor UIElements::color_t(Qt::red);
QColor UIElements::color_default(Qt::black);
QColor UIElements::color_discrepancy(153, 102, 255);

QColor UIElements::color_happy(Qt::green);
QColor UIElements::color_sad(Qt::blue);
QColor UIElements::color_unknown(Qt::magenta);
QColor UIElements::color_link(Qt::yellow);
QColor UIElements::color_orientation(Qt::red);

QColor & UIElements::getBaseColor(char base)
{
  switch (base)
  {
    case 'A':
    case 'a': return color_a;

    case 'C':
    case 'c': return color_c;
    
    case 'G':
    case 'g': return color_g;

    case 'T':
    case 't': return color_t;

    default: return color_default;
  };
}

void UIElements::setBasePen(QPen & pen, char base)
{
  pen.setColor(getBaseColor(base));
}


QColor & UIElements::getInsertColor(char state)
{
  switch (state)
  {
    case 'H': return color_happy;
    case 'S': return color_sad;
    case 'L': return color_link;
    case 'O': return color_orientation;

    default: return color_unknown;
  };
}
