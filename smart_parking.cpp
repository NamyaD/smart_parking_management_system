#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <ctime>
#include <iomanip>
#include <algorithm>
using namespace std;

class Vehicle {
private:
    string vehicleNumber;
    string vehicleType;
    time_t entryTime;
    int slotNumber;
    double parkingFee;

public:
    Vehicle() {
        vehicleNumber = "";
        vehicleType = "";
        entryTime = time(0);
        slotNumber = 0;
        parkingFee = 0;
    }

    Vehicle(string number, string type, int slot) {
        vehicleNumber = number;
        vehicleType = type;
        slotNumber = slot;
        entryTime = time(0);
        parkingFee = 0;
    }

    string getVehicleNumber() const {
        return vehicleNumber;
    }

    string getVehicleType() const {
        return vehicleType;
    }

    int getSlotNumber() const {
        return slotNumber;
    }

    time_t getEntryTime() const {
        return entryTime;
    }

    int getDuration() const {
        time_t currentTime = time(0);
        double seconds = difftime(currentTime, entryTime);
        int hours = (int)(seconds / 3600) + 1;
        return max(1, hours);
    }

    double calculateFee() {
        int hours = getDuration();

        if (vehicleType == "Bike")
            parkingFee = hours * 20;
        else if (vehicleType == "Car")
            parkingFee = hours * 50;
        else if (vehicleType == "Truck")
            parkingFee = hours * 100;
        else if (vehicleType == "EV")
            parkingFee = hours * 40;
        else if (vehicleType == "VIP")
            parkingFee = hours * 150;

        return parkingFee;
    }

    void displayVehicle() const {
        cout << left << setw(15) << vehicleNumber
             << setw(10) << vehicleType
             << setw(10) << slotNumber
             << setw(10) << getDuration() << " hrs" << endl;
    }
};

class ParkingSystem {
private:
    int totalSlots;
    vector<bool> slots;
    map<string, Vehicle> activeVehicles;
    vector<Vehicle> history;
    queue<Vehicle> waitingQueue;

    double totalRevenue;
    int bikeCount, carCount, truckCount, evCount, vipCount;

public:
    ParkingSystem(int capacity) {
        totalSlots = capacity;
        slots.resize(totalSlots + 1, false);
        totalRevenue = 0;

        bikeCount = carCount = truckCount = evCount = vipCount = 0;

        Vehicle v1("HP12A1111", "Car", 1);
        Vehicle v2("HP12B2222", "Bike", 2);
        Vehicle v3("HP12C3333", "EV", 3);

        activeVehicles["HP12A1111"] = v1;
        activeVehicles["HP12B2222"] = v2;
        activeVehicles["HP12C3333"] = v3;

        slots[1] = true;
        slots[2] = true;
        slots[3] = true;

        carCount++;
        bikeCount++;
        evCount++;
    }

    void showCurrentTime() {
        time_t now = time(0);
        char *dt = ctime(&now);
        cout << "\nCurrent Date & Time: " << dt;
    }

    int allocateSlot() {
        for (int i = 1; i <= totalSlots; i++) {
            if (!slots[i]) {
                slots[i] = true;
                return i;
            }
        }
        return -1;
    }

    string chooseVehicleType() {
        int typeChoice;

        cout << "\nSelect Vehicle Type:\n";
        cout << "1. Bike  - Rs. 20/hour\n";
        cout << "2. Car   - Rs. 50/hour\n";
        cout << "3. Truck - Rs. 100/hour\n";
        cout << "4. EV    - Rs. 40/hour\n";
        cout << "5. VIP   - Rs. 150/hour\n";
        cout << "Enter choice: ";
        cin >> typeChoice;

        switch (typeChoice) {
            case 1: return "Bike";
            case 2: return "Car";
            case 3: return "Truck";
            case 4: return "EV";
            case 5: return "VIP";
            default: return "Invalid";
        }
    }

    void updateVehicleCount(string type) {
        if (type == "Bike") bikeCount++;
        else if (type == "Car") carCount++;
        else if (type == "Truck") truckCount++;
        else if (type == "EV") evCount++;
        else if (type == "VIP") vipCount++;
    }

