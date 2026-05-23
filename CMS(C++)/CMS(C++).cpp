#define NOMINMAX
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>
#include <sstream>
#include <iomanip>

#include <ctime>
#include <algorithm>
#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;

// ANSI COLOR CODES
#define RESET    "\033[0m"
#define RED      "\033[31m"
#define GREEN    "\033[32m"
#define YELLOW   "\033[33m"
#define CYAN     "\033[36m"
#define MAGENTA  "\033[35m"
#define BLUE     "\033[34m"
#define WHITE    "\033[37m"
#define BOLD     "\033[1m"
#define BG_BLUE  "\033[44m"
#define BG_GREEN "\033[42m"

string getCurrentTime()
{
    time_t now = time(nullptr);
    tm t_val;
#ifdef _WIN32
    localtime_s(&t_val, &now);
#else
    localtime_r(&now, &t_val);
#endif
    tm* t = &t_val;
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", t);
    return string(buf);
}

void printLine(char ch = '-', int len = 65)
{
    cout << CYAN;
    for (int i = 0; i < len; i++)
        cout << ch;
    cout << RESET << "\n";
}


void clearInput()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}
// STRUCT: AuditLog

struct AuditLog
{
    string timestamp;
    string actor;
    string action;

    void save(ofstream& fout) const
    {
        fout << timestamp << "|" << actor << "|" << action << "\n";
    }

    static bool load(ifstream& fin, AuditLog& l)
    {
        string line;
        if (!getline(fin, line))
            return false;
        stringstream ss(line);
        getline(ss, l.timestamp, '|');
        getline(ss, l.actor, '|');
        getline(ss, l.action, '|');
        return !l.timestamp.empty();
    }
};
// STRUCT: Complaint

struct Complaint
{
    int    id;
    string trackingNo;
    string status;
    string description;
    string category;
    string priority;
    string username;
    string timestamp;
    string adminNote;

    void save(ofstream& fout) const
    {
        fout << id << "\n"
            << trackingNo << "\n"
            << status << "\n"
            << description << "\n"
            << category << "\n"
            << priority << "\n"
            << username << "\n"
            << timestamp << "\n"
            << adminNote << "\n";
    }

    static bool load(ifstream& fin, Complaint& c)
    {
        string idLine;
        if (!getline(fin, idLine))
            return false;
        c.id = stoi(idLine);
        if (!getline(fin, c.trackingNo))
            return false;
        if (!getline(fin, c.status))
            return false;
        if (!getline(fin, c.description))
            return false;
        if (!getline(fin, c.category))
            return false;
        if (!getline(fin, c.priority))
            return false;
        if (!getline(fin, c.username))
            return false;
        if (!getline(fin, c.timestamp))
            return false;
        if (!getline(fin, c.adminNote))
            return false;
        return true;
    }

    string statusBadge() const
    {
        if (status == "Pending")
            return string(YELLOW) + "[PENDING]" + RESET;
        if (status == "Confirmed")
            return string(GREEN) + "[CONFIRMED]" + RESET;
        if (status == "Rejected")
            return string(RED) + "[REJECTED]" + RESET;
        if (status == "Deleted")
            return string(RED) + "[DELETED]" + RESET;
        return status;
    }
    string priorityLabel() const
    {
        if (priority == "High")
            return string(RED) + "HIGH" + RESET;
        if (priority == "Medium")
            return string(YELLOW) + "MEDIUM" + RESET;
        return string(GREEN) + "LOW" + RESET;
    }
    void printSummary() const
    {
        cout << BOLD << "  #" << trackingNo << RESET
            << "  " << statusBadge()
            << "  Priority: " << priorityLabel()
            << "  Category: " << CYAN << category << RESET << "\n";
        cout << "  Filed by: " << MAGENTA << username << RESET
            << "  at " << timestamp << "\n";
        cout << "  Desc    : " << description << "\n";
        if (!adminNote.empty())
            cout << "  Admin note: " << BLUE << adminNote << RESET << "\n";
    }
};

