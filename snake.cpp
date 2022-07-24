#include <iostream>
#include <cstdlib>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

using namespace std;

const float FPS = 5;
const int SCREEN_W = 500;
const int SCREEN_H = 550;
const int QUAD_SIZE = 20;

//matriz definindo mapa do jogo: 1 representa paredes, 0 representa corredor
char MAPA[26][26] =
{
    "1111111111111111111111111",
    "1000000000000000000000001",
    "1000000000000000000000001",
    "1000000000000000000000001",
    "1000000000000000000000001",
    "1000000000000000000000001",
    "1000000000000000000000001",
    "1000000000000000000000001",
    "1000000000000000000000001",
    "1000000000000000000000001",
    "1000000000000000000000001",
    "1000000000000000000000001",
    "1000000000000000000000001",
    "1000000000000000000000001",
    "1000000000000000000000001",
    "1000000000000000000000001",
    "1000000000000000000000001",
    "1000000000000000000000001",
    "1000000000000000000000001",
    "1000000000000000000000001",
    "1000000000000000000000001",
    "1000000000000000000000001",
    "1000000000000000000000001",
    "1000000000000000000000001",
    "1111111111111111111111111",
};

int andou[26][26] = {0};
int passo = 0, placar=0;

ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_BITMAP *fim   = NULL;
ALLEGRO_BITMAP *mapa   = NULL;
ALLEGRO_BITMAP *maca = NULL;
ALLEGRO_BITMAP *cobra = NULL;

int i = 15, j = 12;   //posicao inicial da Snake na matriz
int q = 20;           //tamanho de cada celula no mapa
int posy = i*q;       //posicao da Snake no display
int posx = j*q;

