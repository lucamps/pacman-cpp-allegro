#include <allegro5/allegro.h>
#include "allegro5/allegro_image.h"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <iostream>
#include <cstdio>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

const float FPS = 6.8;
const int SCREEN_W = 460;
const int SCREEN_H = 550;

enum MYKEYS
{
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT
};

enum GKEYS // Teclas do fantasma
{
    G_UP, G_DOWN, G_LEFT, G_RIGHT
};


//matriz definindo mapa do jogo: 2 representa bolas, 1 representa paredes, 0 representa corredor
char MAPA[24][24] =
{
    "11111111111111111111111",
    "12222222222122222222221",
    "12111211112121111211121",
    "12111211112121111211121",
    "12222222222222222222221",
    "12111212111111121211121",
    "12222212222122221222221",
    "11111211110101111211111",
    "11111210000000001211111",
    "11111210111011101211111",
    "00000000111011100000000",
    "11111210111011101211111",
    "11111210111111101211111",
    "11111210000000001211111",
    "11111210111111101011111",
    "12222222222122222222221",
    "12111211112121111211121",
    "12221222222022222212221",
    "11121212111111121212111",
    "12222212222122221222221",
    "12111111112121111111121",
    "12222222222222222222221",
    "11111111111111111111111",
};
int bola=183; //numero de bolinhas no mapa

ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_BITMAP *mapa   = NULL;
ALLEGRO_BITMAP *pacman   = NULL;
ALLEGRO_BITMAP *ghost   = NULL;
ALLEGRO_BITMAP *shutup   = NULL;
ALLEGRO_BITMAP *aux   = NULL;   //p abrir e fechar boca pacman
ALLEGRO_BITMAP *pac_up   = NULL;
ALLEGRO_BITMAP *pac_left   = NULL;
ALLEGRO_BITMAP *pac_down   = NULL;
ALLEGRO_BITMAP *pac_right   = NULL;
ALLEGRO_BITMAP *bolas   = NULL;
ALLEGRO_SAMPLE *sample = NULL;
ALLEGRO_SAMPLE *win = NULL;
ALLEGRO_FONT *fonte = NULL;
ALLEGRO_BITMAP *ghostBurro1 = NULL;
int i = 17, j = 11; //posi��o inicial do Pacman na matriz
int g = 1, h = 1; // Inicio do fantasma inteligente
int r = 1, t = 2; // r para linha e t para coluna
int q = 20; //tamanho de cada c�lula no mapa

// Variáveis de posição dos fantasmas e do pacman
int posy = i*q;
int posx = j*q;
int gposY = g*q; // Smart ghost
int gposX = h*q;
int gBurro1X = t*q;
int gBurro1Y = r*q;
int randomIndex = -1;
int lastPos = -1;
int lastSmartPos = -1;
bool stuck = true; // Guarda a ultima decisao do fantasma burro

int k = 0, l = 0;  //variaveis usadas para apari��o das bolas

bool key[4] = {false, false, false, false} ;
bool redraw = true;
bool sair = false;

void unstuck(char M[][24], int i, int j, int &x, int &y, int &gposX, int &gposY){


}
void ghostMove(char M[][24], int i, int j, int &x, int &y, int &gposX, int &gposY) {

    // Lembrar que aqui o eixo X e controlado pelas colunas (y) e o Y pelas linhas (x)

    // Para nao alterarmos o valor de x e y nas condicionais
    int auxX = x;
    int auxY = y;

                //Desce uma linha se o pacman esta embaixo

                if(i > x && M[auxX+1][y] != '1') {
                    if(lastSmartPos == 0) return; // Se a ultima
                    lastPos = 1;
                    x++;
                    gposY = x*q;
                }

                else if(i < x && M[auxX-1][y] != '1') {
                    if(lastSmartPos == 1) return;
                    lastPos = 0;
                    x--;
                    gposY = x*q;
                }

                else if(j > y && M[x][auxY+1] != '1') {
                    if(lastSmartPos == 3) return;
                    lastPos = 2;
                    y++;
                    gposX = y*q;
                }

                else if(j < y && M[x][auxY-1] != '1') {
                    if(lastSmartPos == 2) return;
                    lastPos = 3;
                    y--;
                    gposX = y*q;
                }

                else if(x == 10 && y == -1){
                    x = 10;
                    y = 23;
                    gposX = y*q;
                    gposY = x*q;
                }

                else if(x == 10 && y == 22){
                    x = 10;
                    y = -1;
                    gposX = y*q;
                    gposY = x*q;
                }


}

