//#include <string.h>
#include <iostream>
#include <cmath>
#include "wx/sizer.h"

#include "classes.cpp"
#include "window.cpp"
#include "input.cpp"

using namespace std;

/*
	TODO
	material class
	fix fresnel
	add absorbtion spectra
	linear rgb conversion
	end step brightness adjustment
	object inside list tracking & priority system
	brightness correction (need math)
	diffuse objects
	actual lighting
*/


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

	// fix this it is only partially correct and also unoptimized, also need the new material property in here
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