// STRUCT: User

struct User
{
    string username;
    string password;
    string role;

    void save(ofstream& fout) const
    {
        fout << username << "|" << password << "|" << role << "\n";
    }

    static bool load(ifstream& fin, User& u)
    {
        string line;
        if (!getline(fin, line))
            return false;
        stringstream ss(line);
        getline(ss, u.username, '|');
        getline(ss, u.password, '|');
        getline(ss, u.role, '|');
        return !u.username.empty();
    }
};

// CLASS: Login

class Login
{
protected:
    vector<User> users;
    string loggedRole;
    string loggedUser;

    const string usersFile = "users.txt";

public:
    void loadUsers()
    {
        users.clear();
        ifstream fin(usersFile);
        if (!fin.is_open())
            return;
        User u;
        while (User::load(fin, u))
            users.push_back(u);
    }

    void saveUsers()
    {
        ofstream fout(usersFile);
        for (const auto& u : users)
            u.save(fout);
    }

    bool hasUsers() const
    {
        return !users.empty();
    }

    void registerAccounts()
    {
        users.clear();

        printLine('=');
        cout << BOLD << MAGENTA << "  FIRST-TIME SETUP -- Create Accounts\n" << RESET;
        printLine('=');

        User admin;
        admin.role = "admin";
        cout << YELLOW << "  Admin Username : " << RESET;
        cin >> admin.username;
        cout << YELLOW << "  Admin Password : " << RESET;
        cin >> admin.password;

        User user;
        user.role = "user";
        cout << YELLOW << "  User  Username : " << RESET;
        cin >> user.username;
        cout << YELLOW << "  User  Password : " << RESET;
        cin >> user.password;

        users.push_back(admin);
        users.push_back(user);
        saveUsers();

        cout << GREEN << "\n  Accounts created successfully!\n" << RESET;
    }
    bool addUser(const string& uname, const string& pass, const string& role)
    {
        for (const auto& u : users)
            if (u.username == uname)
                return false;
        User nu;
        nu.username = uname;
        nu.password = pass;
        nu.role = role;
        users.push_back(nu);
        saveUsers();
        return true;
    }
    bool deleteUser(const string& uname)
    {
        for (auto it = users.begin(); it != users.end(); ++it)
        {
            if (it->username == uname && it->role != "admin")
            {
                users.erase(it);
                saveUsers();
                return true;
            }
        }
        return false;
    }
    bool login()
    {
        string username, password;
        printLine('=');
        cout << BOLD << MAGENTA << "  LOGIN\n" << RESET;
        printLine('=');
        cout << YELLOW << "  Username : " << RESET;
        cin >> username;
        cout << YELLOW << "  Password : " << RESET;
        cin >> password;

        for (const auto& u : users)
        {
            if (u.username == username && u.password == password)
            {
                loggedRole = u.role;
                loggedUser = u.username;
                cout << GREEN << "\n   Welcome, " << username
                    << "! [" << u.role << "]\n" << RESET;
                return true;
            }
        }
        cout << RED << "\n   Invalid credentials.\n" << RESET;
        return false;
    }
    void logout()
    {
        loggedRole.clear();
        loggedUser.clear();
    }

    string getLoggedRole() const
    {
        return loggedRole;
    }
    string getLoggedUser() const
    {
        return loggedUser;
    }
    void listUsers() const
    {
        printLine();
        cout << BOLD << "  ALL ACCOUNTS\n" << RESET;
        printLine();
        for (const auto& u : users)
        {
            string roleColor = (u.role == "admin") ? MAGENTA : CYAN;
            cout << "  " << roleColor << "[" << u.role << "]" << RESET
                << " " << u.username << "\n";
        }
        printLine();
    }
};
// CLASS: Dashboard

