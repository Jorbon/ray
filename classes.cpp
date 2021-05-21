#include <cmath>

using namespace std;


const double pi = 3.1415926535897932384626433;
const double SMOL = 0.000000000001;


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


struct Scene;

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

struct Material {
	
};

struct Geometry {
	virtual const Impact intersect(const Ray *r);
};

struct Sphere : Geometry {
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