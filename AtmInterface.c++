#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <map>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

struct Transaction {
    string type;
    double amount;
    string date;
    double balance_after;
    
    Transaction(string t, double a, string d, double b) 
        : type(t), amount(a), date(d), balance_after(b) {}
};

class Account {
private:
    string account_number;
    string pin;
    string holder_name;
    double balance;
    vector<Transaction> transaction_history;
    queue<string> pending_notifications;
    stack<string> recent_activities;
    
public:
    Account(string acc_num, string p, string name, double initial_balance) 
        : account_number(acc_num), pin(p), holder_name(name), balance(initial_balance) {
        addTransaction("Account Created", initial_balance, balance);
    }
    
    string getAccountNumber() const { return account_number; }
    string getPin() const { return pin; }
    string getHolderName() const { return holder_name; }
    double getBalance() const { return balance; }
    
    void addTransaction(string type, double amount, double new_balance) {
        time_t now = time(0);
        string date = ctime(&now);
        date = date.substr(0, date.length() - 1);
        
        transaction_history.push_back(Transaction(type, amount, date, new_balance));
        recent_activities.push(type + " - Rs." + to_string(amount));
        
        if (recent_activities.size() > 10) {
            stack<string> temp;
            for (int i = 0; i < 10; i++) {
                temp.push(recent_activities.top());
                recent_activities.pop();
            }
            recent_activities = temp;
        }
    }
    
    bool deposit(double amount) {
        if (amount <= 0) {
            pending_notifications.push("Invalid deposit amount!");
            return false;
        }
        balance += amount;
        addTransaction("Deposit", amount, balance);
        pending_notifications.push("Deposit successful! New balance: Rs." + to_string(balance));
        return true;
    }
    
    bool withdraw(double amount) {
        if (amount <= 0) {
            pending_notifications.push("Invalid withdrawal amount!");
            return false;
        }
        if (amount > balance) {
            pending_notifications.push("Insufficient funds!");
            return false;
        }
        balance -= amount;
        addTransaction("Withdrawal", amount, balance);
        pending_notifications.push("Withdrawal successful! New balance: Rs." + to_string(balance));
        return true;
    }
    
    bool transfer(double amount, string recipient_acc) {
        if (amount <= 0) {
            pending_notifications.push("Invalid transfer amount!");
            return false;
        }
        if (amount > balance) {
            pending_notifications.push("Insufficient funds for transfer!");
            return false;
        }
        balance -= amount;
        addTransaction("Transfer to " + recipient_acc, amount, balance);
        pending_notifications.push("Transfer successful! New balance: Rs." + to_string(balance));
        return true;
    }
    
    void displayTransactionHistory() {
        if (transaction_history.empty()) {
            cout << "No transactions found.\n";
            return;
        }
        
        cout << "\n=== TRANSACTION HISTORY ===\n";
        cout << left << setw(20) << "Type" << setw(15) << "Amount" 
             << setw(25) << "Date" << setw(15) << "Balance" << endl;
        cout << string(75, '-') << endl;
        
        vector<Transaction> sorted_transactions = transaction_history;
        sort(sorted_transactions.begin(), sorted_transactions.end(), 
             [](const Transaction& a, const Transaction& b) {
                 return a.date > b.date;
             });
        
        for (const auto& txn : sorted_transactions) {
            cout << left << setw(20) << txn.type << setw(15) << fixed << setprecision(2) << txn.amount
                 << setw(25) << txn.date << setw(15) << txn.balance_after << endl;
        }
    }
    
    void displayRecentActivities() {
        if (recent_activities.empty()) {
            cout << "No recent activities.\n";
            return;
        }
        
        cout << "\n=== RECENT ACTIVITIES ===\n";
        stack<string> temp = recent_activities;
        int count = 1;
        while (!temp.empty() && count <= 5) {
            cout << count << ". " << temp.top() << endl;
            temp.pop();
            count++;
        }
    }
    
    void displayNotifications() {
        if (pending_notifications.empty()) {
            cout << "No notifications.\n";
            return;
        }
        
        cout << "\n=== NOTIFICATIONS ===\n";
        while (!pending_notifications.empty()) {
            cout << "• " << pending_notifications.front() << endl;
            pending_notifications.pop();
        }
    }
    
