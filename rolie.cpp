#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <vector>

using namespace std;
using namespace sf;

#define PI 3.1415926535898

const double width = 1080;
const double height = 720;
const int focusPointSize = 3;
const float mspeed = 10;
const double epsz = 3;
int numberOfReflections = 30;
const unsigned short quality = (3 * (height + width) - sqrt((3 * height + width) * (height + 3 * width)));
double ra = width / 2;
double rb = height / 2;
double c = sqrt((ra * ra) - (rb * rb));
const double f1x = -c;
const double f1y = 0;
const double f2x = c;
const double f2y = 0;

struct pont
{
	float x;
	float y;
	double szog;
};

void tangent(double xi, double yi, double &px, double &py, double nagysag)
{
	double m = -(rb * rb * xi) / (ra * ra * yi);
	cout << "direction factor : " << m << endl;
	px = nagysag;
	py = m * (px - xi) + yi;
	cout << "px = " << px << endl;
	cout << "py = " << py << endl;
}

void reflection(double xkp, double ykp, double xi, double yi, double &x_metsz, double &y_metsz)
{

	x_metsz = 0;
	y_metsz = 0;
	double m = -(rb * rb * xi) / (ra * ra * yi);
	double n = (-m * xi) + yi;
	double xkp_szim = xkp * ((1 - m * m) / (1 + m * m)) + ykp * ((2 * m) / (1 + m * m)) - (n * 2 * m) / (1 + m * m);
	double ykp_szim = xkp * ((2 * m) / (1 + m * m)) - ykp * ((1 - m * m) / (1 + m * m)) + (2 * n) / (1 + m * m);

	// meroleges metszete az erintovel
	// double xp = (m_erinto*xi - yi + ykp - m_meroleges*xkp) / (m_erinto - m_meroleges);
	// double yp = m_erinto*(xp - xi) + yi;
	// x_metsz = xp;
	// y_metsz = yp;
	// cout << "x_p = " << xp << endl;
	// cout << "y_p = " << yp << endl;
	// kezdopont szimetrikusa
	// double xkp_szim = 2*xp - xkp;
	// double ykp_szim = 2*yp - ykp;
	// x_metsz = xkp_szim;
	// y_metsz = ykp_szim;
	// // y = m*x + n a visszavert fenysugar egyenlete
	// double m = (ykp_szim - xi) / (xkp_szim - xi);
	// double n = (xi*(xkp_szim - ykp_szim)) / (xkp_szim - xi);

	// // metszes az elipszissel

	m = (yi - ykp_szim) / (xi - xkp_szim);
	n = (xi * ykp_szim - xkp_szim * yi) / (xi - xkp_szim);

	double delta = 4 * m * m * n * n * ra * ra * ra * ra - 4 * (ra * ra * m * m + rb * rb) * (ra * ra * (n * n - rb * rb));
	double a = ra * ra * m * m + rb * rb;
	double b = 2 * m * n * ra * ra;
	double x1 = (-b + sqrt(delta)) / (2 * a);
	double x2 = (-b - sqrt(delta)) / (2 * a);

	if (abs(x1 - xi) < epsz)
	{
		x_metsz = x2;
	}
	else
	{
		x_metsz = x1;
	}
	y_metsz = m * x_metsz + n;
}

void print_usage()
{
	cout << endl;
	cout << "Reflection of light in Ellipse" << endl
		 << endl;
	cout << "Number of points = " << quality << endl;
	cout << "A = " << ra << endl;
	cout << "B = " << rb << endl;
	cout << "C = " << c << endl;
	cout << "First focus point : (" << -f1x << ", " << f1y << ")" << endl;
	cout << "Second focus point : (" << f2x << ", " << f2y << ")" << endl;
	cout << endl;
	cout << "Commands:" << endl;
	cout << "-> Left : Move left" << endl;
	cout << "-> Right : Move right" << endl;
	cout << "-> Up: Move up" << endl;
	cout << "-> Down: Move down" << endl;
	cout << "-> Z: Zoom in" << endl;
	cout << "-> X: Zoom out" << endl;
	cout << "-> P / Space: Pause/Resume reflections" << endl;
	cout << "-> A: Increase number of reflections" << endl;
	cout << "-> S: Decrease number of reflections" << endl;
	cout << "-> Esc: Close simulation" << endl;
}

