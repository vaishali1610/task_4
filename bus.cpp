#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

using namespace std;
 
vector<string> split(const string &line, char delimiter) {
    vector<string> tokens;
    string token;
    stringstream ss(line);
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}
bool fileExists(const string &filename) {
    ifstream f(filename);
    return f.good();
}
bool isValidPhone(const string &phone) {
    if (phone.size() != 10) return false;
    for (char c : phone) if (!isdigit(c)) return false;
    return true;
}
bool isValidEmail(const string &email) {
    size_t atPos = email.find('@');
    size_t dotPos = email.rfind(".com");
    if (atPos == string::npos || dotPos == string::npos) return false;
    if (atPos == 0 || atPos > dotPos) return false;
    return true;
}
bool registerUser() {
    string name, phone, email, password;
    cout << "Enter Name: "; getline(cin, name);
    do {
        cout << "Enter Phone (10 digits): ";
        getline(cin, phone);
        if (!isValidPhone(phone)) cout << "Invalid phone number,Try again.\n";
    }
    while (!isValidPhone(phone));
    do {
        cout << "Enter Email (must contain @ and end with .com): ";
        getline(cin, email);
        if (!isValidEmail(email)) cout << "Invalid email! Try again.\n";
    } while (!isValidEmail(email));
 
    cout << "Enter Password: "; getline(cin, password);
    bool hasDigit = false, hasUpper = false;
    
    if (password.length() < 6) {
        cout << "Password must be at least 6 characters long." << endl;
    } else {
        for (char ch : password) {
            if (isdigit(ch)) hasDigit = true;
            if (isupper(ch)) hasUpper = true;
        }

        if (!hasDigit || !hasUpper) {
            cout << "Password must include at least one uppercase letter and one digit." << endl;
        } else {
            cout << "Password is valid!" << endl;
        }
    }

 
    ifstream fin("users.csv");
    string line;
    while (getline(fin, line)) {
        vector<string> row = split(line, ',');
        if (!row.empty() && row[1] == phone) {
            cout << "User already exists!\n";
            return false;
        }
    }
    fin.close();
 
    ofstream fout("users.csv", ios::app);
    fout << name << "," << phone << "," << email << "," << password << "\n";
    fout.close();
    cout << "Registration successful!\n";
    return true;
}
 
bool loginUser(string &loggedPhone) {
    string phone, password;
    cout << "Enter Phone: "; getline(cin, phone);
    cout << "Enter Password: "; getline(cin, password);
 
    ifstream fin("users.csv");
    string line;
    while (getline(fin, line)) {
        vector<string> row = split(line, ',');
        if (row.size() >= 4 && row[1] == phone && row[3] == password) {
            loggedPhone = phone;
            cout << "Login successful!\n";
            return true;
        }
    }
    cout << "Invalid login!\n";
    return false;
}
vector<vector<string>> loadBuses() {
    vector<vector<string>> buses;
    ifstream fin("board.csv");
    string line;
    while (getline(fin, line)) buses.push_back(split(line, ','));
    return buses;
}
 
void saveBuses(const vector<vector<string>> &buses) {
    ofstream fout("board.csv");
    for (auto &row : buses) {
        for (size_t i = 0; i < row.size(); i++) {
            fout << row[i];
            if (i < row.size()-1) fout << ",";
        }
        fout << "\n";
    }
}