class Dashboard : public Login
{
private:
    vector<Complaint> complaints;
    vector<AuditLog>  auditLogs;
    int complaintCount = 0;
    const string complaintsFile = "complaints.txt";
    const string auditFile = "audit.txt";
    int pickMenu(int min, int max)
    {
        int ch;
        while (true)
        {
            cout << YELLOW << "  Enter choice: " << RESET;
            cin >> ch;
            if (!cin.fail() && ch >= min && ch <= max)
                return ch;
            clearInput();
            cout << RED << "  Invalid! Enter a number " << min << "-" << max << ".\n" << RESET;
        }
    }
    Complaint* findById(int id)
    {
        for (auto& c : complaints)
            if (c.id == id)
                return &c;
        return nullptr;
    }
    string makeTrackingNo(int id)
    {
        ostringstream oss;
        oss << "CMS-" << setw(4) << setfill('0') << id;
        return oss.str();
    }
    void log(const string& actor, const string& action)
    {
        AuditLog l;
        l.timestamp = getCurrentTime();
        l.actor = actor;
        l.action = action;
        auditLogs.push_back(l);
        ofstream fout(auditFile, ios::app);
        l.save(fout);
    }
    void loadAudit()
    {
        auditLogs.clear();
        ifstream fin(auditFile);
        if (!fin.is_open())
            return;
        AuditLog l;
        while (AuditLog::load(fin, l))
            auditLogs.push_back(l);
    }
    string pickCategory()
    {
        vector<string> cats = {
            "Theft / Robbery",
            "Harassment",
            "Fraud / Scam",
            "Vandalism",
            "Noise / Disturbance",
            "Traffic Violation",
            "Other"
        };
        cout << CYAN << "\n  Select category:\n" << RESET;
        for (int i = 0; i < (int)cats.size(); i++)
            cout << "    " << (i + 1) << ". " << cats[i] << "\n";
        int ch = pickMenu(1, (int)cats.size());
        return cats[ch - 1];
    }
    string pickPriority()
    {
        cout << CYAN << "\n  Select priority:\n" << RESET;
        cout << "    1. Low\n    2. Medium\n    3. High\n";
        int ch = pickMenu(1, 3);
        if (ch == 1)
            return "Low";
        if (ch == 2)
            return "Medium";
        return "High";
    }
    void displayComplaints(const vector<Complaint*>& list)
    {
        if (list.empty())
        {
            cout << RED << "  No complaints found.\n" << RESET;
            return;
        }
        for (const auto* c : list)
        {
            printLine();
            c->printSummary();
        }
        printLine();
        cout << GREEN << "  Total: " << list.size() << " complaint(s).\n" << RESET;
    }
public:
    void loadComplaints()
    {
        complaints.clear();
        complaintCount = 0;
        ifstream fin(complaintsFile);
        if (!fin.is_open())
            return;
        Complaint c;
        while (Complaint::load(fin, c))
        {
            complaints.push_back(c);
            if (c.id > complaintCount)
                complaintCount = c.id;
        }
    }
    void saveComplaints()
    {
        ofstream fout(complaintsFile);
        if (!fout.is_open())
        {
            cerr << RED << "  Error saving complaints.\n" << RESET;
            return;
        }
        for (const auto& c : complaints)
            c.save(fout);
    }

    // USER FEATURES

