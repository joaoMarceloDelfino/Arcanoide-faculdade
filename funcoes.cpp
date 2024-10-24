 #include <conio.h> //cores
#include <ctime> //rand
#include <iostream> //cin e cout
#include <string> //string
#include <windows.h> //playusound
#include "Cabeçalho.h" //declarar funcao
#include <chrono> //mudar frequencia de att do jogo
#include <vector>
#include <fstream>
#include <thread>
#include <sstream>
#include <utility>//retornar 2 valores diferentes na funcao, 1 bool e 1 int
#include <algorithm>
#include <cctype>
#include <climits> //ignore int max
using namespace chrono;
using namespace std;
//cor BLOCO
void EscolherCor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

//Sortear direcao bolinha inicio
bool sorteio() {
    int n = rand() % 2;
    if (n == 0) {
        return true;
    }
    else {
        return false;
    }
}
//posicionar blocos
void posicionarBlocos(int m[20][20], int primeiraLinha, int ultimaLinha, int nBlocos) {
    int nTotal = (ultimaLinha - primeiraLinha) * 18;
    int nEspacosBranco = nTotal - nBlocos;
    int nSorteio;
    int durabilidade;
    int nBlocosDisponiveis = nBlocos;

    // montar os blocos
    for (int i = primeiraLinha; i < ultimaLinha; i++) {
        for (int j = 1; j < 19; j++) {
            nSorteio = rand() % 3;
            if (nSorteio >= 1) {
                if (nBlocosDisponiveis > 0) {
                    durabilidade = rand() % 3;
                    if (durabilidade == 0) {
                        m[i][j] = 3;
                    }
                    else {
                        m[i][j] = 2;
                    }
                    nBlocosDisponiveis--;
                }
                else {
                    nEspacosBranco--;
                }

            }
            else if (nSorteio == 0) {
                if (nEspacosBranco > 0) {
                    nEspacosBranco--;
                }
                else {
                    durabilidade = rand() % 3;
                    if (durabilidade == 0) {
                        m[i][j] = 3;
                    }
                    else {
                        m[i][j] = 2;
                    }
                    nBlocosDisponiveis--;
                }
            }

        }
    }
}

// função de vitoria
void contarPontos(int m[20][20], int& pontuacao) {
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
            if (m[i][j] >= 2) {
                pontuacao += 100;
            }
        }
    }
}

//reiniciar
void reiniciar(int& blocosquebrados, Paddle& jogador, Bola& bola, int m[20][20], int& totaldepontos, int& pontos, int& duracaoTotal, steady_clock::time_point& inicio) {
    jogador.y = 16;
    jogador.x = 9;
    bola.x = 10;
    bola.y = 5;
    bola.velocidadeX = sorteio() == true ? -1 : 1;
    bola.velocidadeY = 1;
    jogador.vidas = 3;
    totaldepontos = 0;
    blocosquebrados = 0;
    pontos = 0;
    duracaoTotal = 0;
    inicio = steady_clock::now();
    posicionarBlocos(m, 2, 5, 36);
    contarPontos(m, totaldepontos);
}

int particionar(vector<Usuario>& usuarios, int inicio, int fim) {
    int pivo = usuarios[fim].pontuacao;
    int i = inicio - 1;

    for (int j = inicio; j < fim; j++) {
        if (usuarios[j].pontuacao >= pivo) { // Mantenha a comparação como '>=' para ordem decrescente
            i++;
            swap(usuarios[i], usuarios[j]);
        }
    }
    swap(usuarios[i + 1], usuarios[fim]); // Coloca o pivô na posição correta
    return i + 1;
}


// Faz o Rank ficar ordenado
void quicksort(vector<Usuario>& usuarios, int inicio, int fim) {
    if (inicio < fim) {
        int pi = particionar(usuarios, inicio, fim);
        quicksort(usuarios, inicio, pi - 1);
        quicksort(usuarios, pi + 1, fim);
    }
}

// Atualiza o Ranking
void atualizarRanking(vector<Usuario>& usuarios) {
    int n = usuarios.size();
    ofstream arquivo;
    quicksort(usuarios, 0, n - 1);
    arquivo.open("ranking.txt", std::ios::trunc);
    for (int i = 0; i < n; i++) {
        arquivo << usuarios[i].nome << " - " << usuarios[i].pontuacao << " - " << usuarios[i].data << " - " << usuarios[i].duracao << endl;
    }
    arquivo.close();
}


