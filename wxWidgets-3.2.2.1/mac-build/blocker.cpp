#include <sys/stat.h>
#include <wx/calctrl.h>
#include <wx/datectrl.h>
#include <wx/image.h>
#include <wx/textctrl.h>
#include <wx/timectrl.h>
#include <wx/tooltip.h>
#include <wx/valtext.h>
#include <wx/wxprec.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class IcnBMP : public wxBitmap {
   public:
    IcnBMP() : wxBitmap() {}
    IcnBMP(const wxImage &img, int depth = wxBITMAP_SCREEN_DEPTH) : wxBitmap(img, depth) {}
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
class MyApp : public wxApp {
   public:
    virtual bool OnInit();
};

class CalendarDialog : public wxDialog {
   public:
    CalendarDialog() : wxDialog() {}
    CalendarDialog(wxWindow *parent, wxWindowID id, const wxString &title);
};

CalendarDialog::CalendarDialog(wxWindow *parent, wxWindowID id, const wxString &title)
    : wxDialog(parent, id, title, wxDefaultPosition, wxDefaultSize,
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) {
    // wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

    if (this->ShowModal() == wxID_OK) {
        wxCalendarCtrl *calendar = new wxCalendarCtrl(this, wxID_ANY);
        // calendar->Show(true);

        // topSizer->Add(new wxCalendarCtrl(this, wxID_ANY), 1, wxEXPAND | wxALL, 10);
        // SetSizer(topSizer);
        // topSizer->Fit(this);
        // topSizer->SetSizeHints(this);
    }
}

class MyScrolled : public wxScrolledWindow {
   public:
    MyScrolled() : wxScrolledWindow() {}
    MyScrolled(wxWindow *parent);
    void collectPaths();
    void collectIcns();
    void setBMPs(wxVector<IcnBMP> bmps);
    wxVector<IcnBMP> getBMPs();
    void addToList(const std::string &item, const std::string &filename);
    std::vector<std::string> readList(const std::string &filename);
    void setAppPaths(std::vector<std::string> appPaths);
    std::vector<std::string> getAppNames();
    void establishLayout();
    wxPoint getLocationOf(const std::string &txt);
    wxPoint getLocationOf(const int &i, const int &j);
    wxSize getExtentOf(const std::string &txt);
    int getNumApps();
    int getRows();
    int getCols();
    wxCoord getIcnW();
    wxCoord getIcnH();
    void setIcnGridPaint(wxPaintDC *icnGridPaint);
    wxPaintDC *getIcnGridPaint();
    IcnBMP findClickedIcn(wxPoint clickPos);
    void blockApp(IcnBMP clickedIcn);

   private:
    wxVector<IcnBMP> bmps;
    std::vector<std::string> appNames;
    wxVector<wxVector<wxString>> linesInAppName;
    std::unordered_map<std::string, wxPoint> locationOfTxtBlock;
    wxVector<wxVector<wxPoint>> locationOfTxtLine;
    std::unordered_map<std::string, wxSize> extentOf;
    std::vector<std::string> appPaths;
    int numApps;
    int rows;
    int cols = 8;
    wxCoord icnW = 70;
    wxCoord icnH = 70;
    wxPaintDC *icnGridPaint;
    // std::vector<std::string> blocklist;

    void OnPaint(wxPaintEvent &event);
    void OnClick(wxMouseEvent &event);

    wxDECLARE_EVENT_TABLE();
};

class StaticTextCtrl : public wxTextCtrl {
   public:
    StaticTextCtrl(wxWindow *parent, const wxString &value = "", wxPoint pos = wxDefaultPosition,
                   wxSize size = wxDefaultSize, long style = 0)
        : wxTextCtrl(parent, wxID_ANY, value, pos, size, style) {}

   private:
    void OnScrollEvent(wxScrollWinEvent &event) {}
    void OnMouseWheel(wxMouseEvent &event) {}

    wxDECLARE_EVENT_TABLE();
};

class PromptFrame : public wxFrame {
   public:
    // PromptFrame() : wxFrame(NULL, wxID_ANY, "") {}
    PromptFrame(wxWindow *parent, wxPoint pos = wxDefaultPosition, wxSize size = wxDefaultSize,
                long style = 0)
        : wxFrame(parent, wxID_ANY, "", pos, size, style) {}
};