    void vehicleEntry() {
        string number;

        cout << "\nEnter Vehicle Number: ";
        cin >> number;

        if (activeVehicles.find(number) != activeVehicles.end()) {
            cout << "Vehicle already parked!\n";
            return;
        }

        string type = chooseVehicleType();

        if (type == "Invalid") {
            cout << "Invalid vehicle type!\n";
            return;
        }

        int slot = allocateSlot();

        if (slot == -1) {
            cout << "\nParking is full!\n";
            cout << "Vehicle added to waiting queue.\n";

            Vehicle waitingVehicle(number, type, 0);
            waitingQueue.push(waitingVehicle);
            return;
        }

        Vehicle v(number, type, slot);
        activeVehicles[number] = v;
        updateVehicleCount(type);

        cout << "\nVehicle parked successfully!\n";
        cout << "Allocated Slot Number: " << slot << endl;
    }

    void generateReceipt(Vehicle v, double fee) {
        time_t now = time(0);

        cout << "\n========== PARKING RECEIPT ==========\n";
        cout << "Receipt Time   : " << ctime(&now);
        cout << "Vehicle Number : " << v.getVehicleNumber() << endl;
        cout << "Vehicle Type   : " << v.getVehicleType() << endl;
        cout << "Slot Number    : " << v.getSlotNumber() << endl;
        cout << "Duration       : " << v.getDuration() << " hour(s)" << endl;
        cout << "Parking Fee    : Rs. " << fee << endl;
        cout << "=====================================\n";
    }

    void assignWaitingVehicleToSlot() {
        if (!waitingQueue.empty()) {
            int slot = allocateSlot();

            if (slot != -1) {
                Vehicle oldVehicle = waitingQueue.front();
                waitingQueue.pop();

                Vehicle newVehicle(oldVehicle.getVehicleNumber(), oldVehicle.getVehicleType(), slot);
                activeVehicles[newVehicle.getVehicleNumber()] = newVehicle;
                updateVehicleCount(newVehicle.getVehicleType());

                cout << "\nWaiting vehicle " << newVehicle.getVehicleNumber()
                     << " has been assigned Slot " << slot << ".\n";
            }
        }
    }

    void vehicleExit() {
        string number;

        cout << "\nEnter Vehicle Number: ";
        cin >> number;

        auto it = activeVehicles.find(number);

        if (it == activeVehicles.end()) {
            cout << "Vehicle not found!\n";
            return;
        }

        Vehicle v = it->second;
        double fee = v.calculateFee();

        slots[v.getSlotNumber()] = false;
        totalRevenue += fee;
        history.push_back(v);
        activeVehicles.erase(it);

        cout << "\nVehicle exited successfully!\n";
        generateReceipt(v, fee);

        assignWaitingVehicleToSlot();
    }

    void searchVehicle() {
        string number;

        cout << "\nEnter Vehicle Number to Search: ";
        cin >> number;

        auto it = activeVehicles.find(number);

        if (it == activeVehicles.end()) {
            cout << "Vehicle not found in parking.\n";
            return;
        }

        cout << "\nVehicle Found:\n";
        cout << left << setw(15) << "Number"
             << setw(10) << "Type"
             << setw(10) << "Slot"
             << setw(10) << "Duration" << endl;

        it->second.displayVehicle();
    }

    void displayParkingStatus() {
        cout << "\n========== PARKING STATUS ==========\n";

        if (activeVehicles.empty()) {
            cout << "No vehicles currently parked.\n";
            return;
        }

        cout << left << setw(15) << "Number"
             << setw(10) << "Type"
             << setw(10) << "Slot"
             << setw(10) << "Duration" << endl;

        for (auto &pair : activeVehicles) {
            pair.second.displayVehicle();
        }
    }

    void showSlots() {
        cout << "\n========== SLOT STATUS ==========\n";

        for (int i = 1; i <= totalSlots; i++) {
            cout << "Slot " << setw(3) << i << " : ";

            if (slots[i])
                cout << "Occupied\n";
            else
                cout << "Available\n";
        }
    }