void burroGhostMove(char M[][24], int i, int j, int &x, int &y, int &gposX, int &gposY){

    int auxX = x; // Necessarios pois x e y sao recebidos por referencia
    int auxY = y;

    srand(time(NULL));
    randomIndex = rand()%4;

    // Se estiver preso, sortear outra posicao
    if(stuck) {
        srand(time(NULL));
        ghostMove(M,i,j,x,y,gposX,gposY);
        stuck = false;
        printf("stuck tentando ir para %d\n",randomIndex);
    }

    // Teleporte
    if(auxX == 10 && auxY == -1){
        x = 10;
        y = 22;
        gposX = y*q;
        gposY = x*q;
        printf("Entrei no portal da esquerda\n");
        return;
    }

    else if(auxX == 10 && auxY == 23){
        x = 10;
        y = 0;
        gposX = y*q;
        gposY = x*q;
        printf("Entrei no portal da direita\n");
        return;
    }

    if(randomIndex == 0 && M[auxX-1][y] != '1'){
        if(lastPos == 1) return;
        lastPos = 0;
        x--;
        gposY = x*q;
        stuck = false;


    }

    else if(randomIndex == 1 && M[auxX+1][y] != '1'){
        if(lastPos == 0) return;
        lastPos = 1;
        x++;
        gposY = x*q;
        stuck = false;
    }

    else if(randomIndex == 2 && M[x][auxY+1] != '1'){
        if(lastPos == 3) return;
        lastPos = 2;
        y++;
        gposX = y*q;
        stuck = false;

    }

    else if(randomIndex == 3 && M[x][auxY-1] != '1'){
        if(lastPos == 2) return;
        lastPos = 3;
        y--;
        gposX = y*q;
        stuck = false;
    }

    else {
        stuck = true;
    }

}