    bool changePIN(string old_pin, string new_pin) {
        if (old_pin != pin) {
            pending_notifications.push("Invalid old PIN!");
            return false;
        }
        if (new_pin.length() != 4) {
            pending_notifications.push("PIN must be 4 digits!");
            return false;
        }
        pin = new_pin;
        addTransaction("PIN Changed", 0, balance);
        pending_notifications.push("PIN changed successfully!");
        return true;
    }
};

class ATM {
private:
    unordered_map<string, Account*> accounts;
    map<string, int> login_attempts;
    Account* current_account;
    
    bool binarySearchAccount(const vector<string>& account_nums, const string& target) {
        int left = 0, right = account_nums.size() - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (account_nums[mid] == target) return true;
            if (account_nums[mid] < target) left = mid + 1;
            else right = mid - 1;
        }
        return false;
    }
    
public:
    ATM() : current_account(nullptr) {
        accounts["1234567890"] = new Account("1234567890", "1234", "Shubham kumar", 10000.0);
        accounts["0987654321"] = new Account("0987654321", "4321", "Navneet parmar", 15000.0);
        accounts["1111222233"] = new Account("1111222233", "9999", "Sikandar", 5000.0);
    }
    
    ~ATM() {
        for (auto& pair : accounts) {
            delete pair.second;
        }
    }
    
    bool createAccount(string acc_num, string pin, string name, double initial_deposit) {
        if (accounts.find(acc_num) != accounts.end()) {
            cout << "Account already exists!\n";
            return false;
        }
        if (pin.length() != 4) {
            cout << "PIN must be 4 digits!\n";
            return false;
        }
        if (initial_deposit < 500) {
            cout << "Minimum initial deposit is Rs.500!\n";
            return false;
        }
        
        accounts[acc_num] = new Account(acc_num, pin, name, initial_deposit);
        cout << "Account created successfully!\n";
        return true;
    }
    
    bool login(string acc_num, string pin) {
        if (accounts.find(acc_num) == accounts.end()) {
            cout << "Account not found!\n";
            return false;
        }
        
        if (login_attempts[acc_num] >= 3) {
            cout << "Account locked due to too many failed attempts!\n";
            return false;
        }
        
        Account* account = accounts[acc_num];
        if (account->getPin() != pin) {
            login_attempts[acc_num]++;
            cout << "Invalid PIN! Attempts remaining: " << (3 - login_attempts[acc_num]) << endl;
            return false;
        }
        
        current_account = account;
        login_attempts[acc_num] = 0;
        cout << "Login successful! Welcome, " << account->getHolderName() << "!\n";
        return true;
    }
    
    void logout() {
        current_account = nullptr;
        cout << "Logged out successfully!\n";
    }
    
    bool isLoggedIn() const {
        return current_account != nullptr;
    }
    
    Account* getCurrentAccount() const {
        return current_account;
    }
    
    void displayAllAccounts() {
        cout << "\n=== ALL ACCOUNTS ===\n";
        cout << left << setw(15) << "Account No." << setw(20) << "Holder Name" 
             << setw(15) << "Balance" << endl;
        cout << string(50, '-') << endl;
        
        for (const auto& pair : accounts) {
            Account* acc = pair.second;
            cout << left << setw(15) << acc->getAccountNumber() 
                 << setw(20) << acc->getHolderName()
                 << setw(15) << fixed << setprecision(2) << acc->getBalance() << endl;
        }
    }
};

class ATMInterface {
private:
    ATM atm;
    
    void displayMainMenu() {
        cout << "\n" << string(40, '=') << endl;
        cout << "ATM MANAGEMENT SYSTEM" << endl;
        cout << string(40, '=') << endl;
        cout << "1. Create New Account" << endl;
        cout << "2. Login" << endl;
        cout << "3. Admin Panel" << endl;
        cout << "4. Exit" << endl;
        cout << "Enter your choice: ";
    }
    