    void showQueueStatus() {
        cout << "\n========== WAITING QUEUE ==========\n";

        if (waitingQueue.empty()) {
            cout << "No vehicles in waiting queue.\n";
            return;
        }

        queue<Vehicle> temp = waitingQueue;

        while (!temp.empty()) {
            Vehicle v = temp.front();
            temp.pop();

            cout << "Vehicle Number: " << v.getVehicleNumber()
                 << " | Type: " << v.getVehicleType() << endl;
        }
    }

    void dashboard() {
        int occupied = activeVehicles.size();
        int available = totalSlots - occupied;

        cout << "\n========== PARKING DASHBOARD ==========\n";
        cout << "Total Slots              : " << totalSlots << endl;
        cout << "Occupied Slots           : " << occupied << endl;
        cout << "Available Slots          : " << available << endl;
        cout << "Vehicles Currently Parked: " << activeVehicles.size() << endl;
        cout << "Vehicles Exited          : " << history.size() << endl;
        cout << "Vehicles in Queue        : " << waitingQueue.size() << endl;
        cout << "Total Revenue Generated  : Rs. " << totalRevenue << endl;

        cout << "\nVehicle Category Count:\n";
        cout << "Bike  : " << bikeCount << endl;
        cout << "Car   : " << carCount << endl;
        cout << "Truck : " << truckCount << endl;
        cout << "EV    : " << evCount << endl;
        cout << "VIP   : " << vipCount << endl;
    }

    bool adminLogin() {
        string username, password;

        cout << "\n========== ADMIN LOGIN ==========\n";
        cout << "Username: ";
        cin >> username;
        cout << "Password: ";
        cin >> password;

        if (username == "admin" && password == "admin123") {
            cout << "Login successful!\n";
            return true;
        }

        cout << "Invalid username or password!\n";
        return false;
    }

    void adminPanel() {
        if (!adminLogin()) {
            return;
        }

        int choice;

        do {
            cout << "\n========== ADMIN PANEL ==========\n";
            cout << "1. Display Parking Status\n";
            cout << "2. Show Slot Status\n";
            cout << "3. Show Dashboard\n";
            cout << "4. Show Waiting Queue\n";
            cout << "5. Back to Main Menu\n";
            cout << "Enter choice: ";
            cin >> choice;

            switch (choice) {
                case 1:
                    displayParkingStatus();
                    break;

                case 2:
                    showSlots();
                    break;

                case 3:
                    dashboard();
                    break;

                case 4:
                    showQueueStatus();
                    break;

                case 5:
                    cout << "Returning to Main Menu...\n";
                    break;

                default:
                    cout << "Invalid choice!\n";
            }

        } while (choice != 5);
    }
};

bool userLogin() {
    int choice;
    string username, password;

    cout << "\n========== LOGIN SYSTEM ==========\n";
    cout << "1. User Login\n";
    cout << "2. Continue as Guest\n";
    cout << "Enter choice: ";
    cin >> choice;

    if (choice == 1) {
        cout << "Username: ";
        cin >> username;
        cout << "Password: ";
        cin >> password;

        if (username == "user" && password == "user123") {
            cout << "User login successful!\n";
            return true;
        } else {
            cout << "Invalid login. Continuing as guest...\n";
            return true;
        }
    }

    return true;
}

int main() {
    ParkingSystem parking(10);

    if (!userLogin()) {
        return 0;
    }

    int choice;

    do {
        parking.showCurrentTime();

        cout << "\n====================================\n";
        cout << "   SMART PARKING MANAGEMENT SYSTEM\n";
        cout << "====================================\n";
        cout << "1. Vehicle Entry\n";
        cout << "2. Vehicle Exit\n";
        cout << "3. Search Vehicle\n";
        cout << "4. Display Parking Status\n";
        cout << "5. Show Available/Occupied Slots\n";
        cout << "6. Admin Panel\n";
        cout << "7. Exit Program\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                parking.vehicleEntry();
                break;

            case 2:
                parking.vehicleExit();
                break;

            case 3:
                parking.searchVehicle();
                break;

            case 4:
                parking.displayParkingStatus();
                break;

            case 5:
                parking.showSlots();
                break;

            case 6:
                parking.adminPanel();
                break;

            case 7:
                cout << "Thank you for using Smart Parking System!\n";
                break;

            default:
                cout << "Invalid choice! Try again.\n";
        }

    } while (choice != 7);

    return 0;
}