int inicializa() {
    if(!al_init())
    {
        cout << "Falha ao carregar Allegro" << endl;
        return 0;
    }
    if(!al_install_keyboard())
    {
        cout << "Falha ao inicializar o teclado" << endl;
        return 0;
    }
    timer = al_create_timer(0.8 / FPS);
    if(!timer)
    {
        cout << "Falha ao inicializar o temporizador" << endl;
        return 0;
    }

   if(!al_install_audio()){
      fprintf(stderr, "failed to initialize audio!\n");
      return -1;
   }

   if(!al_init_acodec_addon()){
      fprintf(stderr, "failed to initialize audio codecs!\n");
      return -1;
   }

   if (!al_reserve_samples(2)){
      fprintf(stderr, "failed to reserve samples!\n");
      return -1;
   }
    win = al_load_sample("beggining.wav" );
    if(!win){
        printf("Audio clip sample not loaded! \n");
        return -1;
    }

    sample = al_load_sample("suspense.wav" ); //musica que sera carregada

   if (!sample){
      printf( "Audio clip sample not loaded!\n" );
      return -1;
   }
    al_play_sample(sample, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);

    if(!al_init_image_addon())
    {
        cout <<"Falha ao iniciar al_init_image_addon!" << endl;
        return 0;
    }

    display = al_create_display(SCREEN_W, SCREEN_H);
    if(!display)
    {
        cout << "Falha ao inicializar a tela" << endl;
        al_destroy_timer(timer);
        return 0;
    }


    mapa = al_load_bitmap("map.bmp");
    if(!mapa)
    {
        cout << "Falha ao carregar o mapa!" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(mapa,0,0,0);

    pacman = al_load_bitmap("pacman.png");
    pac_up = al_load_bitmap("pac_up.png");
    pac_down = al_load_bitmap("pac_down.png");
    pac_left = al_load_bitmap("pac_left.png");
    pac_right = al_load_bitmap("pac_right.png");
	shutup = al_load_bitmap("shutup.png");
	shutup = al_load_bitmap("shutup.png");
	ghost = al_load_bitmap("ghost.png");
	ghostBurro1 = al_load_bitmap("gburro1.png");

    if(!pacman)
    {
        cout << "Falha ao carregar o pacman!" << endl;
        al_destroy_display(display);

        return 0;
    }
    al_draw_bitmap(pacman,posx,posy,0);

     if(!ghost)
    {
        cout << "Falha ao carregar o fantasma!" << endl;
        al_destroy_display(display);

        return 0;
    }

    al_draw_bitmap(ghost,gposX,gposY,0);

    if(!ghostBurro1)
    {
        cout << "Falha ao carregar o fantasma burro!" << endl;
        al_destroy_display(display);

        return 0;
    }

    al_draw_bitmap(ghostBurro1,gBurro1X,gBurro1Y,0);





    bolas = al_load_bitmap("bolas.png");

    if(!bolas)
    {
        cout << "Falha ao carregar as bolas!" << endl;
        al_destroy_display(display);
        return 0;
    }

    al_draw_bitmap(bolas,k*20,l*20,0);

    al_init_font_addon();
    al_init_ttf_addon();

    // Inicializa��o das fontes
    if (!al_init_ttf_addon())
    {
        cout<< "Falha ao inicializar add-on allegro_ttf."<<endl;;
        return -1;
    }
      // Carregando o arquivo de fonte
       // Carregando o arquivo de fonte tanto para Ubuntu como para Windows
      fonte = al_load_font("/usr/share/fonts/truetype/lato/Lato-Black.ttf", 28, 0);
      if (!fonte)
    {
        fonte = al_load_font("C:/Windows/Fonts/OCRAEXT.ttf", 28, 0);
       // return -1;
    }
        if (!fonte)
    {
        al_destroy_display(display);
       cout<< "Falha ao carregar fonte."<<endl;
        return -1;
    }

    event_queue = al_create_event_queue();
    if(!event_queue)
    {
        cout << "Falha ao criar a fila de eventos" << endl;
        al_destroy_display(display);
        al_destroy_timer(timer);
        return 0;
    }

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    al_clear_to_color(al_map_rgb(0,0,0));
    al_flip_display();
    al_start_timer(timer);

    return 1;
}
	 int lastmouth, sim=0;//duas variaveis que ser�o usadas p/ abrir e fechar a boca do pac
int main(int argc, char **argv)
{
    int pontos=0;
    if(!inicializa()) return -1;

    while(!sair)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);


        if(ev.type == ALLEGRO_EVENT_TIMER){


        /*Movimentacao do pac man*/

        // Se chegar na borda do mapa, teleportar para outra
            if(key[KEY_RIGHT] && i == 10 && j == 22){

                i = 10;
                j = -1;
                posx = j*q;
                posy = i*q;
            }


             if(key[KEY_LEFT] && i == 10 && j == -1){

                i = 10;
                j = 23;
                posx = j*q;
                posy = i*q;
            }


            if(key[KEY_UP] && MAPA[i-1][j] != '1')
            {


               pacman = pac_up;
				lastmouth=2;

                i--;
                posy = i*q;

                if(MAPA[i][j] == '2'){   //se passa pela bola, a bola some
                    MAPA[i][j] = '0';
                    bola--; //subtrai no numero de bolinhas
                    pontos++;
                }
            }

            if(key[KEY_DOWN] && MAPA[i+1][j] != '1')
            {

				lastmouth=0;
                pacman=pac_down;
                i++;
                posy = i*q;

                if(MAPA[i][j] == '2'){   //se passa pela bola, a bola some
                    MAPA[i][j] = '0';
                    bola--;
                    pontos++;
                }
            }

            if(key[KEY_LEFT] && MAPA[i][j-1] != '1')
            {

				lastmouth=1;
                pacman=pac_left;
                j--;
                posx = j*q;


                if(MAPA[i][j] == '2'){   //se passa pela bola, a bola some
                    MAPA[i][j] = '0';
                    bola--;
                    pontos++;
                }
            }

            if(key[KEY_RIGHT] && MAPA[i][j+1] != '1')
            {
				lastmouth=3;
                j++;
                pacman=pac_right;
                posx = j*q;


                if(MAPA[i][j] == '2'){   //se passa pela bola, a bola some
                    MAPA[i][j] = '0';
                    bola--;
                    pontos++;
                }
            }


        if(sim%2==0){
				aux = pacman;
           		pacman=shutup;  //se a variavel sim � par, redraw o pacman com boca fechada
				redraw = true;
			}else{
				pacman=aux;
				redraw = true; //se nao, da redraw nele normal
                switch(lastmouth){ //switch para redesenhar ultima posica do pacman apos fechar boca
                    case 0:
                        pacman=pac_down; break;
                    case 1:
                        pacman=pac_left;  break;
                    case 2:
                         pacman=pac_up;  break;
                    case 3:
                        pacman=pac_right;  break;
                }
			}
			sim++;

            
        }
        else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            break;
        }
        else if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch(ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_UP:
                key[KEY_UP] = false;
                break;

            case ALLEGRO_KEY_DOWN:
                key[KEY_DOWN] = false;
                break;

            case ALLEGRO_KEY_LEFT:
                key[KEY_LEFT] = false;
                break;

            case ALLEGRO_KEY_RIGHT:
                key[KEY_RIGHT] = false;
                break;
            }
        }
        else if(ev.type == ALLEGRO_EVENT_KEY_UP)
        {

            switch(ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_UP:
                key[KEY_UP] = true;
                key[KEY_DOWN] = false; //codigo para o pacman andar ate pressionar outra tecla
                key[KEY_LEFT] = false;  //analogo para as teclas de baixo
                key[KEY_RIGHT] = false;
                break;

            case ALLEGRO_KEY_DOWN:
                key[KEY_UP] = false;
                key[KEY_DOWN] = true;
                key[KEY_LEFT] = false;
                key[KEY_RIGHT] = false;
                break;

            case ALLEGRO_KEY_LEFT:
                key[KEY_UP] = false;
                key[KEY_DOWN] = false;
                key[KEY_LEFT] = true;
                key[KEY_RIGHT] = false;
                break;

            case ALLEGRO_KEY_RIGHT:
                key[KEY_UP] = false;
                key[KEY_DOWN] = false;
                key[KEY_LEFT] = false;
                key[KEY_RIGHT] = true;
                break;

            // MOVIMENTACAO DO FANTASMA


            case ALLEGRO_KEY_ESCAPE:
                sair = true;
                break;
            }
        }

         al_draw_textf(fonte, al_map_rgb(200, 200, 200), 0, 505, 0, "Score: %d", pontos);

        if(redraw && al_is_event_queue_empty(event_queue))
        {

            redraw = false;

            al_clear_to_color(al_map_rgb(0,0,0));

            al_draw_bitmap(mapa,0,0,0);
            al_draw_bitmap(pacman,posx,posy,0);
			for(k=0; k <26; k++){
                for (l=0; l<26; l++){
					if(bola == 0)
						continue;
                    else if(MAPA[k][l] == '2')
                        al_draw_bitmap(bolas,l*20,k*20,0);
                }
            }
            al_draw_bitmap(ghost,gposX,gposY,0);
            al_draw_bitmap(ghostBurro1, gBurro1X,gBurro1Y,0);
            al_draw_textf(fonte, al_map_rgb(200, 200, 200), 0, 505, 0, "Score: %d", pontos);

            

            ghostMove(MAPA,i,j,g,h,gposX,gposY);
            burroGhostMove(MAPA,i,j,r,t,gBurro1X,gBurro1Y);
            al_flip_display();


        }
		if(bola==0){
                al_play_sample(win, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                al_rest(4.8);
                return 0;
            }
    }

    al_destroy_bitmap(mapa);
    al_destroy_font(fonte);
    al_destroy_bitmap(pacman);
    al_destroy_bitmap(bolas);
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_sample(sample);
    al_destroy_bitmap(ghost);
    al_destroy_bitmap(ghostBurro1);
    return 0;
}
