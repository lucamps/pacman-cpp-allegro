#include <allegro5/allegro.h>
#include "allegro5/allegro_image.h"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;
const float FPS = 6.6;
const int SCREEN_W = 460; 
const int SCREEN_H = 550;   

// Struct usado para manipular as informacoes da IA do Blinky
struct Info{
    double dist;
    int x;
    int y;
    char seta;
};
bool sominicial=true; //bool usado para tocar a musica do inicio do jogo
// Funcao usada para a IA do Blinky
double distancia(int x1,int y1,int x2,int y2){

    return abs(sqrt((x2 - x1)*(x2 - x1) +  (y2 - y1)*(y2 - y1)));

}

//Variaveis para a movimenta��o do PacMan
//1= esquerda, 2=baixo, 3=direita, 4=cima.
int intencao = 0;	//inten�ao de movimento atual
int anterior = 0;	//inten��o anterior de movimento

enum MYKEYS{
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT
};


// Matriz definindo mapa do jogo: 2 representa bolas, 1 paredes e 0 corredor
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
    "11111210111111101211111",
    "12222222222122222222221",
    "12111211112121111211121",
    "12221222222022222212221",
    "11121212111111121212111",
    "12222212222122221222221",
    "12111111112121111111121",
    "12222222222222222222221",
    "11111111111111111111111",
};

// Numero de bolinhas no mapa
int bola=184;
int k=0,l=0; //Auxiliar para controlar posicionamento das bolas

ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_BITMAP *mapa   = NULL;
ALLEGRO_BITMAP *mapa2  = NULL;
ALLEGRO_BITMAP *perdeu = NULL;
ALLEGRO_BITMAP *bolas   = NULL;
ALLEGRO_BITMAP *pacman   = NULL;
ALLEGRO_BITMAP *pac_up   = NULL;
ALLEGRO_BITMAP *pac_left   = NULL;
ALLEGRO_BITMAP *pac_down   = NULL;
ALLEGRO_BITMAP *pac_right   = NULL;
ALLEGRO_BITMAP *shutup   = NULL;
ALLEGRO_BITMAP *aux   = NULL;   //Para abrir e fechar boca pacman
ALLEGRO_BITMAP *blinky   = NULL;
ALLEGRO_BITMAP *ghostAmarelo = NULL;
ALLEGRO_BITMAP *ghostVerde = NULL;
ALLEGRO_BITMAP *azul = NULL;
ALLEGRO_SAMPLE *sample = NULL;
ALLEGRO_SAMPLE *win = NULL;
ALLEGRO_SAMPLE *death = NULL;
ALLEGRO_SAMPLE *beggining = NULL;
ALLEGRO_FONT *fonte = NULL;

int i = 17, j = 11; //Posicao do PacMan
int r = 8, t = 11; // Posicao do Azul
int aX = 9, aY = 11; // Posicao do fantasma Amarelo
int vX = 10, vY = 11; // Posicao do fantasma Verde
int g = 11, h = 11; // Posicao do Blinky
int q = 20; //tamanho de cada celula no mapa

bool gameover = false; 
bool playwaka = false;
bool acabar = false;

// Variveis de posicao do pacman na tela
int posy = i*q;
int posx = j*q;
// Posi��es do Blinky
int bY = g*q;
int bX = h*q;
//Posicoes do Azul
int azulX = t*q;
int azulY = r*q;
//Posicoes do Amarelo
int amareloX = aY * q;
int amareloY = aX * q;
//Posicoes do Verde
int verdeX = vY *q;
int verdeY = vX *q;

//Auxiliares de "memoria" dos personagens, indicam o ultimo movimento realizado
int lastRandomPos = -1;
int lastAmareloPos = -1;
int lastAzulPos = -1;
int lastVerdePos = -1;
int randomIndex = -1;

//Ultimas posicoes, usado pro Blinky


//Auxiliares para controlar abrir e fechar da boca do pacman
int lastmouth, sim=0;

//bool key[4] = {false, false, false, false};
bool redraw = true;
bool sair = false;

int ulx=g; //Ultimas coordenadas do Blinky, usado pra morte do PacMan e para a movimentacao dele mesmo
int uly=h;

//Ultimas coordenadas de 3 fantasmas e do PacMan (para realizar a morte do PacMan)
int lastamarelox=aX , lastamareloy=aY, lastazulx=r, lastazuly=t, lastverdex=vX, lastverdey=vY;
int lastpacmanx = i, lastpacmany = j, lastvermelhox=g, lastvermelhoy=h;

