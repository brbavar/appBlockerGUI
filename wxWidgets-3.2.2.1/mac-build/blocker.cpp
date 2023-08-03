#include <wx/wxprec.h>
#include <wx/image.h>
#include <wx/valtext.h>
// #include <boost/container_hash/hash.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
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
    void setRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h);
    wxRegion getRegion();
    void setVectIndex(const int &i);
    int getVectIndex();

private:
    int w = 70, h = 70;
    wxRegion region;
    int vectIndex;
};
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

// class HashablePair
// {
// public:
//     template <class T1, class T2>
//     std::size_t operator()(const std::pair<T1, T2> &p) const
//     {
//         std::size_t seed = 0;
//         boost::hash_combine(seed, p.first);
//         boost::hash_combine(seed, p.second);
//         return seed;
//     }
// };

class MainFrame : public wxFrame
{
public:
    MainFrame();
    void setBMPs(wxVector<IcnBMP> bmps);
    wxVector<IcnBMP> getBMPs();
    void addToList(const std::string &item, const std::string &filename);
    std::vector<std::string> readList(const std::string &filename);
    void setAppPaths(std::vector<std::string> appPaths);
    std::vector<std::string> getAppPaths();
    // void setPNGPaths(std::vector<std::string> pngPaths);
    // std::vector<std::string> getPNGPaths();
    wxCoord getIcnW();
    wxCoord getIcnH();
    void setIcnGridPaint(wxPaintDC *icnGridPaint);
    wxPaintDC *getIcnGridPaint();
    // void setDlg(wxPasswordEntryDialog *dlg);
    // wxPasswordEntryDialog *getDlg();
    IcnBMP findClickedIcn(wxPoint clickPos);
    // void addToBlocklist(const std::string &appToBlock);
    std::vector<std::string> getBlocklist();
    void setNeedBlockPermissions(const bool &blockErrorShown);
    bool getNeedBlockPermissions();
    void setBlockCmd(const std::string &blockCmd);
    std::string getBlockCmd();
    void blockApp(IcnBMP clickedIcn);

private:
    wxVector<IcnBMP> bmps;
    std::vector<std::string> appPaths;
    // std::vector<std::string> pngPaths;
    wxCoord icnW = 70;
    wxCoord icnH = 70;
    wxPaintDC *icnGridPaint;
    // wxPasswordEntryDialog *dlg;
    std::vector<std::string> blocklist;
    bool needBlockPermissions = false;
    std::string blockCmd;

    void OnHello(wxCommandEvent &event);
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnPaint(wxPaintEvent &event);
    void OnClick(wxMouseEvent &event);

    wxDECLARE_EVENT_TABLE();
};

// class PasswordEntryDialogFrame : public wxFrame
// {
// public:
//     PasswordEntryDialogFrame(wxFrame *parent, wxPasswordEntryDialog *dlg, std::string cmd);
// };

std::string run(std::string cmd, int size);
template <typename T>
bool contains(std::vector<T> v, T item);
std::vector<std::string> getListItems(const std::string &list);
std::string getAppPath(const std::string &appName, const std::string &dir);
std::string lsGrep(const std::string &path, const std::string &searchStr);
bool hasContents(const std::string &appPath);
bool containsResources(const std::string &appPath);
void collectPaths(MainFrame *mainFrame);
void collectIcns(MainFrame *mainFrame);

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

// Check if vector v of Ts contains item.
template <typename T>
bool contains(std::vector<T> v, T item)
{
    return find(v.begin(), v.end(), item) != v.end();
}

// Extract from a string all lines separated by newline characters, and return vector
// of those lines.
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
void collectPaths(MainFrame *mainFrame)
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
    // std::vector<std::string> pngPaths;
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

                // std::string pngPath = "app-icons/" + appName + ".png";
                if (lsGrep("app-icons", appName + std::string(".png")).size())
                {
                    // pngPaths.push_back(pngPath);
                    appPaths.push_back(appPath);
                }
            }
            else
            {
                std::cout << "APP WITH NO CONTENTS: " << appName << '\n';
                // If can't find app icon at all, use NoAppIconPlaceholder.png, which is in some folder somewhere named "Resources"
            }
        }
    }

    // mainFrame->setPNGPaths(pngPaths);
    mainFrame->setAppPaths(appPaths);
}

