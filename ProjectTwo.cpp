#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

/*
Project Two - Advising Assistance Program
- Loads course data from a CSV file into a hash table (separate chaining)
- Prints a sorted list of all courses (alphanumeric by course number)
- Prints a single course with its prerequisites
*/

struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
    int sourceLine = 0;  // Used for line-specific validation messages.
};

struct Node {
    Course course;
    Node* next = nullptr;

    explicit Node(const Course& c) : course(c), next(nullptr) {}
};

class HashTable {
public:
    explicit HashTable(size_t bucketCountIn = 179)
        : bucketCount(bucketCountIn), buckets(bucketCountIn, nullptr) {}

    ~HashTable() { Clear(); }

    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    void Clear() {
        // Release all nodes in every bucket to avoid memory leaks.
        for (size_t i = 0; i < bucketCount; ++i) {
            Node* current = buckets[i];
            while (current != nullptr) {
                Node* next = current->next;
                delete current;
                current = next;
            }
            buckets[i] = nullptr;
        }
    }

    void Insert(const Course& c) {
        const size_t idx = IndexForKey(c.courseNumber);

        // Separate chaining: search the chain first. If duplicate, overwrite.
        // Overwrite behavior is intentional: one record per courseNumber.
        for (Node* cur = buckets[idx]; cur != nullptr; cur = cur->next) {
            if (cur->course.courseNumber == c.courseNumber) {
                cur->course = c;
                return;
            }
        }

        // Not found in chain - prepend a new node to this bucket.
        Node* n = new Node(c);
        n->next = buckets[idx];
        buckets[idx] = n;
    }

    const Course* Search(const string& courseNumber) const {
        const size_t idx = IndexForKey(courseNumber);

        // Lookup is limited to one bucket chain (average O(1) behavior).
        for (Node* cur = buckets[idx]; cur != nullptr; cur = cur->next) {
            if (cur->course.courseNumber == courseNumber) {
                return &cur->course;
            }
        }
        return nullptr;
    }

    vector<Course> GetAllCourses() const {
        // Used for printing sorted output and validation passes.
        vector<Course> courses;
        for (size_t i = 0; i < bucketCount; ++i) {
            for (Node* cur = buckets[i]; cur != nullptr; cur = cur->next) {
                courses.push_back(cur->course);
            }
        }
        return courses;
    }

private:
    size_t bucketCount;
    vector<Node*> buckets;

    static unsigned int HashString(const string& key) {
        // Simple polynomial rolling hash (31). Good enough for this project.
        unsigned int h = 0;
        for (unsigned char ch : key) {
            h = (h * 31u) + static_cast<unsigned int>(ch);
        }
        return h;
    }

    size_t IndexForKey(const string& key) const {
        return static_cast<size_t>(HashString(key) % static_cast<unsigned int>(bucketCount));
    }
};

static string Trim(const string& str) {
    // Removes leading/trailing spaces, tabs, and CRLF artifacts (notably '\r').
    const string whitespace = " \t\n\r";
    const size_t first = str.find_first_not_of(whitespace);
    if (first == string::npos) {
        return "";
    }
    const size_t last = str.find_last_not_of(whitespace);
    return str.substr(first, (last - first + 1));
}

static string ToUpper(const string& str) {
    // Normalizes course IDs so lookups are case-insensitive.
    string out = str;
    for (char& ch : out) {
        ch = static_cast<char>(toupper(static_cast<unsigned char>(ch)));
    }
    return out;
}

static void StripBOM(string& str) {
    // Removes UTF-8 BOM bytes (0xEF, 0xBB, 0xBF) when present at start of file.
    // This prevents invisible bytes from corrupting the first courseNumber key.
    if (str.size() >= 3) {
        const unsigned char b0 = static_cast<unsigned char>(str[0]);
        const unsigned char b1 = static_cast<unsigned char>(str[1]);
        const unsigned char b2 = static_cast<unsigned char>(str[2]);
        if (b0 == 0xEF && b1 == 0xBB && b2 == 0xBF) {
            str.erase(0, 3);
        }
    }
}

