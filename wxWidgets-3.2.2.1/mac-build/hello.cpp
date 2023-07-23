// wxWidgets "Hello World" Program

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#include <wx/wrapsizer.h>
// #include <wx/imagpng.h>
// #include <wx/listctrl.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
    MyFrame();

private:
    wxImage img;

    void OnHello(wxCommandEvent &event);
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    // void OnPaint(wxPaintEvent &event);

    // wxDECLARE_EVENT_TABLE();
};

// wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
//     EVT_PAINT(MyFrame::OnPaint)
// wxEND_EVENT_TABLE()

std::string run(std::string, int);
std::vector<std::string> getListItems(std::string);

// Return the output of a shell command, namely cmd.
std::string run(std::string cmd, int size = 100)
{
    std::string output = "";
    char buf[size];
    FILE *stream = popen(cmd.c_str(), "r");
    if (stream)
        while (!feof(stream))
            if (fgets(buf, size, stream) != NULL)
                output += buf;
    pclose(stream);
    if (output[output.size() - 1] == '\n' || output[output.size() - 1] == '\r')
        output.erase(output.size() - 1, 1); // Remove trailing newline/carriage return
    return output;
}

/* Extract from a string all lines separated by newline characters, and return vector
   of those lines. */
std::vector<std::string> getListItems(std::string list)
{
    std::vector<std::string> items;
    std::stringstream listream(list);
    std::string line;
    while (std::getline(listream, line, '\n'))
        items.push_back(line);
    return items;
}

enum
{
    ID_Hello = 1
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    wxImage::AddHandler(new wxPNGHandler);
    MyFrame *frame = new MyFrame();
    frame->Show(true);
    return true;
}

MyFrame::MyFrame()
    : wxFrame(NULL, wxID_ANY, "App Blocker", wxDefaultPosition, wxSize(1200, 800))
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
                     "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);

    std::vector<std::string> appDirs = {"/Applications", "/System/Applications",
                                        "/System/Library/CoreServices", "~/Downloads"};
    std::vector<std::string> appNames;

    wxVector<std::pair<wxBitmapBundle, std::string>> bmpsWithAppNames;

    for (std::string dir : appDirs)
    {
        std::string find = "find " + dir;
        find += " -maxdepth 3 -regex \"" + dir;
        find += "/.*\\.app/Contents/Info.plist$\" | awk -F/ '{print $" + std::to_string((int)(std::count(dir.begin(), dir.end(), '/') + 2));
        find += "}' | awk -F\".app\" '{print $1}'";

        appNames = getListItems(run(find));

        for (std::string appName : appNames)
        {
            std::string contentsPath = dir + '/';
            contentsPath += appName + ".app/Contents";

            std::string getIcnFile = "defaults read \"" + contentsPath;
            getIcnFile += "/Info.plist\" CFBundleIconFile | awk -F. '{print $1}'";
            std::string icnFilename = run(getIcnFile);

            std::string makeIconset = "iconutil -c iconset \"" + contentsPath;
            makeIconset += "/Resources/" + icnFilename;
            makeIconset += ".icns\" >nul 2>&1";
            system(makeIconset.c_str());

            // wxBitmapBundle icnBundle = wxBitmapBundle::FromResources(wxString(appName));

            std::string iconsetPath = contentsPath + "/Resources/";
            iconsetPath += icnFilename + ".iconset";

            std::string getPNGs = "ls -1 \"" + iconsetPath;
            getPNGs += "\"";
            std::vector<std::string> pngs = getListItems(run(getPNGs));

            wxVector<wxBitmap> bitmaps;
            for (std::string png : pngs)
            {
                std::string bmpPath = iconsetPath + '/';
                bmpPath += png;
                wxBitmap bmp = wxBitmap(bmpPath, wxBITMAP_TYPE_PNG);
                if (bmp.IsOk())
                    bitmaps.push_back(bmp);
            }
            bmpsWithAppNames.push_back(std::make_pair(wxBitmapBundle::FromBitmaps(bitmaps), appName));
        }
    }

    int cols = 6;
    int rows = ceil((double)bmpsWithAppNames.size() / cols);

    wxScrolled<wxPanel> *scrollArea = new wxScrolled<wxPanel>(this);

    // wxWrapSizer *wrap = new wxWrapSizer();
    wxGridSizer *grid = new wxGridSizer(rows, cols, 20, 20);
    grid->Layout();

    scrollArea->SetSizer(grid);
    scrollArea->SetScrollRate(0, 12);
    scrollArea->Bind(wxEVT_SIZE, [this](wxSizeEvent &event)
                     { SetVirtualSize(wxSize(GetClientSize().x, -1)); });

    for (auto pair : bmpsWithAppNames)
    {
        wxButton *btn = new wxButton(scrollArea, wxID_ANY, pair.second, wxDefaultPosition, wxSize(100, 10), wxBORDER_NONE);
        btn->SetBitmap(pair.first);
        // wxMemoryDC icnMem = wxMemoryDC();
        // icnMem.SelectObject(pair.first);
        // wxCoord w, h;
        // icnMem.GetSize(&w, &h);

        // wxPaintDC *icnPaint = new wxPaintDC(this);
        // icnPaint->Blit(0, 0, w, h, &icnMem, 0, 0);

        grid->Add(icnPaint, 1, wxEXPAND | wxALL);
    }
}

void MyFrame::OnExit(wxCommandEvent &event)
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent &event)
{
    wxMessageBox("This is a wxWidgets Hello World example",
                 "About Hello World", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent &event)
{
    wxLogMessage("Hello world from wxWidgets!");
}

// void MyFrame::OnPaint(wxPaintEvent& event)
// {
//     std::string cmd = "sips -s format png /Applications/Spotify.app/Contents/Resources/Icon.icns --out Icon.png";
//     system(cmd.c_str());
//     wxImage::AddHandler(new wxPNGHandler);
//     wxImage image(wxT("/Users/benbavar/appBlockerFrontend/wxWidgets-3.2.2.1/mac-build/Icon.png"), wxBITMAP_TYPE_PNG);
//     wxBitmap icoBMP(image.Scale(90, 90, wxIMAGE_QUALITY_HIGH));

//     wxMemoryDC icoMem = wxMemoryDC();
//     icoMem.SelectObject(icoBMP);
//     wxCoord w, h;
//     icoMem.GetSize(&w, &h);

//     wxPaintDC icoPaint = wxPaintDC(frame);
//     icoPaint.Blit(0, 0, w, h, &icoMem, 0, 0);
// }