void blinkyMove(char M[][24],int &x, int &y, int &bX, int &bY) {

                //Desce uma linha se o pacman esta embaixo
                //Se a zona for fechada, talvez essa parte nao seja necessaria

				if((x==8) && (y==11) && (j<=y)) { //ifs para o blinky nao entrar na zona de nascimento
					ulx=x;
					uly=y;
					y--;
					bX = y*q;
					return;
				}else if((x==8) && (y==11) && (j>=y)) {
					ulx=x;
					uly=y;
					y++;
					bX = y*q;
					return;
				}
				Info vai[4]; //calculo das distancias das 4 posicoes ao redor de blinky
				vai[0].dist= distancia(x+1,y,i,j); vai[0].x=x+1; vai[0].y=y;vai[0].seta='S';
				vai[1].dist= distancia(x,y-1,i,j); vai[1].x=x; vai[1].y=y-1;vai[1].seta='A';
				vai[2].dist= distancia(x-1,y,i,j); vai[2].x=x-1; vai[2].y=y;vai[2].seta='W';
				vai[3].dist= distancia(x,y+1,i,j); vai[3].x=x; vai[3].y=y+1;vai[3].seta='D';

				double less=1000;
				int which=1000;
				for(int c=0;c<4;c++){  //vendo qual distancia � menor e ao mesmo tempo acessivel
						if((vai[c].dist<less) && (M[vai[c].x][vai[c].y]!='1')){
							if((vai[c].x!=ulx) || (vai[c].y!=uly)){
							  less=vai[c].dist;
							  which=c;
							}
						}
				}
				ulx=x;
				uly=y;


				if(which==0) {   //indo pela menor distancia
					x++;
					bY = x*q;
				}

				else if(which==2) {
					x--;
					bY = x*q;
				}

				else if(which==3) {
					y++;
					bX = y*q;
				}

				else if(which==1) {
					y--;
					bX = y*q;
				}

				if(x == 10 && y == -1){
					x = 10;
					y = 23;
					bX = y*q;
					bY = x*q;
				}

				else if(x == 10 && y == 22){
					x = 10;
					y = -1;
					bX = y*q;
					bY = x*q;
				}
				

}

