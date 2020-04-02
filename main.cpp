#include <thread>
#include <mutex>
#include <chrono>
#include <unistd.h>
#include <ncurses.h>
#include <iostream>
//ilosc filozofow i widelcow
#define PHILOSOPHERS_NUMBER 5
bool dinnerTime = true;
bool displayOn = true;
bool exitProgram = false;
//klasa widelec (mutex))
class Fork{
    std::mutex fmutex;
    std::string status;

    public:
    //podniesienie widelca
    void getFork(){
        fmutex.lock();
        status = "Occupied";
    };
    //odlozenie widelca
    void putDownFork(){
        fmutex.unlock();
        status = "Free";
    };

    std::string getForkStatus(){
        return status;
    };

    Fork(){
        status = "Free";
    }
};

//Klasa filozof
class Philosopher{
    std::string status; 
    std::string name;

public:
//dwa widelce
    Fork* right;
    Fork* left;

    int thinkProgress;
    int eatProgress;
    int mealCount = 0;
//ilosc posilkow spozytych
    int getMealCount(){
        return mealCount;
    }
//funkcje zwracajace stopien w jakim wykonano czynnosci
    int getThinkProgress(){
        return thinkProgress;
    }
    int getEatProgress(){
        return eatProgress;
    }
    
    Philosopher(Fork* right, Fork* left): right(right), left(left){}
    Philosopher(){
        name = "Filozof";
    }
    //"jedzenie" czyli podniesienie obu widelcy a nastepnie odlozenie
    void eat(){
        if(dinnerTime){
        eatProgress = 0;
        right->getFork();        
        left->getFork();
        status = "EATING";
        mealCount++;
        //zeby pokazac progres
        for(int i = 0; i <10; i++){
            eatProgress++;
            usleep(300000 + std::rand() % 50000);
        }
        right->putDownFork();
        left->putDownFork();
        }
    };
    //myslenie
    void think(){
        thinkProgress = 0;
        status = "THINKING";
        for(int i = 0; i < 10; i++){
            thinkProgress++;
            usleep(300000 + std::rand()%50000);
        }//jesli po okresie myslenia filozof od razu nie uzyska widelcy dostaje status czekajacego
        status = "WAITING";
    }
    std::string getName(){
        return this->name;
    }
    int getStatus(){

        if(this->status == "THINKING"){
            return 1;
        } else if (this->status == "EATING"){
            return 2;
        }
        else return 3;
    }
};

Philosopher philosophers[PHILOSOPHERS_NUMBER];
Fork forks[PHILOSOPHERS_NUMBER];

//Funkcja odpowiadajaca za wyswietlanie
void display() {
    int thinkProgress=0;
    int eatProgress = 0 ;

    noecho();

    while(displayOn) {
        clear();
        mvprintw(0, 0,"Dining philosophers problem. Press ESC to exit");
        mvprintw(1, 0,"____________________________________________________________________________________________________");

        for(int i = 0; i < PHILOSOPHERS_NUMBER; i++) {
            
            mvprintw(i+2, 1, "Philosopher %d", i+1);
            mvprintw(i+2, 0, "|");
            mvprintw(i+2, 30, "Progress:");
            mvprintw(i+2, 40, "__________");
            //Stan myslenia
            switch (philosophers[i].getStatus()){
                case 1:
                    mvprintw(i+2, 15, "thinking");
                    thinkProgress = philosophers[i].getThinkProgress();
                    for(int j=0; j<thinkProgress; j++)
                    {
                        mvprintw(i+2, 40+j, "#");

                    };
                    break;
                    //Stan jedzenia
                case 2:
                    mvprintw(i+2, 15, "eating");
                    eatProgress = philosophers[i].getEatProgress();
                    for(int j=0; j<eatProgress; j++)
                    {
                        mvprintw(i+2, 40+j, "#");

                    };
                    break;
                //Stan czekania
                case 3:
                    mvprintw(i+2, 15, "WAITING");
                    eatProgress = philosophers[i].getEatProgress();
                    for(int j=0; j<eatProgress; j++)
                    {
                        mvprintw(i+2, 40+j, "#");

                    };
                    break;               
            };
            mvprintw(i+2, 60, "Meals: %d", philosophers[i].getMealCount());
            mvprintw(i+2, 70, "Fork %d: ", i);
            mvprintw(i+2, 80, forks[i].getForkStatus().c_str());
            mvprintw(i+2, 100, "|");
        }
        mvprintw(7, 0,"____________________________________________________________________________________________________");
        refresh();
        usleep(10000);
    }
}
//posilek filozofa
void dine(Philosopher* p){
    while(dinnerTime){
    p -> think();
    p -> eat();
    } ;
};

int main(){
    
    srand(time(NULL));

    initscr();
    refresh();

    std::thread threads[PHILOSOPHERS_NUMBER];
    //setup
    for(int i = 0; i < PHILOSOPHERS_NUMBER-1; i++){
        philosophers[i].left = &forks[i];
        philosophers[i].right = &forks[i+1];
    };
    philosophers[PHILOSOPHERS_NUMBER-1].left = &forks[0];
    philosophers[PHILOSOPHERS_NUMBER-1].right = &forks[PHILOSOPHERS_NUMBER-1];

    //watki
     for(int i = 0; i < PHILOSOPHERS_NUMBER; i++)
         threads[i] = std::thread(dine, &philosophers[i]);

     std::thread displayThread(display);
    //ESC zamyka program
    while (!exitProgram)
    {
        int k = getch();
        if (k == 27)
        {
            exitProgram = true;
        }
    }
    dinnerTime = false;
    displayOn = false;
//joinowanie watkow
    for(int i = 0 ; i < PHILOSOPHERS_NUMBER; i ++)
        threads[i].join();
    dinnerTime = false;
    displayOn = false;
    displayThread.join();
//wyjscie do konsoli
    endwin();
    return 0;
}