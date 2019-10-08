#include <QApplication>
#include <QLabel>
#include <QWidget>
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <string.h>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QPointF>
#include <QVector>
using namespace std;
int seg_start=0,seg_end=1;
struct Point
{
	char letter;
	double x, y;
	Point & operator = (Point const & b) { letter = b.letter;  x = b.x; y = b.y; return *this; }
	Point(const Point & b) : letter(b.letter), x(b.x), y(b.y) {}
	Point(char _letter, double _x, double _y) : letter(_letter), x(_x), y(_y) {}
	bool operator <(const Point & b) const
	{
		if (y < b.y - 1.0e-9) return true;
		else if (y > b.y + 1.0e-9) return false;
		else if (x < b.x - 1.0e-9) return true;
		else return false;
	}
	bool operator ==(const Point & b) const
	{
		return fabs(y - b.y) < 1.0e-9 && fabs(x - b.x) < 1.0e-9;
	}
	bool operator !=(const Point & b) const
	{
		return fabs(y - b.y) > 1.0e-9 || fabs(x - b.x) > 1.0e-9;
	}
};
/**
 * @brief this is struct which represent any line segment.
 * 
 */
struct Line
{
	Point beg, end;
	Line & operator = (Line const & b) { beg = b.beg; end = b.end; return *this; }
	Line(const Line & b) : beg(b.beg), end(b.end) {}
	Line(const Point & _beg, const Point & _end) : beg(_beg), end(_end) {}
};
/**
 * @brief this is event-less class which contain operator function which define some operation on the line.
 * 
 */
class event_less
{
public:
	bool operator()(const pair<double, int> & a, const pair<double, int> & b) const
	{
		if (a.first < b.first - 1.0e-9)
			return true;
		else if (a.first > b.first + 1.0e-9)
			return false;
		else if (a.second < b.second)
			return true;
		return false;
	}
};



/**
 * @brief this function make some possible pair.
 * 
 * @param a 
 * @param b 
 * @param print 
 * @return pair<bool, Point> 
 */
pair<bool, Point> intersect(const Line & a, const Line & b, bool print)
{
	Point ret('a', 0, 0);
	double div = (a.beg.x - a.end.x)*(b.beg.y - b.end.y) - (a.beg.y - a.end.y)*(b.beg.x - b.end.x), t;
	if (fabs(div) < 1.0e-13)
	{
		if (print) cout << "divisor is zero" << endl;
		return make_pair(false, ret);
	}
	ret.x = ((a.beg.x*a.end.y - a.beg.y*a.end.x)*(b.beg.x - b.end.x) - (a.beg.x - a.end.x)*(b.beg.x*b.end.y - b.beg.y*b.end.x)) / div;
	ret.y = ((a.beg.x*a.end.y - a.beg.y*a.end.x)*(b.beg.y - b.end.y) - (a.beg.y - a.end.y)*(b.beg.x*b.end.y - b.beg.y*b.end.x)) / div;
	if (print) cout << "found (" << ret.x << "," << ret.y << ")" << endl;
	if (fabs(a.end.x - a.beg.x) > 1.0e-9)
	{
		t = (ret.x - a.beg.x) / (a.end.x - a.beg.x);
		if (t < 1.0e-9 || t > 1.0 - 1.0e-9)  { if (print) cout << "out of bound: " << t << endl; return make_pair(false, ret); }
	}
	if (fabs(a.end.y - a.beg.y) > 1.0e-9)
	{
		t = (ret.y - a.beg.y) / (a.end.y - a.beg.y);
		if (t < 1.0e-9 || t > 1.0 - 1.0e-9)  { if (print) cout << "out of bound: " << t << endl; return make_pair(false, ret); }
	}
	if (fabs(b.end.x - b.beg.x) > 1.0e-9)
	{
		t = (ret.x - b.beg.x) / (b.end.x - b.beg.x);
		if (t < 1.0e-9 || t > 1.0 - 1.0e-9)  { if (print) cout << "out of bound: " << t << endl; return make_pair(false, ret); }
	}
	if (fabs(b.end.y - b.beg.y) > 1.0e-9)
	{
		t = (ret.y - b.beg.y) / (b.end.y - b.beg.y);
		if (t < 1.0e-9 || t > 1.0 - 1.0e-9)  { if (print) cout << "out of bound: " << t << endl; return make_pair(false, ret); }
	}
	if (print) cout << "intersection accepted" <<endl;
	return make_pair(true, ret);
}

