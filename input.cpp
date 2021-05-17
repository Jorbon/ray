#include <cmath>

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