class AppFrame : public wxFrame {
   public:
    AppFrame();
    void setPassword(std::string password);
    std::string getPassword();
    void setScrolled(MyScrolled *scrolled);
    MyScrolled *getScrolled();
    void setTimePromptFrame(PromptFrame *timePromptWindow);
    PromptFrame *getTimePromptFrame();
    // void setTimePromptPanel(wxPanel* timePromptPanel);
    // wxPanel* getTimePromptPanel();
    // void setTimePrompt(StaticTextCtrl* timePrompt);
    // StaticTextCtrl* getTimePrompt();
    // void setDatePicker(wxDatePickerCtrl* datePicker);
    // wxDatePickerCtrl* getDatePicker();
    // void setTimePicker(wxTimePickerCtrl* timePicker);
    // wxTimePickerCtrl* getTimePicker();
    void makeBlockPrompt(std::string appName, std::string appPath);
    void sudo(std::string cmd);

   private:
    std::string password = "";
    MyScrolled *scrolled = nullptr;
    PromptFrame *timePromptFrame = nullptr;
    // wxPanel* timePromptPanel = nullptr;
    // StaticTextCtrl* timePrompt = nullptr;
    // wxDatePickerCtrl* datePicker = nullptr;
    // wxTimePickerCtrl* timePicker = nullptr;

    void OnHello(wxCommandEvent &event);
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
};

std::string run(std::string cmd, int size);
template <typename T>
bool contains(std::vector<T> v, T item);
std::vector<std::string> getListItems(const std::string &list);
std::string getAppPath(const std::string &appName, const std::string &dir);
std::string lsGrep(const std::string &path, const std::string &searchStr);
bool hasContents(const std::string &appPath);
bool containsResources(const std::string &appPath);

// Return the output of a shell command, namely cmd.
std::string run(std::string cmd, int size = 100) {
    std::string output = "";
    char buf[size];
    FILE *stream = popen(cmd.c_str(), "r");
    if (stream)
        while (!feof(stream))
            if (fgets(buf, size, stream) != NULL) output += buf;
    pclose(stream);
    if (output[output.size() - 1] == '\n' || output[output.size() - 1] == '\r')
        output.erase(output.size() - 1, 1);  // Remove trailing newline/carriage return
    return output;
}

// Check if vector v of Ts contains item.
template <typename T>
bool contains(std::vector<T> v, T item) {
    return find(v.begin(), v.end(), item) != v.end();
}

// Extract from a string all lines separated by newline characters, and return vector
// of those lines.
std::vector<std::string> getListItems(const std::string &list) {
    std::vector<std::string> items;
    std::stringstream listream(list);
    std::string line;
    while (std::getline(listream, line, '\n')) items.push_back(line);
    return items;
}

std::string getAppPath(const std::string &appName, const std::string &dir) {
    return dir + '/' + appName + std::string(".app");
}

std::string lsGrep(const std::string &path, const std::string &searchStr) {
    return run("ls \"" + path + std::string("\" | grep \"") + searchStr + '"');
}

bool hasContents(const std::string &appPath) { return lsGrep(appPath, "Contents").size(); }

bool containsResources(const std::string &contentsPath) {
    std::string appPath = contentsPath.substr(0, contentsPath.size() - 9);
    return hasContents(appPath) && lsGrep(contentsPath, "Resources").size();
}

enum { ID_Hello = 1 };

int IcnBMP::getW() { return this->w; }

int IcnBMP::getH() { return this->h; }

void IcnBMP::setRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h) {
    this->region = wxRegion(x, y, w, h);
}

wxRegion IcnBMP::getRegion() { return this->region; }

void IcnBMP::setVectIndex(const int &i) { this->vectIndex = i; }

int IcnBMP::getVectIndex() { return this->vectIndex; }

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    wxImage::AddHandler(new wxPNGHandler);

    AppFrame *frame = new AppFrame();

    // std::string plistPathFound =
    //     run("find /Library/LaunchDaemons -name
    //     com.app-blocker.launch-notification-tracker.plist");
    // if (plistPathFound == "") {
    //     std::string makePlist =
    //         "/usr/libexec/PlistBuddy -c \"Save\" "
    //         "com.app-blocker.launch-notification-tracker.plist";
    //     frame->sudo(makePlist);
    // }

    frame->Show(true);

    return true;
}

