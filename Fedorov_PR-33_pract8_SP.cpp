
#include <iostream>
#include <Windows.h>
#include <vector>
#include <cstdlib>
#include <ctime>

const int TRACK_LENGTH = 50;
const int TURTLES = 5;
CRITICAL_SECTION cs;
bool winner_declared = false;
int winner_id = -1;

struct TurtleData
{
    int id;
    int position;
    HANDLE hThread;
};

DWORD WINAPI TurtleTheardProc(LPVOID lpParam)
{
    TurtleData* data = static_cast<TurtleData*>(lpParam);
    srand(static_cast<unsigned>(time(nullptr) + data->id));
    while (data->position < TRACK_LENGTH)
    {
        int step = rand() % 3;
        data->position += step;
        EnterCriticalSection(&cs);
        std::cout << "Черепашка " << data->id << " сделала шаг на +" << step << ", позиция: " << data->position << std::endl;
        if (data->position >= TRACK_LENGTH && !winner_declared)
        {
            winner_declared = true;
            winner_id = data->id;
            std::cout << "\nЧерпашка " << data->id << " победила!\n\n";
        }
        LeaveCriticalSection(&cs);
        int delay = 1000 + (rand() % 3000);
        Sleep(delay);
    }
    EnterCriticalSection(&cs);
    std::cout << "Черепашка " << data->id << " достигла финиша\n";
    LeaveCriticalSection(&cs);
    return 0;
}

int main()
{
    SetConsoleOutputCP(1251);
    InitializeCriticalSection(&cs);
    srand(static_cast<unsigned>(time(nullptr)));
    std::vector<TurtleData>turtles(TURTLES);
    std::vector<HANDLE> threadHandles(TURTLES);
    for (int i = 0; i < TURTLES; i++)
    {
        turtles[i].id = i + 1;
        turtles[i].position = 0;
        turtles[i].hThread = CreateThread(nullptr, 0, TurtleTheardProc, &turtles[i], 0, nullptr);
        if (turtles[i].hThread == nullptr)
        {
            std::cerr << "Ошибка создания потока" << i + 1 << std::endl;
            DeleteCriticalSection(&cs);
            return 1;
        }
        threadHandles[i] = turtles[i].hThread;
    }
    WaitForMultipleObjects(TURTLES, threadHandles.data(), TRUE, INFINITE);
    for (int i = 0; i< threadHandles.size(); ++i)
    {
        HANDLE h = threadHandles[i];
        CloseHandle(h);
    }
    DeleteCriticalSection(&cs);
    std::cout << "Гонка завершена.\n";
    return 0;
}
