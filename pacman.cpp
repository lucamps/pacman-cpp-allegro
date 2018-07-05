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

const float FPS = 5.8;
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

void ghostMove(char M[][24], int *pacX, int *pacY, int *ghostX, int *ghostY) {};


//matriz definindo mapa do jogo: 2 representa bolas, 1 representa paredes, 0 representa corredor
char MAPA[24][24] = //13, 12
{
    "11111111111111111111111",
    "12222222222122222222221",
    "12111211112121111211121",
    "12111211112121111211121",
    "12222222222222222222221",
    "12111212111111121211121",
    "12222212222122221222221",
    "11111211112121111211111",
    "11111212222222221211111",
    "11111212111211121211111",
    "22222222122222122222222",
    "11111212122222121211111",
    "11111212111111121211111",
    "11111212222022221211111",
    "11111212111111121211111",
    "12222222222122222222221",
    "12111211112121111211121",
    "12221222222222222212221",
    "11121212111111121212111",
    "12222212222122221222221",
    "12111111112121111111121",
    "12222222222222222222221",
    "11111111111111111111111",
};
int bola=253; //numero de bolinhas no mapa

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
ALLEGRO_FONT *fonte = NULL;
int i = 13, j = 11; //posi��o inicial do Pacman na matriz
int g = 13, h = 12;
int q = 20; //tamanho de cada c�lula no mapa
int posy = i*q;
int posx = j*q;
int gposY = g*q;
int gposX = h*q;
int k = 0, l = 0;  //variaveis usadas para apari��o das bolas

bool key[4] = { false, false, false, false };
bool gKey[4] = {false, false, false, false};
bool redraw = true;
bool sair = false;

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

   if (!al_reserve_samples(1)){
      fprintf(stderr, "failed to reserve samples!\n");
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
      //fonte = al_load_font("/usr/share/fonts/truetype/lato/Lato-Black.ttf", 28, 0);
      fonte = al_load_font("C:/Windows/Fonts/OCRAEXT.ttf", 28, 0);
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
    int lastmouth, sim=0;//duas variaveis que serão usadas p/ abrir e fechar a boca do pac
int main(int argc, char **argv)
{
    int pontos=0;
    if(!inicializa()) return -1;

    while(!sair)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        // Movimentacao do Fantasma
            srand(time(NULL));
            int gMovimento = rand()%4;

            if(gMovimento == G_UP && MAPA[g-1][h] != '1')
            {
                h--;
                gKey[0] = true;
                gposY = h*q;

                //if(gposY%20==0)
                  //  g--;
            }

            if(gMovimento == G_DOWN && MAPA[g+1][h] != '1'){

                    g++;
                    gposY = g*q;
                    gKey[1] = true;
                   // if(gposY%20==0)
                       // g++;
                }

            if(gMovimento == G_RIGHT && MAPA[g][h+1] != '1'){

                    h++;
                    gposX = h*q;
                    gKey[2] = true;
                   // if(gposX%20==0)
                     //   h++;
            }

            if(gMovimento == G_LEFT && MAPA[g][h-1] != '1'){

                    h--;
                    gposX = h*q;
                    gKey[1] = true;
                    //if(gposX%20==0)
                      //  h--;
            }

        if(ev.type == ALLEGRO_EVENT_TIMER)
        {

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
                lastmouth=2; //armazena qual ultima posicao do pacman (p abrir e fechar boca)

                //if(posy%20==0)
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

                pacman=pac_down;
                i++;
                posy = i*q;
                lastmouth=0;

               // if(posy%20==0)
                 //   i++;

                if(MAPA[i][j] == '2'){   //se passa pela bola, a bola some
                    MAPA[i][j] = '0';
                    bola--;
                    pontos++;
                }
            }

            if(key[KEY_LEFT] && MAPA[i][j-1] != '1')
            {

                pacman=pac_left;
                j--;
                posx = j*q;
                lastmouth=1;

               // if(posx%20==0)
                 //   j--;

                if(MAPA[i][j] == '2'){   //se passa pela bola, a bola some
                    MAPA[i][j] = '0';
                    bola--;
                    pontos++;
                }
            }

            if(key[KEY_RIGHT] && MAPA[i][j+1] != '1')
            {
                j++;
                pacman=pac_right;
                posx = j*q;
                lastmouth=3;


                //if(posx%20==0)
                //    j++;

                if(MAPA[i][j] == '2'){   //se passa pela bola, a bola some
                    MAPA[i][j] = '0';
                    bola--;
                    pontos++;
                }
            }
            

        if(sim%2==0){ 
           		pacman=shutup;  //se a variavel sim e' par, redraw o pacman com boca fechada
			}else{
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
            redraw=true;

            if(bola==0){
                return 0;
            }
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
            al_draw_bitmap(ghost,gposX,gposY,0);
            al_draw_textf(fonte, al_map_rgb(200, 200, 200), 0, 505, 0, "Score: %d", pontos);

            for(k=0; k <26; k++){
                for (l=0; l<26; l++){
                    if(MAPA[k][l] == '2')
                        al_draw_bitmap(bolas,l*20,k*20,0);
                }
            }


            al_flip_display();


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
    return 0;
}
