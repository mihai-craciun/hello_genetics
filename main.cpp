#include <iostream>
#include <algorithm>
#include <random>
#include <vector>
#include <unordered_map>
#include <string>
#include <chrono>

/** Returns a default random engine seeded with current milliseconds */
std::default_random_engine& get_engine() {
    static unsigned long seed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch()).count();
    static std::default_random_engine generator(seed);
    return generator;
}

/** Returns an uniformly distributed random double between a and b */
double unif(double a, double b) {
    std::uniform_real_distribution<double> dist(a, b);
    return dist(get_engine());
}

/** Returns an uniformly distributed random double between 0 and 1 */
double unif() {
    return unif(0, 1);
}

/** Returns an uniformly distributed integer between a and b */
int randint(int a, int b) {
    std::uniform_int_distribution<int> dist(a, b);
    return dist(get_engine());
}

/** Returns a random character between ' ' and '~' in ASCII */
char rand_char() {
    return randint((int)' ', (int)'~');
}

/** Returns a word with random characters that has 'size' length */
std::string rand_word(int size) {
    std::string s;
    for(int i=0; i<size; i++) {
        s += rand_char();
    }
    return s;
}

/** Retruns a fitness score for a string, given the goal */
double fitness(std::string s, std::string goal) {
    double c=0;
    for(int i=0; i<s.length(); i++) {
        c += s[i] == goal[i] ? 1 : 0;
    }
    return c / s.length();
}

/** Returns the element with the maximum score in a score map */
std::string get_max_elem(std::unordered_map<std::string, double>& m) {
    std::string max = "";
    for (auto i=m.begin(); i!=m.end(); i++) {
        if (max == "" || i->second > m[max]) {
            max = i->first;
        }
    }
    return max;
}

/** Transforms the score map into a cummulative score spectrum for easy picking */
double transform_scores(std::unordered_map<std::string, double>& m) {
    std::string max = get_max_elem(m);
    double c=0;
    for (auto i=m.begin(); i!=m.end(); i++) {
        i->second = c + i->second / m[max];
        c = i->second; 
    }
    return c;
}

/** Returns a random element in a map with calculated spectrum */
std::string get_random_elem(std::unordered_map<std::string, double>& m, double limit) {
    double r = unif(0, limit);
    for (auto i=m.begin(); i!=m.end(); i++) {
        if (r <= i->second) {
            return i->first;
        }
    }
}

/** Combines two members of a population */
std::string breed(std::string a, std::string b) {
    std::string s = a;
    for(int i=0; i<s.length(); i++) {
        int r = randint(0, 1);
        s[i] = r == 0 ? s[i] : b[i];
    }
    return s;
}

/** Performs mutation on a member with a given chance for each letter */
std::string mutate(std::string x, double chance) {
    for(int i=0; i<x.length(); i++) {
        if (unif() < chance) {
            x[i] = rand_char();
        }
    }
    return x;
}

int main() {
    // The population size
    int n = 200;
    // The mutation chance
    double m = 0.01;
    // The phrase goal
    std::string goal = "Hello World!";
    
    // Create initial population
    std::vector<std::string> p(n);
    for(int i=0; i<n; i++) {
        p[i] = rand_word(goal.length());
    }

    int gen = 1;
    std::unordered_map<std::string, double> scores;
    while (true) {
        // Calculate the scores map
        scores.clear();
        for (int i=0; i<n; i++) {
            scores.emplace(std::make_pair(p[i], fitness(p[i], goal)));
        }


        // Display the fittest member in this generation
        std::string max = get_max_elem(scores);
        std::cout<<gen<<" "<<max<<" "<<scores[max]<<"\n";

        // If the member is as fit as possible, stop
        if (scores[max] == 1) {
            break;
        }

        // Transform socres in cummulative, get the limit
        double limit = transform_scores(scores);

        // create the new generation
        for (int i=0; i<n; i++) {
            std::string x = get_random_elem(scores, limit);
            std::string y = get_random_elem(scores, limit);
            p[i] = mutate(breed(x, y), m);        
        }
        gen++;
    }
}
