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
  static QColor & getSNPColoring(char base, bool dark=false);
  static QColor & getInsertColor(Insert::MateState state);
  static QColor & getFeatureColor(AMOS::FeatureType_t type);

  static QColor color_tiling;
  static QColor color_tilingoffset;
  static QColor color_tilingtrim;

  static QColor color_a;
  static QColor color_c;
  static QColor color_g;
  static QColor color_t;
  static QColor color_default;

  static QColor color_discrepancy;
  static QColor color_snpgap;

  static QColor color_FContig;
  static QColor color_RContig;
  static QColor color_Scaffold;

  static QColor color_Happy;
  static QColor color_Unknown;
  static QColor color_ExpandedMate;
  static QColor color_CompressedMate;
  static QColor color_SingletonMate;
  static QColor color_OrientationViolation;
  static QColor color_LinkingMate;
  static QColor color_NoMate;


static QColor color_featdefault;
static QColor color_featrepeat;
static QColor color_featunitig;
static QColor color_featjoin;
static QColor color_featfix;
static QColor color_featorf;

static QColor color_featcoverage;
static QColor color_featsnp;
static QColor color_featbreak;
static QColor color_featkmer;
static QColor color_featmatepair;
static QColor color_featmisassembly;

static QColor color_insertcoverage;
static QColor color_readcoverage;

  //  static const char * allFeatureTypes;
  static const char * getFeatureStr(AMOS::FeatureType_t type);
};


#endif
