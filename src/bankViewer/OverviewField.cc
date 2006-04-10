#include "OverviewField.hh"
#include <qcanvas.h>

#include "DataStore.hh"

#include <vector>

using namespace std;
using namespace AMOS;


OverviewField::OverviewField(DataStore * datastore,
                             QCanvas * canvas,
                             QWidget * parent, 
                             const char * name)
  :QCanvasView(canvas, parent, name)
{
  m_datastore = datastore;
}
