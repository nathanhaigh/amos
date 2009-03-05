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
QColor UIElements::color_ExpandedMate(QColor(30,170,255));
QColor UIElements::color_CompressedMate(QColor(255,255,30));
QColor UIElements::color_OrientationViolation(QColor(255,30,30));
QColor UIElements::color_LinkingMate(QColor(255,150,255));
QColor UIElements::color_SingletonMate(QColor(190,80,255));
QColor UIElements::color_NoMate(QColor(200,200,200));
QColor UIElements::color_Unknown(Qt::white);


QColor UIElements::color_tiling(180,180,180);
QColor UIElements::color_tilingoffset(100,100,100);
QColor UIElements::color_tilingtrim(204, 153, 153);

QColor UIElements::color_snpgap(Qt::cyan);


QColor UIElements::color_featdefault(QColor(200,200,200));
QColor UIElements::color_featrepeat(Qt::white);
QColor UIElements::color_featunitig(Qt::white);
QColor UIElements::color_featjoin(Qt::white);
QColor UIElements::color_featfix(Qt::white);
QColor UIElements::color_featorf(Qt::white);

QColor UIElements::color_featcoverage(QColor(0,140,221));
QColor UIElements::color_featsnp(QColor(255,30,30));
QColor UIElements::color_featbreak(QColor(255,150,150));
QColor UIElements::color_featkmer(QColor(255,255,0));
QColor UIElements::color_featmatepair(QColor(125,200,255));
QColor UIElements::color_featmisassembly(QColor(255,150,0));

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
    case AMOS::Feature_t::REPEAT:       return color_featrepeat;
    case AMOS::Feature_t::UNITIG:       return color_featunitig;
    case AMOS::Feature_t::JOIN:         return color_featjoin;
    case AMOS::Feature_t::FIX:          return color_featfix;
    case AMOS::Feature_t::ORF:          return color_featorf;
    case AMOS::Feature_t::COVERAGE:     return color_featcoverage;
    case AMOS::Feature_t::POLYMORPHISM: return color_featsnp;
    case AMOS::Feature_t::BREAKPOINT:   return color_featbreak;
    case AMOS::Feature_t::KMER:         return color_featkmer;
    case AMOS::Feature_t::MATEPAIR:     return color_featmatepair;
    case AMOS::Feature_t::MISASSEMBLY:  return color_featmisassembly;
  default: return color_featdefault;
  };
}

static const char * repeatstr = "Repeat";
static const char * unitigstr = "Unitig";
static const char * joinstr = "Join";
static const char * fixstr = "Fix";
static const char * orfstr = "Orf";
static const char * coveragestr = "Coverage";
static const char * polymorphismstr = "Polymorphism";
static const char * breakstr = "Breakpoint";
static const char * kmerstr = "Kmer";
static const char * matepairstr = "Matepair";
static const char * misassemblystr = "Misassembly";
static const char * unknownstr = "Unknown";

//const char * UIElements::allFeatureTypes = "UCPBX";

const char * UIElements::getFeatureStr(AMOS::FeatureType_t type)
{
  switch (type)
  {
    case AMOS::Feature_t::REPEAT:       return repeatstr;
    case AMOS::Feature_t::UNITIG:       return unitigstr;
    case AMOS::Feature_t::JOIN:         return joinstr;
    case AMOS::Feature_t::FIX:          return fixstr;
    case AMOS::Feature_t::ORF:          return orfstr;
    case AMOS::Feature_t::COVERAGE:     return coveragestr;
    case AMOS::Feature_t::POLYMORPHISM: return polymorphismstr;
    case AMOS::Feature_t::BREAKPOINT:   return breakstr;
    case AMOS::Feature_t::KMER:         return kmerstr;
    case AMOS::Feature_t::MATEPAIR:     return matepairstr;
    case AMOS::Feature_t::MISASSEMBLY:  return misassemblystr;
    case AMOS::Feature_t::NULL_FEATURE:
    default:                            return unknownstr;
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
    case Insert::CompressedMate:       return color_CompressedMate;
    case Insert::ExpandedMate:         return color_ExpandedMate;
    case Insert::SingletonMate:        return color_SingletonMate;
    case Insert::OrientationViolation: return color_OrientationViolation;
    case Insert::LinkingMate:          return color_LinkingMate;
    case Insert::NoMate:               return color_NoMate;

    default: return color_Unknown;
  };
}