void semiSmart(char M[][24],int &x, int &y, int &gposX, int &gposY, int &lastThisPos) {

    // Lembrar que aqui o eixo X e controlado pelas colunas (y) e o Y pelas linhas (x)
    // Para nao alterarmos o valor de x e y nas condicionais, pois sao recebidos por referencia
    int auxX = x;
    int auxY = y;


                //Desce uma linha se o pacman esta embaixo
                if(i > x && M[auxX+1][y] != '1') {
                    if(lastThisPos == 0) return; // Se o ultimo movimento foi o contr�rio do atual, nao fazer nada
                    lastThisPos = 1;
                    x++;
                    gposY = x*q;
                }

                else if(i < x && M[auxX-1][y] != '1') {
                    if(lastThisPos == 1) return;
                    lastThisPos = 0;
                    x--;
                    gposY = x*q;
                }

                else if(j > y && M[x][auxY+1] != '1') {
                    if(lastThisPos == 3) return;
                    lastThisPos = 2;
                    y++;
                    gposX = y*q;
                }

                else if(j < y && M[x][auxY-1] != '1') {
                    if(lastThisPos == 2) return;
                    lastThisPos = 3;
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

void randomMove(char M[][24],int &x, int &y, int &gposX, int &gposY, int phantom){

    int auxX = x;
    int auxY = y;

    srand(time(NULL));
    randomIndex= rand()%4;

    // Para que os fantasmas n�o escolham fazer o mesmo movimento "aleatorio", sorteia-se outro
    // numero com base no hor�rio do sistema.
    if(phantom == 1)
        srand(time(NULL));
    randomIndex = rand()%4;

    if(phantom == 2)
        srand(time(NULL));
    randomIndex = rand()%4;


    // Teleporte
    if(auxX == 10 && auxY == -1){
        x = 10;
        y = 22;
        gposX = y*q;
        gposY = x*q;
        return;
    }

    else if(auxX == 10 && auxY == 23){
        x = 10;
        y = 0;
        gposX = y*q;
        gposY = x*q;
        return;
    }

    if(randomIndex == 0 && M[auxX-1][y] != '1'){
        if(lastRandomPos == 1) return;
        lastRandomPos = 0;
        x--;
        gposY = x*q;
    }

    else if(randomIndex == 1 && M[auxX+1][y] != '1'){
        if(lastRandomPos == 0) return;
        lastRandomPos = 1;
        x++;
        gposY = x*q;
    }

    else if(randomIndex == 2 && M[x][auxY+1] != '1'){
        if(lastRandomPos == 3) return;
        lastRandomPos = 2;
        y++;
        gposX = y*q;
    }

    else if(randomIndex == 3 && M[x][auxY-1] != '1'){
        if(lastRandomPos == 2) return;
        lastRandomPos = 3;
        y--;
        gposX = y*q;
    }

    else
        semiSmart(M,x,y,gposX,gposY, randomIndex);

}
bool comecou;

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
    win = al_load_sample("you win.wav" );
    death = al_load_sample("wasted.wav" );

    beggining = al_load_sample("beggining.wav" );
    if(!win){
        printf("Winning audio clip sample not loaded! \n");
        return -1;
    }
    if(!death){
        printf("Death clip sample not loaded! \n");
        return -1;
    }
    if(!beggining){
        printf("Beggining clip sample not loaded! \n");
        return -1;
    }
    
    sample = al_load_sample("waka.wav" ); //musica que sera carregada

   if (!sample){
      printf( "Audio clip sample not loaded!\n" );
      return -1;
   }
   
	

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
	
	
	mapa2 = al_load_bitmap("map2.bmp");
    if(!mapa2)
    {
        cout << "Falha ao carregar o mapa 2!" << endl;
        al_destroy_display(display);
        return 0;
    }
	
	perdeu = al_load_bitmap("mapmorte.bmp");
    if(!perdeu)
    {
        cout << "Falha ao carregar a imagem mapmorte.bmp!" << endl;
        al_destroy_display(display);
        return 0;
    }
    

    pacman = al_load_bitmap("pacman.png");
    pac_up = al_load_bitmap("pac_up.png");
    pac_down = al_load_bitmap("pac_down.png");
    pac_left = al_load_bitmap("pac_left.png");
    pac_right = al_load_bitmap("pac_right.png");
	shutup = al_load_bitmap("shutup.png");
	blinky = al_load_bitmap("blinky.png");
	azul = al_load_bitmap("azul.png");
	ghostAmarelo = al_load_bitmap("amarelo.png");
	ghostVerde = al_load_bitmap("verde.png");

    if(!pacman)
    {
        cout << "Falha ao carregar o pacman!" << endl;
        al_destroy_display(display);

        return 0;
    }
    al_draw_bitmap(pacman,posx,posy,0);

     if(!blinky)
    {
        cout << "Falha ao carregar o Blinky!" << endl;
        al_destroy_display(display);

        return 0;
    }
    al_draw_bitmap(blinky,bX,bY,0);

    if(!azul)
    {
        cout << "Falha ao carregar o fantasma Azul!" << endl;
        al_destroy_display(display);

        return 0;
    }
    al_draw_bitmap(azul,azulX,azulY,0);

    if(!ghostAmarelo)
    {
        cout << "Falha ao carregar o fantasma Amarelo!" << endl;
        al_destroy_display(display);

        return 0;
    }
    al_draw_bitmap(ghostAmarelo,amareloX,amareloY,0);

    if(!ghostVerde)
    {
        cout << "Falha ao carregar o fantasma Verde!" << endl;
        al_destroy_display(display);

        return 0;
    }
    al_draw_bitmap(ghostVerde,verdeX,verdeY,0);


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

    // Inicializacao das fontes
    if (!al_init_ttf_addon())
    {
        cout<< "Falha ao inicializar add-on allegro_ttf."<<endl;;
        return -1;
    }

       // Carregando o arquivo de fonte tanto para Ubuntu como para Windows
      fonte = al_load_font("/usr/share/fonts/truetype/liberation/LiberationMono-Bold.ttf", 28, 0);
      if (!fonte)
    {
        fonte = al_load_font("C:/Windows/Fonts/OCRAEXT.ttf", 28, 0);
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
int main(int argc, char **argv)
{
	int contador = 0;
	
	
	
    int pontos=0;
    if(!inicializa()) return -1;

    while(!sair)
    {
		cout << contador << endl;
		contador++;
		
		
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);
        

        if(ev.type == ALLEGRO_EVENT_TIMER){


        
        /*Movimentacao do pac man*/
       
        // Se chegar na borda do mapa, teleportar para outra
			if(intencao!=1 && j >= 22){
                i = 10;
				j = 0;
                posx = j*q;
                posy = i*q;
				
            }

            else if(intencao!=3 && j <= -1){
                i = 10;
                j = 22;
                posx = j*q;
                posy = i*q;
				
            }
		//fim do teleporte//
	
		//movimentacao baseada no jogo original
		if(intencao==4){
			if(MAPA[i-1][j] != '1'){
				pacman=pac_up;
				lastmouth=2;
				i--;
				posy = i*q;
				anterior=4;	
			}
			else{
				if(anterior==1 && MAPA[i][j-1] != '1'){
					pacman=pac_left;
					j--;
					posx = j*q;
				}
				if(anterior==3 && MAPA[i][j+1] != '1'){
					lastmouth=3;
					pacman=pac_right;
					j++;
					posx = j*q;
				}
			}
			//Se passa pela bola, a bola some
                if(MAPA[i][j] == '2'){
                    MAPA[i][j] = '0';
                    bola--;
                    pontos++;
                }
		}
		
		if(intencao==2){
			if(MAPA[i+1][j] != '1'){
				pacman=pac_down;
				lastmouth=0;
				i++;
				posy = i*q;
				anterior=2;
			}
			else{
				if(anterior==1 && MAPA[i][j-1] != '1'){			
					pacman=pac_left;
					j--;
					posx = j*q;
				}
				if(anterior==3 && MAPA[i][j+1] != '1'){
					lastmouth=3;
					pacman=pac_right;
					j++;
					posx = j*q;
				}	
			}
			if(MAPA[i][j] == '2'){
				MAPA[i][j] = '0';
				bola--;
				pontos++;
			}
		}
			
		if(intencao==1){
			if(MAPA[i][j-1] != '1'){
				pacman=pac_left;
				lastmouth=1;
				j--;
				posx = j*q;
				anterior=1;
			}
			else{
				if(anterior==2 && MAPA[i+1][j] != '1'){
					lastmouth=0;
					pacman=pac_down;
					i++;
					posy = i*q;
				}
				if(anterior==4 && MAPA[i-1][j] != '1'){					
					lastmouth=2;
					pacman=pac_up;
					i--;
					posy = i*q;
				}
			}
			if(MAPA[i][j] == '2'){
				MAPA[i][j] = '0';
				bola--;
				pontos++;
			}
		}
			
           

		if(intencao==3){ 
			if(MAPA[i][j+1] != '1'){
				pacman=pac_right;
				lastmouth=3;
				j++;
				posx = j*q;
				anterior=3;
			}
			else{				
				if(anterior==2 && MAPA[i+1][j] != '1'){
					lastmouth=0;
					pacman=pac_down;
					i++;
					posy = i*q;
				}
				if(anterior==4 && MAPA[i-1][j] != '1'){
					lastmouth=2;
					pacman=pac_up;
					i--;
					posy = i*q;
				}
			}
			if(MAPA[i][j] == '2'){
				MAPA[i][j] = '0';
				bola--;
				pontos++;
			}
		}
		//**********************************//
		
        // Controle do abrir e fechar da boca do pacman
        if(sim%2==0){
            aux = pacman;
            pacman=shutup;  //Se a variavel sim for par, redraw o pacman com boca fechada
            redraw = true;
        }
        else{
            pacman=aux;
            redraw = true; //Se nao, da redraw nele normal

            //Switch para redesenhar ultima posica do pacman apos fechar boca
            switch(lastmouth){
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
            break;
		
		
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch(ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_UP:
				anterior=intencao;
                
				intencao= 4;
                break;

            case ALLEGRO_KEY_DOWN:
				anterior=intencao;
             
				intencao=2;
                break;

            case ALLEGRO_KEY_LEFT:
				anterior=intencao;
              
				intencao=1;
                break;

            case ALLEGRO_KEY_RIGHT:
				anterior=intencao;
             
				intencao=3;
                break;
			
            }
        }
		
        
		
		else if(ev.type == ALLEGRO_EVENT_KEY_UP)
        {
			switch(ev.keyboard.keycode)
            {	
            case ALLEGRO_KEY_ESCAPE:
                sair = true;
                break;
			} 
        }
		

        al_draw_textf(fonte, al_map_rgb(200, 200, 200), 0, 505, 0, "Score: %d", pontos);
		
		
		
		
		

        if(redraw && al_is_event_queue_empty(event_queue)){

            redraw = false;

			
            
            al_clear_to_color(al_map_rgb(0,0,0));
			
			
			
			//wasted gta
			if(!gameover)
				al_draw_bitmap(mapa,0,0,0);
			else{			//troca o mapa se perdeu
				al_draw_bitmap(mapa2,0,0,0);
				
				al_destroy_sample(sample);
				al_play_sample(death, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				al_rest(0.8);
				acabar=true;
				al_draw_bitmap(perdeu,0,0,0);
				al_rest(1.3);
			}
			//******************//
			
			
            
			al_draw_textf(fonte, al_map_rgb(200, 200, 200), 0, 505, 0, "Score: %d", pontos);



			for(k=0; k <24; k++){
                for (l=0; l<24; l++){
					if(bola == 0)
						continue;
                    else if(MAPA[k][l] == '2')
                        al_draw_bitmap(bolas,l*20,k*20,0);
                }
            }
			al_draw_bitmap(pacman,posx,posy,0);
            al_draw_bitmap(ghostAmarelo,amareloX,amareloY,0);
            al_draw_bitmap(blinky,bX,bY,0);
            al_draw_bitmap(ghostVerde,verdeX,verdeY,0);
            al_draw_bitmap(azul, azulX,azulY,0);
			
			//fantasmas
			randomMove(MAPA,r,t,azulX,azulY,0); //o azul inicia com o jogo
			
			if(contador >=70) //depois de um tempo o amarelo sai
				randomMove(MAPA,aX,aY,amareloX,amareloY,1);
            
			if(contador >=140) //depois o verde
				randomMove(MAPA,vX,vY,verdeX,verdeY,2);
            
			if(contador >= 210) //por ultimo o blinky
				blinkyMove(MAPA,g,h,bX,bY);
			//***********//

            
            al_flip_display();

        }
		
		if(playwaka){
			al_play_sample(sample, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);
			playwaka = false;
		}
		
		if (sominicial){
            al_play_sample(beggining, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
            sominicial=false;
			al_rest(3.1);
			playwaka = true;
        }
		
		
		
		
		//Codigo para o pacman morrer quando ocupar a mesma posição que um fantasma
			if( (g==i && h==j) || (i==r && j==t) || (i==aX && j==aY) || (i==vX && j==vY)){
				gameover=true;
			}

                //Codigo para o pacman morrer quando ele e algum fantasma trocarem de posições
        else if ((lastpacmanx==g && lastpacmany==h && i==ulx && j==uly) || 
                  (lastpacmanx==r && lastpacmany==t && i==lastazulx && j==lastazuly) ||
                  (lastpacmanx==aX && lastpacmany==aY && i==lastamarelox && j==lastamareloy) ||
                  (lastpacmanx==vX && lastpacmany==vY && i==lastverdex && j==lastverdey) ){
                    gameover=true;
                }
				
		if(acabar){
			al_destroy_bitmap(pacman);
			al_destroy_bitmap(bolas);
			al_destroy_bitmap(blinky);
			al_destroy_bitmap(azul);
			al_destroy_bitmap(ghostAmarelo);
			al_destroy_bitmap(ghostVerde);
            al_rest(3.92);
            return 0;
            }
			

		if(bola==0){
            al_destroy_sample(sample);
            al_play_sample(win, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
            al_rest(4.8);
            return 0;
        }
       
    
        //armazena as ultimas posicoes dos fantasmas e do pacman
        lastpacmanx=i; lastpacmany=j;
        lastvermelhox=g; lastvermelhoy=h;
        lastamarelox=aX; lastamareloy=aY;
        lastverdex=vX; lastverdey=vY;
        lastazulx=r; lastazuly=t;
		
		
    }
	
	

    al_destroy_bitmap(mapa);
    al_destroy_font(fonte);
    al_destroy_bitmap(pacman);
    al_destroy_bitmap(bolas);
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_sample(sample);
    al_destroy_bitmap(blinky);
    al_destroy_bitmap(azul);
    al_destroy_bitmap(ghostAmarelo);
    al_destroy_bitmap(ghostVerde);
	al_destroy_bitmap(perdeu);
	
    return 0;
}