void intersect(int a, int b, const Point & I, vector<Line> & lines, multimap<Point, int> & sweep, multimap<pair<double,int>, int,event_less> & events, bool print)
{
	//remove event of ending of old Line
	{
		int rem_end_events[2];
		rem_end_events[0] = a;
		rem_end_events[1] = b;
		for (int k = 0; k < 2; ++k)
		{
			pair< multimap<pair<double,int>, int,event_less>::iterator, multimap<pair<double,int>,int,event_less>::iterator > del = events.equal_range(make_pair(lines[rem_end_events[k]].end.x,seg_end)); //get all events at position of the end
			bool flag = false;
			for (multimap<pair<double,int>, int,event_less>::iterator it = del.first; it != del.second; ++it) //search over all events
			{
				if (it->first.second == seg_end && it->second == rem_end_events[k]) //event is end of Line and Line matches current
				{
					events.erase(it); //remove that Line
					flag = true;
					break; //do not expect any more
				}
			}
			if (!flag) cout << "Cannot find proper ending event for Line" << endl;
		}
	}
	lines.push_back(Line(I, lines[a].end));
	events.insert(make_pair(make_pair(I.x,seg_start), (int)lines.size() - 1));
	events.insert(make_pair(make_pair(lines.back().end.x,seg_end),(int)lines.size() - 1));
	lines[a].end = I;
	events.insert(make_pair(make_pair(I.x,seg_end), a));
	lines.push_back(Line(I, lines[b].end));
	events.insert(make_pair(make_pair(I.x,seg_start), (int)lines.size() - 1));
	events.insert(make_pair(make_pair(lines.back().end.x,seg_end), (int)lines.size() - 1));
	lines[b].end = I;
	events.insert(make_pair(make_pair(I.x,seg_end), b));
	if (print)
	{
		cout << "Number of events: " << events.size() << endl;
		for (multimap<pair<double, int>, int,event_less>::iterator it = events.begin(); it != events.end(); ++it)
			cout << "x: " << it->first.first << " type " << (it->first.second == seg_start ? "start" : "end") << " Line " << it->second << endl;
	}
}
/**
 * @brief this function find all possible intersection of the n given lines.
 * 
 * @param lines 
 * @param intersections 
 * @param print 
 */
