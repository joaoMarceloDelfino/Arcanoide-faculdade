#pragma once
#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <iostream>

using namespace std;
template <typename T>
int jogar(T& nome);

void sobre();
void EscolherCor(int color);
bool sorteio();
template <typename T>
void sobre(T& nome);
void mostrarRanking();
int main();

struct Bola {

    int velocidadeX;
    int velocidadeY;
    int x;
    int y;

    Bola(const int& iX, const int& iY) : x(iX), y(iY) {}

    void andar(int velocidadeX, int velocidadeY = 0) {
        x += velocidadeX;
        y += velocidadeY;
    }
};

struct Paddle {
    int x;
    int y;
    int vidas = 3;
    Paddle(const int& iX, const int& iY) : x(iX), y(iY) {}

    void andar(int velocidadeX) {
        x += velocidadeX;
    }
};

struct Item {
    int x;
    int y;
    char caractere;

    Item(const char& iCaractere, const int& iX, const int& iY) : caractere(iCaractere), x(iX), y(iY) {}

    void andar(int velocidadeX) {
        x += velocidadeX;
    }

};

struct Usuario {
    string nome;
    int pontuacao;
    int duracao;
    string data;
    Usuario(const string& iNome, const int& iPontuacao, const string& iData, const int& iDuracao) : nome(iNome), pontuacao(iPontuacao), data(iData), duracao(iDuracao) {}
};
