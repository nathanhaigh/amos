#include "UIElements.hh"

QColor UIElements::color_a(Qt::darkGreen);
QColor UIElements::color_c(Qt::blue);
QColor UIElements::color_g(Qt::yellow);
QColor UIElements::color_t(Qt::red);
QColor UIElements::color_default(Qt::black);
QColor UIElements::color_discrepancy(153, 102, 255);

QColor UIElements::color_Happy(Qt::green);
QColor UIElements::color_StretchedMate(Qt::blue);
QColor UIElements::color_CompressedMate(QColor(255,165,0));
QColor UIElements::color_SingletonMate(Qt::yellow);
QColor UIElements::color_OrientationViolation(Qt::red);
QColor UIElements::color_MissingMate(Qt::magenta);
QColor UIElements::color_NoMate(Qt::cyan);
QColor UIElements::color_Unknown(Qt::white);

QColor UIElements::color_tiling(180,180,180);
QColor UIElements::color_tilingoffset(100,100,100);
QColor UIElements::color_tilingtrim(204, 153, 153);

QColor UIElements::color_snpgap(Qt::cyan);

QColor UIElements::color_featrepeat(Qt::white);
QColor UIElements::color_featjoin(Qt::white);
QColor UIElements::color_featorf(Qt::white);
QColor UIElements::color_featdefault(Qt::white);

QColor UIElements::color_featunitig(Qt::cyan);
QColor UIElements::color_featcoverage(Qt::magenta);
QColor UIElements::color_featsnp(Qt::red);

QColor UIElements::color_insertcoverage(Qt::magenta);
QColor UIElements::color_readcoverage(Qt::green);


QColor & UIElements::getSNPColoring(char base, bool dark)
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

    case ' ': return color_tilingoffset;
    case '-': return color_snpgap;

    default: return color_default;
  };
}


QColor & UIElements::getFeatureColor(AMOS::FeatureType_t type)
{
  switch (type)
  {
    case 'R':  return color_featrepeat;
    case 'U':  return color_featunitig;
    case 'J':  return color_featjoin;
    case 'C':  return color_featcoverage;
    case 'O':  return color_featorf;
    case 'P':  return color_featsnp;
    default: return color_featdefault;
  };
}

static const char * repeatstr = "Repeat";
static const char * unitigstr = "Unitig";
static const char * joinstr = "Join";
static const char * coveragestr = "Coverage";
static const char * orfstr = "ORF";
static const char * snpstr = "SNP";
static const char * otherstr = "Other";

const char * UIElements::allFeatureTypes = "UCPX";

const char * UIElements::getFeatureStr(AMOS::FeatureType_t type)
{
  switch (type)
  {
    case 'R':  return repeatstr;
    case 'U':  return unitigstr;
    case 'J':  return joinstr;
    case 'C':  return coveragestr;
    case 'O':  return orfstr;
    case 'P':  return snpstr;
    default:   return otherstr;
  };
}


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


QColor & UIElements::getInsertColor(Insert::MateState state)
{
  switch (state)
  {
    case Insert::Happy:                return color_Happy;
    case Insert::StretchedMate:        return color_StretchedMate;
    case Insert::CompressedMate:       return color_CompressedMate;
    case Insert::SingletonMate:        return color_SingletonMate;
    case Insert::OrientationViolation: return color_OrientationViolation;
    case Insert::MissingMate:          return color_MissingMate;
    case Insert::NoMate:               return color_NoMate;

    default: return color_Unknown;
  };
}