// void OnScrollEvent(wxScrollWinEvent &event) {}
//     void OnMouseWheel(wxMouseEvent &event) {}

wxBEGIN_EVENT_TABLE(StaticTextCtrl, wxTextCtrl) EVT_SCROLLWIN(StaticTextCtrl::OnScrollEvent)
    EVT_MOUSEWHEEL(StaticTextCtrl::OnMouseWheel) wxEND_EVENT_TABLE()

        AppFrame::AppFrame()
    : wxFrame(NULL, wxID_ANY, "App Blocker", wxDefaultPosition, wxSize(1090, 828)) {
    scrolled = new MyScrolled(this);

    std::vector<std::string> appList = scrolled->readList(".appList.txt");
    if (appList.size())
        scrolled->setAppPaths(appList);
    else
        scrolled->collectPaths();
    scrolled->collectIcns();

    scrolled->SetVirtualSize(915, 135 * scrolled->getRows() + 30);

    for (std::string appName : scrolled->getAppNames()) {
        wxWindow *txtWndw = new wxWindow(scrolled, wxID_ANY, scrolled->getLocationOf(appName),
                                         scrolled->getExtentOf(appName));
        txtWndw->SetToolTip(appName);
    }

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

    Bind(wxEVT_MENU, &AppFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &AppFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &AppFrame::OnExit, this, wxID_EXIT);
}

void AppFrame::setPassword(std::string password) { this->password = password; }

std::string AppFrame::getPassword() { return password; }

void AppFrame::setScrolled(MyScrolled *scrolled) { this->scrolled = scrolled; }

MyScrolled *AppFrame::getScrolled() { return scrolled; }

void AppFrame::setTimePromptFrame(PromptFrame *timePromptFrame) {
    this->timePromptFrame = timePromptFrame;
}

PromptFrame *AppFrame::getTimePromptFrame() { return timePromptFrame; }

// void AppFrame::setTimePromptPanel(wxPanel* timePromptPanel)
// {
//     this->timePromptPanel = timePromptPanel;
// }

// wxPanel* AppFrame::getTimePromptPanel() {
//     return timePromptPanel;
// }

// void AppFrame::setTimePrompt(StaticTextCtrl* timePrompt)
// {
//     this->timePrompt = timePrompt;
// }

// StaticTextCtrl* AppFrame::getTimePrompt()
// {
//     return timePrompt;
// }

// void AppFrame::setDatePicker(wxDatePickerCtrl* datePicker)
// {
//     this->datePicker = datePicker;
// }

// wxDatePickerCtrl* AppFrame::getDatePicker()
// {
//     return datePicker;
// }

// void AppFrame::setTimePicker(wxTimePickerCtrl* timePicker)
// {
//     this->timePicker = timePicker;
// }

// wxTimePickerCtrl* AppFrame::getTimePicker()
// {
//     return timePicker;
// }