void intersect(vector<Line> & lines, vector<Point> & intersections, bool print)
{
	multimap<pair<double,int>,int,event_less> events;
	multimap<Point,int> sweep;
	
	if( print )
	{
		cout << "Input lines[" << lines.size() << "]: " << endl;
		for (vector<Line>::iterator it = lines.begin(); it != lines.end(); ++it)
			cout << "[ " << it->beg.letter << "(" << it->beg.x << "," << it->beg.y << "), " << it->end.letter << "(" << it->end.x << "," << it->end.y << ") ] " << endl;
		cout << "Create events based on lines." << endl;
	}

	for (int k = 0; k < (int)lines.size(); ++k)
	{
		if (lines[k].beg.x > lines[k].end.x)
			swap(lines[k].beg, lines[k].end);
		events.insert(make_pair(make_pair(lines[k].beg.x,seg_start),k));
		events.insert(make_pair(make_pair(lines[k].end.x,seg_end), k));
	}


	if (print)
	{
		cout << "Number of events: " << events.size() << endl;
		for (multimap<pair<double, int>, int,event_less>::iterator it = events.begin(); it != events.end(); ++it)
			cout << "x: " << it->first.first << " type " << (it->first.second == seg_start ? "start" : "end") << " Line " << it->second << endl;
		
		cout << " Start parsing events" << endl;
	}
	
	while (!events.empty())
	{
		multimap<pair<double,int>,int,event_less>::iterator first = events.begin();
		int t = first->first.second;
		int s = first->second;
		events.erase(first);
		if (t == seg_start)
		{
			if( print ) cout << "Line " << s << " start" << endl;
			//check if there is a line with same position
			multimap<Point, int>::iterator ins = sweep.insert(make_pair(lines[s].beg, s));
			if (print)
			{
				cout << "Inserted into sweep" << endl;
				for (multimap<Point, int>::iterator it = sweep.begin(); it != sweep.end(); ++it)
					cout << it->first.letter << "(" << it->first.x << "," << it->first.y << ")" << " Line " << it->second << endl;
			}
			//check line (or lines above current)
			for (int dir = 0; dir <= 1; ++dir) // look up or down
			{
				if( print ) cout << "Looking " << (dir ? "up" : "down") << endl;
				multimap<Point, int>::iterator iter = ins;
				while ((dir ? ++iter : iter--) != (dir ? sweep.end() : sweep.begin())) //y is greater for next
				{
					if (print) cout << "test " << s << " with " << iter->second << endl;
					if (lines[s].beg != lines[iter->second].beg) //ignore same starting position
					{
						if (print) cout << "checking intersection" << endl;
						pair<bool, Point> I = intersect(lines[s], lines[iter->second],print);
						if (I.first)
						{
							I.second.letter += (char)intersections.size();
							if( print ) cout << "Intersection of " << s << " and " << iter->second << " at " << I.second.letter << "(" << I.second.x << "," << I.second.y << ")" << endl;
							intersections.push_back(I.second);
							intersect(s, iter->second, I.second, lines, sweep, events,print);
						}
					}
					else if (print) cout << "skipping lines with same starting point" << endl;
					if ((2*dir-1)*(iter->first.y - ins->first.y) > 0) //visited line is above (below) current
						break; //stop search
				}
			}
		}
		else if (t == seg_end)
		{
			if( print ) cout << "Line " << s << " end" << endl;
			//remove Line from sweep
			pair< multimap<Point, int>::iterator, multimap<Point, int>::iterator > range = sweep.equal_range(lines[s].beg);
			if( print ) cout << "Range distance " << distance(range.first,range.second) << " sweep size " << sweep.size() << endl;
			multimap<Point, int>::iterator above = range.second, below = range.first;
			bool flag = false, test = true;
			if( below-- == sweep.begin() ) test = false;
			if( above == sweep.end() ) test = false;
			if( test && print ) cout << "Test will be performed" << endl;
			for (multimap<Point, int>::iterator it = range.first; it != range.second; ++it) //search over all events
			{
				if( it->second == s) //found necessery Line
				{
					if (print)
					{
						cout << "Erase Line " << s << " from sweep: " << endl;
						for (multimap<Point, int>::iterator it = sweep.begin(); it != sweep.end(); ++it)
							cout << it->first.letter << "(" << it->first.x << "," << it->first.y << ")" << " Line " << it->second << endl;
					}
					sweep.erase(it);
					flag = true;
					break; //do not expect any more
				}
			}
			if (!flag) cout << __FILE__ << ":" << __LINE__ <<  " Error: cannot find Line " << s << " in sweep" << endl;
			if (test)
			{
				if (print) cout << "test " << below->second << " with " << above->second << endl;
				if (lines[above->second].beg != lines[below->second].beg)
				{
					if (print) cout << "checking intersection" << endl;
					pair<bool, Point> I = intersect(lines[below->second], lines[above->second],print);
					if (I.first)
					{
						I.second.letter += (char)intersections.size();
						if( print ) cout << "Intersection of " << below->second << " and " << above->second << " at " << I.second.letter << "(" << I.second.x << "," << I.second.y << ")" << endl;
						intersections.push_back(I.second);
						intersect(below->second, above->second, I.second, lines, sweep, events,print);
					}
				}
				else if (print) cout << "skipping lines with same starting point" << endl;
			}
		}
	}
	
}
/**
 * @brief this fucntion find linearfit of M found intersection points using least square method.
 * 
 * @param x 
 * @param y 
 * @param n 
 */
