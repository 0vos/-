#include <iostream>
#include <signal.h>
#include <string>
#include <locale>
#include <codecvt>

using namespace std;

wstring string_to_wstring(const string &str) {
    wstring_convert<codecvt_utf8_utf16<wchar_t> > converter;
    return converter.from_bytes(str);
}

string wstring_to_string(const wstring &wstr) {
    wstring_convert<codecvt_utf8_utf16<wchar_t> > converter;
    return converter.to_bytes(wstr);
}

void handle_segfault(int signum) {
    cerr << "Segmentation fault detected. Exiting program." << endl;
    exit(EXIT_FAILURE);
}

int repeat(int i, int j, int *next, const wstring &in_test) {
    if (i == 0) {
        return 0;
    } else {
        i--;
        i = next[i];
        if (in_test[i] == in_test[j]) {
            return i;
        } else {
            i = repeat(i, j, next, in_test);
            return i;
        }
    }
}

string search(const string &test, const string &in_test) {
    wstring wtest = string_to_wstring(test);
    wstring win_test = string_to_wstring(in_test);

    int *next = new int[win_test.size()];
    for (size_t i = 0; i < win_test.size(); ++i) {
        next[i] = 0;
    }

    int i = 0, j = 1;
    while (j < win_test.size()) {
        if (win_test[j] == win_test[i]) {
            next[j] = i + 1;
            i++;
            j++;
        } else {
            if (i == 0) {
                j++;
            } else {
                i = repeat(i, j, next, win_test);
            }
        }
    }

    i = 0;
    j = 0;
    int judge = 0;
    string position = "";

    while (j <= wtest.size()) {
        if (i == win_test.size()) {
            judge++;
            position += to_string(j - win_test.size());
            position += "_";
            i = 0;
        } else {
            if (wtest[j] == win_test[i]) {
                i++;
                j++;
            } else {
                if (i == 0) {
                    j++;
                } else {
                    i = next[i - 1];
                    if (wtest[j] == win_test[i]) {
                        i++;
                    }
                    j++;
                }
            }
        }
    }

    position += to_string(judge);
    delete[] next;
    return position;
}

int count_num_of_same(const string &test, const string &in_test) {
    string position = search(test, in_test);
    size_t last_delim = position.find_last_of('_');
    if (last_delim == string::npos) {
        return 0;
    }
    string num_of_position = position.substr(last_delim + 1);
    return stoi(num_of_position);
}

void posion_of_number(int *p, const string &test, const string &in_test) {
    string position = search(test, in_test);
    string temp_position = "";
    int j = 0, k = 0;

    while (position[j] != '\0') {
        if (position[j] != '_') {
            temp_position += position[j];
        } else {
            p[k] = stoi(temp_position);
            k++;
            temp_position = "";
        }
        j++;
    }
}

string get_total_part(const string &test, const string &in_test) {
    try {
        wstring wtest = string_to_wstring(test);
        wstring win_test = string_to_wstring(in_test);

        int count = count_num_of_same(test, in_test);
        int *p = new int[count];
        posion_of_number(p, test, in_test);

        if (count == 0) {
            delete[] p;
            return "";
        }

        wstring final_all = L"";
        for (int j = 0; j < count; j++) {
            wstring part1, part2;
            if (p[j] - 6 > 0) {
                part1 = wtest.substr(p[j] - 6, 6);
                if (p[j] + win_test.size() + 6 < wtest.size()) {
                    part2 = wtest.substr(p[j] + win_test.size(), 6);
                } else {
                    part2 = wtest.substr(p[j] + win_test.size());
                }
            } else {
                part1 = wtest.substr(0, p[j]);
                if (p[j] + win_test.size() + 6 < wtest.size()) {
                    part2 = wtest.substr(p[j] + win_test.size(), 6);
                } else {
                    part2 = wtest.substr(p[j] + win_test.size());
                }
            }

            final_all += to_wstring(j + 1) + L"." + part1 + L"(" + win_test + L")" + part2 + L"\n";
        }

        delete[] p;
        string result = wstring_to_string(final_all);
        cout << result;
        return result;

    } catch (const std::exception &e) {
        cerr << "Error occurred in get_total_part function: " << e.what() << endl;
        return "Not Found";
    }
    return "Not Found";
}

// int main() {
//     // Register the segmentation fault handler
//     signal(SIGSEGV, handle_segfault);

//     string test = "This is a sample test string for testing string functions.";
//     string in_test = "test";

//     cout << get_total_part(test, in_test) << endl;

//     return 0;
// }