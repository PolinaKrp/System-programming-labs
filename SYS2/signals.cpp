#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <unordered_set>
#include <sys/prctl.h>
#include <wait.h>
#include "check.hpp"
#include <random>
using namespace std;

volatile sig_atomic_t waiting;        // 1 - wait, 0 - go
volatile sig_atomic_t win;            // 1 - число угадано, 0 - не угадано
volatile sig_atomic_t value = -1;     // Загаданное число
volatile sig_atomic_t got_value = -1; // Полученное предположенное число

int getRandomNumber(int min, int max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(min, max);

    return distr(gen);
}

int getRoundsCount()
{
    int rounds = 10;
    std::cout << "Input amount of rounds: ";
    std::cin >> rounds;
    std::cout << std::endl;
    return rounds;
}

void guess_handler(int sig, siginfo_t *si, void *ctx)
{
    waiting = 0;
    got_value = si->si_value.sival_int;
}

void win_handler(int sig)
{
    waiting = 0;
    win = 1;
}

void fail_handler(int sig)
{
    waiting = 0;
    win = 0;
}

void ready_handler(int sig)
{
    waiting = 0;
}

int main()
{
    int max_rounds = getRoundsCount(); 
    int min = 1, max = 10;
    int round = 0;
    int tries = 0;
    unordered_set<int> what_guessed{};

    waiting = 1;

    sigset_t block_set, free_set;
    sigfillset(&block_set);
    sigfillset(&free_set);
    sigdelset(&free_set, SIGRTMIN); // Передача числа
    sigdelset(&free_set, SIGUSR1);  // Угадал
    sigdelset(&free_set, SIGUSR2);  // Не угадал
    sigdelset(&free_set, SIGCHLD); // Готов к отгадыванию
    sigprocmask(SIG_BLOCK, &free_set, NULL);

    struct sigaction
        guess_action
    {
    },                  // Сигнал реального времени для передачи числа
        win_action{},   // Если угадал
        fail_action{},  // Если не угадал
        ready_action{}; // Число загадано, ожидание попытки угадать
    // alarm_action {};

    guess_action.sa_sigaction = guess_handler;
    guess_action.sa_flags = SA_SIGINFO;
    guess_action.sa_mask = block_set;
    check(sigaction(SIGRTMIN, &guess_action, NULL));
    win_action.sa_handler = win_handler;
    win_action.sa_mask = block_set;
    check(sigaction(SIGUSR1, &win_action, NULL));
    fail_action.sa_handler = fail_handler;
    fail_action.sa_mask = block_set;
    check(sigaction(SIGUSR2, &fail_action, NULL));
    ready_action.sa_handler = ready_handler;
    ready_action.sa_mask = block_set;
    check(sigaction(SIGCHLD, &ready_action, NULL));

    pid_t pid = check(fork());

    // while (true)
    {
        while (round < max_rounds)
        {
            what_guessed = {};
            got_value = -1;
            value = -1;
            if (round % 2 == 0) // В четных раундах загадывает дочерний
            {
                if (pid == 0) // Дочерний загадывает
                {
                    value = getRandomNumber(min, max);
                    waiting = 1;
                    cout << "proc2: <<I guessed a number from 1 to 10. Try to guess it!>> " << value << endl;
                    check(kill(getppid(), SIGCHLD));
                    //----------
                    while (true) // Говорим, угадал или нет
                    {
                        while (waiting == 1)
                            ; //-----------
                        waiting = 1;

                        if (value == got_value)
                        {
                            cout << "proc1 guessed" << endl;
                            check(kill(getppid(), SIGUSR1));
                            break;
                        }
                        else
                        {
                            cout << "proc1 didnt guess" << endl;
                            check(kill(getppid(), SIGUSR2));
                        }
                    }
                    while (waiting == 1)
                        ;

                    round++;
                }
                else if (pid > 0) // Родительский ждет, пока загадают
                {
                    int tries = 0;
                    win = 0;
                    waiting = 1;
                    kill(pid, SIGCHLD);
                    while (waiting == 1)
                        ;
                    waiting = 1;
                    while (win == 0)
                    {
                        while (true)
                        {
                            value = getRandomNumber(min, max);
                            if (what_guessed.find(int(value)) == what_guessed.end())
                            {
                                what_guessed.insert(int(value));
                                break;
                            }
                        }
                        union sigval valueToSend;
                        valueToSend.sival_int = value;
                        tries++;
                        cout << "proc1 tries to guess: " << value << endl;
                        check(sigqueue(pid, SIGRTMIN, valueToSend));
                        while (waiting == 1)
                            ;
                        waiting = 1;
                    }

                    waiting = 1;
                    cout << "Round " << round << ". Number of attempts" << tries << endl
                         << endl
                         << endl;

                    check(kill(pid, SIGCHLD));
                    while (waiting)
                        ;
                    round++;
                }
            }

            else // В нечетных загадывает родительский
            {
                if (pid > 0) // Родитель загадывает
                {
                    value = getRandomNumber(min, max);
                    waiting = 1;
                    cout << "proc1: <<I guessed a number from 1 to 10. Try to guess it!>> " << value << endl;
                    check(kill(pid, SIGCHLD));

                    while (true) // Говорим, угадал или нет
                    {
                        while (waiting == 1)
                            ;
                        waiting = 1;

                        if (value == got_value)
                        {
                            cout << "proc2 guessed" << endl;
                            check(kill(pid, SIGUSR1));
                            break;
                        }
                        else
                        {
                            cout << "proc2 didnt guess" << endl;
                            check(kill(pid, SIGUSR2));
                        }
                    }
                    while (waiting == 1)
                        ;

                    round++;
                }
                else if (pid == 0) //
                {
                    int tries = 0;
                    win = 0;
                    waiting = 1;
                    check(kill(getppid(), SIGCHLD));
                    while (waiting == 1)
                        ;
                    waiting = 1;

                    while (win == 0)
                    {
                        while (true)
                        {
                            value = getRandomNumber(min, max);
                            if (what_guessed.find(int(value)) == what_guessed.end())
                            {
                                what_guessed.insert(int(value));
                                break;
                            }
                        }
                        union sigval valueToSend;
                        valueToSend.sival_int = value;
                        tries++;
                        cout << "proc2 tries to guess:" << value << endl;
                        check(sigqueue(getppid(), SIGRTMIN, valueToSend));
                        while (waiting == 1)
                            ;
                        waiting = 1;
                    }

                    waiting = 1;
                    cout << "Round " << round << ". Number of attempts" << tries << endl
                         << endl
                         << endl;
                    check(kill(getppid(), SIGCHLD));
                    while (waiting)
                        ;

                    round++;
                }
            }
        }
    }
    if (pid == 0)
    {
        kill(getppid(), SIGCHLD);
    }
    if (pid > 0)
    {

        kill(pid, SIGCHLD);
        wait(NULL); // Ожидаем завершения дочернего процесса
    }
    return 0;
}