#include <set>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <string>
#include<iostream>
#include <math.h>
#include <map>

using namespace std;
void printDict(std::map<int, int> *dict){
    for(map<int, int>::const_iterator it = dict->begin(); it != dict->end(); ++it)
    {
        //double d = (double)(it->second.first);
        std::cout <<  (it->first) << ": " <<  it->second <<  "\n";
    }
}
void printDict(std::map<int, double> *dict){
    for(map<int, double>::const_iterator it = dict->begin(); it != dict->end(); ++it)
    {
        //double d = (double)(it->second.first);
        std::cout <<  (it->first) << ": " <<  it->second <<  "\n";
    }
}

void printPoint(pair<int, int> pii){
    cout << "(" << pii.first << ", " << pii.second << ")";
}
void printSet(set<pair<int, int> > *s){
    for(set<pair<int, int> >::const_iterator it = s->begin(); it != s->end(); ++it){
        printPoint(*it);
        
    }
    cout << endl;
}
int gcd(int a, int b){
    //I'm new at C++, and __gcd() wasn't working.
    int temp = 0;
    while(a != 0){
        temp = b;
        b = a;
        a = temp%a;
    }
    return b;
}
pair<int, int> farthestPoint(pair<int, int> *point, set<pair<int, int> > *others){
    // By taking a given point p1, finding the point p2 that is farthest from p1, then finding p3 farthest from p2, 
    // we get the farthest two points, p2 and p3. This might just be approximate.
    // The problem is ill-defined because it does not specify what to do if two lines have the same length (but different numbers of marbles).
    // I guess it might make sense to choose the one with more marbles, but that would slow this down greatly.
    pair<int, int> winner = *point;
    int pointX = winner.first;
    int pointY = winner.second;
    float dis = 0;
    pair<int, int> unpacked;
    int otherX = 0;
    int otherY = 0;
    std::set<pair<int, int> >::iterator it = others->begin();
    
    while (it != others->end())
    {
        unpacked = *it;
        otherX = unpacked.first;
        otherY = unpacked.second;

        float newDis = sqrt((otherX - pointX)*(otherX - pointX) + (otherY - pointY)*(otherY - pointY));
        if(newDis > dis){
            dis = newDis;
            winner = unpacked;
        }
        
        ++it;
    }
    return winner;
}
int longestLineNum(set<pair<int, int> > *marbles){
    //Given the list of all the positions of marbles, find the number that is on the line containing the most.

    if(marbles->size() <= 2){
        
        return marbles->size();
    }
    pair<int, int> given = *(marbles->begin());
    //printPoint(given);
    pair<int, int> farthestFromGiven = farthestPoint(&given, marbles);
    //printPoint(farthestFromGiven);
    pair<int, int> farFar = farthestPoint(&farthestFromGiven, marbles);
    //printPoint(farFar);
    int xDiff = ((farFar.first) - (farthestFromGiven.first));
    //cout << endl << xDiff << endl;
    int yDiff = ((farFar.second) - (farthestFromGiven.second));
    //cout<< yDiff << endl;
    int grcd = abs(gcd(xDiff, yDiff));
    //cout<< grcd << endl;
    int xStep = xDiff/grcd;
    int yStep = yDiff/grcd;
    int checkX = (farthestFromGiven.first) + xStep;
    int checkY = (farthestFromGiven.second) + yStep;
    int count = 2;
    
    for(int spot = 1; spot < grcd; spot++){
        count += marbles->count(pair<int, int>(checkX, checkY));
        checkX += xStep;
        checkY += yStep;
    }
    return count;
}
int runProcess(int n, int k){
    //hitSet contains all of the positions the path has gone through. A square matrix might be faster, at least for smaller values.
    set <pair<int, int> > hitSet;
    set <pair<int, int> > marbles;
    //Position
    int posX = 0;
    int posY = 0;
    //Keeps track of how many movements since last placed marble.
    int count = 0;
    
    //One more than n*k because we want n*k movements rather than n*k positions. If you interpret the problem differently, you may want to change this.
    int nk = n*k+1;
    
    bool choiceArray[4] = {false,false,false,false};
    int i;
    for(i = 0; i < nk; i++){
        //Keep track of the places it has been.
        hitSet.insert(pair<int, int> (posX, posY));
        //When count == n, a marble is placed.
        ++count;
        if (count == n){
            count = 0;
            marbles.insert(pair<int, int> (posX, posY));
        }
        //Finds which paths are still open to it
        choiceArray[0] = !(bool)hitSet.count(pair<int, int> (posX, posY+1));//up
        choiceArray[1] = !(bool)hitSet.count(pair<int, int> (posX, posY-1));//down
        choiceArray[2] = !(bool)hitSet.count(pair<int, int> (posX+1, posY));//right
        choiceArray[3] = !(bool)hitSet.count(pair<int, int> (posX-1, posY));//left
        
        int sumChoices = choiceArray[0] + choiceArray[1] + choiceArray[2] + choiceArray[3];
        
        //If the path has trapped itself, it returns with -1. This is causes the avg_result and probabilities methods to retry.
        if(!sumChoices){
            return -1;
        }
        int randy = rand() % sumChoices;
        
        //Finds which number it actually picked by skipping the ones that it could not pick.
        for(int o = 0; o <= randy; o++){
            randy += !choiceArray[o];
        }
        //Moves based on its choice
        posX += (randy == 2)-(randy == 3);
        posY += (randy == 0)-(randy == 1);

        //Reset the array
        choiceArray[0] = false;
        choiceArray[1] = false;
        choiceArray[2] = false;
        choiceArray[3] = false;
    }
    
    //printSet(&marbles);

    return longestLineNum(&marbles);
}
double avg_result(int n, int k, int repeats){
    uint sum = 0;
    int res;
    for(int i = 0; i < repeats; i++){
        //runProcess returns None when the path traps itself. I decided to interpret that as a redo.
        do{
            res = runProcess(n,k);
        }while(res < 0);
        sum += res;
    }
    //The arithmetic mean might not be the best way to process this information
    return sum/(double)repeats;
}
//Puts the probabilities in the dictionary
void probabilities(int n, int k, uint repeats, std::map<int, double> *dict){
    std::map<int, uint> intDict;
    
    int noneCheck;
    for(uint i = 0; i < repeats; i++){
        do{
            noneCheck = runProcess(n, k);
        }while(noneCheck < 0);

        //There is probably a faster way to do this, but this isn't the slow part anyways.
        std::map<int, uint>::iterator it = intDict.find(noneCheck);
        
        if(it != intDict.end()){
            it->second++;
        }else{
            intDict.insert(pair<int, uint>(noneCheck, 1));
        }
    }
    
    for(map<int, uint>::iterator it = intDict.begin(); it != intDict.end(); ++it)
    {
        dict->insert(std::pair<int, double>(it->first, it->second / (double)repeats));
        
    }
    
}
int main(){
    //Randomizes the randomizer
    srand ( time(NULL) );
    
    //Try values of n and k here.
    //Find the result of a single attempt. (might return -1, meaning that it traps itself.)
    cout<<runProcess(1,2)<<endl;
    //Find the average result of a number of attempts. Retries if it traps itself.
    cout<<avg_result(6,6,1000)<<endl;
    
    //Find the probabilities of each outcome other than -1.
    std::map<int, double> dict;
    probabilities(1,3, 10000, &dict);
    printDict(&dict);
    
    return 0;
}