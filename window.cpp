#include <ctime>
#include "wx/wx.h"

using namespace std;



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




string getTimeString() {
	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%H-%M-%S %d:%m:%Y", timeinfo);
	return buffer;
}




