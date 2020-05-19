#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <iostream>

using namespace std;

#define INSUFF_FUNDS 300
#define OK 200

class Account {
    public:
        int money;
        int id;
        Account(int id) {
            this->id = id;
            this->money = 20;
        }

        void add(int sum) {
            this->money += sum;
        }

        int withdraw(int sum) {
            if (sum > this->money) {
                return INSUFF_FUNDS;
            }
            this->money -= sum;
            return OK;
        }

        void show_sum() {
            cout << "Cont " << this->id << " : " << this->money << endl;
        }
};

#endif