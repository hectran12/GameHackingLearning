using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Threading;

class Program
{
    static bool isHackAmmo = false;
    static bool isHackHealth = false;
    static bool isHackShield = false;

    [DllImport("kernel32.dll")]
    static extern IntPtr OpenProcess(int dwDesiredAccess, bool bInheritHandle, int dwProcessId);

    [DllImport("kernel32.dll")]
    static extern bool ReadProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, byte[] lpBuffer, int dwSize, out int lpNumberOfBytesRead);

    [DllImport("kernel32.dll")]
    static extern bool WriteProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, byte[] lpBuffer, int dwSize, out int lpNumberOfBytesWritten);

    [DllImport("user32.dll")]
    static extern IntPtr FindWindow(string lpClassName, string lpWindowName);

    [DllImport("user32.dll")]
    static extern int GetWindowThreadProcessId(IntPtr hWnd, out int processId);

    static IntPtr GetModuleBase(int procID, string modName)
    {
        Process process = Process.GetProcessById(procID);
        foreach (ProcessModule module in process.Modules)
        {
            if (module.ModuleName == modName)
                return module.BaseAddress;
        }
        return IntPtr.Zero;
    }

    static void HackMemory(IntPtr hProc, IntPtr baseAddress, int offset, ref bool isHack, int newValue)
    {
        IntPtr entity = baseAddress + 0x0017E0A8;
        byte[] buffer = new byte[4];
        ReadProcessMemory(hProc, entity, buffer, buffer.Length, out _);
        IntPtr address = (IntPtr)BitConverter.ToInt32(buffer, 0) + offset;

        while (true)
        {
            if (isHack)
            {
                byte[] newValueBytes = BitConverter.GetBytes(newValue);
                WriteProcessMemory(hProc, address, newValueBytes, newValueBytes.Length, out _);
            }
            Thread.Sleep(100);
        }
    }

    static void Menu()
    {
        while (true)
        {
            Console.Clear();
            Console.WriteLine("AssaultCube Hack By HecTran");
            Console.WriteLine("1. Toggle Ammo Hack ({0})", isHackAmmo ? "ON" : "OFF");
            Console.WriteLine("2. Toggle Health Hack ({0})", isHackHealth ? "ON" : "OFF");
            Console.WriteLine("3. Toggle Shield Hack ({0})", isHackShield ? "ON" : "OFF");
            Console.WriteLine("4. Exit");
            Console.Write("Choose: ");

            int choice;
            if (int.TryParse(Console.ReadLine(), out choice))
            {
                if (choice == 1) isHackAmmo = !isHackAmmo;
                else if (choice == 2) isHackHealth = !isHackHealth;
                else if (choice == 3) isHackShield = !isHackShield;
                else if (choice == 4) Environment.Exit(0);
            }
        }
    }

    static void Main()
    {
        IntPtr hWnd = FindWindow(null, "AssaultCube");
        if (hWnd == IntPtr.Zero)
        {
            Console.WriteLine("Game not found!");
            return;
        }

        GetWindowThreadProcessId(hWnd, out int procID);
        if (procID == 0)
        {
            Console.WriteLine("Failed to get process ID");
            return;
        }

        IntPtr hProc = OpenProcess(0x1F0FFF, false, procID);
        if (hProc == IntPtr.Zero)
        {
            Console.WriteLine("Failed to open process");
            return;
        }

        IntPtr baseAddress = GetModuleBase(procID, "ac_client.exe");

        Thread ammoThread = new Thread(() => HackMemory(hProc, baseAddress, 0x140, ref isHackAmmo, 100));
        Thread healthThread = new Thread(() => HackMemory(hProc, baseAddress, 0xEC, ref isHackHealth, 10000));
        Thread shieldThread = new Thread(() => HackMemory(hProc, baseAddress, 0xF0, ref isHackShield, 10000));

        ammoThread.Start();
        healthThread.Start();
        shieldThread.Start();

        Menu();

        ammoThread.Join();
        healthThread.Join();
        shieldThread.Join();
    }
}