void collectIcns(MainFrame *mainFrame)
{
    std::vector<std::string> appPaths = mainFrame->getAppPaths();
    std::vector<std::string> pngPaths;
    for (int i = 0; i < appPaths.size(); i++)
    {
        int j = 0;
        for (j = appPaths[i].size() - 1; appPaths[i][j - 1] != '/'; j--)
            ;
        std::string appName = appPaths[i].substr(j);
        appName.erase(appName.size() - 4);
        pngPaths.push_back(run("find app-icons -name \"" + appName + std::string(".png\"")));
    }
    wxVector<IcnBMP> bmps;
    int i = 0;
    for (std::string pngPath : pngPaths)
    {
        std::cout << pngPath << '\n';
        wxImage img(pngPath, wxBITMAP_TYPE_PNG);
        if (img.IsOk())
        {
            IcnBMP bmp(img.Scale(mainFrame->getIcnW(), mainFrame->getIcnH(), wxIMAGE_QUALITY_HIGH));
            bmp.setVectIndex(i++);
            // if (bmp.GetLogicalWidth() || bmp.GetLogicalHeight() /* bmp.IsOk() */)
            bmps.push_back(bmp);
        }
    }
    mainFrame->setBMPs(bmps);
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

void IcnBMP::setRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
    this->region = wxRegion(x, y, w, h);
}

wxRegion IcnBMP::getRegion()
{
    return this->region;
}

void IcnBMP::setVectIndex(const int &i)
{
    this->vectIndex = i;
}

int IcnBMP::getVectIndex()
{
    return this->vectIndex;
}

void MainFrame::setBMPs(wxVector<IcnBMP> bmps)
{
    if (!this->bmps.empty())
        this->bmps.clear();

    for (IcnBMP bmp : bmps)
        this->bmps.push_back(bmp);
}

wxVector<IcnBMP> MainFrame::getBMPs()
{
    return this->bmps;
}

void MainFrame::addToList(const std::string &item, const std::string &filename)
{
    std::string addPermissions = "chmod 200 " + filename + std::string(" >nul 2>&1");
    system(addPermissions.c_str());
    std::ofstream log(filename.c_str(), std::ios_base::app);
    if (log.is_open())
    {
        std::vector<std::string> list = this->readList(filename);
        if (filename == ".appList.txt")
            if (!contains(list, item))
                log << item << '\n';
        // if (filename == ".pngList.txt")
        //     if (!contains(list, item))
        //         log << item << '\n';
        if (filename == ".blocklist.txt")
            if (!contains(list, item))
                log << item << '\n';
    }
    log.close();
    std::string removePermissions = addPermissions.substr(0, 6) + '0' + addPermissions.substr(7);
    system(removePermissions.c_str());
}

// Read all items listed in text file and return a vector of them.
std::vector<std::string> MainFrame::readList(const std::string &filename)
{
    std::vector<std::string> savedItems;
    std::string addPermissions = "chmod 400 " + filename + std::string(" >nul 2>&1");
    system(addPermissions.c_str());
    std::ifstream log(filename.c_str());
    if (log.is_open())
        while (!log.eof())
        {
            std::string savedItem;
            std::getline(log, savedItem);
            for (char c : savedItem)
                if (!isspace(c))
                {
                    savedItems.push_back(savedItem);
                    break;
                }
        }
    log.close();
    std::string removePermissions = addPermissions.substr(0, 6) + '0' + addPermissions.substr(7);
    system(removePermissions.c_str());
    return savedItems;
}

void MainFrame::setAppPaths(std::vector<std::string> appPaths)
{
    if (!this->appPaths.empty())
        this->appPaths.clear();

    for (std::string appPath : appPaths)
    {
        this->appPaths.push_back(appPath);
        this->addToList(appPath, ".appList.txt");
    }
}

std::vector<std::string> MainFrame::getAppPaths()
{
    return this->appPaths;
}

// void MainFrame::setPNGPaths(std::vector<std::string> pngPaths)
// {
//     if (!this->pngPaths.empty())
//         this->pngPaths.clear();

//     for (std::string pngPath : pngPaths)
//     {
//         this->pngPaths.push_back(pngPath);
//         this->addToList(pngPath, ".pngList.txt");
//     }
// }

// std::vector<std::string> MainFrame::getPNGPaths()
// {
//     return this->pngPaths;
// }

wxCoord MainFrame::getIcnW()
{
    return this->icnW;
}

wxCoord MainFrame::getIcnH()
{
    return this->icnH;
}

