#include "birangecontrol.hh"

intpair::intpair()
{
	m_inf = 0;
	m_sup = 100;	
}

intpair::intpair(int i, int s)
{
	setValues(i, s);
}

void intpair::setValues(int i, int s)
{
	/* watch for errors */
	if (i < s)
	{
		m_sup = s;
		m_inf = i;
	}
	else
	{
		m_sup = i;
		m_inf = s;
	}
}

void intpair::setValuesInPair(int i, int s, intpair p)
{
	setValues(i, s);
	if (m_inf < p.inf())
		m_inf = p.inf();
	if (m_sup > p.sup())
		m_sup = p.sup();
}

int intpair::inf()
{
	return m_inf;
}

int intpair::sup()
{
	return m_sup;
}


int biRangeControl::minVal()
{
	return bounds.inf();
}

int biRangeControl::maxVal()
{
	return bounds.sup();
}

int biRangeControl::infVal()
{
	return range.inf();
}

int biRangeControl::supVal()
{
	return range.sup();
}

int biRangeControl::minDist()
{
	// uh ?
	return 0;
}

void biRangeControl::setBounds(int min, int max)
{
	bounds.setValues(min, max);
}

void biRangeControl::setRange(int inf, int sup)
{
	range.setValuesInPair(inf, sup, bounds);
}

void biRangeControl::translate(int by)
{
	int dist = range.sup() - range.inf();
	if (by > 0)
	{
		if (range.sup() + by > bounds.sup() )
		{
			range.setValues(bounds.sup() - dist, bounds.sup());
		}
		else
		{
			range.setValues(range.sup() + by, range.inf() + by );
		}
	}
	else
	{
		if (range.inf() + by < bounds.inf())
		{
			range.setValues(bounds.inf(), bounds.inf() + dist);
		}
		else
		{
			range.setValues(range.inf() + by, range.sup() + by);
		}
	}

}