    void registerComplaint()
    {
        printLine('=');
        cout << BOLD << "  FILE A COMPLAINT\n" << RESET;
        printLine('=');
        Complaint c;
        c.id = ++complaintCount;
        c.trackingNo = makeTrackingNo(c.id);
        c.status = "Pending";
        c.username = loggedUser;
        c.timestamp = getCurrentTime();
        c.adminNote = "";
        c.category = pickCategory();
        c.priority = pickPriority();
        clearInput();
        cout << CYAN << "\n  Description: " << RESET;
        getline(cin, c.description);
        complaints.push_back(c);
        saveComplaints();
        log(loggedUser, "Filed complaint " + c.trackingNo + " [" + c.category + "]");

        cout << GREEN << "\n   Complaint filed! Tracking No: " << BOLD
            << c.trackingNo << RESET << "\n";
    }
    void editComplaint()
    {
        printLine('=');
        cout << BOLD << "  EDIT COMPLAINT\n" << RESET;
        printLine('=');
        vector<Complaint*> mine;
        for (auto& c : complaints)
            if (c.username == loggedUser && c.status == "Pending")
                mine.push_back(&c);
        if (mine.empty())
        {
            cout << YELLOW << "  You have no editable (Pending) complaints.\n" << RESET;
            return;
        }
        cout << CYAN << "  Your pending complaints:\n" << RESET;
        for (const auto* c : mine)
            cout << "    ID " << c->id << " | " << c->trackingNo
            << " | " << c->description << "\n";
        cout << YELLOW << "\n  Enter complaint ID to edit: " << RESET;
        int id;
        cin >> id;
        Complaint* c = findById(id);
        if (!c || c->username != loggedUser)
        {
            cout << RED << "  Complaint not found or not yours.\n" << RESET; return;
        }
        if (c->status != "Pending")
        {
            cout << RED << "  Cannot edit -- status is " << c->status << ".\n" << RESET; return;
        }
        cout << CYAN << "  What do you want to change?\n" << RESET;
        cout << "    1. Description only\n    2. Category and Priority too\n";
        int ch = pickMenu(1, 2);
        if (ch == 2)
        {
            c->category = pickCategory();
            c->priority = pickPriority();
        }
        clearInput();
        cout << CYAN << "  New description: " << RESET;
        getline(cin, c->description);

        saveComplaints();
        log(loggedUser, "Edited complaint " + c->trackingNo);

        cout << GREEN << "  Complaint updated.\n" << RESET;
    }

    void viewMyComplaints()
    {
        printLine('=');
        cout << BOLD << "  MY COMPLAINTS\n" << RESET;
        printLine('=');

        cout << CYAN << "  Filter by status:\n" << RESET;
        cout << "    1. All\n    2. Pending\n    3. Confirmed\n    4. Rejected\n";
        int ch = pickMenu(1, 4);
        string filter;
        if (ch == 2)
            filter = "Pending";
        else if (ch == 3)
            filter = "Confirmed";
        else if (ch == 4)
            filter = "Rejected";

        vector<Complaint*> result;
        for (auto& c : complaints)
        {
            if (c.username != loggedUser)
                continue;
            if (!filter.empty() && c.status != filter)
                continue;
            result.push_back(&c);
        }
        displayComplaints(result);
    }

    void searchComplaints()
    {
        printLine('=');
        cout << BOLD << "  SEARCH MY COMPLAINTS\n" << RESET;
        printLine('=');

        clearInput();
        cout << YELLOW << "  Enter keyword: " << RESET;
        string kw;
        getline(cin, kw);
        string kwl = kw;
        transform(kwl.begin(), kwl.end(), kwl.begin(), ::tolower);

        vector<Complaint*> result;
        for (auto& c : complaints)
        {
            if (c.username != loggedUser)
                continue;
            string dl = c.description;
            transform(dl.begin(), dl.end(), dl.begin(), ::tolower);
            string cl = c.category;
            transform(cl.begin(), cl.end(), cl.begin(), ::tolower);
            if (dl.find(kwl) != string::npos || cl.find(kwl) != string::npos)
                result.push_back(&c);
        }
        displayComplaints(result);
    }

    void trackComplaint()
    {
        printLine('=');
        cout << BOLD << "  TRACK COMPLAINT\n" << RESET;
        printLine('=');

        clearInput();
        cout << YELLOW << "  Enter tracking number (e.g. CMS-0001): " << RESET;
        string tn;
        getline(cin, tn);

        for (const auto& c : complaints)
        {
            if (c.trackingNo == tn)
            {
                printLine();
                c.printSummary();
                printLine();
                return;
            }
        }
        cout << RED << "  Tracking number not found.\n" << RESET;
    }