void AppFrame::makeBlockPrompt(std::string appName, std::string appPath) {
    if (!timePromptFrame && hasContents(appPath)) {
        wxSize frameSize = wxSize(437, 250);
        timePromptFrame =
            new PromptFrame(this, wxDefaultPosition, frameSize,
                            wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX));
        timePromptFrame->Center(wxBOTH);
        timePromptFrame->SetMinSize(frameSize);
        timePromptFrame->SetMaxSize(frameSize);
        timePromptFrame->SetBackgroundColour(wxColour(100, 100, 100));

        StaticTextCtrl *timePrompt = new StaticTextCtrl(
            timePromptFrame, "When would you like to start blocking " + appName + '?',
            wxPoint(10, 10), wxSize(400, 100),
            wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
        timePrompt->SetBackgroundColour(wxColour(81, 81, 81));
        timePrompt->SetFont(
            wxFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

        wxDatePickerCtrl *datePicker =
            new wxDatePickerCtrl(timePromptFrame, wxID_ANY, wxDefaultDateTime, wxPoint(0, 60),
                                 wxDefaultSize, wxDP_ALLOWNONE);
        datePicker->CenterOnParent(wxHORIZONTAL);
        datePicker->SetRange(
            wxDateTime::Now(),
            wxDefaultDateTime);  // Any date, except one that has passed, can be chosen
        wxDateTime defaultDatePickerVal = datePicker->GetValue();

        wxTimePickerCtrl *timePicker =
            new wxTimePickerCtrl(timePromptFrame, wxID_ANY, wxDefaultDateTime, wxPoint(0, 120));
        timePicker->CenterOnParent(wxHORIZONTAL);
        wxDateTime defaultTimePickerVal = timePicker->GetValue();

        wxButton *nextBtn =
            new wxButton(timePromptFrame, wxID_ANY, "Next", wxPoint(0, 190), wxDefaultSize);
        nextBtn->CenterOnParent(wxHORIZONTAL);

        // wxDateTime blockStartDate = blockStartTime = wxDefaultDateTime;

        nextBtn->Bind(wxEVT_BUTTON, [nextBtn, appName, timePrompt, datePicker, defaultDatePickerVal,
                                     timePicker, defaultTimePickerVal, timePromptFrame,
                                     appPath](wxCommandEvent &event) {
            wxDateTime blockStartDate = datePicker->GetValue();
            wxDateTime blockStartTime = timePicker->GetValue();

            timePrompt->SetEditable(true);
            timePrompt->SetValue("When would you like to regain access to " + appName + '?');
            timePrompt->SetEditable(false);

            // std::cout << defaultDatePickerVal.GetMonth() << ' ' << defaultDatePickerVal.GetDay()
            // << ' ' << defaultDatePickerVal.GetYear() << '\n';
            datePicker->SetValue(defaultDatePickerVal);
            timePicker->SetValue(defaultTimePickerVal);

            nextBtn->SetSize(100, nextBtn->GetSize().y);
            nextBtn->CenterOnParent(wxHORIZONTAL);
            nextBtn->SetLabel("Save choices");

            // wxDateTime blockEndDate = blockEndTime = wxDefaultDateTime;

            nextBtn->Bind(wxEVT_BUTTON, [datePicker, timePicker, blockStartDate, blockStartTime,
                                         timePromptFrame, appPath](wxCommandEvent &event) {
                wxDateTime blockEndDate = datePicker->GetValue();
                wxDateTime blockEndTime = timePicker->GetValue();

                std::string exe = run("defaults read \"" + appPath +
                                      std::string("/Contents/Info.plist\" CFBundleExecutable"));
                std::string kill = "killall \"" + exe + std::string("\" >nul 2>&1");
                system(kill.c_str());

                std::string exePath = appPath + std::string("/Contents/MacOS/") + exe;
                std::string block = "chmod -x \"" + exePath + "\" 2>&1";
                std::string unblock = "chmod +x \"" + exePath + "\" 2>&1";

                std::string addCronJob = "(crontab -l; echo '" blockStartTime.GetMinute() + ' ' +
                                         blockStartTime.GetHour() + ' ' + blockStartDate.GetDay() +
                                         ' ' + blockStartDate.GetMonth() + " * " + kill + "; " +
                                         block + "') | crontab -";
                if (run(addCronJob).size()) this->sudo(addCronJob);

                addCronJob = "(crontab -l; echo '" blockEndTime.GetMinute() + ' ' +
                             blockEndTime.GetHour() + ' ' + blockEndDate.GetDay() + ' ' +
                             blockEndDate.GetMonth() + " * " + unblock + "') | crontab -";
                if (run(addCronJob).size()) this->sudo(addCronJob);

                this->addToList(appPath, ".blocklist.txt");

                timePromptFrame = nullptr;
            });
        });
    }
}

void AppFrame::sudo(std::string cmd) {
    if (password.empty()) {
        wxPasswordEntryDialog passDlg(
            this, "Enter the password you use to log in to your computer as " + run("whoami"));
        passDlg.SetTextValidator(wxFILTER_NONE);
        if (passDlg.ShowModal() == wxID_OK) {
            password = std::string(passDlg.GetValue());
            size_t it = password.find("'");
            while (it != std::string::npos) {
                password.replace(it, 1, "\'");
                it = password.find("'");
            }
        }
    }

    std::string sudoCmd = "echo '" + password + std::string("' | sudo -S ") + cmd;
    system(sudoCmd.c_str());
}

void AppFrame::OnHello(wxCommandEvent &event) { wxLogMessage("Hello world from wxWidgets!"); }

void AppFrame::OnExit(wxCommandEvent &event) { Close(true); }

void AppFrame::OnAbout(wxCommandEvent &event) {
    wxMessageBox("This is a wxWidgets Hello World example", "About Hello World",
                 wxOK | wxICON_INFORMATION);
}

MyScrolled::MyScrolled(wxWindow *parent)
    : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, parent->GetSize()) {
    SetScrollRate(10, 10);
    SetBackgroundColour(wxColour(0, 0, 0));

    Bind(wxEVT_LEFT_DOWN, &MyScrolled::OnClick, this, wxID_ANY);
}

