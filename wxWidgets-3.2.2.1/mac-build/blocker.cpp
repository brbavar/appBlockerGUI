#include <wx/wxprec.h>
#include <wx/image.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class IcnBMP : public wxBitmap
{
public:
    IcnBMP() : wxBitmap() {}
    IcnBMP(const wxImage &img, int depth = wxBITMAP_SCREEN_DEPTH)
        : wxBitmap(img, depth) {}
    int getW();
    int getH();
    void setX(const int &x);
    int getX();
    void setY(const int &y);
    int getY();
    void setVectIndex(const int &i);
    int getVectIndex();

private:
    int w = 70, h = 70;
    int x, y;
    // std::pair<int, int> xBounds, yBounds;
    int vectIndex;
};
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
    MyFrame();
    void setBMPs(wxVector<IcnBMP>);
    wxVector<IcnBMP> getBMPs();
    void setAppPaths(std::vector<std::string>);
    std::unordered_map<std::pair<int, int>, IcnBMP> getMap();
    std::vector<std::string> getAppPaths();
    IcnBMP findClickedIcn(wxPoint clickPos);

private:
    wxVector<IcnBMP> bmps;
    std::unordered_map<std::pair<int, int>, IcnBMP> bmpAt;
    std::vector<std::string> appPaths;

    void OnHello(wxCommandEvent &event);
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnPaint(wxPaintEvent &event);

    wxDECLARE_EVENT_TABLE();
};

std::string run(std::string cmd, int size);
std::vector<std::string> getListItems(const std::string &list);
std::string getAppPath(const std::string &appName, const std::string &dir);
std::string lsGrep(const std::string &path, const std::string &searchStr);
bool hasContents(const std::string &appPath);
bool containsResources(const std::string &appPath);
void collectIcns(MyFrame *frame);

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
std::vector<std::string> getListItems(const std::string &list)
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

// Story for interview: At first I had put the code below inside definition of OnPaint method, but
// that meant it was executed with every wxPaintEvent, such as when the window was resized (hence repainted).
// Made the app extremely laggy, so I moved this code into its own separate function to call once in OnInit.
void collectIcns(MyFrame *frame)
{
    std::vector<std::string> appDirs = {"/Applications", "/Applications/Utilities",
                                        "/Applications/Xcode.app/Contents/Applications",
                                        "/Applications/Xcode.app/Contents/Developer/Applications",
                                        "/System/Applications", "/System/Applications/Utilities",
                                        "/System/Library/CoreServices", "/System/Library/CoreServices/Applications",
                                        "/System/Library/CoreServices/Finder.app/Contents/Applications",
                                        "~/Downloads"};
    std::vector<std::string> appNames;
    std::vector<std::string> appPaths;
    wxVector<IcnBMP> bmps;

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
                    wxImage img(pngPath, wxBITMAP_TYPE_PNG);
                    IcnBMP bmp(img.Scale(70, 70, wxIMAGE_QUALITY_HIGH));

                    if (bmp.IsOk())
                    {
                        bmps.push_back(bmp);
                        appPaths.push_back(appPath);
                    }
                }
            }
            else
            {
                std::cout << "APP WITH NO CONTENTS: " << appName << '\n';
            }
        }
    }

    frame->setBMPs(bmps);
    frame->setAppPaths(appPaths);
}

enum
{
    ID_Hello = 1
};

int IcnBMP::getW()
{
    return this->w;
}

int IcnBMP::getH()
{
    return this->h;
}

void IcnBMP::setX(const int &x)
{
    this->x = x;
    // this->xBounds.first = x;
    // this->xBounds.second = x + this->getW();
}

int IcnBMP::getX()
{
    return this->x;
}

void IcnBMP::setY(const int &y)
{
    this->y = y;
    // this->yBounds.first = y;
    // this->yBounds.second = y + this->getH();
}

int IcnBMP::getY()
{
    return this->y;
}

void IcnBMP::setVectIndex(const int &i)
{
    this->vectIndex = i;
}

int IcnBMP::getVectIndex()
{
    return this->vectIndex;
}

void MyFrame::setBMPs(wxVector<IcnBMP> bmps)
{
    for (IcnBMP bmp : bmps)
    {
        this->bmps.push_back(bmp);
        this->bmpAt[std::make_pair(bmp.getX(), bmp.getY())] = bmp
    }
}

