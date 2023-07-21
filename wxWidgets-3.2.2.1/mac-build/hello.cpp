// wxWidgets "Hello World" Program

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
// #include <wx/imagpng.h>
#include <wx/listctrl.h>
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
    void OnPaint(wxPaintEvent &event);

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

// MyFrame *frame = new MyFrame();

bool MyApp::OnInit()
{
    // wxInitAllImageHandlers();
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

    // CreateStatusBar();
    // SetStatusText("Welcome to wxWidgets!");

    Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);

    // wxIcon icoIcon = wxIcon("/Applications/Spotify.app/Contents/Resources/Icon.icns");
    // wxBitmap icoBMP = wxBitmap("/Applications/Spotify.app/Contents/Resources/Icon.icns", wxBITMAP_TYPE_ICO);
    // wxMemoryDC icoDC = wxMemoryDC();
    // icoDC.DrawBitmap(icoBMP, 0, 0);
    // wxGridSizer *grid = new wxGridSizer(1, 1, 1);
    // grid->Add(icoDC);

    std::vector<std::string> appDirs = {"/Applications", "/System/Applications",
                                        "/System/Library/CoreServices", "~/Downloads"};
    std::vector<std::string> appNames;
    // std::unordered_map<std::string, std::string> appToDir;

    wxVector<wxBitmapBundle> icnBundles;

    // for (std::string dir : appDirs)
    // {
    //     std::string find = "find " + dir;
    //     find += " -maxdepth 3 -regex \"" + dir;
    //     find += "/.*\\.app/Contents/Info.plist$\" | awk -F/ '{print $" + std::to_string((int)(std::count(dir.begin(), dir.end(), '/') + 2));
    //     find += "}' | awk -F\".app\" '{print $1}'";

    //     appNames = getListItems(run(find));

    //     for (std::string appName : appNames)
    //     {
    //         std::string contentsPath = dir + '/';
    //         contentsPath += appName + ".app/Contents";

    //         std::string getIcnFile = "defaults read \"" + contentsPath;
    //         getIcnFile += "/Info.plist\" CFBundleIconFile | awk -F. '{print $1}'";
    //         std::string icnFilename = run(getIcnFile);

    //         std::string makeIconset = "iconutil -c iconset \"" + contentsPath;
    //         makeIconset += "/Resources/" + icnFilename;
    //         makeIconset += ".icns\" >nul 2>&1";
    //         system(makeIconset.c_str());

    //         // wxBitmapBundle icnBundle = wxBitmapBundle::FromResources(wxString(appName));

    //         std::string iconsetPath = contentsPath + "/Resources/";
    //         iconsetPath += icnFilename + ".iconset";

    std::string iconsetPath = "/Applications/Steam.app/Contents/Resources/Steam.iconset";

    // std::string getPNGs = "ls -1 \"" + iconsetPath;
    // getPNGs += "\"";
    // std::vector<std::string> pngs = getListItems(run(getPNGs));
    std::string getPNG = "ls \"" + iconsetPath;
    getPNG += "\" | grep 32x32.png";
    std::string png = run(getPNG);

    // wxVector<wxBitmap> bitmaps;
    wxImageList *pngList = new wxImageList(32, 32, true);
    // for (std::string png : pngs)
    // {
    std::string bmpPath = iconsetPath + '/';
    bmpPath += png;

    // wxImage image(bmpPath, wxBITMAP_TYPE_PNG);
    // wxBitmap bmp(image);
    wxBitmap bmp = wxBitmap(bmpPath, wxBITMAP_TYPE_PNG);
    // bmp.ResetAlpha();

    // std::cout << "path: " << bmpPath << "\n\n";
    // std::cout << "size: (" << bmp.GetHeight() << ',' << bmp.GetWidth() << ")\n\n";
    // std::cout << "color depth: " << bmp.GetDepth() << "\n\n";
    // std::cout << "hasAlpha: " << bmp.HasAlpha() << "\n\n";

    // wxImage img = bmp.ConvertToImage();

    // std::cout << "alpha: ";
    // int i = 0;
    // std::cout << (img.GetAlpha() != nullptr ? "NOT " : "") << "nullptr\n\n\n";
    // while (img.GetAlpha()[i] != '\0')
    // {
    //     std::cout << img.GetAlpha()[i] << "\n\n";
    //     i++;
    //     std::cout << "is " << (img.GetAlpha()[i] == '\0' ? "" : "NOT ")
    //               << "null terminated\n\n\n";
    // }

    // for (int x = 0; x < bmp.GetWidth(); x++)
    //     for (int y = 0; y < bmp.GetHeight(); y++)
    //     {
    //         int r = img.GetRed(x, y) - '\0';
    //         int g = img.GetGreen(x, y) - '\0';
    //         int b = img.GetBlue(x, y) - '\0';
    //         int a = img.GetAlpha(x, y) - '\0';
    //         std::cout << '(' << x << ',' << y << "): "
    //                   << "RGB(" << r << ',' << g << ',' << b << ") Alpha: " << a << '\n';
    //     }

    // std::cout << "\n\n";

    // if (bmp.IsOk())
    // {
    //     // bitmaps.push_back(bmp);
    //     pngList->Add(bmp);
    // }
    // }

    // wxBitmapBundle icnBundle = wxBitmapBundle::FromBitmaps(bitmaps);
    // icnBundles.push_back(icnBundle);

    // appToDir[appName] = dir;
    //     }
    // }

    wxIcon icn = wxIcon();
    icn.CopyFromBitmap(bmp);
    int imgIndex = pngList->Add(icn);
    std::cout << "imgIndex: " << imgIndex << '\n';

    // pngList->Add(wxIcon(bmpPath, wxBITMAP_TYPE_PNG));

    wxListView *icnList = new wxListView(this, wxID_ANY, wxDefaultPosition, wxSize(400, 400), wxLC_REPORT);
    icnList->AssignImageList(pngList, wxIMAGE_LIST_NORMAL);
    // icnList.SetNormalImages(icnBundles);

    wxListItem col;
    for (int i = 0; i < 3; i++)
    {
        col.SetText(std::string("Column ") + std::to_string(i + 1));
        col.SetImage(imgIndex);
        icnList->InsertColumn(i, col);
        icnList->SetColumnWidth(i, 400);
    }

    int imageIndex = 0;
    wxString buf;

    buf.Printf(wxT("This is item %d"), 0);
    icnList->InsertItem(0, buf, imageIndex);

    icnList->SetItemData(0, 0);
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