// Story for interview: At first I had put the code below inside definition of OnPaint method, but
// that meant it was executed with every wxPaintEvent, such as when the window was resized (hence
// repainted). Made the app extremely laggy, so I moved this code into its own separate function to
// call once in OnInit.
void MyScrolled::collectPaths() {
    std::vector<std::string> appDirs = {
        "/Applications",
        "/Applications/Utilities",
        "/Applications/Xcode.app/Contents/Applications",
        "/Applications/Xcode.app/Contents/Developer/Applications",
        "/System/Applications",
        "/System/Applications/Utilities",
        "/System/Library/CoreServices",
        "/System/Library/CoreServices/Applications",
        "/System/Library/CoreServices/Finder.app/Contents/Applications",
        "~/Downloads"};
    std::vector<std::string> appPaths;
    wxVector<IcnBMP> bmps;

    for (std::string dir : appDirs) {
        int dirLevel = std::count(dir.begin(), dir.end(), '/');

        std::string findApps = "find " + dir;
        findApps += " -maxdepth 1 -regex \"" + dir;
        findApps += "/.*\\.app$\" | awk -F/ '{print $" + std::to_string(dirLevel + 2);
        findApps += "}'";

        std::vector<std::string> appNames = getListItems(run(findApps));

        for (int i = 0; i < appNames.size(); i++) appNames[i].erase(appNames[i].size() - 4);

        for (std::string appName : appNames) {
            std::string appPath = getAppPath(appName, dir);
            std::string contentsPath = appPath + std::string("/Contents");

            std::string makePNG;

            if (hasContents(appPath)) {
                std::string getIcnName = "defaults read \"" + contentsPath;
                getIcnName += "/Info.plist\" CFBundleIconFile | awk -F. '{print $1}'";
                std::string icnName = run(getIcnName);
                if (icnName.empty()) {
                    std::size_t pos = getIcnName.find("CFBundleIconFile");
                    getIcnName.replace(pos, 16, "CFBundleIconName");
                    icnName = run(getIcnName);
                    std::cout << "icnName: " << icnName << '\n';
                }
                if (containsResources(contentsPath)) {
                    std::string findIcnFile = "ls \"" + contentsPath;
                    findIcnFile += "/Resources\" | grep \"" + icnName;
                    findIcnFile += ".icns\"";
                    if (run(findIcnFile).empty()) {
                        std::string makeICNS = "iconutil -c icns \"" + contentsPath;
                        makeICNS += "/Resources/Assets.car\" " + icnName;
                        makeICNS += " -o \"app-icons/" + icnName;
                        makeICNS += ".icns\" >nul 2>&1";
                        system(makeICNS.c_str());

                        makePNG = "sips -s format png \"app-icons/" + icnName;
                        makePNG += ".icns\" --out \"app-icons/" + appName;
                        makePNG += ".png\" >nul 2>&1 && rm \"app-icons/" + icnName;
                        makePNG += ".icns\" >nul 2>&1";
                    } else {
                        makePNG = "sips -s format png \"" + contentsPath;
                        makePNG += "/Resources/" + icnName;
                        makePNG += ".icns\" --out \"app-icons/" + appName;
                        makePNG += ".png\" >nul 2>&1";
                    }
                    system(makePNG.c_str());
                } else {
                    std::cout << "APP WITH CONTENTS BUT NO RESOURCES: " << appName << '\n';
                    makePNG = "cp app-icons/NoAppIconPlaceholder.png \"app-icons/" + appName;
                    makePNG += ".png\" >nul 2>&1";
                    system(makePNG.c_str());
                }

                if (lsGrep("app-icons", appName + std::string(".png")).size()) {
                    appPaths.push_back(appPath);
                }
            } else {
                std::cout << "APP WITH NO CONTENTS: " << appName << '\n';
                // If can't find app icon at all, use NoAppIconPlaceholder.png, which is in some
                // folder somewhere named "Resources"
                makePNG = "cp app-icons/NoAppIconPlaceholder.png \"app-icons/" + appName;
                makePNG += ".png\" >nul 2>&1";
                system(makePNG.c_str());
            }
        }
    }
    this->setAppPaths(appPaths);
}