    void deleteMyComplaint()
    {
        printLine('=');
        cout << BOLD << "  DELETE MY COMPLAINT\n" << RESET;
        printLine('=');

        cout << YELLOW << "  Enter complaint ID to delete: " << RESET;
        int id;
        cin >> id;
        Complaint* c = findById(id);

        if (!c || c->username != loggedUser)
        {
            cout << RED << "  Not found or not yours.\n" << RESET;
            return;
        }
        if (c->status != "Pending")
        {
            cout << RED << "  Can only delete Pending complaints.\n" << RESET;
            return;
        }

        cout << RED << "  Are you sure? (y/n): " << RESET;
        char confirm;
        cin >> confirm;
        if (confirm != 'y' && confirm != 'Y')
        {
            cout << YELLOW << "  Cancelled.\n" << RESET;
            return;
        }

        string tn = c->trackingNo;
        complaints.erase(remove_if(complaints.begin(), complaints.end(), [id](const Complaint& x)
            {
                return x.id == id;
            }), complaints.end());
        saveComplaints();
        log(loggedUser, "Deleted complaint " + tn);

        cout << GREEN << "  Complaint deleted.\n" << RESET;
    }

    // ADMIN FEATURES


    void adminViewComplaints()
    {
        printLine('=');
        cout << BOLD << "  ALL COMPLAINTS\n" << RESET;
        printLine('=');

        cout << CYAN << "  Filter:\n" << RESET;
        cout << "    1. All\n    2. Pending only\n    3. Confirmed only\n"
            "    4. Rejected only\n    5. By priority (High first)\n";
        int ch = pickMenu(1, 5);

        vector<Complaint*> result;
        for (auto& c : complaints)
        {
            if (c.status == "Deleted") continue;
            if (ch == 2 && c.status != "Pending")   continue;
            if (ch == 3 && c.status != "Confirmed") continue;
            if (ch == 4 && c.status != "Rejected")  continue;
            result.push_back(&c);
        }

        if (ch == 5)
        {
            auto prioVal = [](const string& p) {
                if (p == "High") return 0;
                if (p == "Medium") return 1;
                return 2;
                };
            sort(result.begin(), result.end(), [&prioVal](const Complaint* a, const Complaint* b) {
                return prioVal(a->priority) < prioVal(b->priority);
                });
        }

        displayComplaints(result);
    }

    void adminSearch()
    {
        printLine('=');
        cout << BOLD << "  ADMIN SEARCH\n" << RESET;
        printLine('=');

        cout << CYAN << "  Search by:\n  1. Keyword in description\n  2. Category\n  3. Username\n" << RESET;
        int ch = pickMenu(1, 3);

        clearInput();
        string kw;
        if (ch == 1) { cout << YELLOW << "  Keyword: " << RESET; getline(cin, kw); }
        else if (ch == 2) kw = pickCategory();
        else { cout << YELLOW << "  Username: " << RESET; getline(cin, kw); }

        string kwl = kw; transform(kwl.begin(), kwl.end(), kwl.begin(), ::tolower);

        vector<Complaint*> result;
        for (auto& c : complaints)
        {
            if (c.status == "Deleted") continue;
            string target;
            if (ch == 1) target = c.description;
            else if (ch == 2) target = c.category;
            else target = c.username;
            transform(target.begin(), target.end(), target.begin(), ::tolower);
            if (target.find(kwl) != string::npos)
                result.push_back(&c);
        }
        displayComplaints(result);
    }

