#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <map>

using namespace std;

int gcdex(int a, int b, int& x, int& y) {
    if (a == 0) {
        x = 0; y = 1;
        return b;
    }
    int x1, y1;
    int d = gcdex(b % a, a, x1, y1);
    x = y1 - (b / a) * x1;
    y = x1;
    return d;
}

// Возведение в степень по модулю
long long int exponentiationModulo(long long int x, long long int y, long long int N) {
    if (y == 0) return 1;
    long long int z = exponentiationModulo(x, y / 2, N);
    if (y % 2 == 0)
        return (z * z) % N;
    else
        return (x * z * z) % N;
}

// Обратное возведение в степень
long long int moduloInverse(int a, int m) {
    if (a < 0)
        a += m;
    int x, y;
    int g = gcdex(a, m, x, y);
    if (g != 1)
        cout << "error" << endl;
    else {
        x = (x % m + m) % m;
        return x;
    }
}

// Поиск точек
vector<vector<int>> findPoints(vector<int> Ep, int p) {
    vector<vector<int>> points;
    int a = Ep[0];
    int b = Ep[1];

    for (int i = 0; i < p; i++)
        for (int j = 1; j < p; j++)
        {
            if (exponentiationModulo(i, 2, p) == int(pow(j, 3) + a * j + b) % p)
            {
                vector<int> point{ j,i };
                points.push_back(point);
            }
        }
    return points;
}

//Нахождение первообразного корня по простому модулю р
int primitiveRoot(long long p) {
    vector<int> fact; //динамический массив для факториала
    int phi = p - 1, n = phi; //функция пробегающая по всем числам от 1 до n-1
    for (int i = 2; i * i <= n; ++i)
        if (n % i == 0)
        {
            fact.push_back(i);//помещает факториал в конец
            while (n % i == 0)
                n /= i;

        }
    if (n > 1)
        fact.push_back(n);
    for (int res = 2; res <= p; ++res)
    {
        bool ok = true;
        for (size_t i = 0; i < fact.size() && ok; ++i)
            ok &= exponentiationModulo(res, phi / fact[i], p) != 1;
        if (ok) return res;

    }
    return -1;//если нет первообразного корня, передает в результат -1
}

// Сумма точек
vector<int> sumPoints(int x1, int y1, int x2, int y2, int a, int p) {
    long long int l = 0;
    if ((x1 == x2) && (y1 == y2))
        if (y1 == 0)
            return { 0,0 };
        else {
            l = ((3 * x1 * x1 + a) * moduloInverse(2 * y1, p)) % p;
            if (l < 0)
                l += p;
        }
    else if (x2 - x1 == 0)
        return { 0,0 };
    else {
        l = ((y2 - y1) * moduloInverse(x2 - x1, p)) % p;
        if (l < 0)
            l += p;
    }

    int x3 = (l * l - x1 - x2) % p;
    if (x3 < 0)
        x3 += p;
    int y3 = (l * (x1 - x3) - y1) % p;
    if (y3 < 0)
        y3 += p;
    return { x3, y3 };
}

// Произведение точки на число
vector<int> multiplyPoint(int k, int x, int y, int a, int p) {
    vector<int> startPoint{ x,y };
    vector<int> secondPoint{ x,y };
    for (int i = 0; i < k; i++) {
        secondPoint = sumPoints(startPoint[0], startPoint[1], secondPoint[0], secondPoint[1], a, p);
        if ((secondPoint[0] == 0) && (secondPoint[1] == 0)) {
            secondPoint = { x,y };
        }
    }
    return secondPoint;
}