void MyScrolled::collectIcns() {
    std::vector<std::string> appPaths = this->appPaths;
    std::vector<std::string> pngPaths;
    for (int i = 0; i < appPaths.size(); i++) {
        int j = 0;
        for (j = appPaths[i].size() - 1; appPaths[i][j - 1] != '/'; j--);
        std::string appName = appPaths[i].substr(j);
        appName.erase(appName.size() - 4);
        pngPaths.push_back(run("find app-icons -name \"" + appName + std::string(".png\"")));
    }
    wxVector<IcnBMP> bmps;
    int i = 0;
    for (std::string pngPath : pngPaths) {
        wxImage img(pngPath, wxBITMAP_TYPE_PNG);
        if (img.IsOk()) {
            IcnBMP bmp(img.Scale(this->getIcnW(), this->getIcnH(), wxIMAGE_QUALITY_HIGH));
            bmp.setVectIndex(i++);
            bmps.push_back(bmp);
        }
    }
    this->setBMPs(bmps);

    this->establishLayout();
}

void MyScrolled::setBMPs(wxVector<IcnBMP> bmps) {
    if (!this->bmps.empty()) this->bmps.clear();

    for (IcnBMP bmp : bmps) this->bmps.push_back(bmp);

    this->numApps = this->bmps.size();

    this->linesInAppName.resize(this->numApps);
    this->locationOfTxtLine.resize(this->numApps);

    this->rows = ceil((double)this->numApps / this->cols);
}

wxVector<IcnBMP> MyScrolled::getBMPs() { return this->bmps; }

void MyScrolled::addToList(const std::string &item, const std::string &filename) {
    std::string addPermissions = "chmod 200 " + filename + std::string(" >nul 2>&1");
    system(addPermissions.c_str());
    std::ofstream log(filename.c_str(), std::ios_base::app);
    if (log.is_open()) {
        std::vector<std::string> list = this->readList(filename);
        if (filename == ".appList.txt")
            if (!contains(list, item)) log << item << '\n';
        if (filename == ".blocklist.txt")
            if (!contains(list, item)) log << item << '\n';
    }
    log.close();
    std::string removePermissions = addPermissions.substr(0, 6) + '0' + addPermissions.substr(7);
    system(removePermissions.c_str());
}

// Read all items listed in text file and return a vector of them.
std::vector<std::string> MyScrolled::readList(const std::string &filename) {
    std::vector<std::string> savedItems;
    std::string addPermissions = "chmod 400 " + filename + std::string(" >nul 2>&1");
    system(addPermissions.c_str());
    std::ifstream log(filename.c_str());
    if (log.is_open())
        while (!log.eof()) {
            std::string savedItem;
            std::getline(log, savedItem);
            for (char c : savedItem)
                if (!isspace(c)) {
                    savedItems.push_back(savedItem);
                    break;
                }
        }
    log.close();
    std::string removePermissions = addPermissions.substr(0, 6) + '0' + addPermissions.substr(7);
    system(removePermissions.c_str());
    return savedItems;
}