static vector<string> Split(const string& line, char delimiter) {
    // Note: This is a simple split that assumes commas do not appear inside fields.
    // Standard ABCU course files adhere to this format (no quoted commas).
    vector<string> tokens;
    string token;
    stringstream ss(line);

    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

static string NormalizeId(const string& raw) {
    return ToUpper(Trim(raw));
}

static bool ParseLineToCourse(const string& line,
                              int lineNumber,
                              bool isFirstDataLine,
                              Course& outCourse,
                              string& err) {
    // Converts one CSV line into a Course object.
    // Returns false if the line is blank or malformed (missing required fields).
    outCourse = Course{};
    outCourse.sourceLine = lineNumber;
    err.clear();

    const string trimmedLine = Trim(line);
    if (trimmedLine.empty()) {
        err = "blank line";
        return false;
    }

    vector<string> tokens = Split(trimmedLine, ',');
    if (tokens.size() < 2) {
        err = "invalid course number or title";
        return false;
    }

    // Trim each token so "CSCI101, Intro" and "CSCI101,Intro" behave the same.
    for (string& t : tokens) {
        t = Trim(t);
    }

    // BOM can only appear at the start of the file, so only strip on first data line.
    if (isFirstDataLine) {
        StripBOM(tokens[0]);
    }

    outCourse.courseNumber = NormalizeId(tokens[0]);
    outCourse.courseTitle = Trim(tokens[1]);

    if (outCourse.courseNumber.empty() || outCourse.courseTitle.empty()) {
        err = "invalid course number or title";
        return false;
    }

    // Remaining tokens (if any) are prerequisites (IDs only).
    for (size_t i = 2; i < tokens.size(); ++i) {
        const string prereqId = NormalizeId(tokens[i]);
        if (!prereqId.empty()) {
            outCourse.prerequisites.push_back(prereqId);
        }
    }

    return true;
}

static int ValidatePrerequisites(const HashTable& table) {
    // Post-load referential integrity check.
    // Must run after all courses are loaded to avoid false errors on forward references.
    int errorCount = 0;
    const vector<Course> courses = table.GetAllCourses();

    for (const Course& c : courses) {
        for (const string& prereq : c.prerequisites) {
            if (table.Search(prereq) == nullptr) {
                cout << "Error line " << c.sourceLine << ": " << c.courseNumber
                     << " missing prerequisite " << prereq << endl;
                ++errorCount;
            }
        }
    }

    return errorCount;
}

static void PrintCourseListSorted(const HashTable& table) {
    // Hash table is unordered, so extract to a vector then sort.
    vector<Course> courses = table.GetAllCourses();
    sort(courses.begin(), courses.end(), [](const Course& a, const Course& b) {
        return a.courseNumber < b.courseNumber;
    });

    cout << "Here is a sample schedule:" << endl;
    for (const Course& c : courses) {
        cout << c.courseNumber << ", " << c.courseTitle << endl;
    }
}

static void PrintSingleCourse(const HashTable& table, const string& rawId) {
    // Looks up one course and prints title and prerequisites.
    const string id = NormalizeId(rawId);
    const Course* c = table.Search(id);

    if (c == nullptr) {
        cout << "Course " << id << " not found." << endl;
        return;
    }

    cout << c->courseNumber << ", " << c->courseTitle << endl;

    if (c->prerequisites.empty()) {
        // Some samples accept "None". Keep it explicit and readable.
        cout << "Prerequisites: None" << endl;
        return;
    }

    cout << "Prerequisites: ";
    for (size_t i = 0; i < c->prerequisites.size(); ++i) {
        cout << c->prerequisites[i];
        if (i + 1 < c->prerequisites.size()) {
            cout << ", ";
        }
    }
    cout << endl;
}

static bool LoadCoursesFromFile(const string& fileName, HashTable& table) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Could not open file: " << fileName << endl;
        return false;
    }

    // Reload should replace previous data completely.
    table.Clear();

    string line;
    int lineNumber = 0;
    bool isFirstNonEmptyLine = true;

    while (getline(file, line)) {
        ++lineNumber;

        // Skip blank lines to keep parsing predictable.
        if (Trim(line).empty()) {
            continue;
        }

        Course c;
        string err;
        const bool ok = ParseLineToCourse(line, lineNumber, isFirstNonEmptyLine, c, err);

        // Only the first non-empty line can contain a BOM.
        if (isFirstNonEmptyLine) {
            isFirstNonEmptyLine = false;
        }

        if (!ok) {
            if (err != "blank line") {
                cout << "Format error line " << lineNumber << ": " << err << endl;
            }
            continue;
        }

        table.Insert(c);
    }

    file.close();

    // Validation warnings do not block program use, but they provide required reporting.
    ValidatePrerequisites(table);

    // If nothing valid was loaded, treat load as failed to prevent empty printing.
    if (table.GetAllCourses().empty()) {
        cout << "No valid courses loaded." << endl;
        return false;
    }

    return true;
}

static void PrintMenu() {
    cout << "1. Load Data Structure." << endl;
    cout << "2. Print Course List." << endl;
    cout << "3. Print Course." << endl;
    cout << "9. Exit" << endl;
}

struct MenuInput {
    bool parsed = false;
    int choice = -1;
    string raw;
};

static MenuInput ReadMenuChoice() {
    // Reads an entire line so bad input cannot poison cin state.
    MenuInput mi;
    getline(cin, mi.raw);
    mi.raw = Trim(mi.raw);

    if (mi.raw.empty()) {
        return mi;
    }

    // Accept only a clean integer token (rejects "2a").
    stringstream ss(mi.raw);
    ss >> mi.choice;
    if (!ss.fail() && ss.eof()) {
        mi.parsed = true;
    }

    return mi;
}

int main() {
    cout << "Welcome to the course planner." << endl;

    HashTable table(179);
    bool loaded = false;

    while (true) {
        PrintMenu();
        cout << "What would you like to do? ";

        const MenuInput mi = ReadMenuChoice();

        if (mi.parsed && mi.choice == 1) {
            cout << "Enter the file name: ";
            string fileName;
            getline(cin, fileName);
            fileName = Trim(fileName);

            loaded = LoadCoursesFromFile(fileName, table);

        } else if (mi.parsed && mi.choice == 2) {
            if (!loaded) {
                cout << "Please load data first." << endl;
                continue;
            }
            PrintCourseListSorted(table);

        } else if (mi.parsed && mi.choice == 3) {
            if (!loaded) {
                cout << "Please load data first." << endl;
                continue;
            }
            cout << "What course do you want to know about? ";
            string id;
            getline(cin, id);
            PrintSingleCourse(table, id);

        } else if (mi.parsed && mi.choice == 9) {
            cout << "Thank you for using the course planner!" << endl;
            break;

        } else {
            // Ignore empty input; otherwise match the sample invalid-option pattern.
            if (!mi.raw.empty()) {
                cout << mi.raw << " is not a valid option." << endl;
            }
        }
    }

    return 0;
}