// Шифрование
vector<vector<vector<int>>> encryption(string message, map<char, vector<int>> dict, vector<int> Pb, vector <int> Ep, int p, vector<int> G) {
    cout << endl << "Шифрование: " << endl;
    vector<vector<vector<int>>> C;
    for (int i = 0; i < message.length(); i++) {
        vector<int> Pm = dict[message[i]];
        int k = rand() % 10 + 1;
        cout << "Очередной сеансовый ключ k= " << k << endl;
        vector<int> kxPb = multiplyPoint(k, Pb[0], Pb[1], Ep[0], p);
        vector<vector<int>> Cm = { multiplyPoint(k, G[0], G[1],Ep[0], p), sumPoints(Pm[0], Pm[1], kxPb[0], kxPb[1], Ep[0], p) };
        cout << "Результат шифрования символа Cm = (" << Cm[0][0] << ", " << Cm[0][1] << ") ; (" << Cm[1][0] << ", " << Cm[1][1] << ")" << endl;
        C.push_back(Cm);
        cout << "---------" << endl;

    }
    cout << endl;
    return C;
}

// Расшифрование
string decryption(vector<vector<vector<int>>> ecnryptedMessage, int nB, map<char, vector<int>> dict, vector <int> Ep, int p) {
    string s = "";
    for (auto Cm : ecnryptedMessage) {
        vector<int> nbxKxG = multiplyPoint(nB, Cm[0][0], Cm[0][1], Ep[0], p);
        vector<int> em = sumPoints(Cm[1][0], Cm[1][1], nbxKxG[0], -nbxKxG[1], Ep[0], p);
        cout << "(" << em[0] << ", " << em[1] << ") -  ";
        for (std::map<char, vector<int>>::iterator it = dict.begin(); it != dict.end(); ++it)
            if (it->second[0] == em[0] && it->second[1] == em[1]) {
                s += it->first;
                cout << it->first << endl;
            }
    }
    return s;
}
int main()
{
    srand(time(0));
    setlocale(LC_ALL, "RUS");
    // Параметры эллиптической кривой
    vector <int> Ep{ 1,3 };

    int p = 41;
    // Точки на прямой
    vector <vector<int>> points = findPoints(Ep, p);
    // Словарь буква-точка
    map<char, vector<int>> dict = {
        {'a', {23, 37}},
        {'b', {27, 19}},
        {'c', {1, 28}},
        {'d', {12, 29}},
        {'e', {15, 23}},
        {'f', {7, 26}},
        {'g', {7, 5}},
        {'h', {15, 20}},
        {'i', {12, 12}},
        {'j', {1, 13}},
        {'k', {27, 22}},
        {'l', {23, 4}}
    };
    // Шифруемое сообщение
    string message = "cake";
    cout << "Сообщение = " << message << endl;
    // Закрытый ключ участника B
    int nB = 5;
    cout <<"Закрытый ключ B: nB = "<< nB << endl;
    // Точка G
    vector<int> G = {26, 4};
    cout << "Точка G = (" << G[0]<<", "<<G[1]<<")" << endl;
    // Открытый ключ участника B
    vector<int> Pb = multiplyPoint(nB, G[0], G[1], Ep[0], p);
    cout << "Открытый ключ B: Pb = (" << Pb[0] <<", "<< Pb[1] <<")"<< endl;

    vector<vector<vector<int>>> ecnryptedMessage = encryption(message, dict, Pb, Ep, p, G);

    cout << "Расшифрование " << endl;
    string decryptedMessage = decryption(ecnryptedMessage, nB, dict, Ep, p);

    cout <<"Результат: "<< decryptedMessage << endl;

 

    

    //for (vector<int> var : points)
    //    cout << var[0] << ' ' << var[1] << endl;
    //cout << endl;



    //int x = 23;
    //int y = 37;

    //a = 5;
    //vector<int>Pa = multiplyPoint(a, x, y, Ep[0], p);
    //cout << "Pa = " << Pa[0] << ' ' << Pa[1] << endl;

    //b = 9;
    //vector<int>Pb = multiplyPoint(b, x, y, Ep[0], p);
    //cout << "Pb = " << Pb[0] << ' ' << Pb[1] << endl;

    //vector<int> Ka = multiplyPoint(a, Pb[0], Pb[1], Ep[0], p);
    //cout << "Ka = " << Ka[0] << ' ' << Ka[1] << endl;

    //vector<int> Kb = multiplyPoint(b, Pa[0], Pa[1], Ep[0], p);
    //cout << "Kb = " << Kb[0] << ' ' << Kb[1] << endl;

}