wxVector<IcnBMP> MyFrame::getBMPs()
{
    return this->bmps;
}

std::unordered_map<std::pair<int, int>, IcnBMP> MyFrame::getMap()
{
    return this->bmpAt;
}

void MyFrame::setAppPaths(std::vector<std::string> appPaths)
{
    for (std::string appPath : appPaths)
        this->appPaths.push_back(appPath);
}

std::vector<std::string> MyFrame::getAppPaths()
{
    return this->appPaths;
}

IcnBMP MyFrame::findClickedIcn(wxPoint clickPos)
{
    wxVector<IcnBMP> bmps = this->getBMPs();
    auto bmpAt = this->getMap();

    std::vector<std::pair<int, int>> orderedPairs;
    for (IcnBMP bmp : bmps)
        orderedPairs.push_back(std::make_pair(bmp.getX(), bmp.getY()));

    // Arrange ordered pairs such that x-coordinates are in ascending order.
    sort(orderedPairs.begin(), orderedPairs.end());

    int i = bmps.size() / 2;
    IcnBMP bmp = bmpAt[orderedPairs[i]];
    int w = bmp.getW(), h = bmp.getH();

    // Search in ascending order of x-coordinates until we find an icon with a
    // right edge that does not lie to the left of the point clicked within this frame.
    while (clickPos.x > bmp.getX() + w)
    {
        orderedPairs.erase(orderedPairs.begin() + i);
        bmp = bmpAt[orderedPairs[i]];
    }

    // Decrement index until we find an icon with a left edge that does
    // not lie to the right of the point clicked within this frame.
    while (clickPos.x < bmp.getX())
    {
        orderedPairs.erase(orderedPairs.begin() + i);
        bmp = bmpAt[orderedPairs[--i]];
    }

    sort(orderedPairs.begin(), orderedPairs.end(),
         [](const pair<int, int> &p1, const pair<int, int> &p2)
         { return p1.second < p2.second });

    i = orderedPairs.size() / 2;
    bmp = bmpAt[orderedPairs[i]];

    // Search in ascending order of y-coordinates until we find an icon with a
    // bottom edge that does not lie above the point clicked within this frame.
    while (clickPos.y > bmp.getY() + h)
    {
        orderedPairs.erase(orderedPairs.begin() + i);
        bmp = bmpAt[orderedPairs[i]];
    }

    // Decrement index until we find an icon with a top edge that does
    // not lie below the point clicked within this frame.
    while (clickPos.y < bmp.getY())
    {
        orderedPairs.erase(orderedPairs.begin() + i);
        bmp = bmpAt[orderedPairs[--i]];
    }

    int x = orderedPairs[i].first, y = orderedPairs[i].second;

    return clickPos == wxPoint(x, y) ? bmp : IcnBMP();
}

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    wxImage::AddHandler(new wxPNGHandler);
    MyFrame *frame = new MyFrame();
    collectIcns(frame);
    frame->Show(true);
    return true;
}

MyFrame::MyFrame()
    : wxFrame(NULL, wxID_ANY, "App Blocker", wxDefaultPosition, wxSize(915, 828))
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
    wxVector<IcnBMP> bmps = this->getBMPs();

    wxPaintDC *icnPaint = new wxPaintDC(this);

    int hgap = 45, vgap = 45;
    int cols = 8;
    int gridMarginTop = 30, gridMarginLeft = 30;
    int numApps = bmps.size();
    int rows = ceil((double)numApps / cols);
    int i = 0;

    for (int x = 0; x < cols; x++)
    {
        for (int y = 0; y < rows; y++)
        {
            i = cols * y + x;
            if (i >= numApps)
                break;

            wxMemoryDC icnMem = wxMemoryDC();
            IcnBMP bmp = bmps[i];
            icnMem.SelectObject(bmp);

            bmp.setX(x);
            bmp.setY(y);
            icnPaint->Blit(x * hgap + (x * 90) + gridMarginLeft,
                           vgap * y + (y * 90) + gridMarginTop,
                           bmp.getW(), bmp.getH(), &icnMem, 0, 0);
        }
    }
}

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_PAINT(MyFrame::OnPaint)
        wxEND_EVENT_TABLE()