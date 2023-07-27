#include <wx/wxprec.h>
#include <wx/wrapsizer.h>
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
    wxImage img; // LIKELY SUPERFLUOUS

    void OnHello(wxCommandEvent &event);
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnPaint(wxPaintEvent &event);

    wxDECLARE_EVENT_TABLE();
};

std::string run(std::string cmd, int size);
std::vector<std::string> getListItems(std::string list);
std::string getAppPath(const std::string &appName, const std::string &dir);
std::string lsGrep(const std::string &path, const std::string &searchStr);
bool hasContents(const std::string &appPath);
bool containsResources(const std::string &appPath);

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

std::string getAppPath(const std::string &appName, const std::string &dir)
{
    return dir + '/' + appName + std::string(".app");
}

std::string lsGrep(const std::string &path, const std::string &searchStr)
{
    return run("ls \"" + path + std::string("\" | grep \"") + searchStr + '"');
}

bool hasContents(const std::string &appPath)
{
    return lsGrep(appPath, "Contents").size();
}

bool containsResources(const std::string &contentsPath)
{
    std::string appPath = contentsPath.substr(0, contentsPath.size() - 9);
    return hasContents(appPath) && lsGrep(contentsPath, "Resources").size();
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

void MyFrame::OnPaint(wxPaintEvent &event)
{
    std::vector<std::string> appDirs = {"/Applications", "/Applications/Utilities",
                                        "/Applications/Xcode.app/Contents/Applications",
                                        "/Applications/Xcode.app/Contents/Developer/Applications",
                                        "/System/Applications", "/System/Applications/Utilities",
                                        "/System/Library/CoreServices", "/System/Library/CoreServices/Applications",
                                        "/System/Library/CoreServices/Finder.app/Contents/Applications",
                                        "~/Downloads"};
    std::vector<std::string> appNames;
    wxVector<std::pair<wxBitmap, std::string>> bmpsWithAppNames;

    for (std::string dir : appDirs)
    {
        int dirLevel = std::count(dir.begin(), dir.end(), '/');

        std::string findApps = "find " + dir;
        findApps += " -maxdepth 1 -regex \"" + dir;
        findApps += "/.*\\.app$\" | awk -F/ '{print $" + std::to_string(dirLevel + 2);
        findApps += "}'";

        appNames = getListItems(run(findApps));

        for (int i = 0; i < appNames.size(); i++)
            appNames[i].erase(appNames[i].size() - 4);

        for (std::string appName : appNames)
        {
            std::string appPath = getAppPath(appName, dir);
            std::string contentsPath = appPath + std::string("/Contents");
            if (hasContents(appPath))
            {
                std::string getIcnName = "defaults read \"" + contentsPath;
                getIcnName += "/Info.plist\" CFBundleIconFile | awk -F. '{print $1}'";
                std::string icnName = run(getIcnName);
                if (icnName.empty())
                {
                    std::size_t pos = getIcnName.find("CFBundleIconFile");
                    getIcnName.replace(pos, 16, "CFBundleIconName");
                    icnName = run(getIcnName);
                    std::cout << "icnName: " << icnName << '\n';
                }
                if (containsResources(contentsPath))
                {
                    std::string findIcnFile = "ls \"" + contentsPath;
                    findIcnFile += "/Resources\" | grep \"" + icnName;
                    findIcnFile += ".icns\"";
                    std::string makePNG;
                    if (run(findIcnFile).empty())
                    {
                        std::string makeICNS = "iconutil -c icns \"" + contentsPath;
                        makeICNS += "/Resources/Assets.car\" " + icnName;
                        makeICNS += " -o \"app-icons/" + icnName;
                        makeICNS += ".icns\" >nul 2>&1";
                        system(makeICNS.c_str());

                        makePNG = "sips -s format png \"app-icons/" + icnName;
                        makePNG += ".icns\" --out \"app-icons/" + appName;
                        makePNG += ".png\" >nul 2>&1 && rm \"app-icons/" + icnName;
                        makePNG += ".icns\" >nul 2>&1";
                    }
                    else
                    {
                        makePNG = "sips -s format png \"" + contentsPath;
                        makePNG += "/Resources/" + icnName;
                        makePNG += ".icns\" --out \"app-icons/" + appName;
                        makePNG += ".png\" >nul 2>&1";
                    }
                    system(makePNG.c_str());
                }
                else
                {
                    std::cout << "APP WITH CONTENTS BUT NO RESOURCES: " << appName << '\n';
                }

                std::string pngPath = "app-icons/" + appName + ".png";
                if (lsGrep("app-icons", appName + std::string(".png")).size())
                {
                    wxImage image(pngPath, wxBITMAP_TYPE_PNG);
                    // wxBitmap bmp(image.Scale(90, 90, wxIMAGE_QUALITY_HIGH)); // FIX THIS; IT IS CAUSING ERROR

                    // if (bmp.IsOk())
                    //     bmpsWithAppNames.push_back(std::make_pair(bmp, appName));
                }
            }
            else
            {
                std::cout << "APP WITH NO CONTENTS: " << appName << '\n';
            }
        }
    }

    wxPaintDC *icnPaint = new wxPaintDC(this);
    int hgap = 20;
    int vgap = 20;
    int cols = 8;
    int rows = ceil((double)bmpsWithAppNames.size() / cols);
    for (int x = 0; x < cols; x++)
        for (int y = 0; y < rows; y++)
        {
            auto pair = bmpsWithAppNames[cols * y + x];

            wxMemoryDC icnMem = wxMemoryDC();
            icnMem.SelectObject(pair.first);
            wxCoord w, h;
            icnMem.GetSize(&w, &h);

            icnPaint->Blit(x * hgap + (x * 90), vgap * y + (y * 90), w, h, &icnMem, 0, 0);
        }
}

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_PAINT(MyFrame::OnPaint)
        wxEND_EVENT_TABLE()