void MyScrolled::setAppPaths(std::vector<std::string> appPaths) {
    if (!this->appPaths.empty()) this->appPaths.clear();

    for (std::string appPath : appPaths) {
        this->appPaths.push_back(appPath);
        this->addToList(appPath, ".appList.txt");
    }

    if (!this->appNames.empty()) this->appNames.clear();

    for (int i = 0; i < appPaths.size(); i++) {
        std::string truncAppPath = this->appPaths[i].substr(0, this->appPaths[i].size() - 4);
        int j;
        for (j = truncAppPath.size() - 1; truncAppPath[j - 1] != '/'; j--);
        this->appNames.push_back(truncAppPath.substr(j));
    }
}

std::vector<std::string> MyScrolled::getAppNames() { return this->appNames; }

void MyScrolled::establishLayout() {
    wxPaintDC *icnGridPaint = new wxPaintDC(this);

    wxVector<IcnBMP> bmps = this->getBMPs();
    int icnW = this->getIcnW(), icnH = this->getIcnH();
    int hgap = 45, vgap = 45;
    int rows = this->rows, cols = this->cols;
    int numApps = this->numApps;
    int gridMarginTop = 30, gridMarginLeft = 30;
    int i = 0;

    for (int x = 0; x < cols; x++) {
        for (int y = 0; y < rows; y++) {
            i = cols * y + x;  // DEBUGGING NOTE: Check if every index is represented (and none
                               // double-counted; none seems to be)
            if (i >= numApps) break;

            wxCoord bmpX = x * hgap + (x * 90) + gridMarginLeft;
            wxCoord bmpY = vgap * y + (y * 90) + gridMarginTop;
            bmps[i].setRegion(bmpX, bmpY, icnW, icnH);

            if (i < numApps) {
                std::string appName = this->appNames[i];
                wxVector<wxString> appNameLines;
                wxString wxAppName = wxString(appName);
                appNameLines.push_back(wxAppName);
                int numLines = 1;
                wxCoord txtW, txtH;

                if (appName.find(' ') != std::string::npos) do {
                        // Move last word in name to a new line, until all lines
                        // are under 115 in width
                        wxString line1 = appNameLines[0];
                        icnGridPaint->GetTextExtent(line1, &txtW, &txtH);

                        int j;
                        for (j = line1.size() - 1; j >= 0 && line1[j - 1] != ' '; j--);
                        wxString lastWord = line1.Mid(j);

                        if (j > 0 && line1[j - 1] == ' ') appNameLines[0].erase(j - 1);
                        if (numLines == 1)
                            appNameLines.push_back(lastWord);
                        else if (j > -1)
                            appNameLines.insert(appNameLines.begin() + numLines - 1, lastWord);

                        numLines = appNameLines.size();
                    } while (txtW > 114);

                if (numLines > 2) appNameLines[1] += "...";

                this->linesInAppName[i] = appNameLines;

                int scrollableAreaW, scrollableAreaH;
                this->GetVirtualSize(&scrollableAreaW, &scrollableAreaH);
                wxCoord blockX = scrollableAreaW, blockY;
                wxCoord blockW = 0, blockH = 0;
                for (int j = 0; j < std::min(numLines, 2); j++) {
                    wxString line = appNameLines[j];
                    icnGridPaint->GetTextExtent(line, &txtW, &txtH);
                    blockW = std::max(blockW, txtW);
                    blockH += txtH;

                    wxCoord txtX =
                        txtW < icnW ? (bmpX + (icnW - txtW) / 2) : (bmpX - (txtW - icnW) / 2);
                    wxCoord txtY = bmpY + 80 + (20 * j);
                    this->locationOfTxtLine[i].push_back(wxPoint(txtX, txtY));

                    blockX = std::min(blockX, txtX);
                    if (j == 0) {
                        blockY = txtY;
                    }
                }

                if (this->extentOf.find(appName) == this->extentOf.end())
                    this->extentOf[appName] = wxSize(blockW, blockH - 2);

                wxPoint nameLocation = wxPoint(blockX, blockY + 5);
                if (this->locationOfTxtBlock.find(appName) == this->locationOfTxtBlock.end())
                    this->locationOfTxtBlock[appName] = nameLocation;
            }
        }
    }

    this->setBMPs(bmps);
}

