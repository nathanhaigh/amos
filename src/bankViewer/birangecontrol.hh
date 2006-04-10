
class intpair
{
	int m_inf, m_sup;

	public:	
	
	intpair();
	intpair(int, int);

	void setValues(int, int);
	void setValuesInPair(int, int, intpair);
	
	int inf();
	int sup();
	
};

class biRangeControl
{
	intpair bounds;
	intpair range;

	public:

	int maxVal();
	int minVal();
	int infVal();
	int supVal();
	int minDist();

	void setBounds(int, int);
	void setRange(int, int);
	void translate(int);

};