int main()
{

	srand(time(NULL));

	RenderWindow window(VideoMode(width, height), "Reflection of light in Ellipse");

	View view = window.getView();
	view.setCenter(0, 0);
	window.setView(view);

	CircleShape degrees_1(focusPointSize);
	CircleShape degrees_2(focusPointSize);

	degrees_1.setFillColor(Color(255, 0, 0));
	degrees_2.setFillColor(Color(255, 0, 0));

	///////////////////////////////////////////////////
	// Drawing ellipse //
	///////////////////////////////////////////////////

	double rad;
	double x;
	double y;
	pont *points = new pont[quality];
	Color lightColor(255, 224, 102, 255);

	ConvexShape ellipse;
	ellipse.setPointCount(quality);

	for (unsigned short i = 0; i < quality; ++i)
	{

		rad = (i * 2 * PI) / quality;
		x = cos(rad) * ra;
		y = sin(rad) * rb;
		points[i].x = x;
		points[i].y = y;
		ellipse.setPoint(i, Vector2f(x, y));
	}
	ellipse.setFillColor(Color::Transparent);
	ellipse.setOutlineColor(Color::Green);
	ellipse.setOutlineThickness(1);
	ellipse.setPosition(0, 0);

	/////////////////////////////////////////////////////
	// Reflection //
	/////////////////////////////////////////////////////

	double x_metsz = 0;
	double y_metsz = 0;
	double p1x;
	double p1y;
	double p2x;
	double p2y;
	double nagysag = ra + 100;

	// erinto(xi, yi, p1x, p1y, nagysag);
	// erinto(xi, yi, p2x, p2y, -nagysag);

	double xi = points[0].x;
	double yi = points[0].y;
	double xkp = f1x;
	double ykp = f1y;
	vector<double> v;
	int pause = 1;

	/////////////////////////////////////////////////////

	// CircleShape szim(6);
	// szim.setPosition(x_metsz, -y_metsz);
	// szim.setFillColor(Color(255,0,0));

	print_usage();
	window.setFramerateLimit(60);

	int it = 0;
	while (window.isOpen())
	{
		Event event;

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.key.code == sf::Keyboard::Escape)
				window.close();

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			{
				view.move(mspeed, 0);
				window.setView(view);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			{
				view.move(-mspeed, 0);
				window.setView(view);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				view.move(0, mspeed);
				window.setView(view);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				view.move(0, -mspeed);
				window.setView(view);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			{
				numberOfReflections++;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			{
				numberOfReflections--;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
			{
				view.zoom(0.98f);
				window.setView(view);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
			{
				view.zoom(1.02f);
				window.setView(view);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
			{
				pause = -1;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
			{
				pause = 1;
			}
		}

		// Vertex ox[] = {
		// 	Vertex(sf::Vector2f(-width, 0),Color::Red),
		// 	Vertex(sf::Vector2f(width, 0),Color::Red)
		// };

		// Vertex oy[] = {
		// 	Vertex(sf::Vector2f(0, -height),Color::Red),
		// 	Vertex(sf::Vector2f(0, height),Color::Red)
		// };

		degrees_1.setPosition(f1x - focusPointSize, f1y - focusPointSize);
		degrees_2.setPosition(f2x - focusPointSize, f2y - focusPointSize);

		if (pause == 1)
		{
			if (it < quality)
			{
				window.clear();

				window.draw(ellipse);
				window.draw(degrees_1);
				window.draw(degrees_2);
				// window.draw(ox, 2, sf::Lines);
				// window.draw(oy, 2, sf::Lines);

				xi = points[it].x;
				yi = points[it].y;
				xkp = f1x;
				ykp = f1y;

				reflection(xkp, ykp, xi, yi, x_metsz, y_metsz);
				v.push_back(xkp);
				v.push_back(ykp);
				v.push_back(xi);
				v.push_back(yi);
				v.push_back(x_metsz);
				v.push_back(y_metsz);

				Vertex first[] = {
					Vertex(sf::Vector2f(v[0], v[1]), lightColor),
					Vertex(sf::Vector2f(v[2], -v[3]), lightColor)};
				window.draw(first, 2, sf::Lines);

				for (int j = 0; j < numberOfReflections; j++)
				{
					xkp = xi;
					ykp = yi;
					xi = x_metsz;
					yi = y_metsz;
					reflection(xkp, ykp, xi, yi, x_metsz, y_metsz);
					v.push_back(x_metsz);
					v.push_back(y_metsz);
				}

				for (int i = 2; i < v.size() - 2; i += 2)
				{
					Vertex ref[] = {
						Vertex(sf::Vector2f(v[i], -v[i + 1]), lightColor),
						Vertex(sf::Vector2f(v[i + 2], -v[i + 3]), lightColor)};
					window.draw(ref, 2, sf::Lines);
				}
				v.clear();
				window.display();
				it++;
				sleep(milliseconds(40));
			}
			else
			{
				it = 0;
			}
		}
	}

	delete[] points;
	return EXIT_SUCCESS;
}