wxPoint MyScrolled::getLocationOf(const std::string &txt) { return this->locationOfTxtBlock[txt]; }

wxPoint MyScrolled::getLocationOf(const int &i, const int &j) {
    return this->locationOfTxtLine[i][j];
}

wxSize MyScrolled::getExtentOf(const std::string &txt) { return this->extentOf[txt]; }

int MyScrolled::getNumApps() { return this->numApps; }

int MyScrolled::getRows() { return this->rows; }

int MyScrolled::getCols() { return this->cols; }

wxCoord MyScrolled::getIcnW() { return this->icnW; }

wxCoord MyScrolled::getIcnH() { return this->icnH; }

void MyScrolled::setIcnGridPaint(wxPaintDC *icnGridPaint) { this->icnGridPaint = icnGridPaint; }

wxPaintDC *MyScrolled::getIcnGridPaint() { return this->icnGridPaint; }

IcnBMP MyScrolled::findClickedIcn(wxPoint clickPos) {
    wxVector<IcnBMP> bmps = this->getBMPs();
    wxVector<wxRegion> regions;

    for (IcnBMP bmp : bmps)
        if (bmp.getRegion().Contains(clickPos)) {
            int i = bmp.getVectIndex();
            auto appPaths = this->appPaths;
            std::cout << '(' << clickPos.x << ", " << clickPos.y
                      << "): clicked icon associated with app at appPaths[" << i
                      << "] = " << appPaths[i] << '\n';
            return bmp;
        }

    std::cout << "No icon clicked\n";
    return IcnBMP();
}

void MyScrolled::blockApp(IcnBMP clickedIcn) {
    int i = clickedIcn.getVectIndex();

    AppFrame *frame = static_cast<AppFrame *>(GetParent());
    frame->makeBlockPrompt(this->appNames[i], this->appPaths[i]);
    frame->getTimePromptFrame()->Show();

    std::string appPath = this->appPaths[i];
    std::string exe = run("defaults read \"" + appPath +
                          std::string("/Contents/Info.plist\" CFBundleExecutable"));
    if (hasContents(appPath)) {
        //

        std::string kill = "killall \"" + exe + std::string("\" >nul 2>&1");
        system(kill.c_str());
        std::string exePath = appPath + std::string("/Contents/MacOS/") + exe;
        std::string block = "chmod -x \"" + exePath + "\" 2>&1";

        //

        if (run(block).size()) frame->sudo(block);

        //

        this->addToList(appPath, ".blocklist.txt");
    }
}

void MyScrolled::OnPaint(wxPaintEvent &event) {
    wxPaintDC *icnGridPaint = new wxPaintDC(this);
    this->setIcnGridPaint(icnGridPaint);
    DoPrepareDC(*icnGridPaint);

    wxVector<IcnBMP> bmps = this->getBMPs();
    for (int i = 0; i < this->getNumApps(); i++) {
        IcnBMP bmp = bmps[i];

        wxRect outerBounds = bmp.getRegion().GetBox();
        wxCoord x = outerBounds.x, y = outerBounds.y, w = outerBounds.width, h = outerBounds.height;

        wxMemoryDC icnMem = wxMemoryDC();
        icnMem.SelectObject(bmp);

        icnGridPaint->Blit(x, y, w, h, &icnMem, 0, 0);

        wxVector<wxString> appNameLines = linesInAppName[i];
        for (int j = 0; j < std::min(appNameLines.size(), (size_t)2); j++) {
            wxString line = appNameLines[j];
            wxPoint lineLocation = this->locationOfTxtLine[i][j];
            wxCoord x = lineLocation.x, y = lineLocation.y;
            icnGridPaint->DrawText(line, x, y);
        }
    }
}

void MyScrolled::OnClick(wxMouseEvent &event) {
    wxPoint clickPos = event.GetLogicalPosition(*(this->getIcnGridPaint()));
    IcnBMP clickedIcn = this->findClickedIcn(clickPos);
    this->blockApp(clickedIcn);
}

wxBEGIN_EVENT_TABLE(MyScrolled, wxScrolledWindow) EVT_PAINT(MyScrolled::OnPaint) wxEND_EVENT_TABLE()