void MainFrame::setIcnGridPaint(wxPaintDC *icnGridPaint)
{
    this->icnGridPaint = icnGridPaint;
}

wxPaintDC *MainFrame::getIcnGridPaint()
{
    return this->icnGridPaint;
}

// void MainFrame::setDlg(wxPasswordEntryDialog *dlg)
// {
//     this->dlg = dlg;
// }

// wxPasswordEntryDialog *MainFrame::getDlg()
// {
//     return this->dlg;
// }

IcnBMP MainFrame::findClickedIcn(wxPoint clickPos)
{
    wxVector<IcnBMP> bmps = this->getBMPs();
    wxVector<wxRegion> regions;

    for (IcnBMP bmp : bmps)
        if (bmp.getRegion().Contains(clickPos))
        {
            int i = bmp.getVectIndex();
            auto appPaths = this->getAppPaths();
            std::cout << '(' << clickPos.x << ", " << clickPos.y
                      << "): clicked icon associated with app at appPaths["
                      << i << "] = " << appPaths[i] << '\n';
            return bmp;
        }

    std::cout << "No icon clicked\n";
    return IcnBMP();
}

// PasswordEntryDialogFrame::PasswordEntryDialogFrame(wxFrame *parent, wxPasswordEntryDialog *dlg, std::string cmd)
//     : wxFrame(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(915, 828))
// {
//     dlg = new wxPasswordEntryDialog(this, "Enter the password you use to log in to your computer as " + run("whoami"));
//     dlg->SetTextValidator(wxFILTER_NONE);
//     if (dlg->ShowModal() == wxID_OK)
//     {
//         std::string pass = std::string(dlg->GetValue());
//         std::string sudoCmd = "echo \"" + pass + std::string("\" | sudo -S ") + cmd;
//         std::cout << "sudoCmd: " << sudoCmd << '\n';
//         system(sudoCmd.c_str());
//     }
// }

// // Add path to app user wants to block to blocklist text file, if haven't already.
// void MainFrame::addToBlocklist(const std::string &appToBlock)
// {
//     system("chmod 200 .blocklist.txt >nul 2>&1");
//     std::ofstream log(".blocklist.txt", std::ios_base::app);
//     if (log.is_open())
//         if (find(blocklist.begin(), blocklist.end(), appToBlock) == blocklist.end())
//         {
//             log << appToBlock << '\n';
//             this->blocklist.push_back(appToBlock);
//         }
//     log.close();
//     system("chmod 000 .blocklist.txt >nul 2>&1");
// }

std::vector<std::string> MainFrame::getBlocklist()
{
    return this->blocklist;
}

void MainFrame::setNeedBlockPermissions(const bool &blockErrorShown)
{
    this->needBlockPermissions = blockErrorShown;
}

bool MainFrame::getNeedBlockPermissions()
{
    return this->needBlockPermissions;
}

void MainFrame::setBlockCmd(const std::string &blockCmd)
{
    this->blockCmd = blockCmd;
}

std::string MainFrame::getBlockCmd()
{
    return this->blockCmd;
}

void MainFrame::blockApp(IcnBMP clickedIcn)
{
    int i = clickedIcn.getVectIndex();
    std::string appPath = this->getAppPaths()[i];
    std::string exe = run("defaults read \"" + appPath + std::string("/Contents/Info.plist\" CFBundleExecutable"));
    if (hasContents(appPath))
    {
        std::string kill = "killall \"" + exe + std::string("\" >nul 2>&1");
        system(kill.c_str());
        std::string block = "chmod -x \"" + appPath + std::string("/Contents/MacOS/") + exe + '"';

        this->setNeedBlockPermissions(run(block).size());
        // if (run(block).size())
        // {
        //     // wxPasswordEntryDialog *dlg = nullptr;
        //     // PasswordEntryDialogFrame *dlgFrame = new PasswordEntryDialogFrame(this, dlg, block);
        //     // dlgFrame->Raise();

        //     // wxPasswordEntryDialog dlg(this, "Enter the password you use to log in to your computer as " + run("whoami"));
        //     wxPasswordEntryDialog *dlg = this->getDlg();
        //     dlg->SetTextValidator(wxFILTER_NONE);
        //     if (dlg->ShowModal() == wxID_OK)
        //     {
        //         std::string pass = std::string(dlg->GetValue());
        //         std::string sudoBlock = "echo \"" + pass + std::string("\" | sudo -S ") + block;
        //         std::cout << "sudoBlock: " << sudoBlock << '\n';
        //         system(sudoBlock.c_str());
        //     }
        // }
    }
    addToList(appPath, ".blocklist.txt");
}

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    wxImage::AddHandler(new wxPNGHandler);
    MainFrame *mainFrame = new MainFrame();
    std::vector<std::string> appList = mainFrame->readList(".appList.txt");
    // std::vector<std::string> pngList = mainFrame->readList(".pngList.txt");
    if (appList.size())
    {
        mainFrame->setAppPaths(appList);
        // if (pngList.size())
        //     mainFrame->setPNGPaths(pngList);
    }
    else
        collectPaths(mainFrame);
    collectIcns(mainFrame);
    mainFrame->Show(true);
    return true;
}