    void confirmComplaint()
    {
        printLine('=');
        cout << BOLD << "  CONFIRM COMPLAINT\n" << RESET;
        printLine('=');

        cout << YELLOW << "  Enter complaint ID to confirm: " << RESET;
        int id; cin >> id;
        Complaint* c = findById(id);
        if (!c) { cout << RED << "  Not found.\n" << RESET; return; }
        if (c->status != "Pending")
        {
            cout << YELLOW << "  Already " << c->status << ".\n" << RESET; return;
        }

        clearInput();
        cout << CYAN << "  Add admin note (press Enter to skip): " << RESET;
        string note; getline(cin, note);
        c->adminNote = note.empty() ? "Reviewed and confirmed." : note;
        c->status = "Confirmed";
        saveComplaints();
        log(loggedUser, "Confirmed complaint " + c->trackingNo);

        cout << GREEN << "   Complaint " << c->trackingNo << " confirmed.\n" << RESET;
    }

    void rejectComplaint()
    {
        printLine('=');
        cout << BOLD << "  REJECT COMPLAINT\n" << RESET;
        printLine('=');

        cout << YELLOW << "  Enter complaint ID to reject: " << RESET;
        int id; cin >> id;
        Complaint* c = findById(id);
        if (!c) { cout << RED << "  Not found.\n" << RESET; return; }
        if (c->status != "Pending")
        {
            cout << YELLOW << "  Already " << c->status << ".\n" << RESET; return;
        }

        clearInput();
        cout << CYAN << "  Reason for rejection (required): " << RESET;
        string note; getline(cin, note);
        if (note.empty()) note = "Rejected by admin.";
        c->adminNote = note;
        c->status = "Rejected";
        saveComplaints();
        log(loggedUser, "Rejected complaint " + c->trackingNo + " -- " + note);

        cout << GREEN << "   Complaint " << c->trackingNo << " rejected.\n" << RESET;
    }

    void adminDeleteComplaint()
    {
        printLine('=');
        cout << BOLD << "  ADMIN DELETE COMPLAINT\n" << RESET;
        printLine('=');

        cout << YELLOW << "  Enter complaint ID: " << RESET;
        int id; cin >> id;
        Complaint* c = findById(id);
        if (!c) { cout << RED << "  Not found.\n" << RESET; return; }

        cout << RED << "  Permanently mark as Deleted? (y/n): " << RESET;
        char conf; cin >> conf;
        if (conf != 'y' && conf != 'Y') { cout << YELLOW << "  Cancelled.\n" << RESET; return; }

        string tn = c->trackingNo;
        c->status = "Deleted";
        saveComplaints();
        log(loggedUser, "Admin deleted complaint " + tn);

        cout << GREEN << "   Complaint " << tn << " deleted.\n" << RESET;
    }

    void showStatistics()
    {
        printLine('=');
        cout << BOLD << "  STATISTICS REPORT\n" << RESET;
        printLine('=');

        int total = 0, pending = 0, confirmed = 0, rejected = 0, deleted = 0;
        int highPrio = 0, medPrio = 0, lowPrio = 0;

        vector<string> catNames;
        vector<int>    catCounts;


        auto getCatIdx = [&](const string& cat) -> int {
            for (int i = 0; i < (int)catNames.size(); i++)
                if (catNames[i] == cat) return i;
            catNames.push_back(cat);
            catCounts.push_back(0);
            return (int)catNames.size() - 1;
            };

        for (const auto& c : complaints)
        {
            total++;
            if (c.status == "Pending")        pending++;
            else if (c.status == "Confirmed") confirmed++;
            else if (c.status == "Rejected")  rejected++;
            else if (c.status == "Deleted")   deleted++;

            if (c.priority == "High")        highPrio++;
            else if (c.priority == "Medium") medPrio++;
            else                             lowPrio++;

            int idx = getCatIdx(c.category);
            catCounts[idx]++;
        }

        auto bar = [](int n, int tot, int width = 20) {
            int filled = (tot > 0) ? (n * width / tot) : 0;
            string b = "  [";
            for (int i = 0; i < width; i++) b += (i < filled) ? '#' : '-';
            b += "]";
            return b;
            };

        cout << BOLD << "\n  STATUS BREAKDOWN\n" << RESET;
        cout << YELLOW << bar(pending, total) << " Pending   : " << pending << "\n" << RESET;
        cout << GREEN << bar(confirmed, total) << " Confirmed : " << confirmed << "\n" << RESET;
        cout << RED << bar(rejected, total) << " Rejected  : " << rejected << "\n" << RESET;
        cout << MAGENTA << bar(deleted, total) << " Deleted   : " << deleted << "\n" << RESET;
        cout << CYAN << "  Total                            : " << total << "\n" << RESET;

        cout << BOLD << "\n  PRIORITY BREAKDOWN\n" << RESET;
        cout << RED << bar(highPrio, total) << " High   : " << highPrio << "\n" << RESET;
        cout << YELLOW << bar(medPrio, total) << " Medium : " << medPrio << "\n" << RESET;
        cout << GREEN << bar(lowPrio, total) << " Low    : " << lowPrio << "\n" << RESET;

        cout << BOLD << "\n  BY CATEGORY\n" << RESET;
        for (int i = 0; i < (int)catNames.size(); i++)
            cout << "  " << bar(catCounts[i], total) << " " << catNames[i]
            << " : " << catCounts[i] << "\n";

        printLine();
    }