bool paymentProcess() {
    cout << "\nChoose Payment Method:\n";
    cout << "1. UPI\n2. Net Banking\n3. Credit Card\nChoice: ";
    int choice; cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
 
    switch (choice) {
        case 1: cout << "Processing UPI Payment...\n"; break;
        case 2: cout << "Processing Net Banking Payment...\n"; break;
        case 3: cout << "Processing Credit Card Payment...\n"; break;
        default: cout << "Invalid payment method!\n"; return false;
    }
    this_thread::sleep_for(chrono::seconds(2));
    cout << "Payment Successful!\n";
    return true;
}
void trackBus(const string &busID) {
    ifstream fin("checkpoint.csv");
    string line;
    vector<string> checkpoints;
   
    while(getline(fin, line)) {
        vector<string> row = split(line, ',');
        if(row[0] == busID) {
            for(size_t i = 1; i < row.size(); i++)
                checkpoints.push_back(row[i]);
            break;
        }
    }
   
    if(checkpoints.empty()) {
        cout << "No bus found with ID " << busID << "\n";
        return;
    }
   
    string boarding = checkpoints.front();
    string drop = checkpoints.back();
   
    cout << "\nLive Tracking for Bus " << busID << ":\n";
    cout << "Starting from: " << boarding << "\n";
    this_thread::sleep_for(chrono::seconds(2));
   
    for(size_t i = 1; i < checkpoints.size()-1; i++) {
        cout << "Reached: " << checkpoints[i] << "\n";
        this_thread::sleep_for(chrono::seconds(2));
    }
   
    cout << "Final Destination: " << drop << "\n";
}
vector<vector<string>> searchBuses(string from, string to, string date) {
    vector<vector<string>> all = loadBuses();
    vector<vector<string>> result;
    for (auto &b : all) {
        if (b.size() >= 6 && b[1] == from && b[2] == to && b[3] == date) {
            result.push_back(b);
        }
    }
    return result;
}
bool bookSeat(string phone) {
    string from, to, date;
    cout << "Enter From: ";
    getline(cin, from);
    cout << "Enter To: ";
    getline(cin, to);
    cout << "Enter Date (yyyy-mm-dd): ";
    getline(cin, date);
 
    vector<vector<string>> result = searchBuses(from, to, date);
    if (result.empty()) {
        cout << "No buses found!\n";
        return false;
    }
 
    cout << "\nAvailable Buses:\n";
    for (size_t i = 0; i < result.size(); i++) {
        cout << i + 1
             << ". BusID: " << result[i][0]
             << " | " << result[i][1] << " → " << result[i][2]
             << " | Date: " << result[i][3]
             << " | Time: " << result[i][4]
             << " | Seats: " << result[i][5]
             << " | Boarding: " << result[i][6]
             << " | Drop: " << result[i][7]
             << " | Amount per seat: " << result[i][8] << "\n";
    }
 
    int choice;
    cout << "Choose bus: ";
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
 
    if (choice < 1 || choice > (int)result.size()) {
        cout << "Invalid bus choice!\n";
        return false;
    }
 
    int availableSeats = stoi(result[choice - 1][5]);
    cout << "How many seats do you want to book? ";
    int numSeats;
    cin >> numSeats;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
 
    if (numSeats <= 0 || numSeats > availableSeats) {
        cout << "Invalid seat count!\n";
        return false;
    }
    int amountPerSeat = stoi(result[choice - 1][8]);
    int totalAmount = numSeats * amountPerSeat;
    cout << "Total Amount to Pay: " << totalAmount << "\n"; 
    vector<pair<string, string>> passengers;
    for (int i = 0; i < numSeats; i++) {
        string pname, age;
        cout << "Passenger " << i + 1 << " Name: ";
        getline(cin, pname);
        cout << "Passenger " << i + 1 << " Age: ";
        getline(cin, age); 
        int ageInt = stoi(age);
        if (ageInt <= 3 || ageInt >= 100) {
            cout << "Invalid age.\n";
            return false;
        }
 
        passengers.push_back({pname, age});
    }
 
    if (!paymentProcess()) {
        cout << "Payment failed.\n";
        return false;
    }
    vector<vector<string>> all = loadBuses();
    for (auto &b : all) {
        if (b[0] == result[choice - 1][0]) {
            int s = stoi(b[5]);
            b[5] = to_string(s - numSeats);
        }
    }
    saveBuses(all);
    ofstream fout("tickets.csv", ios::app);
    for (auto &p : passengers) {
        string ticketId = "T" + to_string(rand() % 10000);
        fout << ticketId << "," << phone << "," << result[choice - 1][0]
             << "," << p.first << "," << p.second
             << "," << result[choice - 1][6] 
             << "," << result[choice - 1][7] 
             << "\n";
        cout << "Ticket booked! Ticket ID: " << ticketId
             << " | Passenger: " << p.first << "\n";
    }
    fout.close();
    cout << "Do you want to track the bus now? (y/n): ";
    char t;
    cin >> t;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (t == 'y' || t == 'Y') trackBus(result[choice - 1][0]); 
    return true;
}
 
int main() {
    srand(time(0));
    if (!fileExists("users.csv")) ofstream("users.csv").close();
    string loggedPhone;
    cout << "Red Bus\n";
    while (true) {
        cout << "\n1. Register\n2. Login\n3. Exit\nChoice: ";
        int ch;
        cin >> ch;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter a number.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (ch == 1) {
            registerUser();
        } else if (ch == 2) {
            if (loginUser(loggedPhone)) {
                int sub;
                do {
                    cout << "\n1. Book Ticket\n2. Logout\nChoice: ";
                    cin >> sub;
                    if (cin.fail()) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid input! Please enter a number.\n";
                        continue;
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    if (sub == 1) bookSeat(loggedPhone);
                    else if (sub == 2) break;
                    else cout << "Invalid choice!\n";
                } while (sub != 2);
            }
        } else if (ch == 3) {
            cout << "Bon Voyage!\n";
            break;
        } else {
            cout << "Invalid choice!\n";
        }
    }
    return 0;
}

