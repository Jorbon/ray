#include "wx/wx.h"
#include "wx/sizer.h"

//#include <string.h>
#include <iostream>
#include <cmath>
#include <ctime>

using namespace std;


const double pi = 3.1415926535897932384626433;
const double SMOL = 0.000000000001;

struct Scene;

struct Color {
	double r, g, b;
	Color() { r = 0, g = 0, b = 0; }
	Color(double r, double g, double b) { this->r = r, this->g = g, this->b = b; }
	Color(const Color &c) { r = c.r, g = c.g, b = c.b; }
	Color add(const Color &c) const { return Color(r+c.r, g+c.g, b+c.b); }
	Color mult(double c) const { return Color(r*c, g*c, b*c); }
	int rint() const { return max(min((int)(r*255), 255), 0); }
	int gint() const { return max(min((int)(g*255), 255), 0); }
	int bint() const { return max(min((int)(b*255), 255), 0); }
};
Color operator +(const Color &u, const Color &v) { return Color(u.r+v.r, u.g+v.g, u.b+v.b); }
Color operator *(double c, const Color &v) { return Color(v.r*c, v.g*c, v.b*c); }
Color operator *(const Color &v, double c) { return Color(v.r*c, v.g*c, v.b*c); }
Color operator /(const Color &v, double c) { double tmp = 1/c; return Color(v.r*tmp, v.g*tmp, v.b*tmp); }

struct Vec3 {
	double x, y, z;
	Vec3() { x = 0, y = 0, z = 0; }
	Vec3(double x, double y, double z) { this->x = x, this->y = y, this->z = z; }
	Vec3(const Vec3 &v) { x = v.x, y = v.y, z = v.z; }

