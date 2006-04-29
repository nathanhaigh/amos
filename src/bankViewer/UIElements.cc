#include "UIElements.hh"

QColor UIElements::color_a(Qt::darkGreen);
QColor UIElements::color_c(Qt::blue);
QColor UIElements::color_g(Qt::yellow);
QColor UIElements::color_t(Qt::red);
QColor UIElements::color_default(Qt::black);
QColor UIElements::color_discrepancy(153, 102, 255);

QColor UIElements::color_FContig(QColor(170,170,255));
QColor UIElements::color_RContig(QColor(100,100,255));
QColor UIElements::color_Scaffold(UIElements::color_FContig);

QColor UIElements::color_Happy(QColor(120,255,120));
QColor UIElements::color_StretchedMate(QColor(30,170,255));
QColor UIElements::color_CompressedMate(QColor(255,255,30));
QColor UIElements::color_OrientationViolation(QColor(255,30,30));
QColor UIElements::color_MissingMate(QColor(255,150,255));
QColor UIElements::color_SingletonMate(QColor(190,80,255));
QColor UIElements::color_NoMate(QColor(200,200,200));
QColor UIElements::color_Unknown(Qt::white);


QColor UIElements::color_tiling(180,180,180);
QColor UIElements::color_tilingoffset(100,100,100);
QColor UIElements::color_tilingtrim(204, 153, 153);

QColor UIElements::color_snpgap(Qt::cyan);


QColor UIElements::color_featdefault(Qt::white);
QColor UIElements::color_featrepeat(Qt::white);
QColor UIElements::color_featjoin(Qt::white);
QColor UIElements::color_featorf(Qt::white);

QColor UIElements::color_featsnp(QColor(255,30,30));
QColor UIElements::color_featbreak(QColor(255,150,150));
QColor UIElements::color_featunitig(QColor(125,200,255));
QColor UIElements::color_featcoverage(QColor(0,140,221));
QColor UIElements::color_insertcoverage(QColor(255,150,255));
QColor UIElements::color_readcoverage(QColor(120,255,120));


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
    case 'B':  return color_featbreak;
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
static const char * breakstr = "Breakpoint";

const char * UIElements::allFeatureTypes = "UCPBX";

const char * UIElements::getFeatureStr(AMOS::FeatureType_t type)
{
  switch (type)
  {
    case 'B':  return breakstr;
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