MainFrame::MainFrame()
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

    // wxPasswordEntryDialog *dlg = new wxPasswordEntryDialog(this, "Enter the password you use to log in to your computer as " + run("whoami"));
    // this->setDlg(dlg);

    // wxPasswordEntryDialog *dlg2 = this->getDlg();
    // dlg2->SetTextValidator(wxFILTER_NONE);
    // if (dlg2->ShowModal() == wxID_OK)
    // {
    //     std::string pass = std::string(dlg2->GetValue());
    //     std::string block = "chmod -x /Applications/GarageBand.app/Contents/MacOS/GarageBand";
    //     std::string sudoBlock = "echo \"" + pass + std::string("\" | sudo -S ") + block;
    //     std::cout << "sudoBlock: " << sudoBlock << '\n';
    //     system(sudoBlock.c_str());
    // }

    Bind(wxEVT_MENU, &MainFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);

    Bind(wxEVT_LEFT_DOWN, &MainFrame::OnClick, this, wxID_ANY);
    wxPasswordEntryDialog *dlg = new wxPasswordEntryDialog(this, "Enter the password you use to log in to your computer as " + run("whoami"));
    dlg->Bind(wxEVT_LEFT_DOWN, [dlg, this](wxMouseEvent &evt)
              {
                bool needBlockPermissions = this->getNeedBlockPermissions();
                std::cout << "needBlockPermissions = " << needBlockPermissions << '\n';
                if (needBlockPermissions)
                {
                    std::string block = this->getBlockCmd();
                    if (dlg->ShowModal() == wxID_OK) {
                        std::string pass = std::string(dlg->GetValue());
                        std::string sudoBlock = "echo \"" + pass + std::string("\" | sudo -S ") + block;
                        std::cout << "sudoBlock: " << sudoBlock << '\n';
                        system(sudoBlock.c_str());
                    }
                }

                this->setNeedBlockPermissions(false); });
}

void MainFrame::OnExit(wxCommandEvent &event)
{
    Close(true);
}

void MainFrame::OnAbout(wxCommandEvent &event)
{
    wxMessageBox("This is a wxWidgets Hello World example",
                 "About Hello World", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnHello(wxCommandEvent &event)
{
    wxLogMessage("Hello world from wxWidgets!");
}

void MainFrame::OnPaint(wxPaintEvent &event)
{
    wxVector<IcnBMP> bmps = this->getBMPs();

    this->setIcnGridPaint(new wxPaintDC(this));
    wxPaintDC *icnGridPaint = this->getIcnGridPaint();

    int icnW = this->getIcnW(), icnH = this->getIcnH();
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
            icnMem.SelectObject(bmps[i]);

            wxCoord bmpX = x * hgap + (x * 90) + gridMarginLeft;
            wxCoord bmpY = vgap * y + (y * 90) + gridMarginTop;
            bmps[i].setRegion(bmpX, bmpY, icnW, icnH);

            icnGridPaint->Blit(bmpX, bmpY, icnW, icnH, &icnMem, 0, 0);
        }
    }

    this->setBMPs(bmps);
}

void MainFrame::OnClick(wxMouseEvent &event)
{
    wxPoint clickPos = event.GetLogicalPosition(*(this->getIcnGridPaint()));
    IcnBMP clickedIcn = this->findClickedIcn(clickPos);
    // wxSize icnSize = clickedIcn.GetSize();
    // int w = icnSize.GetWidth(), h = icnSize.GetHeight();
    // if ((w > 0 || h > 0) && clickedIcn.IsOk())
    // {
    this->blockApp(clickedIcn);
    // }
}

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_PAINT(MainFrame::OnPaint)
        wxEND_EVENT_TABLE()