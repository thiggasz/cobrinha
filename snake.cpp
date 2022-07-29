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
    "0000000000000000000000000",
    "0000000000000000000000000",
    "0000000000000000000000000",
    "0000000000000000000000000",
    "0000000000000000000000000",
    "0000000000000000000000000",
    "0000000000000000000000000",
    "0000000000000000000000000",
    "0000000000000000000000000",
    "0000000000000000000000000",
    "0000000000000000000000000",
    "0000000000000000000000000",
    "0000000000000000000000000",
    "0000000000000000000000000",
    "0000000000000000000000000",
    "0000000000000000000000000",
    "0000000000000000000000000",
    "0000000000000000000000000",
    "0000000000000000000000000",
    "0000000000000000000000000",
    "0000000000000000000000000",
    "0000000000000000000000000",
    "0000000000000000000000000",
    "1111111111111111111111111",
};

int andou[26][26] = {0};
char direcao[26][26];    //matriz para marcar onde o corpo deve virar e a direcao
int passo = 0, placar = 0;


ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_BITMAP *mapa   = NULL; //mapa
ALLEGRO_BITMAP *cobra = NULL;  //cobra
ALLEGRO_BITMAP *maca = NULL;  //fruta
ALLEGRO_BITMAP *doce = NULL;  //bonus
ALLEGRO_BITMAP *fim   = NULL; //tela final

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

    mapa = al_load_bitmap("mapa.png");
    if(!mapa)
    {
        cout << "Falha ao carregar o mapa!" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(mapa,0,0,0);

    cobra = al_load_bitmap("cobra(c).png");		//cria a cobra
    if(!cobra)
    {
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }
    al_draw_bitmap(cobra,0,0,0);

    maca = al_load_bitmap("maca.tga");      //cria a fruta
    if(!maca)
    {
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }
    al_draw_bitmap(maca,0,0,0);

    al_set_target_bitmap(al_get_backbuffer(display)); //muda de volta o destino dos desenhos para o display


    event_queue = al_create_event_queue();
    if(!event_queue)
    {
        cout << "Falha ao criar a fila de eventos" << endl;
        al_destroy_bitmap(cobra);
        al_destroy_bitmap(maca);
        al_destroy_bitmap(doce);
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
    int id=rand()%23+1,jd=rand()%23+1; //posicoes do doce
    int tam=5;    //tamanho da cobra
    int bonus=0; //contador do bonus
    int v[10000]; //vetor para posicoes do corpo da cobra
    int cont0=-2; //usado no fim como a quantidade anterior de quadrados do corpo da cobra, para verificar se bateu na parede ou em si mesma
    bool atravessa;  //verifica se pode atravessar parede

    while(!sair){
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if(ev.type == ALLEGRO_EVENT_TIMER){
            passo++;
            andou[i][j]=passo;

            if(j==1||j==23)  //se esta nas colunas extremas
                atravessa=true;  //pode atravessar
            else 
                atravessa=false;

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
            if(esq && MAPA[i][j-1] == '1') //se a parede esta a esquerda
            {
                if(atravessa)  //se ja estava na primeira coluna
                    j=23;
                posx = j*q;
            }
            if(dir && MAPA[i][j+1] != '1')
            {
                j++;
                posx = j*q;
            }
            if(dir && MAPA[i][j+1] == '1') //se a parede esta a direita
            {
                if(atravessa)  //se ja estava na ultima coluna
                    j=1;
                posx=j*q;
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
            bool comeu=false;  //definir se comeu a fruta
            bool mostre=true;  //explicado abaixo
            for(int i=0;i<26;i++)
                for(int j=0;j<26;j++)
                    if(andou[i][j]>0 && andou[i][j]>=passo-tam-1){
                        cont++;
                        v[cont*2]=j;
                        v[cont*2+1]=i;
                        if(j==jm&&i==im) //encontra com a fruta
                            comeu=true;
                        if(andou[i][j]==passo){  //se for cabeca
                            if(cima){
                                cobra = al_load_bitmap("cobra(c).png");  //carrega a imagem para cima
                                direcao[i][j]='c';  //marca que o corpo vira para cima nesta posicao 
                            }
                            if(baixo){
                                cobra = al_load_bitmap("cobra(b).png");
                                direcao[i][j]='b';
                            }
                            if(dir){
                                cobra = al_load_bitmap("cobra(d).png");
                                direcao[i][j]='d';
                            }
                            if(esq){
                                cobra = al_load_bitmap("cobra(e).png");
                                direcao[i][j]='e';
                            }
                        }
                        else if(andou[i][j]==passo-tam-1){ //se for rabo
                            if(direcao[i][j]=='c')
                                cobra = al_load_bitmap("rabo(c).png");
                            if(direcao[i][j]=='b')
                                cobra = al_load_bitmap("rabo(b).png");
                            if(direcao[i][j]=='d')
                                cobra = al_load_bitmap("rabo(d).png");
                            if(direcao[i][j]=='e')
                                cobra = al_load_bitmap("rabo(e).png");
                        }
                        
                        else{   //se for corpo
                            if(direcao[i][j]=='c')
                                if(andou[i][j-1]>=passo-tam-1&&direcao[i][j-1]=='d')   //se o quadrado a esquerda faz parte do corpo e esta para a direita
                                    cobra = al_load_bitmap("corpo(dc).png");   //carrega imagem da curva saindo da direita indo para cima
                                else if(andou[i][j+1]>=passo-tam-1&&direcao[i][j+1]=='e')
                                    cobra = al_load_bitmap("corpo(ec).png");
                                else
                                    cobra = al_load_bitmap("corpo(c).png"); //carrega imagem do corpo para cima
                            else if(direcao[i][j]=='d')
                                if(andou[i+1][j]>=passo-tam-1&&direcao[i+1][j]=='c') //
                                    cobra = al_load_bitmap("corpo(cd).png"); 
                                else if(andou[i-1][j]>=passo-tam-1&&direcao[i-1][j]=='b') //
                                    cobra = al_load_bitmap("corpo(ec).png");
                                else
                                    cobra = al_load_bitmap("corpo(d).png");
                            else if(direcao[i][j]=='e')
                                if(andou[i+1][j]>=passo-tam-1&&direcao[i+1][j]=='c')
                                    cobra = al_load_bitmap("corpo(ce).png"); 
                                else if(andou[i-1][j]>=passo-tam-1&&direcao[i-1][j]=='b')
                                    cobra = al_load_bitmap("corpo(dc).png");
                                else
                                    cobra = al_load_bitmap("corpo(e).png");
                            else if(direcao[i][j]=='b')
                                if(andou[i][j-1]>=passo-tam-1&&direcao[i][j-1]=='d')
                                    cobra = al_load_bitmap("corpo(ce).png"); 
                                else if(andou[i][j+1]>=passo-tam-1&&direcao[i][j+1]=='e')
                                    cobra = al_load_bitmap("corpo(cd).png");
                                else
                                    cobra = al_load_bitmap("corpo(b).png");
                        }
                        al_draw_bitmap(cobra,j*q,i*q,0);   //desenha parte da cobra
                    }
            if(comeu){
                placar++;  //adiciona ao placar
                tam++;     //aumenta ao tamanho da cobra
                bool teste=true;
                while(teste){ 
                    teste=false;
                    im=rand()%23+1; //muda posicao da fruta
                    jm=rand()%23+1;
                    for(int k=cont*2+1;k>=1;k-=2)  //testa se a posicao gerada coincide com a cobra, se sim, volta para o topo e gera outra
                        if(im==v[k]&&jm==v[k-1]) 
                            teste=true;
                }
            }
            al_draw_bitmap(maca,jm*q,im*q,0); //desenha fruta
            if(bonus==99){
                bool teste=true;
                while(teste){  
                    teste=false;
                    id=rand()%23+1; //muda posicao do doce
                    jd=rand()%23+1;
                    for(int k=cont*2+1;k>=1;k-=2)  //testa se a posicao gerada coincide com a cobra, se sim, volta para o topo e gera outra
                        if((id==v[k]&&jd==v[k-1]) || (id==im && jd==jm) ) 
                            teste=true;}
                bonus++;
            }
            else if(bonus==100){//desenhar o bonus
                doce = al_load_bitmap("doce.tga");
                al_draw_bitmap(doce,jd*q,id*q,0); //desenha o doce
                if(j==jd&&i==id){//se comeu 
                    placar+=3;
                    tam+=3;
                    bonus=0;}
            }
            else    //aumentar o contador do bonus
                bonus++;

            if(cont0>cont){ //se o cont0, a quantidade anterior de quadrados do corpo da cobra, foi maior que agora, ela encolheu ao chegar na parede ou entrou em si mesma
                sair=true;
                mostre=0; //nao vamos mostrar isso no display
            }
            cont0=cont;
            if(mostre)
                al_flip_display();
            if(sair){ //caso a cobra morra
                fim = al_load_bitmap("fim.png");  //tela final
                al_draw_bitmap(fim,0,0,0);        //desenhar tela final
                al_flip_display();
                bool esc=false;
                
                while(!esc){ //enquanto nao fechar o jogo
                    ALLEGRO_EVENT ev;
                    al_wait_for_event(event_queue, &ev);
                    if(ev.type == ALLEGRO_EVENT_KEY_UP)
                    {
                        switch(ev.keyboard.keycode)
                        {
                        case ALLEGRO_KEY_ESCAPE:  //tecla ESC
                            esc=true;
                            break;
                        }
                    }
                     else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                        break;
                    }
                }
            }
        }
    }

    al_destroy_bitmap(cobra);
    al_destroy_bitmap(maca);
    al_destroy_bitmap(doce);
    al_destroy_bitmap(mapa);
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);

    cout << "Seu placar foi: " << placar << endl;

    return 0;
}