// Ainda ter o ranking depois de fechar o codigo
void recuperarRanking(vector<Usuario>& usuarios, bool& isRecuperado) {
    ifstream arquivoLeitura("ranking.txt");
    string linha;
    string nome;
    int pontuacao;
    string data;
    int duracao;
    if (isRecuperado == false) {
        while (getline(arquivoLeitura, linha)) {
            istringstream iss(linha);
            getline(iss, nome, ' ');
            iss.ignore(2);
            iss >> pontuacao;
            iss.ignore(3);
            getline(iss, data, ' ');
            iss.ignore(2);
            iss >> duracao;
            usuarios.push_back(Usuario(nome, pontuacao, data, duracao));
        }
    }
    isRecuperado = true;
    arquivoLeitura.close();


}


// pega a data atual
string getDataAtual() {
    auto tempoAtual = time(nullptr);
    tm tempoLocal;
    localtime_s(&tempoLocal, &tempoAtual);

    int dia = tempoLocal.tm_mday;
    int mes = tempoLocal.tm_mon + 1;
    int ano = tempoLocal.tm_year + 1900;

    ostringstream oss;
    oss << dia << "/" << mes << "/" << ano;

    return oss.str();
}


// função para o paddle andar sozinho
void paddleAndarSozinho(Paddle& jogador, Bola& bola) {
    int sorteio = rand() % 4;
    if (bola.velocidadeX > 0) {

        if (jogador.x <= 14) {
            if (sorteio != 0) {
                jogador.andar(1);
            }

        }
    }
    else if (bola.velocidadeX < 0) {
        if (jogador.x >= 4) {
            if (sorteio != 0) {
                jogador.andar(-1);
            }

        }
    }

}

// escolhe aleatoriamente o item que vai cair
void criarItem(vector<Item>& vetorItens, int x, int y) {
    char caractere;
    int nAleatorio = rand() % 4;

    switch (nAleatorio) {
    case 0:
        caractere = 43;
        break;
    case 1:
        caractere = 'V';
        break;
    case 2:
        caractere = 45;
        break;
    }

    vetorItens.push_back(Item(caractere, x, y));

}

// faz um rand para ver se vai cair o item ou nao
void criarItemProbabilidade(vector<Item>& listaItens, int i, int j) {
    // int nAleatorio=rand()%1;
    int nAleatorio = rand() % 5;

    if (nAleatorio == 4 && j != 1 && j != 19) {
        criarItem(listaItens, i, j);
    }
}

vector<Usuario> usuarios;
bool isRecuperado = false;

