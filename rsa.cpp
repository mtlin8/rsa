#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

// NOTE: making all number variables longs to make number work easier (so no wraparound)
bool checkPrimes (long p, long q);
void decrypt (long p, long q, long d, string input, string output);
string convertToWord(long m, int x);
void encrypt (string output, long n, long e, string message);
long gcd(long x, long y);
long modExp(long m, long e, long n);
long square (long m);
long log_100(long n);

int main(int argc, char** argv) {

    if (argc != 3) {
        cout << "Error: ./rsa p q";
        cout << "(where p and q are both large prime numbers)" << endl;
        return 1;
    }

    long p = atoi(argv[1]);
    long q = atoi(argv[2]); // const char *

    if (!checkPrimes(p, q)) { // this works!!
        cout << "One of or both p and q not prime" << endl;
        return 2;
    }

    // decryption key (d) calculation
    long e = 65537;
    long p_mult_q = (p - 1) * (q - 1);
    long L = p_mult_q/gcd(p - 1, q - 1);

    if (L < e) {
        cout << "Error: L < e" << endl;
        return 3;
    }
    else if (gcd(e, L) != 1) {
        cout << "Error: gcd(e, L) is not 1, poor choice of p and q" << endl;
        return 4;
    }

    long t = 1;
    long old_t = 0;
    long r = e;
    long old_r = L;
    while (r != 0) {
        long quotient = old_r/r;
        long temp = r;
        r = old_r - quotient * r;
        old_r = temp;
        temp = t;
        t = old_t - quotient * t;
        old_t = temp;
    }
    long gcd = old_r;
    long d = old_t;
    if (d < 0) {
        d += p_mult_q;
    }

    bool continueProgram = true; // main program good from here on out
    do {
        cin.clear();
        string command;
        cin >> command; // cin readings separated by whitespace or newline

        if (command == "EXIT") {
            continueProgram = false;
        }
        else if (command == "DECRYPT") {
            string input;
            string output;
           
            if (cin >> input) {
                if (cin >> output) {
                    decrypt(p, q, d, input, output); // only here do we run decrypt
                }
                else {
                    continue;
                }
            }
            else {
                continue;
            }
        }
        else if (command == "ENCRYPT") { 
            string output;
            long n; // can this be double for ease of manipulation?
            string message;
            string copy;
            
            if (cin >> output) {
                if (cin >> n) {
                    if (n < 27) { // checking before doing log_100(n/27)
                        cout << "Error: n < 27. Log incalculable." << endl;
                        return 3;
                    }

                    getline(cin, copy); // with this no need to put message in file
                    if (cin.fail()) {
                        continue;
                    }

                    int copySize = copy.size();  // iterate through to copy into message
                    for (int i = 1; i < copySize; i++) {
                        message += copy[i];
                    }
                    
                    encrypt(output, n, e, message); // only here do we run encrypt
                }
                else {
                    continue;
                }
            }
            else {
               continue;
            }
        }
        else {
            continue;
        }
    }
    while (continueProgram);

    return 0;
}

bool checkPrimes (long p, long q) {
    for (long i = 2; i < p; i++) {
        if (p % i == 0) {
            return false;
        }
    }
    for (long i = 2; i < q; i++) {
        if (q % i == 0) {
            return false;
        }
    }
    return true;
}

void decrypt (long p, long q, long d, string input, string output) {
    long n = p * q;
    int x = 1 + log_100(n/27);

    ifstream ifile(input); // file is open, this a stream we working with
    if (!ifile) {
        cout << "Unable to open " << input << endl;
        return;
    }
    ofstream ofile(output);

    while (true) { // another while loop to iterate through file input
        long c;
        ifile >> c;
        if (ifile.fail()) {
            break;
        }
        long m = modExp(c, d, n);
        cout << m << endl;

        string toWrite = convertToWord(m, x);
        ofile << toWrite; // spaces are already accounted for by 00, no need to worry
    }
    ifile.close();
    ofile.close(); // make it habit to close all ifiles and ofiles
}

string convertToWord(long m, int x) {
    string toReturn;
    string toReverse;
    while (m > 0) {
        int toConvert = m % 100;
        cout << toConvert << endl;
        m /= 100;
        if (toConvert == 0) {
            toReverse += ' ';
        }
        else if (toConvert >= 1 && toConvert <= 26) {
            toReverse += toConvert + 'a' - 1;
        }
    }

    while (toReverse.size() < x) {
        toReverse += ' ';
    }

    int size = toReverse.size();
    for (int i = size - 1; i >= 0; i--) {
        toReturn += toReverse[i];
    }
    return toReturn;
}

void encrypt (string output, long n, long e, string message) {
    int x = 1 + log_100(n/27); // round down
    int messageSize = message.size();
    int messageIterator = 0;
    ofstream ofile(output);

    while (messageIterator < messageSize) { 
        long mHold = 0;

        for (int i = 0; i < x; i++) { // x num chars (chunk we read in at a time)
            long toAdd = 0;
            // there is chance that first letter is a space

            if (mHold == 0) {
                if (message[messageIterator + i] - 'a' + 1 < 10) {
                    toAdd = message[messageIterator + i] - 'a' + 1;
                    mHold += toAdd;
                }
                else {
                    toAdd = message[messageIterator + i] - 'a' + 1;
                    mHold += toAdd;
                }
            }
            else {
                if (message[messageIterator + i] == ' ') {
                    mHold *= 100;
                    continue;
                }
                else {
                    toAdd = message[messageIterator + i] - 'a' + 1;
                    mHold *= 100;
                    mHold += toAdd;
                }
            }
        } // we'll track leading zeroes in decrypt by tracking supposed substring length

        long m = mHold;
        long toWrite = modExp(m, e, n);
        ofile << toWrite;
        messageIterator += x;
        if (messageIterator < messageSize) {
            ofile << ' ';
        }
    }
    ofile.close();
}

// extended Euclidean algorithm in main just needs to run once
long gcd(long x, long y) {
    if (y == 0)  return x;
    return gcd(y, x % y); // had x instead of y as first... big bug beat
}

long modExp(long m, long e, long n) {
    vector<int> binaryArray;
    while (e > 0) {
        binaryArray.push_back(e % 2); // entered in reverse order
        e /= 2; // this was to create binary vector
    }

    int size = binaryArray.size();
    long c = 1;
    long power = m % n;

    for (int i = 0; i < size; i++) {
        if (binaryArray[i] == 1) {
            c = (c * power) % n;
        }
        power = (power * power) % n;
    }
    return c;
}

long square (long m) {
    return m * m; // for modExp()
}

long log_100(long n) { // n/27 done in function call
    long result = log(n)/log(100); // converts to base 100
    return result;
}