    void viewAuditLog()
    {
        printLine('=');
        cout << BOLD << "  AUDIT LOG (last 20 entries)\n" << RESET;
        printLine('=');

        loadAudit();
        if (auditLogs.empty()) { cout << YELLOW << "  No log entries yet.\n" << RESET; return; }

        int start = (int)auditLogs.size() > 20 ? (int)auditLogs.size() - 20 : 0;
        for (int i = start; i < (int)auditLogs.size(); i++)
        {
            const auto& l = auditLogs[i];
            cout << CYAN << "  " << l.timestamp << RESET
                << "  " << MAGENTA << l.actor << RESET
                << "  -> " << l.action << "\n";
        }
        printLine();
    }

    void manageUsers()
    {
        while (true)
        {
            printLine('=');
            cout << BOLD << "  MANAGE USERS\n" << RESET;
            printLine('=');
            cout << "    1. List all users\n"
                "    2. Add new user\n"
                "    3. Delete a user\n"
                "    4. Back\n";
            int ch = pickMenu(1, 4);

            if (ch == 1)
            {
                listUsers();
            }
            else if (ch == 2)
            {
                clearInput();
                string uname, pass, role;
                cout << YELLOW << "  New username : " << RESET; getline(cin, uname);
                cout << YELLOW << "  Password     : " << RESET; getline(cin, pass);
                cout << CYAN << "  Role (1=user, 2=admin): " << RESET;
                int r; cin >> r; role = (r == 2) ? "admin" : "user";
                if (addUser(uname, pass, role))
                {
                    log(loggedUser, "Created account: " + uname + " [" + role + "]");

                    cout << GREEN << "   User added.\n" << RESET;
                }
                else
                    cout << RED << "  Username already exists.\n" << RESET;
            }
            else if (ch == 3)
            {
                clearInput();
                cout << YELLOW << "  Username to delete: " << RESET;
                string uname; getline(cin, uname);
                if (uname == loggedUser)
                {
                    cout << RED << "  Cannot delete yourself.\n" << RESET; continue;
                }
                if (deleteUser(uname))
                {
                    log(loggedUser, "Deleted account: " + uname);

                    cout << GREEN << "   User deleted.\n" << RESET;
                }
                else
                    cout << RED << "  User not found or is admin.\n" << RESET;
            }
            else return;
        }
    }


    // DASHBOARDS