string linearfitline(double x[],double y[],int n,QGraphicsScene *scene)
{
    QVector <QPointF> points;
	int i;
	double a,b;
    double xsum=0,x2sum=0,ysum=0,xysum=0;                //variables for sums/sigma of xi,yi,xi^2,xiyi etc
    for (i=0;i<n;i++)
    {
        xsum=xsum+x[i];                        //calculate sigma(xi)
        ysum=ysum+y[i];                        //calculate sigma(yi)
        x2sum=x2sum+pow(x[i],2);                //calculate sigma(x^2i)
        xysum=xysum+x[i]*y[i];                    //calculate sigma(xi*yi)
    }
    a=(n*xysum-xsum*ysum)/(n*x2sum-xsum*xsum);            //calculate slope
    b=(x2sum*ysum-xsum*xysum)/(x2sum*n-xsum*xsum);
    //calculate intercept

    for(i=0;i<1000;i+=2)
    {
        QGraphicsTextItem * io = new QGraphicsTextItem;
        io->setPos(i,i*a+b);
        io->setPlainText(".");
        io->setDefaultTextColor("blue");
        io->setScale(1.5);
        scene->addItem(io);
    }
    return "The linear fit: "+to_string(a)+"x + "+to_string(b);
}
int main(int argc, char *argv[ ])
{
    //qt
    QApplication app(argc, argv);
    QVector <QPointF> points;

    QGraphicsView * view = new QGraphicsView();
    QGraphicsScene * scene = new QGraphicsScene();
    int N;
	vector<Point> intersections;
	vector<Line> lines;
	//cin>>N;
    N=6;
    double arr[N][4]={{104,212,513,727},{229,424,538,278},{249,324,654,657},{508,440,531,623},{453,295,517,398},{639,290,601,116}};
	for (int i = 0; i < N; ++i)
	{
		double a,b,c,d;
        double max_no,min_no;
		a=arr[i][0];
		b=arr[i][1];
		c=arr[i][2];
		d=arr[i][3];
		lines.push_back(Line(Point(' ',a, b), Point(' ',c,d)));
        if(a>c)
        {
            max_no=a;
            min_no=c;
        }
        else
        {
            max_no=c;
            min_no=a;
        }
        
        for(double j=min_no;j<=max_no;j+=(max_no-min_no)/1000)
        {
            points.append(QPointF(j, b+(j-a)*(d-b)/(c-a)));
        }
        QGraphicsTextItem * io = new QGraphicsTextItem;
        io->setPos(a-5,b-10);
        string s="o ("+to_string((int)a)+","+to_string((int)b)+")";
        io->setPlainText(QString::fromStdString(s));
        io->setScale(1.2);
        scene->addItem(io);
        

        QGraphicsTextItem * io2 = new QGraphicsTextItem;
        io2->setPos(c-5,d-10);
        string s2="o ("+to_string((int)c)+","+to_string((int)d)+")";
        io2->setPlainText(QString::fromStdString(s2));
        io2->setScale(1.2);
        scene->addItem(io2);
	}
	intersect(lines, intersections,false);
	int n=intersections.size();
	double x[n],y[n];
	int i=0;
	for (auto it = intersections.begin(); it != intersections.end(); ++it)
	{
		x[i]=it->x;
		y[i]=it->y;
		//cout <<"(" << it->x << "," << it->y << ") " << endl;
        //points.append(QPointF(it->x, it->y));
        QGraphicsTextItem * io3 = new QGraphicsTextItem;
        io3->setPos(it->x-5,it->y-10);
        io3->setDefaultTextColor("red");
        string s3="o ("+to_string((int)(it->x))+","+to_string((int)(it->y))+")";
        io3->setPlainText(QString::fromStdString(s3));
        io3->setScale(1.2);
        scene->addItem(io3);
		i++;
	}

    // Fill in points with n number of points
    points.append(QPointF(0, 0));

    // Create a view, put a scene in it and add tiny circles
    // in the scene
    
    view->setScene(scene);

    for(int i = 0; i< points.size(); i++)
    {
        scene->addEllipse(points[i].x(), points[i].y(), 1, 1);
    }
    QGraphicsTextItem * io = new QGraphicsTextItem;
    io->setPos(0,0);
    string s="no of intersections:"+to_string(n);
    io->setPlainText(QString::fromStdString(s+"\n"+linearfitline(x,y,n,scene)));
    io->setDefaultTextColor("blue");
    io->setScale(1.5);
    scene->addItem(io);
    view->setWindowTitle("Graph");
    view->resize(1200, 800);
    view->show();
    return app.exec();
}
