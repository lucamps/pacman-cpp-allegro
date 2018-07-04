#include <allegro5/allegro.h>
#include "allegro5/allegro_image.h"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <iostream>

using namespace std;

const float FPS = 5;
const int SCREEN_W = 500;
const int SCREEN_H = 550;

enum MYKEYS
{
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT
};

//matriz definindo mapa do jogo: 1 representa paredes, 0 representa corredor
/*char MAPA[26][26] =
{
    "1111111111111111111111111",
    "1000000001111111000000001",
    "1011111101111111011111101",
    "1011111100000000011111101",
    "1000000001111111000000001",
    "1110111101111111011110111",
    "1000111100001000011110001",
    "1010111111101011111110101",
    "1010000111001001110000101",
    "1011110111011101110111101",
    "1011110100000000010111101",
    "1000110001110111000110001",
    "1110111101110111011110111",
    "1000000001110111000000001",
    "1011111101110111011111101",
    "1011100000000000000011101",
    "1000001111110111111000001",
    "1111101110000000111011111",
    "1111101110111110111011111",
    "1000000000000000000000001",
    "1011101111110111111011101",
    "1000001111110111111000001",
    "1011100000010100000011101",
    "1000001111000001111000001",
    "1111111111111111111111111",
};
*/

//matriz definindo mapa do jogo: 2 representa bolas, 1 representa paredes, 0 representa corredor
char MAPA[26][26] =
{
    "1111111111111111111111111",
    "1222222221111111222222221",
    "1211111121111111211111121",
    "1211111122222222211111121",
    "1222222221111111222222221",
    "1112111121111111211112111",
    "1222111122221222211112221",
    "1212111111121211111112121",
    "1212222111221221112222121",
    "1211112111211121112111121",
    "1211112122222222212111121",
    "1222112221112111222112221",
    "1112111121112111211112111",
    "1222222221112111222222221",
    "1211111121112111211111121",
    "1211122222220222222211121",
    "1222221111112111111222221",
    "1111121112222222111211111",
    "1111121112111112111211111",
    "1222222222222222222222221",
    "1211121111112111111211121",
    "1222221111112111111222221",
    "1211122222212122222211121",
    "1222221111222221111222221",
    "1111111111111111111111111",
};

ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_BITMAP *mapa   = NULL;
ALLEGRO_BITMAP *pacman   = NULL;
ALLEGRO_BITMAP *pac_up   = NULL;
ALLEGRO_BITMAP *pac_left   = NULL;
ALLEGRO_BITMAP *pac_down   = NULL;
ALLEGRO_BITMAP *pac_right   = NULL;
ALLEGRO_BITMAP *bolas   = NULL;
ALLEGRO_SAMPLE *sample=NULL;
int i = 15, j = 12; //posição inicial do Pacman na matriz
int q = 20; //tamanho de cada célula no mapa
int posy = i*q;
int posx = j*q;
int k = 0, l = 0;  //variaveis usadas para aparição das bolas

bool key[4] = { false, false, false, false };
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
	
   	sample = al_load_sample( "waka.wav" );

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
	
    if(!pacman)
    {
        cout << "Falha ao carregar o pacman!" << endl;
        al_destroy_display(display);
		
        return 0;
    }
    al_draw_bitmap(pacman,posx,posy,0);
	
	
	bolas = al_load_bitmap("bolas.png");
	
	if(!bolas)
    {
        cout << "Falha ao carregar as bolas!" << endl;
        al_destroy_display(display);
        return 0;
    }
	
	al_draw_bitmap(bolas,k*20,l*20,0);
	
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

int main(int argc, char **argv)
{
    if(!inicializa()) return -1;

    while(!sair)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if(ev.type == ALLEGRO_EVENT_TIMER)
        {			
			
            if(key[KEY_UP] && MAPA[i-1][j] != '1')
            {
                i--;
				pacman=pac_up;
                posy = i*q;
				
				if(MAPA[i][j] == '2')   //se passa pela bola, a bola some
					MAPA[i][j] = '0';
            }

            if(key[KEY_DOWN] && MAPA[i+1][j] != '1')
            {
                i++;
				pacman=pac_down;
                posy = i*q;
				
				if(MAPA[i][j] == '2')   //se passa pela bola, a bola some
					MAPA[i][j] = '0';
            }

            if(key[KEY_LEFT] && MAPA[i][j-1] != '1')
            {
                j--;
				pacman=pac_left;
                posx = j*q;
				
				if(MAPA[i][j] == '2')   //se passa pela bola, a bola some
					MAPA[i][j] = '0';
            }

            if(key[KEY_RIGHT] && MAPA[i][j+1] != '1')
            {
                j++;
				pacman=pac_right;
                posx = j*q;
				
				if(MAPA[i][j] == '2')   //se passa pela bola, a bola some
					MAPA[i][j] = '0';
            }

            redraw = true;
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
				key[KEY_LEFT] = false;	//analogo para as teclas de baixo
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

            case ALLEGRO_KEY_ESCAPE:
                sair = true;
                break;
            }
        }

        if(redraw && al_is_event_queue_empty(event_queue))
        {
            redraw = false;

            al_clear_to_color(al_map_rgb(0,0,0));

            al_draw_bitmap(mapa,0,0,0);
            al_draw_bitmap(pacman,posx,posy,0);
			
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
    al_destroy_bitmap(pacman);
	al_destroy_bitmap(bolas);
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
	al_destroy_sample(sample);
    return 0;
}