bool redraw = true;
bool sair = false;
bool cima, baixo, esq, dir;

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

    timer = al_create_timer(1.0 / FPS);
    if(!timer)
    {
        cout << "Falha ao inicializar o temporizador" << endl;
        return 0;
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

    mapa = al_load_bitmap("mapa.bmp");
    if(!mapa)
    {
        cout << "Falha ao carregar o mapa!" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(mapa,0,0,0);

    cobra = al_load_bitmap("cobra.tga");
    al_draw_bitmap(cobra,posx,posy,0);
    maca = al_load_bitmap("doce.tga");      //cria a fruta
    al_draw_bitmap(maca,0,0,0);
    
    if(!cobra)
    {
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }
    if(!maca)
    {
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }

    al_set_target_bitmap(al_get_backbuffer(display)); //muda de volta o destino dos desenhos para o display

    event_queue = al_create_event_queue();
    if(!event_queue)
    {
        cout << "Falha ao criar a fila de eventos" << endl;
        al_destroy_bitmap(cobra);
        al_destroy_bitmap(maca);
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

    cima = false;
    baixo = false;
    esq = false;
    dir = false;
    srand(time(0));
    int im=rand()%23+1,jm=rand()%23+1; //posicoes da fruta
    int i0=i,j0=j; //i e j anteriores
    int tam=5; //tamanho da cobra
    int v[10000]; //vetor para posicoes do corpo da cobra
    int cont0=-2; //usado no fim como a quantidade anterior de quadrados no corpo da cobra para comparacao

    while(!sair){

        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if(ev.type == ALLEGRO_EVENT_TIMER){
            passo++;
            andou[i][j]=passo;
            if(cima && MAPA[i-1][j] != '1')
            {
                i--;
                posy = i*q;
            }

            if(baixo && MAPA[i+1][j] != '1')
            {
                i++;
                 posy = i*q;
            }
            
            if(esq && MAPA[i][j-1] != '1')
            {
                j--;
                posx = j*q;
            }

            if(dir && MAPA[i][j+1] != '1')
            {
                j++;
                posx = j*q;
            }

            if(i0==i&&j0==j&&(i!=15||j!=12)) //se i e j nao mudaram e nao estao na posicao de inicio, esta na parede
                sair=true;
            i0=i;j0=j;

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
              case ALLEGRO_KEY_UP:    //setinha pra cima
                if(!baixo){
                    cima = true;          //esta indo pra cima
                    baixo = false;
                    esq = false;
                    dir = false;
                }
                else{  //impede de ir para cima enquanto vai para baixo
                    cima=false;
                    baixo=true;
                    esq=false;
                    dir=false;
                }
                break;

              case ALLEGRO_KEY_DOWN:  //setinha pra baixo
                if(!cima){
                    cima = false;
                    baixo = true;
                    esq = false;
                    dir = false;
                }
                else{  //impede de ir para baixo enquanto vai para cima
                    cima=true;
                    baixo=false;
                    esq=false;
                    dir=false;
                }
                break;

              case ALLEGRO_KEY_LEFT:  //setinha pra esquerda
                if(!dir){
                    cima = false;
                    baixo = false;
                    esq = true;
                    dir = false;
                }
                else{  //impede de ir para esquerda enquanto vai para direita
                    cima=false;
                    baixo=false;
                    esq=false;
                    dir=true;
                }
                break;

              case ALLEGRO_KEY_RIGHT: //setinha pra direita
                if(!esq){
                    cima = false;
                    baixo = false;
                    esq = false;
                    dir = true;
                }
                else{  //impede de ir para direita enquanto vai para esquerda
                    cima = false;
                    baixo = false;
                    esq = true;
                    dir = false;
                }
                break;
            }
        }
        else if(ev.type == ALLEGRO_EVENT_KEY_UP)
        {
            switch(ev.keyboard.keycode)
            {
              case ALLEGRO_KEY_ESCAPE:  //tecla ESC
                sair = true;
                break;
            }
        }

        if(redraw && al_is_event_queue_empty(event_queue))
        {
            redraw = false;

            al_clear_to_color(al_map_rgb(0,0,0));

            al_draw_bitmap(mapa,0,0,0);
            int cont=-1;  //contador para as posicoes do vetor
            int comeu=0;
            int mostre=1;  //explicado abaixo
            for(int i=0;i<26;i++)
                for(int j=0;j<26;j++)
                    if(andou[i][j]>0 && andou[i][j]>=passo-tam-1){
                        cont++;
                        v[cont*2]=j;
                        v[cont*2+1]=i;
                        if(j==jm&&i==im) //encontra com a fruta
                            comeu=1;
                        al_draw_bitmap(cobra,j*q,i*q,0);   //desenha quadrado
                    }
            if(comeu){
                placar++;
                tam++;
                int teste=1;
                while(teste){  
                    teste=0;
                    im=rand()%23+1; //muda posicao da fruta
                    jm=rand()%23+1;
                    for(int k=cont*2+1;k>=1;k-=2)  //testa se a posicao gerada coincide com a cobra, se sim, volta para o topo e gera outra
                        if(im==v[k]&&jm==v[k-1]) 
                            teste=1;
                }
            }
            al_draw_bitmap(maca,jm*q,im*q,0); //desenha fruta
            if(cont0>cont){ //se o cont0, a quantidade anterior de quadrados do corpo da cobra, foi maior do que agora, ela se sobrepos e colidiu
                sair=true;
                mostre=0; //a cobra entrou em si mesma, e nao vamos mostrar isso no display
            }
            cont0=cont;
            if(mostre)
                al_flip_display();
            if(sair){
                fim = al_load_bitmap("fim.bmp");
                al_draw_bitmap(fim,0,0,0);
                al_flip_display();
                int esc=0;
                while(!esc){
                    ALLEGRO_EVENT ev;
                    al_wait_for_event(event_queue, &ev);
                    if(ev.type == ALLEGRO_EVENT_KEY_UP)
                    {
                        switch(ev.keyboard.keycode)
                        {
                        case ALLEGRO_KEY_ESCAPE:  //tecla ESC
                            esc=1;
                            break;
                        }
                    }
                }
            }
        }
    }

    al_destroy_bitmap(cobra);
    al_destroy_bitmap(maca);
    al_destroy_bitmap(mapa);
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);

    cout << "Seu placar foi: " << placar << endl;

    return 0;
}