	Vec3 add(const Vec3 &v) const { return Vec3(x+v.x, y+v.y, z+v.z); }
	Vec3 sub(const Vec3 &v) const { return Vec3(x-v.x, y-v.y, z-v.z); }
	Vec3 mult(double c) const { return Vec3(x*c, y*c, z*c); }
	double abs2() const { return x*x + y*y + z*z; }
	double abs() const { return sqrt(abs2()); }
	Vec3 normalize() const { return mult(1/abs()); }
	double dot(const Vec3 &v) const { return x*v.x + y*v.y + z*v.z; }
	Vec3 cross(const Vec3 &v) const { return Vec3(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
	double angle(const Vec3 &v) const { return acos(this->dot(v) / (this->abs() * v.abs())); }
	Vec3 parallelComponent(const Vec3 &v) const { return v.mult(this->dot(v) / v.abs2()); }
	Vec3 neg() const { return Vec3(-x, -y, -z); }
};
Vec3 operator +(const Vec3 &u, const Vec3 &v) { return Vec3(u.x+v.x, u.y+v.y, u.z+v.z); }
Vec3 operator -(const Vec3 &u, const Vec3 &v) { return Vec3(u.x-v.x, u.y-v.y, u.z-v.z); }
Vec3 operator -(const Vec3 &v) { return Vec3(-v.x, -v.y, -v.z); }
Vec3 operator *(double c, const Vec3 &v) { return Vec3(v.x*c, v.y*c, v.z*c); }
Vec3 operator *(const Vec3 &v, double c) { return Vec3(v.x*c, v.y*c, v.z*c); }
Vec3 operator /(const Vec3 &v, double c) { double tmp = 1/c; return Vec3(v.x*tmp, v.y*tmp, v.z*tmp); }


struct Ray {
	Vec3 p, v;
	// TODO object array for index history
	Ray() { p = Vec3(), v = Vec3(); }
	Ray(Vec3 p, const Vec3 &v, bool norm=true) { this->p = p, this->v = norm ? v.normalize() : Vec3(v); }
	Ray(const Ray &r) { p = Vec3(r.p), v = Vec3(r.v); }
	Color trace(Scene *s, int ord, double mag) const;
	Vec3 point(double t) const { return p + v * t; }
	Ray normalize() const { return Ray(p, v.normalize()); }
};

struct Impact {
	bool valid;
	Vec3 normal;
	double t;
	Impact() { valid = false; }
	Impact(bool valid, Vec3 normal, double t) { this->valid = valid, this->normal = normal, this->t = t; }
};

struct Sphere {
	Vec3 center;
	double radius;
	double n;
	bool reflectonly;
	Sphere() { center = Vec3(), radius = 1, reflectonly = true; }
	Sphere(const Vec3 &c, double r = 1) { center = c, radius = r, reflectonly = true; }
	Sphere(const Vec3 &c, double r, double rindex) { center = c, radius = r, n = rindex, reflectonly = false; }
	Sphere(const Sphere &s) { center = s.center, radius = s.radius, n = s.n, reflectonly = s.reflectonly; }
	Impact intersect(const Ray *r) const;
};

struct Scene {
	Vec3 cam;
	double dx, dy, fov, speed;
	int res, lowres, rayLimit;
	double colorDetail;
	double n;
	wxImage cube[6];
	Sphere spheres[2];
	int spherecount;
	Scene() {
		cam = Vec3(0, 0, -5);
		dx = 0, dy = 0, fov = 90, speed = 0.2;
		res = 1, lowres = 20;
		rayLimit = 1000;
		colorDetail = 0.002;
		n = 1;
		for (int i = 0; i < 6; i++)
			cube[i].LoadFile("resources/" + to_string(i) + ".png", wxBITMAP_TYPE_PNG);
		spherecount = 2;
		spheres[0] = Sphere(Vec3(), 2, 1.5);
		spheres[1] = Sphere(Vec3(), 1.8, 0.88867);
	}
	Color getCubeColor(const Ray *r) const;
};


string getTimeString() {
	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%H-%M-%S %d:%m:%Y", timeinfo);
	return buffer;
}











class BasicDrawPane : public wxPanel {
	public:
	BasicDrawPane(wxFrame *parent);
	
	void paintEvent(wxPaintEvent &event);
	void paintNow(string filename = "");
	
	void render(wxDC &dc, string filename = "");

	void keyPressed(wxKeyEvent &event);
	
	// some useful events
	/*
	void mouseMoved(wxMouseEvent& event);
	void mouseDown(wxMouseEvent& event);
	void mouseWheelMoved(wxMouseEvent& event);
	void mouseReleased(wxMouseEvent& event);
	void rightClick(wxMouseEvent& event);
	void mouseLeftWindow(wxMouseEvent& event);
	void keyReleased(wxKeyEvent& event);
	*/

	Scene *scene;
	
	DECLARE_EVENT_TABLE()
};

class MyApp : public wxApp {
	bool OnInit();
	wxFrame *frame;
	BasicDrawPane *drawPane;
};

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit() {
	wxImage::AddHandler(new wxPNGHandler);

	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	frame = new wxFrame((wxFrame *)NULL, -1, "Ray Bradbury Tracing", wxPoint(50,50), wxSize(400,300));
	
	drawPane = new BasicDrawPane( (wxFrame*) frame);
	sizer->Add(drawPane, 1, wxEXPAND);
	
	frame->SetSizer(sizer);
	frame->SetAutoLayout(true);
	
	frame->Show();
	return true;
} 

BEGIN_EVENT_TABLE(BasicDrawPane, wxPanel)
// some useful events
EVT_KEY_DOWN(BasicDrawPane::keyPressed)
/*
EVT_MOTION(BasicDrawPane::mouseMoved)
EVT_LEFT_DOWN(BasicDrawPane::mouseDown)
EVT_LEFT_UP(BasicDrawPane::mouseReleased)
EVT_RIGHT_DOWN(BasicDrawPane::rightClick)
EVT_LEAVE_WINDOW(BasicDrawPane::mouseLeftWindow)
EVT_KEY_UP(BasicDrawPane::keyReleased)
EVT_MOUSEWHEEL(BasicDrawPane::mouseWheelMoved)
*/

// catch paint events
EVT_PAINT(BasicDrawPane::paintEvent)

END_EVENT_TABLE()


BasicDrawPane::BasicDrawPane(wxFrame* parent) : wxPanel(parent) {
	scene = new Scene();
}

void BasicDrawPane::paintEvent(wxPaintEvent &event) {
	wxPaintDC dc(this);
	render(dc);
}

void BasicDrawPane::paintNow(string filename) {
	wxClientDC dc(this);
	render(dc, filename);
}


/*
 void BasicDrawPane::mouseMoved(wxMouseEvent& event) {}
 void BasicDrawPane::mouseDown(wxMouseEvent& event) {}
 void BasicDrawPane::mouseWheelMoved(wxMouseEvent& event) {}
 void BasicDrawPane::mouseReleased(wxMouseEvent& event) {}
 void BasicDrawPane::rightClick(wxMouseEvent& event) {}
 void BasicDrawPane::mouseLeftWindow(wxMouseEvent& event) {}
 void BasicDrawPane::keyReleased(wxKeyEvent& event) {}
 */

// some useful events
void BasicDrawPane::keyPressed(wxKeyEvent &event) {
	switch (event.GetKeyCode()) {
	case WXK_LEFT:
		scene->dx += 0.1;
		paintNow();
		break;
	case WXK_RIGHT:
		scene->dx -= 0.1;
		paintNow();
		break;
	case WXK_UP:
		scene->dy -= 0.1;
		paintNow();
		break;
	case WXK_DOWN:
		scene->dy += 0.1;
		paintNow();
		break;
	case 65:
		scene->cam.x += scene->speed * cos(scene->dx);
		scene->cam.z -= scene->speed * sin(scene->dx);
		paintNow();
		break;
	case 68:
		scene->cam.x -= scene->speed * cos(scene->dx);
		scene->cam.z += scene->speed * sin(scene->dx);
		paintNow();
		break;
	case 87:
		scene->cam.x += scene->speed * sin(scene->dx);
		scene->cam.z += scene->speed * cos(scene->dx);
		paintNow();
		break;
	case 83:
		scene->cam.x -= scene->speed * sin(scene->dx);
		scene->cam.z -= scene->speed * cos(scene->dx);
		paintNow();
		break;
	case 69:
		scene->cam.y += scene->speed;
		paintNow();
		break;
	case 81:
		scene->cam.y -= scene->speed;
		paintNow();
		break;
	case 76:
		//cout << scene->cam.x << " " << scene->cam.y << " " << scene->cam.z << endl;
		break;
	case 82: // save image
		scene->res = 1;
		paintNow(getTimeString());
		break;
	case 73: // save 4x res image
		scene->res = 4;
		paintNow(getTimeString());
		break;
	}
}




Color Scene::getCubeColor(const Ray *r) const {
	double x = r->v.x, y = r->v.y, z = r->v.z;
	double mx = abs(x), my = abs(y), mz = abs(z);
	int d;
	double t, u, v;
	if (mx > my) {
		if (mx > mz) {
			t = 1/mx;
			u = z*t, v = -y*t;
			if (x > 0)
				d = 2;
			else {
				d = 0;
				u *= -1;
			}
		} else {
			t = 1/mz;
			u = x*t, v = -y*t;
			if (z > 0) {
				d = 3;
				u *= -1;
			} else
				d = 1;
		}
	} else {
		if (my > mz) {
			t = 1/y;
			u = z*t, v = -x*t;
			if (y > 0) {
				d = 4;
				u *= -1;
			} else
				d = 5;
		} else {
			t = 1/mz;
			u = x*t, v = -y*t;
			if (z > 0) {
				d = 3;
				u *= -1;
			} else
				d = 1;
		}
	}

	int width = cube[d].GetWidth(), height = cube[d].GetHeight();
	int a = (int)((u + 1) * 0.5 * width);
	int b = (int)((v + 1) * 0.5 * height);

	if (a < 0)
		a = 0;
	else if (a >= width)
		a = width - 1;
	if (b < 0)
		b = 0;
	else if (b >= height)
		b = height - 1;
	
	return Color(cube[d].GetRed(a, b), cube[d].GetGreen(a, b), cube[d].GetBlue(a, b)) / 255;
}

Impact Sphere::intersect(const Ray *r) const {
	Vec3 relativecenter = r->p - this->center;
	double b = 2 * r->v.dot(relativecenter),
		c = relativecenter.abs2() - this->radius * this->radius;
	double determinant = b*b - 4*c;
	if (determinant < 0) // if ray is not hitting sphere at all
		return Impact();
	
	double root = sqrt(determinant);
	if (-b + root < SMOL) // if ray is already fully past sphere
		return Impact();

	double t;
	if (-b - root < SMOL) // if ray is inside sphere, exiting collision
		t = (-b + root) * 0.5;
	else t = (-b - root) * 0.5; // entering collision

	return Impact(true, (r->point(t) - this->center) / this->radius, t);
};


Color Ray::trace(Scene *scene, int ord=0, double mag=1) const {
	if (mag < scene->colorDetail)
		return Color(0.5, 0.5, 0.5) * mag;
	if (ord > scene->rayLimit)
		return Color(1, 0, 1) * mag;
	
	// find first collision
	Impact record, current;
	int index;
	for (int i = 0; i < scene->spherecount; i++) {
		current = scene->spheres[i].intersect(this);
		if (current.valid && (!record.valid || record.t > current.t)) // TODO multi impact
			record = current, index = i;
	}
	if (!record.valid)
		return scene->getCubeColor(this) * mag;


	double vdotn = v.dot(record.normal);
	Vec3 impactpoint = point(record.t);
	Vec3 parallel = record.normal * vdotn;
	Ray reflection(impactpoint, v - parallel * 2);
	if (scene->spheres[index].reflectonly)
		return reflection.trace(scene, ord + 1, mag);

	double n1, n2;
	if (vdotn >= 0) // leaving object
		n1 = scene->spheres[index].n, n2 = scene->n;
	else n1 = scene->n, n2 = scene->spheres[index].n; // entering object
	double n12 = n1 / n2,
		n21 = n2 / n1;

	// test critical angle
	// cos1 = -vdotn
	double sin1sq = 1 - vdotn * vdotn;
	double sin2sq = n12 * n12 * sin1sq;
	if (sin2sq > 1) // critical angle test for total internal reflection
		return reflection.trace(scene, ord + 1, mag);

	// refraction
	Vec3 tangent = v - parallel;
	Ray refraction(impactpoint, tangent + (n2 > n1 ? -record.normal : record.normal) * sqrt(n21 * n21 - tangent.abs2()));

	double h1 = n1 * abs(vdotn),
		h2 = n2 * sqrt(1 - n12 * n12 * sin1sq);
	double h3 = (h1 - h2) / (h1 + h2);
	double reflectchance = h3 * h3;
	
	return reflection.trace(scene, ord + 1, mag * reflectchance) + refraction.trace(scene, ord + 1, mag * (1 - reflectchance));
}




void BasicDrawPane::render(wxDC& dc, string filename) {
	//auto start = chrono::high_resolution_clock::now();
	int width, height;
	if (filename.length() == 0)
		width = dc.GetSize().GetWidth() / scene->lowres, height = dc.GetSize().GetHeight() / scene->lowres;
	else width = dc.GetSize().GetWidth() * scene->res, height = dc.GetSize().GetHeight() * scene->res;
	wxImage img(width, height);

	int raytotal = width * height, raycount = 0, progress = 0;
	
	double cx = cos(scene->dx), sx = sin(scene->dx), cy = cos(scene->dy), sy = sin(scene->dy);
	Vec3 right(-cx, 0, sx);
	Vec3 down(-sx*sy, -cy, -cx*sy);
	Vec3 facing(sx*cy, -sy, cx*cy);
	
	int windowsize = max(width, height);
	double lateralscale = 2 * tan(scene->fov * pi / 360) / windowsize;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			Ray r(scene->cam, facing + lateralscale * (right * (x + 0.5 * (1 - width)) + down * (y + 0.5 * (1 - height))));
			Color color = r.trace(scene);
			img.SetRGB(x, y, color.rint(), color.gint(), color.bint());

			if (filename.length() > 0 && ++raycount * 100 / raytotal >= progress + 10 / scene->res) {
				progress += 10 / scene->res;
				cout << progress << "%" << endl;
			}
		}
	}

	if (filename.length() > 0) {
		img.SaveFile("output/" + filename + ".png", wxBITMAP_TYPE_PNG);
		return;
	}

	dc.DrawBitmap(img.Rescale(dc.GetSize().GetWidth(), dc.GetSize().GetHeight()), 0, 0);

	//dc.SetPen(*wxWHITE_PEN);
	//auto elapsed = chrono::high_resolution_clock::now() - start;
	//long long microseconds = chrono::duration_cast<chrono::microseconds>(elapsed).count();
	//dc.DrawText(to_string(1000000.0/microseconds), 10, height - 25);
}