    void displayAccountMenu() {
        cout << "\n" << string(40, '=') << endl;
        cout << "         ACCOUNT MENU" << endl;
        cout << string(40, '=') << endl;
        cout << "1. Check Balance" << endl;
        cout << "2. Deposit Money" << endl;
        cout << "3. Withdraw Money" << endl;
        cout << "4. Transfer Money" << endl;
        cout << "5. Transaction History" << endl;
        cout << "6. Recent Activities" << endl;
        cout << "7. Notifications" << endl;
        cout << "8. Change PIN" << endl;
        cout << "9. Logout" << endl;
        cout << "Enter your choice: ";
    }
    
    void handleCreateAccount() {
        string acc_num, pin, name;
        double initial_deposit;
        
        cout << "\n=== CREATE NEW ACCOUNT ===\n";
        cout << "Enter account number (10 digits): ";
        cin >> acc_num;
        cout << "Enter 4-digit PIN: ";
        cin >> pin;
        cin.ignore();
        cout << "Enter account holder name: ";
        getline(cin, name);
        cout << "Enter initial deposit (min Rs.500): ";
        cin >> initial_deposit;
        
        atm.createAccount(acc_num, pin, name, initial_deposit);
    }
    
    void handleLogin() {
        string acc_num, pin;
        
        cout << "\n=== LOGIN ===\n";
        cout << "Enter account number: ";
        cin >> acc_num;
        cout << "Enter PIN: ";
        cin >> pin;
        
        atm.login(acc_num, pin);
    }
    
    void handleAccountOperations() {
        while (atm.isLoggedIn()) {
            displayAccountMenu();
            
            int choice;
            cin >> choice;
            
            Account* account = atm.getCurrentAccount();
            
            switch (choice) {
                case 1: {
                    cout << "\nCurrent Balance: Rs." << fixed << setprecision(2) 
                         << account->getBalance() << endl;
                    break;
                }
                case 2: {
                    double amount;
                    cout << "Enter deposit amount: Rs.";
                    cin >> amount;
                    account->deposit(amount);
                    break;
                }
                case 3: {
                    double amount;
                    cout << "Enter withdrawal amount: Rs.";
                    cin >> amount;
                    account->withdraw(amount);
                    break;
                }
                case 4: {
                    double amount;
                    string recipient;
                    cout << "Enter transfer amount: Rs.";
                    cin >> amount;
                    cout << "Enter recipient account number: ";
                    cin >> recipient;
                    account->transfer(amount, recipient);
                    break;
                }
                case 5: {
                    account->displayTransactionHistory();
                    break;
                }
                case 6: {
                    account->displayRecentActivities();
                    break;
                }
                case 7: {
                    account->displayNotifications();
                    break;
                }
                case 8: {
                    string old_pin, new_pin;
                    cout << "Enter current PIN: ";
                    cin >> old_pin;
                    cout << "Enter new PIN: ";
                    cin >> new_pin;
                    account->changePIN(old_pin, new_pin);
                    break;
                }
                case 9: {
                    atm.logout();
                    break;
                }
                default: {
                    cout << "Invalid choice! Please try again.\n";
                    break;
                }
            }
        }
    }
    
    void handleAdminPanel() {
        string admin_password;
        cout << "Enter admin password: ";
        cin >> admin_password;
        
        if (admin_password == "Skp123") {
            cout << "\n=== ADMIN PANEL ===\n";
            atm.displayAllAccounts();
        } else {
            cout << "Invalid admin password!\n";
        }
    }
    
public:
    void run() {
        cout << "Welcome to the ATM System!" << endl;
        
        while (true) {
            displayMainMenu();
            
            int choice;
            cin >> choice;
            
            switch (choice) {
                case 1: {
                    handleCreateAccount();
                    break;
                }
                case 2: {
                    handleLogin();
                    if (atm.isLoggedIn()) {
                        handleAccountOperations();
                    }
                    break;
                }
                case 3: {
                    handleAdminPanel();
                    break;
                }
                case 4: {
                    cout << "Thank you for using ATM System. Goodbye!\n";
                    return;
                }
                default: {
                    cout << "Invalid choice! Please try again.\n";
                    break;
                }
            }
        }
    }
};

int main() {
    ATMInterface interface;
    interface.run();
    return 0;
}