//funcao do jogo
template <typename T>
int jogar(T& nome) {

    auto inicio = steady_clock::now(); //timer
    auto inicio2 = steady_clock::now(); //velocidade bolinha


    /// ALERTA: NAO MODIFICAR O TRECHO DE CODIGO, A SEGUIR.
    // INICIO: COMANDOS PARA QUE O CURSOR NAO FIQUE PISCANDO NA TELA
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = false; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
    // FIM: COMANDOS PARA QUE O CURSOR NAO FIQUE PISCANDO NA TELA
    // INICIO: COMANDOS PARA REPOSICIONAR O CURSOR NO INICIO DA TELA
    short int CX = 0, CY = 0;
    COORD coord;
    coord.X = CX;
    coord.Y = CY;
    // FIM: COMANDOS PARA REPOSICIONAR O CURSOR NO INICIO DA TELA
    /// ALERTA: NAO MODIFICAR O TRECHO DE CODIGO, ACIMA.




    // Variavel para tecla pressionada
    char tecla;
    int pontos{};
    int totaldepontos{};
    int blocosquebrado{};

    const char bolinha = char(220); //bolinha

    Bola bola(10, 5); //posicação inicial bolinha
    Paddle jogador(9, 16); //posicação inicial paddle


    // decide qual lado a bolinha vai cair
    bola.velocidadeY = 1, bola.velocidadeX = sorteio() == true ? -1 : 1;



    // matriz principal do jogo
    int m[20][20] = {
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
       1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
       1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
       1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
       1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
       1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
       1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
       1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
       1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
       1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
       1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
       1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
       1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
       1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
       1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
       1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
       1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
       1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
       1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

    //total de pontos necessarios para ganhar
    posicionarBlocos(m, 2, 5, 36);
    contarPontos(m, totaldepontos);
    recuperarRanking(usuarios, isRecuperado);

    //escolher dificuldade
    int n = 0;
    int escolhadificuldade;
    vector <Item> listaItens;
    cout << "Escolha a dificuldade: " << endl;
    cout << "1. facil\n 2. medio\n 3. dificil.\n";
    cin >> escolhadificuldade;

    switch (escolhadificuldade) {
    case 1:
        n = 180;
        break;
    case 2:
        n = 150;
        break;
    case 3:
        n = 120;
        break;
    default:
        cout << "\nOpçao invalida!\n" << endl;
    }
    system("cls");


    // decide se vai jogar ou assistir
    int modoJogo;
    cout << "Escolha o modo de jogo: " << endl;
    cout << "1. 1 jogador" << endl;
    cout << "2. espectador" << endl;
    cin >> modoJogo;

    switch (modoJogo) {
    case 1:
        modoJogo = 1;
        break;
    case 2:
        modoJogo = 2;
        break;
    default:
        cout << "\nOpçao invalida!\n" << endl;
        switch (modoJogo);
    }

    system("cls");

    //loop principal do jogo
    while (jogador.vidas > -1) {

        auto agora = steady_clock::now();
        int duracaoTotal = duration_cast<seconds>(agora - inicio).count();
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
        cout << "\r" << "Tempo de jogo: " << duracaoTotal << " segundos" << flush << endl;
        EscolherCor(15);//coloca o cout em branco
        cout << endl << "Voce tem " << jogador.vidas << " vida(s) extras" << endl << pontos << " pontos" << endl;



        /// executa os movimentos da platoforma
        if (modoJogo == 1) {
            if (_kbhit()) {
                tecla = _getch();

                switch (tecla) {
                case 75:
                case 'a': /// esquerda
                    //Paddle nao passa a parede
                    if (jogador.x >= 4) {
                        jogador.andar(-1);
                    }
                    break;
                case 77:
                case 'd': /// direita
                    if (jogador.x <= 14) {
                        jogador.andar(1);
                    }
                    break;
                }
            }
        }

        //comando do chrono
        auto agora2 = steady_clock::now();
        auto duracao2 = duration_cast<milliseconds>(agora2 - inicio2);

        // Atualiza a posição da bolinha a cada n milissegundos
        if (duracao2.count() > n) {  // Aqui, n seria a frequência da atualização
            for (int i = 0; i < 20; i++) {
                for (int j = 0; j < 20; j++) {
                    if (i == jogador.y && j <= jogador.x + 3 && j >= jogador.x - 2) {
                        EscolherCor(15);
                        cout << char(220); // personagem
                    }
                    else if (i == bola.y && j == bola.x) {
                        if (m[i][j] >= 2) {

                            //digonal bola para a direita
                            if (m[i - 1][j] >= 2 && m[i][j - 1] >= 2 && bola.velocidadeX == 1) {

                                bola.velocidadeY = -bola.velocidadeY;

                                switch (m[i - 1][j]) {
                                case 2:
                                    m[i - 1][j] = 0;
                                    blocosquebrado += 100;
                                    pontos += 100;
                                    break;
                                case 3:
                                    m[i - 1][j] = 2;
                                    break;
                                }
                            }

                            //diagonal bola para a esquerda
                            else if (m[i + 1][j] >= 2 && m[i][j - 1] >= 2 && bola.velocidadeX == -1) {
                                bola.velocidadeY = -bola.velocidadeY;

                                switch (m[i + 1][j]) {
                                case 2:
                                    m[i + 1][j] = 0;
                                    blocosquebrado += 100;
                                    pontos += 100;
                                    criarItemProbabilidade(listaItens, i, j);
                                    break;
                                case 3:
                                    m[i + 1][j] = 2;
                                    break;
                                }
                            }

                            //bolinha nao sair do mapa
                            else if (m[i][j] == 1 && bola.velocidadeX == 1 && m[i - 1][j] >= 2) {
                                bola.velocidadeY = -bola.velocidadeY;

                                switch (m[i - 1][j]) {
                                case 2:
                                    m[i - 1][j] = 0;
                                    blocosquebrado += 100;

                                    pontos += 100;
                                    criarItemProbabilidade(listaItens, i, j);

                                    break;
                                case 3:
                                    m[i - 1][j] = 2;
                                    break;
                                }

                            }

                            //bolinha nao sair do mapa
                            else if (m[i][j] == 1 && bola.velocidadeX == -1 && m[i + 1][j] >= 2) {
                                switch (m[i + 1][j]) {
                                case 2:
                                    m[i + 1][j] = 0;
                                    pontos += 100;
                                    blocosquebrado += 100;

                                    criarItemProbabilidade(listaItens, i, j);
                                    break;
                                case 3:
                                    m[i + 1][j] = 2;
                                    break;
                                }

                                bola.velocidadeY = -bola.velocidadeY;

                            }
                            //bolinha vindo de cima não bugar
                            else if (bola.velocidadeY == 1) {
                                bola.velocidadeY = -bola.velocidadeY;

                                switch (m[i][j]) {
                                case 2:
                                    m[i][j] = 0;
                                    pontos += 100;
                                    blocosquebrado += 100;

                                    criarItemProbabilidade(listaItens, i, j);

                                    break;
                                case 3:
                                    m[i][j] = 2;
                                    break;
                                }
                            }

                            //quebra em geral
                            else {
                                bola.velocidadeY = -bola.velocidadeY;

                                switch (m[i][j]) {
                                case 2:
                                    m[i][j] = 0;
                                    pontos += 100;
                                    blocosquebrado += 100;

                                    criarItemProbabilidade(listaItens, i, j);

                                    break;
                                case 3:
                                    m[i][j] = 2;
                                    break;
                                }
                            }
                        }


                        EscolherCor(15);
                        cout << bolinha;

                    }


                    else {

                        switch (m[i][j]) {
                            bool validacao;
                        case 0:
                            validacao = false;
                            for (Item item : listaItens) {
                                if (item.x == i && item.y == j) {
                                    cout << item.caractere;
                                    validacao = true;
                                    break;
                                }
                            }
                            if (validacao == false) {
                                cout << " ";
                            }
                            break; // caminho
                        case 1:
                            EscolherCor(15);
                            cout << char(219);
                            break; // parede
                        case 2:
                            EscolherCor(6);
                            cout << char(219); //blocos
                            break;
                        case 3:
                            EscolherCor(4);
                            cout << char(219); //blocos
                            break;
                        } // fim switch
                    }
                }
                cout << "\n";

            } // fim for mapa
            auto agora2 = steady_clock::now();


            for (Item& item : listaItens) {
                item.andar(1);//movimentação do item
            }


            bola.andar(bola.velocidadeX, bola.velocidadeY);
            if (modoJogo == 2) {
                paddleAndarSozinho(jogador, bola);
            }
            inicio2 = agora2; // Reinicia o contador de tempo

            //vitoria
            if (blocosquebrado == totaldepontos) {
                cout << "\n" << nome << ", voce venceu o jogo, parabens!!!\n";
                cin.ignore();
                cin.ignore(1000, '\n');
                return 0;
            }


            //bolinha colisao paddle


            if (bola.y == jogador.y - 1) {
                if (bola.x <= jogador.x + 3 && bola.x >= jogador.x - 2) {
                    bola.velocidadeY = -bola.velocidadeY;

                    if (bola.x <= jogador.x + 3 && bola.x > jogador.x) { //direita
                        bola.velocidadeX = 1;


                    }

                    else if (bola.x >= jogador.x - 2 && bola.x < jogador.x + 1) { //esquerda
                        bola.velocidadeX = -1;
                    }

                }
            }

            //condicao para perder vida
            else if (bola.y > jogador.y + 2) {
                jogador.vidas--;

                  //condicao derrota
                if (jogador.vidas == -1) {
                    cout << "Voce perdeu," << nome << " sem vidas restantes\n";

                    //reiniciar / sair caso ele esteja sem vidas
                    int opcao;
                    cout << "Caso voce queira reiniciar digite 0, no entanto, se quiser sair, digite 1" << endl; \
                        cin >> opcao;
                    switch (opcao) {
                    case 0:
                        system("cls");
                        cout << "voce escolheu reiniciar o jogo!" << endl;
                        usuarios.push_back(Usuario(nome, pontos, getDataAtual(), duracaoTotal));
                        atualizarRanking(usuarios);
                        do {
                            cout << "Digite seu nome: ";
                            nome = "";
                            cin.clear();
                            cin.ignore();
                            getline(cin, nome);
                            // Verifica se o nome está vazio
                            if (nome.empty()) {
                                cout << "Erro: O nome nao pode estar vazio. Tente novamente.\n";
                                system("pause");
                            }
                        } while (nome.empty());

                        reiniciar(blocosquebrado, jogador, bola, m, totaldepontos, pontos, duracaoTotal, inicio);

                        cin.ignore(1000, '\n');
                        break;
                    case 1:
                        system("cls");
                        cout << "Obrigado por jogar!";
                        usuarios.push_back(Usuario(nome, pontos, getDataAtual(), duracaoTotal));
                        atualizarRanking(usuarios);
                        system("pause");
                        return 0;
                        cin.ignore(1000, '\n');
                        break;
                    default:
                        cout << "Opcao invalida, digite 1 para continuar ou 2 para sair!" << endl;
                    }
                    system("cls");
                }

                //continuar / reiniciar caso ele esteja com vida ainda
                else {
                    int opcao;
                    cout << nome << ", voce perdeu uma vida, agora resta apenas mais "
                        << jogador.vidas << " vida(s)" << endl
                        << "Digite 0 para continuar jogando ou 1 para reiniciar a partida ou 2 para sair";
                    cin >> opcao;

                    switch (opcao) {
                    case 0:
                        system("cls");
                        cout << "Voce escolheu continuar jogando! Boa Sorte!!!";
                        system("pause");
                        cin.ignore(1000, '\n');
                        break;
                    case 1:
                        system("cls");
                        cout << "voce escolheu reiniciar o jogo!" << endl;
                        usuarios.push_back(Usuario(nome, pontos, getDataAtual(), duracaoTotal));
                        atualizarRanking(usuarios);
                        do {
                            cout << "Digite seu nome: ";
                            nome = "";
                            cin.clear();
                            cin.ignore();
                            getline(cin, nome);
                            // Verifica se o nome está vazio
                            if (nome.empty()) {
                                cout << "Erro: O nome nao pode estar vazio. Tente novamente.\n";
                                system("pause");
                            }
                        } while (nome.empty());


                        reiniciar(blocosquebrado, jogador, bola, m, totaldepontos, pontos, duracaoTotal, inicio);
                        system("pause");

                        cin.ignore(1000, '\n');
                        break;
                    case 2:
                        system("cls");
                        cout << "Obrigado por jogar!";
                        system("pause");
                        return 0;
                        cin.ignore(1000, '\n');
                        break;
                    default:
                        cout << "Opcao invalida, digite 0 para continuar 1 para reiniciar ou 2 para sair!" << endl;
                    }
                    system("cls");
                }

                jogador.y = 16;
                jogador.x = 9;
                bola.x = 10;
                bola.y = 5;
                bola.velocidadeX = sorteio() == true ? -1 : 1;
                bola.velocidadeY = 1;
                system("cls");

            }

            for (int i = 0; i < listaItens.size(); i++) {
                if (listaItens[i].x == jogador.y) {
                    if (listaItens[i].y <= jogador.x + 3 && listaItens[i].y >= jogador.x - 2) {
                        switch (listaItens[i].caractere) {
                        case 43: //item de adicionar pontos
                            pontos += 100;
                            break;
                        case 'V': //item de adicionar vida
                            jogador.vidas++;
                            break;
                        case 45: //item de perder pontos
                            pontos -= 100;
                            break;

                        }
                        listaItens.erase(listaItens.begin() + i);
                    }
                }
                else if (listaItens[i].x == 20) {
                    listaItens.erase(listaItens.begin() + i);
                }
            }

            //colidir teto
            if (bola.y == 1) {
                bola.velocidadeY = -bola.velocidadeY;
                bola.velocidadeX = 0;
            }

            //colidir parede
            if (bola.x == 18 || bola.x == 1) {
                bola.velocidadeX = -bola.velocidadeX;
            }
        } // fim do laco do jogo

    } // fim funcao jogar
    return 0;
}

//f. sobre
void sobre() {
    cout << "Equipe de desenvolvimento: Arthur Ferreira, Felipe Meira e Joao Marcelo." << endl;
    cout << "\nSetembro/2024" << endl;
    cout << "\nProfessor: Alex Luciano Roesier Rese - Disciplina: Algoritmos e Programacao 2" << endl;
    system("pause");
}

//f. instrucoes
template <typename T>
void sobre(T& nome) {
    cout << nome << endl;
    cout << "Instrucoes do jogo:\n";
    cout << "- Voce deve quebrar todos os blocos na tela.\n";
    cout << "- Cada bloco quebrado e item pego vale 100 pontos.\n";
    cout << "- O jogo termina quando todos os blocos forem quebrados.\n";
    cout << "- O jogo tem 3 dificuldade, facil, bolinha mais lenta, medio, mais rapida e dificil, bem mais rapida do que no modo facil.\n";
    cout << "- Existem  itens, + (soma mais 100 pontos a pontuacao), - (tira 100 pontos da pontucao) e V (recebe uma vida extra).\n";
    cout << "- Para efeito de ranking quanto menor o tempo maior sera a colocacao.\n";
    cout << "Boa sorte e divirta-se!\n";
    system("pause");
    cout << "\n";
}

//função do ranking
void mostrarRanking() {

    recuperarRanking(usuarios, isRecuperado);
    atualizarRanking(usuarios);
    cout << "Ranking de colocação:\n" << endl;
    for (Usuario usuario : usuarios) {
        cout << usuario.nome << " - " << usuario.pontuacao << " - " << usuario.data << " - " << usuario.duracao << endl;
    }
}

//menu
int main() {
    int opcao;
    string nome;
    srand(time(NULL));
    cout << "Seja bem vindo ao Arkanoid.\n";

    do {
        system("cls");
        cout << "Digite seu nome: ";
        getline(cin, nome);
        // Verifica se o nome está vazio
        if (nome.empty()) {
            cout << "Erro: O nome nao pode estar vazio. Tente novamente.\n";
            system("pause");
        }
    } while (nome.empty());
    system("cls");


    do {
        cout << "Muito bem " << nome << ", entao vamos comecar o jogo!";
        cout << "\nSelecione uma das opcoes do menu:" << endl;
        cout << "\nMenu:\n" << endl;
        cout << "1. Jogar" << endl;
        cout << "2. Sobre" << endl;
        cout << "3. Regras" << endl;
        cout << "4. Ranking" << endl;
        cout << "5. Fim\n" << endl;
        cout << "Escolha uma opçao: ";
        cin >> opcao;

        // Verifica se a entrada é inválida (ex: se um caractere foi inserido)
        if (cin.fail()) {
            cout << "Entrada invalida! Por favor, insira um numero.\n";

            // Limpa o estado do `cin` e ignora os caracteres restantes no buffer
            cin.clear(); // Limpa o estado de erro de `cin`
            cin.ignore(1000, '\n'); // Ignora até 1000 caracteres ou até encontrar um '\n'
            system("cls");
        }
        else {

            switch (opcao) {
            case 1:
                system("cls");
                jogar(nome);
                return 0;
                break;
            case 2:
                system("cls");
                sobre();
                break;
            case 3:
                system("cls");
                sobre(nome);
                break;
            case 4:
                system("cls");
                mostrarRanking();
                system("pause");
                system("clear");
                break;
            case 5:
                system("cls");
                cout << "Você decidiu encerrar o jogo " << nome << ", ate a próxima!" << endl;
                system("pause");
                return 0;
                break;
            default:
                cout << "\nOpçao invalida!\n" << endl;
            }
            system("cls");
        }
    } while (opcao != 5);

    return 0;
}