    void userDashboard()
    {
        while (true)
        {
            printLine('=');
            cout << BOLD << CYAN << "  USER DASHBOARD" << RESET
                << "  -- " << MAGENTA << loggedUser << RESET << "\n";
            printLine('=');
            cout << "    1. File a New Complaint\n"
                "    2. Edit My Complaint\n"
                "    3. View My Complaints\n"
                "    4. Search My Complaints\n"
                "    5. Track by Tracking Number\n"
                "    6. Delete My Complaint\n"
                "    7. Logout\n";
            int ch = pickMenu(1, 7);
            if (ch == 1)      registerComplaint();
            else if (ch == 2) editComplaint();
            else if (ch == 3) viewMyComplaints();
            else if (ch == 4) searchComplaints();
            else if (ch == 5) trackComplaint();
            else if (ch == 6) deleteMyComplaint();
            else { cout << GREEN << "  Logging out...\n" << RESET; return; }
        }
    }

    void adminDashboard()
    {
        while (true)
        {
            printLine('=');
            cout << BOLD << MAGENTA << "  ADMIN DASHBOARD" << RESET
                << "  -- " << CYAN << loggedUser << RESET << "\n";
            printLine('=');
            cout << "    1.  View / Filter All Complaints\n"
                "    2.  Search Complaints\n"
                "    3.  Confirm a Complaint\n"
                "    4.  Reject a Complaint\n"
                "    5.  Delete a Complaint\n"
                "    6.  Statistics Report\n"
                "    7.  Audit Log\n"
                "    8.  Manage User Accounts\n"
                "    9.  Logout\n";
            int ch = pickMenu(1, 9);
            if (ch == 1)
                adminViewComplaints();
            else if (ch == 2) adminSearch();
            else if (ch == 3) confirmComplaint();
            else if (ch == 4) rejectComplaint();
            else if (ch == 5) adminDeleteComplaint();
            else if (ch == 6) showStatistics();
            else if (ch == 7) viewAuditLog();
            else if (ch == 8) manageUsers();
            else { cout << GREEN << "  Logging out...\n" << RESET; return; }
        }
    }


    // ENTRY POINT
    void runSystem()
    {
#ifdef _WIN32
        // Enable UTF-8 output and ANSI color support on Windows
        SetConsoleOutputCP(CP_UTF8);
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif

        loadUsers();
        loadComplaints();

        if (!hasUsers())
            registerAccounts();

        char again;
        do
        {
            bool ok = false;
            for (int attempt = 0; attempt < 3 && !ok; attempt++)
            {
                ok = login();
                if (!ok && attempt < 2)
                    cout << YELLOW << "  " << (2 - attempt) << " attempt(s) remaining.\n" << RESET;
            }

            if (!ok)
            {
                cout << RED << "  Too many failed attempts. Exiting.\n" << RESET;
                break;
            }

            if (loggedRole == "admin") adminDashboard();
            else                       userDashboard();

            cout << CYAN << "\n  Login again? (y/n): " << RESET;
            cin >> again;
        } while (again == 'y' || again == 'Y');

        cout << GREEN << BOLD << "\n  Goodbye! Thank you for using the Complaint Management System.\n" << RESET;
    }
};

// main

int main()
{
    cout << CYAN << BOLD << R"(   ___ ___  __  __ ___ _      _   ___ _  _ _____   __  __   _   _  _   _   ___ ___ __  __ ___ _  _ _____   _____   _____ _____ ___ __  __ 
  / __/ _ \|  \/  | _ \ |    /_\ |_ _| \| |_   _| |  \/  | /_\ | \| | /_\ / __| __|  \/  | __| \| |_   _| / __\ \ / / __|_   _| __|  \/  |
 | (_| (_) | |\/| |  _/ |__ / _ \ | || .` | | |   | |\/| |/ _ \| .` |/ _ \ (_ | _|| |\/| | _|| .` | | |   \__ \\ V /\__ \ | | | _|| |\/| |
  \___\___/|_|  |_|_| |____/_/ \_\___|_|\_| |_|   |_|  |_/_/ \_\_|\_/_/ \_\___|___|_|  |_|___|_|\_| |_|   |___/ |_| |___/ |_| |___|_|  |_|
                                                                                                                                         

)" << RESET << "\n";

    Dashboard system;
    system.runSystem();
    return 0;
}