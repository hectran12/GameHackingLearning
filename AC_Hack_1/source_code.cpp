#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <thread>
using namespace std;

bool isHackAmmo = false;
bool isHackHealth = false;
bool isHackShield = false;

uintptr_t GetModuleBase(DWORD procID, const wchar_t* modName) {
    uintptr_t moduleBase = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procID);

    if (hSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);

        if (Module32First(hSnap, &modEntry)) {
            do {
                if (!_wcsicmp(modEntry.szModule, modName)) {
                    moduleBase = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return moduleBase;
}

void HackAmmo(HANDLE hProc, uintptr_t base) {
    uintptr_t entity = base + 0x0017E0A8;
    uintptr_t ammoAddr;
    ReadProcessMemory(hProc, (BYTE*)entity, &ammoAddr, sizeof(ammoAddr), 0);
    ammoAddr += 0x140;
    int newAmmo = 100;

    while (true) {
        if (isHackAmmo) {
            WriteProcessMemory(hProc, (BYTE*)ammoAddr, &newAmmo, sizeof(newAmmo), 0);
        }
        Sleep(100);
    }
}

void HackHealth(HANDLE hProc, uintptr_t base) {
	uintptr_t entity = base + 0x0017E0A8;
	uintptr_t healthAddr;
	ReadProcessMemory(hProc, (BYTE*)entity, &healthAddr, sizeof(healthAddr), 0);
    healthAddr += 0xEC;
	int newHealth = 10000;

    while (true) {
        if (isHackHealth) {
			WriteProcessMemory(hProc, (BYTE*)healthAddr, &newHealth, sizeof(newHealth), 0);
		}
		Sleep(100);
	}
}

void HackShield(HANDLE hProc, uintptr_t base) {
	uintptr_t entity = base + 0x0017E0A8;
	uintptr_t shieldAddr;
	ReadProcessMemory(hProc, (BYTE*)entity, &shieldAddr, sizeof(shieldAddr), 0);
	shieldAddr += 0xF0;
	int newShield = 10000;

	while (true) {
		if (isHackShield) {
			WriteProcessMemory(hProc, (BYTE*)shieldAddr, &newShield, sizeof(newShield), 0);
		}
		Sleep(100);
	}
}

void menu() {
    while (true) {
		system("cls");
        cout << "AssaultCube Hack By HecTran\n";
        cout << "1. Toggle Ammo Hack (" << (isHackAmmo ? "ON" : "OFF") << ")\n";
		cout << "2. Toggle Health Hack (" << (isHackHealth ? "ON" : "OFF") << ")\n";
		cout << "3. Toggle Shield Hack (" << (isHackShield ? "ON" : "OFF") << ")\n";
        cout << "4. Exit\n";
        cout << "Choose: ";
        int choice;
        cin >> choice;

        if (choice == 1) {
            isHackAmmo = !isHackAmmo;
        }
        else if (choice == 2) {
			isHackHealth = !isHackHealth;
        }
        else if (choice == 3) {
			isHackShield = !isHackShield;
        }
        else if (choice == 3) {
            exit(0);
        }
    }
}

int main() {
    DWORD procID = 0;
    HWND hWnd = FindWindowA(NULL, "AssaultCube"); 

    if (!hWnd) {
        cout << "Game not found!" << endl;
        return -1;
    }

    GetWindowThreadProcessId(hWnd, &procID);
    if (!procID) {
        cerr << "Failed to get process ID" << endl;
        return -1;
    }

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
    if (!hProc) {
        cerr << "Failed to open process" << endl;
        return -1;
    }

    uintptr_t base = GetModuleBase(procID, L"ac_client.exe");
    thread ammoThread(HackAmmo, hProc, base);
	thread healthThread(HackHealth, hProc, base); 
	thread shieldThread(HackShield, hProc, base); 
    menu();

    ammoThread.join(); 
	healthThread.join();
	shieldThread.join();
    CloseHandle(hProc);
    return 0;
}
