#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
using namespace std;
string getChoiceName(int choice)
{
    if (choice == 1)
        return "Rock";
    if (choice == 2)
        return "paper";
    return "scissors";
}
int getresult(int player, int computer)
{
    if (player == computer)
        return 0;
    if ((player == 1 && computer == 3) ||
           (player == 2 && computer == 1) ||
           (player == 3 && computer == 2)){ return 1;
}
return 2;
}
int main()
{
    srand(time(0));
    int playerscore = 0;
    int computerscore = 0;
    int ties = 0;
    char playagain = 'y';
    cout<<"Rock Paper Scissors"<<endl;
    while (playagain == 'y' || playagain == 'Y')
    {
        int playerchoice = 0;
        int computerchoice = rand() % 3 + 1;
    
    cout << "1.Rock" << endl;
    cout << "2.Paper" << endl;
    cout << "3.scissors" << endl;
    cout << "Enter choice(1-3):" << endl;
    cin >> playerchoice;
    if (playerchoice < 1 || playerchoice > 3)
    {
        cout << "invalid choice";
     continue;  
    }
     

    cout << "your choice :"  << getChoiceName(playerchoice) << endl;
    cout << "computer choice : " << getChoiceName(computerchoice) << endl;
    int result = getresult(playerchoice, computerchoice);
    if (result == 0)
    {
        cout << "tie" << endl;
        ties++;
    }
    else if (result == 1)
    {
        cout << "you win" << endl;
        playerscore++;
    }
    else
        {
            cout << "compuer win" << endl;
            computerscore++;
        }
    cout << "play again(y/n)" << endl;
    cin >> playagain;
     }
    cout << "final score" << endl;
    cout << "you" << playerscore <<endl;
    cout<< "computer" << computerscore << endl;
    if (playerscore > computerscore)
    {
        cout << "player wins" << endl;
    }
    else if (playerscore < computerscore)
    {
        cout << "computer wins" << endl;
    }
    else
    {
        cout << "tie" << endl;
    }
    return 0;
}
