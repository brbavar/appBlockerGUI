#include <wx/wx.h>
#include <unordered_map>
#include <iostream>
#include <string>

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);

class MyScrolled : public wxScrolledWindow
{
public:
    MyScrolled() : wxScrolledWindow() {}
    MyScrolled(wxWindow *parent);
    wxPoint getLocationOf(const std::string &txt);
    wxSize getExtentOf(const std::string &txt);

private:
    std::unordered_map<std::string, wxPoint> locationOf;
    std::unordered_map<std::string, wxSize> extentOf;

    void OnPaint(wxPaintEvent &event);
    wxDECLARE_EVENT_TABLE();
};

class MyFrame : public wxFrame
{
public:
    MyFrame() : wxFrame(NULL, wxID_ANY, "Minimal Sample")
    {
        MyScrolled *scrolled = new MyScrolled(this);

        wxPoint loc1 = scrolled->getLocationOf("label1");
        wxSize size1 = scrolled->getExtentOf("label1");
        wxWindow *overlay1 = new wxWindow(scrolled, wxID_ANY, loc1, size1);
        std::cout << "loc1 = wxPoint(" << loc1.x << ", " << loc1.y << ")\n";
        std::cout << "size1 = wxSize(" << size1.x << ", " << size1.y << ")\n";
        // overlay1->SetBackgroundColour(wxColour(255, 0, 0));
        // overlay1->Hide();
        overlay1->SetToolTip("label1");

        wxPoint loc2 = scrolled->getLocationOf("label2");
        wxSize size2 = scrolled->getExtentOf("label2");
        wxWindow *overlay2 = new wxWindow(scrolled, wxID_ANY, loc2, size2);
        std::cout << "loc2 = wxPoint(" << loc2.x << ", " << loc2.y << ")\n";
        std::cout << "size2 = wxSize(" << size2.x << ", " << size2.y << ")\n";
        // overlay2->SetBackgroundColour(wxColour(0, 255, 0));
        overlay2->SetToolTip("label2");
    }
};

MyScrolled::MyScrolled(wxWindow *parent)
    : wxScrolledWindow(parent, wxID_ANY)
{
    SetScrollRate(10, 10);
    SetBackgroundColour(wxColour(0, 0, 0));

    // Bind(wxEVT_LEFT_DOWN, &MyScrolled::OnClick, this, wxID_ANY);
}

wxPoint MyScrolled::getLocationOf(const std::string &txt)
{
    if (txt == "label1")
        return wxPoint(50, 75);
    return wxPoint(50, 150);
}

wxSize MyScrolled::getExtentOf(const std::string &txt)
{
    wxPaintDC *icnGridPaint = new wxPaintDC(this);
    wxCoord w, h;
    icnGridPaint->GetTextExtent(wxString(txt), &w, &h);
    return wxSize(w, h);
}

void MyScrolled::OnPaint(wxPaintEvent &event)
{
    wxPaintDC *icnGridPaint = new wxPaintDC(this);
    DoPrepareDC(*icnGridPaint);

    icnGridPaint->DrawRectangle(0, 0, 200, 200);
    icnGridPaint->SetTextForeground(wxColour(0, 0, 0));
    icnGridPaint->DrawText(wxString("label1"), 50, 75);
    icnGridPaint->DrawText(wxString("label2"), 50, 150);

    // this->locationOf["label1"] = wxPoint(50, 75);
    // this->locationOf["label2"] = wxPoint(50, 150);

    // wxCoord w, h;

    // icnGridPaint->GetTextExtent(wxString("label1"), &w, &h);
    // std::cout << "The string \"label1\" has width " << w << " and height " << h << '\n';
    // this->extentOf["label1"] = wxSize(w, h);

    // icnGridPaint->GetTextExtent(wxString("label2"), &w, &h);
    // std::cout << "The string \"label2\" has width " << w << " and height " << h << '\n';
    // this->extentOf["label2"] = wxSize(w, h);
}

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame();
    frame->Show(true);

    return true;
}

wxBEGIN_EVENT_TABLE(MyScrolled, wxScrolledWindow)
    EVT_PAINT(MyScrolled::OnPaint)
        wxEND_EVENT_TABLE()