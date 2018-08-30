#include <stdio.h>
#include <math.h>
#include <string.h>
#include <allegro.h>
#include <alleggl.h>
#include <GL/glu.h>
#include "harbst.h"
//#include "mat.h"
#include "snd.h"

const bool classic_map[13][21] =
{
      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
      {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
      {1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1},
      {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
      {1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
      {1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1},
      {1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1},
      {1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1},
      {1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
      {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
      {1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
      {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

const bool graveyard_map[13][21] =
{
      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1},
      {1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1},
      {1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1},
      {1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1},
      {1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1},
      {1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1},
      {1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1},
      {1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1},
      {1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

const bool pacman_map[17][25] =
{
      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
      {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
      {1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1},
      {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
      {1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1},
      {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
      {1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1},
      {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
      {1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1},
      {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
      {1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1},
      {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
      {1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1},
      {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
      {1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1},
      {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

typedef struct LEVEL
{
   const char *name;         //Name des levels
   bool **map;               //dynamisches Bitmap mit Kartendaten
   int startposition[2][2];//wo sind die Startpositionen?
   int startdirection[2];//In welche der vier Richtungen zeigen die Racer beim Start?
   int size[2];         //Ausmaﬂe des Levels
   int maxplayers;      //f¸r wieviel Spieler ist das Level geeignet?
   float acceleration;  //Wie stark beschleunigen die Racer?
   float maxspeed;      //wie schnell sind die Racer?
   float shotspeed;     //wie schnell sind die Sch¸sse?
   int shootintervall;  //wieviel Sch¸sse pro Sekunde?
   int maxhits;
   float translate[2];    //je h‰lfte von size, um das level mittig in der 3d-welt auszurichten
   int gemstone_intervall;
}LEVEL;

typedef struct MOVEMENT
{
   float speed_ist;
   float speed_soll;
   float pos[2];
   float lastpos[2];
   int direction; //0, 1, 2, 3
   int heading; //-1, 0, 1
   int lastheading; //-1, 0, 1
   MATRIX_f m;
   bool justturned;
}MOVEMENT;


typedef struct SHOT
{
   bool active;
   MOVEMENT move;
   MODEL3D model3d;
   int t;
}SHOT;

typedef enum RACERSTATES
{
   RACER_IN_GAME,
   RACER_AWAITING_DESTRUCTION,
   RACER_EXPLODING,
   RACER_SCRIPT_CONTROLLED
};

typedef struct RACER
{
   MOVEMENT move;
   MODEL3D model3d;
   SHOT shot;
   int damage;
   int t;
   int lasthit;
   int lastcollided;
   int status;
   bool engine_is_active;
}RACER;

typedef enum GEM_STATES
{
   GEM_ACTIVE,
   GEM_INACTIVE,
   GEM_RAISING,
   GEM_WAITING
};


//////////////////////////////////////////////////////////////

bool         buttn_pressed=0, wait_for_release;
int          buttn_delay=0, input_delay;
float global_scale=8;
MATRIX_f camera_matrix=matr(0, 1000, 0, -64, 0, 6);
float camera_dist_ist=16, camera_dist_soll=16;
SOUND sound("sounds/", &camera_matrix, (VECTOR*)&camera_matrix.t[0], 512.0);
MODEL3D box;
STEREO stereo;
MATRIX_f mc;
float mousex=0, mousey=0, mousez=0;
int mouse_z_old=0;

FONT3D font3d;

RACER racer[2];
RACER *playerracer=&racer[0];
RACER *enemyracer=&racer[1];
int racer_total=2;
LEVEL level[8];
int l=0;
//LEVEL *level=&graveyard;
MODEL3D gr;
MODEL3D grid;
MODEL3D exploder;
MODEL3D gemstone;
MODEL3D maze;
const int  vorne[4]={0, 1, 2, 3};
const int rechts[4]={1, 2, 3, 0};
const int hinten[4]={2, 3, 0, 1};
const int  links[4]={3, 0, 1, 2};

const int  r0[4]={0, 3, 2, 1};
const int  r1[4]={1, 0, 3, 2};
const int  r2[4]={2, 1, 0, 3};
const int  r3[4]={3, 2, 1, 0};

bool engine_sound=0;



struct gemstone1
{
   int state;
   int pos[2], t;
}gemstone1;

void activate_gemstone()
{
  gemstone1.state=GEM_ACTIVE;
//   sound.play("chaos04", 180, 128, 2000, 0);
//   gemstone.move("base", 0, level[l].size[0]-gemstone1.pos[0], NOW);
//            sound.load("appear3");
//   sound.load("appear4");
//   sound.load("one up01");
VECTOR a;
a.x=gemstone1.pos[0];
a.y=0;
a.z=gemstone1.pos[1];

   sound.play("appear4", &a);

}


void start_gemstone()
{
   VECTOR a;
   a.x=gemstone1.pos[0];
a.y=-100;
a.z=gemstone1.pos[1];
printf("gemstone raisin\n");
   sound.play("one up01", &a);

//   gemstone.clear_all_actions();
   gemstone1.state=GEM_RAISING;
   gemstone.show("base");
   gemstone.move("base", 1, -100, NOW);
   do
   {
   gemstone1.pos[0]=ceil(rand()%level[l].size[0]);
   gemstone1.pos[1]=ceil(rand()%level[l].size[1]);
   }while(level[l].map[gemstone1.pos[0]][gemstone1.pos[1]]);
   gemstone.move("base", 0, gemstone1.pos[0], 100);
   gemstone.move("base", 2, gemstone1.pos[1], 100);
   gemstone.move("base", 1, 0, 10);
   gemstone.wait_for_move("base", 1);
   gemstone.start_function(activate_gemstone);
}


//////////////////////////////////////////////////////////////

#define MAXEXPLOSIONS 20
typedef struct EXPLOSION1
{
    short total;
    int t;
    float *particles;
    float mult;
    VECTOR flightdir;
    MATRIX_f m;
}EXPLOSION1;

class EXPLOSION3  //Klasse f¸r Explosionen
{
    private:
    int total;
    int particle_total;
    VECTOR     *particles;
    EXPLOSION1   *explosion1[MAXEXPLOSIONS];
    void kill(short i);
    public:
    void init(int part_total);
    void draw(MATRIX_f *camera_matrix);
    void start(MATRIX_f *m);
    void proceed();
};

EXPLOSION3 explosion;

void EXPLOSION3::init(int part_total)
{
    int i;
    VECTOR v1;
    this->particle_total=part_total;
    particles=(VECTOR*)malloc(sizeof(VECTOR)*particle_total);
    for (i=0; i<particle_total; i++)
	{
	   particles[i].x = rand() % 1024 - 512;
	   particles[i].y = rand() % 1024 - 512;
	   particles[i].z = rand() % 1024 - 512;
	   normalize_vector_f(&particles[i].x, &particles[i].y, &particles[i].z);
	}
	total=0;
}

void EXPLOSION3::draw(MATRIX_f *camera_matrix)
{
    int i, j;
    float mult;
    glDisable(GL_LIGHTING);
    MATRIX_f m=*camera_matrix;
   m.t[0]=0;m.t[1]=0;m.t[2]=0;

    color16(17);
   	for (j=0; j<total; j++)
	  {
   	   glPushMatrix();
   	   glMultMatrix_allegro(&explosion1[j]->m);
   	   for (i=0; i<explosion1[j]->total;i++)
   	   {
         glPushMatrix();
	      mult=explosion1[j]->particles[i]*explosion1[j]->mult;
	      glTranslatef(particles[i].x*mult, particles[i].y*mult, particles[i].z*mult);
  //       glMultMatrix_allegro_inv(&m);
         glMultMatrix_allegro(&m);
	      glBegin(GL_QUADS);
	      glVertex3f(-0.03, 0.03, 0);
	      glVertex3f( 0.03, 0.03, 0);
	      glVertex3f( 0.03,-0.03, 0);
	      glVertex3f(-0.03,-0.03, 0);
	      glEnd();
         glPopMatrix();
       }
   	   glPopMatrix();
    }
        glEnable(GL_LIGHTING);

}

void EXPLOSION3::kill(short i)
{
   free(explosion1[i]->particles);
   free(explosion1[i]);
   total--;
   if(total)explosion1[i]=explosion1[total];

}

void EXPLOSION3::start(MATRIX_f *m)
{
    int j;
    explosion1[total]=(EXPLOSION1*)malloc(sizeof(EXPLOSION1));
    explosion1[total]->particles=(float*)malloc(sizeof(float)*particle_total);
    explosion1[total]->m=identity_matrix_f;
    explosion1[total]->m.t[0]= m->t[0];
    explosion1[total]->m.t[1]= m->t[1]+0.3;
    explosion1[total]->m.t[2]= m->t[2];

    explosion1[total]->t=get_chrono();
    explosion1[total]->mult=0.1;
    explosion1[total]->total=particle_total;
    for(j=0;j<explosion1[total]->total;j++)
    {
       explosion1[total]->particles[j]=rand() % 1024;
       explosion1[total]->particles[j]/=2048;
       explosion1[total]->particles[j]+=0.1;
    }
    total++;
}

void EXPLOSION3::proceed()
{
    int i, j;
    float add=0.3;
  	for (i=0; i<total; i++)
	{
	   add=(get_chrono()-explosion1[i]->t+10000);
	   add=25/add;
   	   if (rand() % 2 ==1) explosion1[i]->total--;
   	   for(j=0;j<explosion1[i]->total;j++) explosion1[i]->mult+=add;
   	   if ((get_chrono()-explosion1[i]->t)>800)
       {
          kill(i);
          i--;
       }
    }
}

//////////////////////////////////////////////////////////////


void init_classic(LEVEL *lvl)
{
   int i, j;
   lvl->startposition[0][0]=1;
   lvl->startposition[0][1]=11;
   lvl->startposition[1][0]=19;
   lvl->startposition[1][1]=11;
   lvl->startdirection[0]=0;
   lvl->startdirection[1]=0;
   lvl->size[0]=21;
   lvl->size[1]=13;
   lvl->maxplayers=2;
   lvl->name="classic";
   lvl->maxspeed=8.0;
   lvl->map=new(bool*[21]);
   for(i=0;i<21;i++)lvl->map[i]=new(bool[13]);
   for(i=0;i<13;i++)for(j=0;j<21;j++)lvl->map[j][i]=classic_map[i][j];
   lvl->acceleration=6.0;
   lvl->shotspeed=12.0;
   lvl->shootintervall=500;
   lvl->maxhits=1;
   lvl->translate[0]=lvl->size[0]/2;
   lvl->translate[1]=lvl->size[1]/2;
   lvl->gemstone_intervall=2000;
}

void init_pacman(LEVEL *lvl)
{
   int i, j;
   lvl->startposition[0][0]=11;
   lvl->startposition[0][1]=9;
   lvl->startposition[1][0]=13;
   lvl->startposition[1][1]=9;
   lvl->startdirection[0]=3;
   lvl->startdirection[1]=1;
   lvl->size[0]=25;
   lvl->size[1]=17;
   lvl->maxplayers=2;
   lvl->name="pacman";
   lvl->maxspeed=8.0;
   lvl->map=new(bool*[25]);
   for(i=0;i<25;i++)lvl->map[i]=new(bool[17]);
   for(i=0;i<17;i++)for(j=0;j<25;j++)lvl->map[j][i]=pacman_map[i][j];
   lvl->acceleration=6.0;
   lvl->shotspeed=12.0;
   lvl->shootintervall=500;
   lvl->maxhits=5;
   lvl->translate[0]=lvl->size[0]/2;
   lvl->translate[1]=lvl->size[1]/2;
   lvl->gemstone_intervall=2000;
}

void init_graveyard(LEVEL *lvl)
{
   int i, j;
   lvl->startposition[0][0]=1;
   lvl->startposition[0][1]=6;
   lvl->startposition[1][0]=19;
   lvl->startposition[1][1]=6;
   lvl->startdirection[0]=0;
   lvl->startdirection[1]=0;
   lvl->size[0]=21;
   lvl->size[1]=13;
   lvl->maxplayers=2;
   lvl->name="graveyard";
   lvl->maxspeed=8.0;
   lvl->map=new(bool*[21]);
   for(i=0;i<21;i++)lvl->map[i]=new(bool[13]);
   for(i=0;i<13;i++)for(j=0;j<21;j++)lvl->map[j][i]=graveyard_map[i][j];
   lvl->acceleration=4.0;
   lvl->shotspeed=12.0;
   lvl->shootintervall=500;
   lvl->maxhits=5;
   lvl->gemstone_intervall=3000;
}

void init_racers()
{
   int i, s;
   for(i=0; i<racer_total;i++)
   {
      racer[i].move.speed_ist=0;
      racer[i].move.pos[0]=level[l].startposition[i][0];
      racer[i].move.pos[1]=level[l].startposition[i][1];
      racer[i].move.lastpos[0]=level[l].startposition[i][0];
      racer[i].move.lastpos[1]=level[l].startposition[i][1];
      racer[i].move.direction=level[l].startdirection[i];
      racer[i].move.heading=0;
      racer[i].move.lastheading=1;
      racer[i].move.m=identity_matrix_f;
      racer[i].move.justturned=FALSE;
      racer[i].model3d.assign_3do("3do/racer2.3do");
      racer[i].damage=0;
      racer[i].shot.model3d.assign_3do("3do/shot.3do");
      racer[i].shot.active=FALSE;
      racer[i].model3d.hide_grid("engine");
      racer[i].model3d.hide_grid("ohne");
      racer[i].model3d.hide_grid("emit");
      racer[i].model3d.hide_grid("tacho");
      racer[i].model3d.hide_grid("tacholicht");
      racer[i].status=RACER_IN_GAME;
      racer[i].lasthit=get_chrono()-1000;
   }
}

//    0
// 3  +  1
//    2

#define MAXSTARS 3000

VECTOR stars[MAXSTARS];

void init_stars()
{
    int i;
    for(i=0; i<MAXSTARS; i++)
    {
       do
       {
          stars[i].x=rand()%10000-5000;
          stars[i].y=rand()%10000-5000;
          stars[i].z=rand()%10000-5000;
       }
       while((stars[i].x*stars[i].x+stars[i].y*stars[i].y+stars[i].z*stars[i].z)>25000000);
       normalize_vector_f(&stars[i].x, &stars[i].y, &stars[i].z);
          stars[i].x*=20000;
          stars[i].y*=20000;
          stars[i].z*=20000;
    }
}

void draw_stars()
{
   color16(15);
  // glPointSize((rand()%1000)/300);
   glVertexPointer(3, GL_FLOAT, 0, &stars[0].x);
   glDrawArrays(GL_POINTS, 0, MAXSTARS);
}
void racer_blink(RACER *rac)
{
   rac->model3d.set_action_group(2);
   rac->model3d.set_color("base", 15);
   rac->model3d.set_color("ohne", 15);
   rac->model3d.set_color("kanone", 15);
   rac->model3d.set_color("emit", 15);
   rac->model3d.wait(90);
   rac->model3d.set_color("base", DEFAULT);
   rac->model3d.set_color("ohne", DEFAULT);
   rac->model3d.set_color("kanone", DEFAULT);
   rac->model3d.set_color("emit", DEFAULT);
   rac->model3d.set_action_group(0);

}

void racer_hit_actions(RACER *rac)
{
   int a=80;
   rac->model3d.set_action_group(3);
   rac->model3d.blink("base",a, a, 15, 0);
   rac->model3d.turn("base", 1, 64, NOW);
   rac->model3d.wait(a);
   rac->model3d.turn("base", 1, 128, NOW);
   rac->model3d.wait(a);
   rac->model3d.turn("base", 1, 192, NOW);
   rac->model3d.wait(a);
   rac->model3d.turn("base", 1, 0, NOW);
   rac->model3d.wait(a);
   rac->model3d.turn("base", 1, 64, NOW);
   rac->model3d.wait(a);
   rac->model3d.turn("base", 1, 128, NOW);
   rac->model3d.wait(a);
   rac->model3d.turn("base", 1, 192, NOW);
   rac->model3d.wait(a);
   rac->model3d.turn("base", 1, 0, NOW);

   rac->model3d.stop_blink("base");
//   rac->model3d.wait(20);

   rac->model3d.set_action_group(0);

}

void racer_final_hit_actions(RACER *rac)
{
   rac->model3d.set_action_group(3);
   rac->model3d.turn("base", 1, 10, NOW);
   rac->model3d.turn("base", 2, 4, NOW);
   rac->model3d.turn("base", 0, -3, NOW);
   rac->model3d.turn("kanone", 2, 11, NOW);
   rac->model3d.move("kanone", 0,-0.08, NOW);

   rac->model3d.blink("base", 80, 250, 6, 0);
   rac->model3d.blink("kanone", 80, 250, 6, 0);
   rac->model3d.blink("ohne", 80, 250, 6, 0);
   rac->model3d.blink("emit", 80, 250, 6, 0);
   rac->model3d.blink("engine", 80, 250, 6, 0);
   rac->model3d.blink("duesen", 80, 250, 6, 0);
   rac->model3d.set_action_group(0);
}

void racer_start_shot_actions(RACER *rac)
{
   int a=70;
   rac->model3d.set_action_group(1);
   rac->model3d.set_color("emit", 15);
   rac->model3d.wait(a);
   rac->model3d.set_color("emit", 13);
   rac->model3d.wait(a);
   rac->model3d.set_color("emit", 11);
   rac->model3d.wait(a);
   rac->model3d.set_color("emit", 9);
   rac->model3d.wait(a);
   rac->model3d.set_color("emit", 7);
   rac->model3d.wait(a);
   rac->model3d.set_color("emit", 5);
   rac->model3d.wait(a);
   rac->model3d.set_color("emit", 3);
   rac->model3d.wait(a);
   rac->model3d.set_color("emit", 1);
   rac->model3d.wait(a);
   rac->model3d.set_color("emit", 0);
   rac->model3d.wait(a);
   rac->model3d.set_action_group(0);
}



void start_shot(RACER *racer)
{
   int i, s=-1, maxt=get_chrono();
   if ((get_chrono()-racer->t)<level[l].shootintervall) return;
//   sound.play("bubble", 180, 128, 1000, 0);
   sound.play("bubble", (VECTOR*)&racer->move.m.t[0]);
   if(racer==enemyracer)
   {
      sound.stop("ufo");
      sound.play("ufo", 20, 128, 1100, 1);
   }
   racer_start_shot_actions(racer);
   if(s<0)
   if(racer->shot.t<maxt)
   {
          maxt=racer->shot.t;
   }
   racer->shot.active=TRUE;
   racer->shot.move=racer->move;
   racer->shot.move.speed_ist=level[l].shotspeed;
   racer->shot.move.speed_soll=level[l].shotspeed;
   racer->t=get_chrono();
   racer->shot.t=get_chrono();
   switch(racer->shot.move.heading)
   {
      case -1:racer->shot.model3d.move("base", 0, -0.25, NOW);break;
      case  0:racer->shot.model3d.move("base", 0,  0, NOW);break;
      case  1:racer->shot.model3d.move("base", 0,  0.25, NOW);break;
   }
   switch(racer->shot.move.direction)  //Schuﬂ wird vor den Racer gesetzt
   {
      case  0:racer->shot.move.pos[1]-=0.75;break;
      case  1:racer->shot.move.pos[0]+=0.75;break;
      case  2:racer->shot.move.pos[1]+=0.75;break;
      case  3:racer->shot.move.pos[0]-=0.75;break;
   }
}

void explode_parts(void *pointer)
{
   RACER *rac=(RACER*)pointer;
   int i;
   for(i=1;i<16;i++)explosion.start(&rac->model3d.get_object_matrix_world(i, &rac->move.m));
   sound.play("expl", 255, 128, 1000, 0);
}

void change_level()
{
   int i;
   l++;
   printf("level: %d \n", l);
   init_racers();
}

void racer_destruct(RACER *rac)
{
   int i, a=2000;
   explosion.start(&rac->move.m);
   explosion.start(&rac->model3d.get_object_matrix_world(1, &rac->move.m));
   explosion.start(&rac->model3d.get_object_matrix_world(3, &rac->move.m));
   sound.play("expl2", 255, 128, 1000, 0);
//   rac->model3d.clear_all_actions();
   rac->model3d.assign_3do("3do/exploder.3do");
   for(i=0;i<16;i++)rac->model3d.set_color(i, 16);
   rac->model3d.explode("a01", 0, 2.5, 0.3, 150, 0.1, a);
   rac->model3d.explode("a02", 0, 5, -5, 600, 0.1, a);
   rac->model3d.explode("a03", 0, 1, -5, 600, 0.1, a);
   rac->model3d.explode("a04", -5, 6, -1, 400, 0.1, a);
   rac->model3d.explode("a05", -4.5, 4, -1, 500, 0.1, a);
   rac->model3d.explode("a06", -3, 4.5, 0, 200, 0.1, a);
   rac->model3d.explode("a07", -4, 1, 0, 250, 0.1, a);
   rac->model3d.explode("a08", 4, 1.8, -0.5, 250, 0.1, a);
   rac->model3d.explode("a09", 0, 7, 0, 900, 0.1, a);
   rac->model3d.explode("a10", -1, 4, 2, 450, 0.1, a);
   rac->model3d.explode("a11", -1, 2, 1, 300, 0.1, a);
   rac->model3d.explode("a12", -1, 4, 1, 400, 0.1, a);
   rac->model3d.explode("a13", 1.5, 4, 0.8, 400, 0.1, a);
   rac->model3d.explode("a14", 1, 3, 0.8, 100, 0.1, a);
   rac->model3d.wait(a);
   for(i=1;i<16;i++)rac->model3d.hide(i);
   rac->model3d.start_function(explode_parts, (void*)rac);

   gr.move("base", 1, -120, 30);
   gr.set_color("head", 18);
   gr.wait_for_move("base", 1);
   printf("oha!\n");
   gr.move("base", 1, 0, 30);
   gr.start_function(change_level);
   gr.wait_for_move("base", 1);
   gr.set_color("head", DEFAULT);
}

void collision_detection()
{
   int r1, s, r2;
   float dx, dz;
   
   for(r1=0;r1<racer_total;r1++)
   if(gemstone1.state==GEM_ACTIVE)
   {
      dx=racer[r1].move.pos[0]-gemstone1.pos[0];if(dx<0)dx=-dx;
      dz=racer[r1].move.pos[1]-gemstone1.pos[1];if(dz<0)dz=-dz;
//      printf(" %.2f %2.f    \n", dx, dz);
      if((dx<1)&&(dz<1))
      {
         racer[r1].damage--;
         gemstone1.state=GEM_INACTIVE;
         racer_blink(&racer[r1]);
         sound.play("sonde02", (VECTOR*)&racer[r1].move.m.t[0]);
         gemstone.hide("base");
      }
   }
   
   for(r1=0;r1<racer_total;r1++)
   for(r2=0;r2<racer_total;r2++)
   if(racer[r1].shot.active)
   {
      dx=racer[r1].shot.move.pos[0]-racer[r2].move.pos[0];
      dz=racer[r1].shot.move.pos[1]-racer[r2].move.pos[1];
      if((dx<0.5)&&(dx>-0.5)&&(dz<0.5)&&(dz>-0.5))
      {
         if(r1!=r2)
         {
            if(racer[r2].status==RACER_AWAITING_DESTRUCTION)
            {
               racer[r2].status=RACER_EXPLODING;
               gr.blink("head",80,80, 15, DEFAULT);
               gr.wait(241);
               gr.stop_blink("head");
               racer_destruct(&racer[r2]);
               racer[r1].shot.active=FALSE;
               if (&racer[r1]==enemyracer)sound.stop("ufo");
            }
            else
            {

            racer[r2].damage++;
            gr.blink("head",80,80, 15, DEFAULT);
            gr.wait(241);
            gr.stop_blink("head");
            racer_hit_actions(&racer[r2]);
            racer[r2].move.speed_ist=level[l].maxspeed*2;
            racer[r2].move.direction=racer[r1].shot.move.direction;
            racer[r1].shot.active=FALSE;
//            sound.play("hit", 220, 128, 1100, 0);
            sound.play("hit", (VECTOR*)&racer[r2].move.m.t[0]);
            if (&racer[r1]==enemyracer)sound.stop("ufo");
            racer[r2].lasthit=get_chrono();
            explosion.start(&racer[r2].move.m);
            if(racer[r2].damage>=level[l].maxhits)
            {
               racer[r2].status=RACER_AWAITING_DESTRUCTION;
               racer[r2].move.speed_soll=0;
               racer_final_hit_actions(&racer[r2]);
               racer[r2].shot.active=FALSE;
            }
            }
            
         }
         else if ((get_chrono()-racer[r1].shot.t)>300)
         {
            racer[r1].shot.active=FALSE;
            sound.play("sonde01", 200, 128, 900, 0);
            if (&racer[r1]==enemyracer)sound.stop("ufo");
            racer_blink(&racer[r1]);
         }
      }
   }
   r1=0;r2=1;
   if((racer[r1].status==RACER_IN_GAME || racer[r1].status==RACER_AWAITING_DESTRUCTION)&&
      (racer[r2].status==RACER_IN_GAME || racer[r2].status==RACER_AWAITING_DESTRUCTION))
   {
      dx=racer[r1].move.pos[0]-racer[r2].move.pos[0];if(dx<0)dx=-dx;
      dz=racer[r1].move.pos[1]-racer[r2].move.pos[1];if(dz<0)dz=-dz;
      if((dx<1)&&(dz<1)&& ((get_chrono()-racer[r1].lastcollided)>200)&& ((get_chrono()-racer[r2].lastcollided)>200))
      {
         float r1_old_speed=racer[r1].move.speed_ist;
         float r2_old_speed=racer[r2].move.speed_ist;
         racer_blink(&racer[r1]);
         racer_blink(&racer[r2]);
         sound.play("classic 02", 220, 128, 1100, 0);
         racer[r1].lastcollided=get_chrono();
         racer[r2].lastcollided=get_chrono();

         if(vorne[racer[r1].move.direction]==racer[r2].move.direction)
         {
            racer[r1].move.speed_ist=r2_old_speed;
            racer[r2].move.speed_ist=r1_old_speed;
            if(racer[r1].move.speed_ist>racer[r2].move.speed_ist)racer[r1].move.speed_ist+=2;
            else racer[r2].move.speed_ist+=2;
            if(racer[r1].move.speed_ist<racer[r1].move.speed_soll)racer[r1].move.speed_soll=racer[r1].move.speed_ist;
            if(racer[r2].move.speed_ist<racer[r2].move.speed_soll)racer[r2].move.speed_soll=racer[r2].move.speed_ist;
         }
         else if(hinten[racer[r1].move.direction]==racer[r2].move.direction)
         {
            racer[r2].move.direction=hinten[racer[r2].move.direction];
            racer[r1].move.direction=hinten[racer[r1].move.direction];
            racer[r1].move.speed_ist=r2_old_speed;
            racer[r2].move.speed_ist=r1_old_speed;
            racer[r1].move.speed_ist+=2;
            racer[r2].move.speed_ist+=2;
            if(racer[r1].move.speed_ist<racer[r1].move.speed_soll)racer[r1].move.speed_soll=racer[r1].move.speed_ist;
            if(racer[r2].move.speed_ist<racer[r2].move.speed_soll)racer[r2].move.speed_soll=racer[r2].move.speed_ist;
         }
         else
         {
            switch(racer[r1].move.direction)
            {
            case 0:
            case 2:
               if(dz>dx){racer[r1].move.direction=hinten[racer[r1].move.direction];racer[r1].move.speed_ist+=2;}
               else {racer[r2].move.direction=hinten[racer[r2].move.direction];racer[r2].move.speed_ist+=2;}
            break;
            case 1:
            case 3:
               if(dz<dx){racer[r1].move.direction=hinten[racer[r1].move.direction];racer[r1].move.speed_ist+=2;}
               else {racer[r2].move.direction=hinten[racer[r2].move.direction];racer[r2].move.speed_ist+=2;}
            break;
            }
         }
         
      }
   }
}

void draw_level()
{
   int x, z;
   MATRIX_f m;
   color16(10, 128);
   for(x=0;x<level[l].size[0];x++)
   for(z=0;z<level[l].size[1];z++)
   {
      m=matr(x, 0, z, 0, 0, 0);
      glPushMatrix();
      glMultMatrix_allegro(&m);
      if(level[l].map[x][z])gr.draw();
      glPopMatrix();
   }
}

bool changedir(MOVEMENT *move) //Funktion, die die Flugrichtung eines Racers oder Schusses ver‰ndert
{
   int x=floor(move->pos[0]);
   int z=floor(move->pos[1]);

   bool p[4], q[4];
   int d=move->direction;
   p[r0[d]] =!level[l].map[x][z-1];
   p[r1[d]]=!level[l].map[x+1][z];
   p[r2[d]]=!level[l].map[x][z+1];
   p[r3[d]] =!level[l].map[x-1][z];
   move->justturned=TRUE;
   switch(move->heading)
   {
      case -1:
         if(p[3])move->direction=(links[d]);
         else if(p[0])move->direction=(vorne[d]);
         else if(p[1])move->direction=(rechts[d]);
         else move->direction=(hinten[d]);
         break;
      case 0:
         if(p[0])move->direction=(vorne[d]);
         else if(move->lastheading==-1)
         {
            if(p[3])move->direction=(links[d]);
            else if(p[1])move->direction=(rechts[d]);
            else move->direction=(hinten[d]);
         }
         else
         {
            if(p[1])move->direction=(rechts[d]);
            else if(p[3])move->direction=(links[d]);
            else move->direction=(hinten[d]);
         }
         move->justturned=FALSE;
         break;
      case 1:
         if(p[1])move->direction=(rechts[d]);
         else if(p[0])move->direction=(vorne[d]);
         else if(p[3])move->direction=(links[d]);
         else move->direction=(hinten[d]);
         break;
   }
   return(d!=move->direction);
}


void check_boundary(MOVEMENT *move)
{
   if((move->pos[0]<0)||(move->pos[0]>(level[l].size[0]))||(move->pos[1]<0)||(move->pos[1]>(level[l].size[1])))
   {
      move->pos[0]=move->lastpos[0];
      move->pos[1]=move->lastpos[1];
   }
}

bool proceed_move(MOVEMENT *move)
//Hier werden Sch¸sse und Racer fortbewegt.
//Es wird auch erkannt, ob ein Richtungswechsel ansteht
{
   move->lastpos[0]=move->pos[0];
   move->lastpos[1]=move->pos[1];
   int x=floor(move->pos[0]), xo=x;
   int z=floor(move->pos[1]), zo=z;
   float delta=0;
   bool c=FALSE;
   accelerate(move->speed_soll, &move->speed_ist, level[l].acceleration);
   switch(move->direction)
   {
      case 0:
         move->pos[1]-=move->speed_ist*get_fmult();
         z=floor(move->pos[1]);
         check_boundary(move);
         if(level[l].map[x][z] || ((z!=zo)&&move->heading&&!move->justturned))
         {
            delta=z+1-move->pos[1];
            move->pos[1]=z+1;
            c=changedir(move);
         }
         break;
     case 1:
         move->pos[0]+=move->speed_ist*get_fmult();
         x=floor(move->pos[0]);
         check_boundary(move);
         if(level[l].map[x+1][z] || ((x!=xo)&&move->heading&&!move->justturned))
         {
            delta=move->pos[0]-x;
            move->pos[0]=x;
            c=changedir(move);
         }
         break;
     case 2:
         move->pos[1]+=move->speed_ist*get_fmult();
         z=floor(move->pos[1]);
         check_boundary(move);
         if(level[l].map[x][z+1] || ((z!=zo)&&move->heading&&!move->justturned))
         {
            delta=move->pos[1]-z;
            move->pos[1]=z;
            c=changedir(move);
         }
         break;
     case 3:
         move->pos[0]-=move->speed_ist*get_fmult();
         x=floor(move->pos[0]);
         check_boundary(move);
         if(level[l].map[x][z] || ((x!=xo)&&move->heading&&!move->justturned))
         {
            delta=x+1-move->pos[0];
            move->pos[0]=x+1;
            c=changedir(move);
         }
         break;
   }
   switch(move->direction)
   {
      case 0: move->pos[1]-=delta;break;
      case 1: move->pos[0]+=delta;break;
      case 2: move->pos[1]+=delta;break;
      case 3: move->pos[0]-=delta;break;
   }

   if(!c) move->justturned=FALSE;
  move->m=matr(move->pos[0], 0, move->pos[1], 0, -64*move->direction, 0);
   return(c);
}

char buf[64];

void stereo_render(bool eye)
/*  In dieser Funkion wird alles Stereoskopische gezeichnet.
 *  Sie wird deshalb auch zweimal pro Frame aufgerufen.
 *  Einmal mit eye=0 und einmal mit eye=1;
 */
{
   int i, j;
   stereo.set_scale(global_scale);
   stereo.set_eye(eye);
	stereo.position_camera(&camera_matrix);
 	glDisable(GL_DEPTH_TEST);
 	draw_stars();
 	glEnable(GL_DEPTH_TEST);
   color16(9);
      grid.draw();

   draw_level();
   explosion.draw(&camera_matrix);

   glPopMatrix();
   glPushMatrix();
   glMultMatrix_allegro(&racer[1].move.m);
   racer[1].model3d.draw();
   glPopMatrix();
   
   glPushMatrix();
   glMultMatrix_allegro(&racer[0].move.m);
   racer[0].model3d.draw();
   glPopMatrix();
   
   maze.draw();
   gemstone.draw();
   
   for(i=0;i<racer_total; i++)
   if(racer[i].shot.active)
   {
      glPushMatrix();
      glMultMatrix_allegro(&racer[i].shot.move.m);
      racer[i].shot.model3d.draw_objects(0);
      glPopMatrix();
   }
//   glTranslatef(level[l].translate[0], 0, level[l].translate[1]);




   
   glPushMatrix();
   glTranslatef(-100, 0, 0);
   glRotatef(90.0, 0.0, 1.0, 0.0);
   color16(8);
   sprintf(buf, "active: %d \ninactive: %d", get_active_actions_total(), get_inactive_actions_total());

   font3d.draw(buf);
   glPopMatrix();

   glPushMatrix();
   glTranslatef(-15, -99.9, -15);
   glRotatef(-90.0, 1.0, 0.0, 0.0);
   sprintf(buf, "%s\n", level[l].name);font3d.draw(buf);

   if(get_chrono()-racer[0].lasthit>200) color16(8); else color16(15);
   sprintf(buf, "\n\nhannes: %d\n", racer[0].damage);
   font3d.draw(buf);

   if(get_chrono()-racer[1].lasthit>200) color16(8); else color16(15);
   sprintf(buf, "martin: %d ", racer[1].damage);
   font3d.draw(buf);
   glPopMatrix();
   glPushMatrix();
   glTranslatef(100, 0, 0);
   glRotatef(270.0, 0.0, 1.0, 0.0);
   sprintf(buf, "fps %.2f", get_fps());
   font3d.draw(buf);
   glPopMatrix();

}


void render()
{

   glViewport(0,  0, SCREEN_W, SCREEN_H); //113
    ////////////////////////////////////////////////////////////////////////////
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
	stereo_render(LEFT_EYE);

	////////////////////////////////////////////////////////////////////////////
	glClear(GL_DEPTH_BUFFER_BIT );
	stereo_render(RIGHT_EYE);

	////////////////////////////////////////////////////////////////////////////
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
//				                   allegro_message("argh!");
	glFlush();
	allegro_gl_flip();

}

void user_input()
{
   int mickeyx, mickeyy, mickeyz;
   poll_keyboard();
   mouse_z_old=mouse_z;                        //Kamera-Steuerung ->
   poll_mouse();
   mickeyz=mouse_z-mouse_z_old;
   float p[3];
   get_mouse_mickeys(&mickeyx, &mickeyy);
   float x=mickeyx;float y=mickeyy;
   camera_dist_soll=playerracer->move.speed_ist/2+6;
   accelerate(camera_dist_soll, &camera_dist_ist, 4);
   get_x_rotate_matrix_f(&mc, -5); //Kamera ist nie direkt auf den playerracer gerichtet
   mousex+=x/8;mousey+=y/8;
   if (mousey<3)mousey=3;
   else if (mousey>64)mousey=64;
   MATRIX_f m=matr(0, 0, 0, -mousey, -mousex, 0);
   p[0]=playerracer->move.m.t[0]+camera_dist_ist*m.v[2][0];
   p[1]=camera_dist_ist*m.v[2][1];
   p[2]=playerracer->move.m.t[2]+camera_dist_ist*m.v[2][2];
   matrix_mul_f(&m, &mc, &m);
   m.t[0]=p[0]; m.t[1]=p[1];m.t[2]=p[2];
//   camera_matrix=m;                             //<- Kamera-Steuerung
                                                //Geschwindigkeit->
   if(playerracer->status==RACER_IN_GAME)
   {
    playerracer->move.speed_soll+=mickeyz*level[l].maxspeed/8;//<-Geschwindigkeit
   if(playerracer->move.speed_soll>level[l].maxspeed)playerracer->move.speed_soll=level[l].maxspeed;
   else if(playerracer->move.speed_soll<0)playerracer->move.speed_soll=0;



   if(mouse_b & 1)  //Steuer links eingeschlagen
   {
       playerracer->move.heading=-1;
       if(playerracer->move.lastheading!=-1)playerracer->model3d.move("kanone", 0, -0.25, NOW);
       playerracer->move.lastheading=-1;
   }
   else if(mouse_b & 2)  //Steuer rechts eingeschlagen
   {
       playerracer->move.heading=1;
       if(playerracer->move.lastheading!=1)playerracer->model3d.move("kanone", 0, 0.25, NOW);
       playerracer->move.lastheading=1;
   }
   else                 //Steuer mittig
   {
      playerracer->move.heading=0;
      if(playerracer->move.lastheading!=0)playerracer->model3d.move("kanone", 0, 0, NOW);
      playerracer->move.lastheading=0;
   }
   if(key[KEY_LCONTROL])  //Schuﬂ!
   {
      start_shot(playerracer);
   }
}
   if(key[KEY_SPACE])  //Schuﬂ!
   {
      start_shot(&racer[1]);
   }
   clear_keybuf();
}
int sn;

void game_loop()
{
   int i, s;
    while (!key[KEY_ESC])				
	{
        proceed_timer();

         for(i=0;i<racer_total;i++)
         {if(proceed_move(&racer[i].move)
         )
//         sound.play("turn3", 90, 128, 1000, 0);
         sound.play("turn3", &racer[i].move.m.t[0]);
      }
       user_input();
      for(i=0;i<racer_total;i++) if(racer[i].shot.active) proceed_move(&racer[i].shot.move);

       proceed_actions();
       explosion.proceed();
       update_color_fx();
      if((playerracer->damage>0) && (enemyracer->damage>0) && (gemstone1.state==GEM_INACTIVE))
      {
         gemstone1.state=GEM_WAITING;
         gemstone.wait(level[l].gemstone_intervall);
         gemstone.start_function(start_gemstone);
      }
      
      collision_detection();
      if ((playerracer->move.speed_ist<playerracer->move.speed_soll))
      {
             sound.adjust(sn, 220, 128, 1000, 1);
             if(!playerracer->engine_is_active){playerracer->model3d.set_color("duesen", 18);playerracer->engine_is_active=1;}
       }
      else
      {
             sound.adjust(sn, 0, 128, 1000, 1);
             if(playerracer->engine_is_active){playerracer->model3d.set_color("duesen", DEFAULT);playerracer->engine_is_active=0;}
      }
      if (enemyracer->shot.active)sound.adjust("ufo", (VECTOR*)&enemyracer->shot.move.m.t[0], 20);


		render();
	}
}

void init_gfx()
{
   allegro_gl_clear_settings();
   set_color_depth(32);
   allegro_gl_set (AGL_COLOR_DEPTH, 32);
   allegro_gl_set (AGL_Z_DEPTH, 32);
   allegro_gl_set (AGL_DOUBLEBUFFER, 1);
   allegro_gl_set (AGL_RENDERMETHOD, 1);
   allegro_gl_set (AGL_FULLSCREEN, TRUE);
   allegro_gl_use_mipmapping(FALSE);
//   allegro_gl_set(AGL_SUGGEST, AGL_COLOR_DEPTH | AGL_Z_DEPTH | AGL_DOUBLEBUFFER | AGL_RENDERMETHOD | AGL_FULLSCREEN);
   set_gfx_mode(GFX_OPENGL_FULLSCREEN, 1024, 768, 0, 0);
//   set_gfx_mode(GFX_OPENGL_WINDOWED, 1024, 768, 0, 0);
	glPolygonMode(GL_FRONT, GL_FILL);            //F¸lle die sichtbaren Polygone aus
   glPolygonMode(GL_BACK, GL_POINT);            //errechne f¸r die unsichtbaren Polys nur die Punkte
	glCullFace(GL_BACK);                         //zeichne nur die Vorderseite der Polygone
	glEnable(GL_CULL_FACE);                      //aktiviere BackFaceCulling
	glFrontFace(GL_CW);                          //die sichtbaren Polygone sind die linksgedrehten
	glShadeModel(GL_FLAT);                       //heiﬂt: kein Gouraud-Shading
	glAlphaFunc(GL_GREATER, 0.5);                //Pixel mit einem Alpha unter 0.5 werden ignoriert
   glEnableClientState (GL_VERTEX_ARRAY);       //wird noch nicht gebraucht
   glBlendFunc(GL_ONE, GL_ONE);                 //Blending-Methode. RGB-Werte werden zusammenaddiert
   glEnable (GL_LINE_SMOOTH);                           //Schaltet Linien-Antialiasing ein
   glEnable (GL_POINT_SMOOTH);                          //Schaltet Punkt-Antialiasing ein
   glEnable(GL_BLEND);                                  //Schaltet Blenden-Effekte ein
   glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  //aktiviert den Alpha-Kanal
   glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);          //?
   glLineWidth (1.0);                                   //Linienst‰rke = 0.2 Ppixel...
   glPointSize(3.0);
   glEnable(GL_DEPTH_TEST);                             //Schalte Tiefentest ein
   glPolygonOffset(1, 1);
   if (!allegro_gl_get(AGL_RENDERMETHOD)) allegro_message("error: hardware-rendering is not performed. \n\nTry installing the vendor's newest graphics-card-drivers");
}

int main(void)
{
   int i;
	allegro_init();
	install_allegro_gl();
	install_keyboard();
	install_mouse();
   set_keyboard_rate(1, 10);
   init_models(32);

   init_stars();
  	init_gfx();
   font3d.assign_3do("3do/font.3do");
   init_actions(256, 256);
   init_stereo_bitmaps(2);

   init_classic(&level[0]);
   init_graveyard(&level[1]);
   init_pacman(&level[2]);
   
 	load_color_table("color_tables/editorphoto.conf");
	stereo.init_lighting();
   init_racers();
   init_sound();
//   sound.camera_m=&camera_matrix;
//   sound.camera_pos=&camera_matrix.t[0];
   explosion.init(32);
   load_3do("3do/exploder.3do");
   gemstone.assign_3do("3do/gemstone.3do");
 //  gemstone.spin("base", 1, 50);
   gemstone.hide("base");
   gemstone1.state=GEM_INACTIVE;
   sound.load("turn3");
   sound.load("hit");
   sound.load("bubble");
   sound.load("sonde01");
   sound.load("chaos04");
   sound.load("classic 02");
   sound.load("ufo");
   sound.load("appear3");
   sound.load("appear4");
   sound.load("one up01");
   sound.load("sonde02");
   sound.load("expl");
   sound.load("expl2");
   sound.load("entering level2");
   sound.load("slotgas");
   sn=sound.load("engine");
   gr.assign_3do("3do/gr.3do");
   gr.hide_fill("base");
   gr.hide_grid("head");
//   unload_3do("3do/exploder.3do");
   grid.assign_3do("3do/grid.3do");
   grid.hide_fill("grid");
   grid.move("base", 1, -100, NOW);
   maze.assign_3do("3do/maze3do.3do");
   maze.move("base", 2, 50, NOW);
   maze.hide_grid(0);
   maze.blink("base", 50, 800, 15, DEFAULT);
//   maze.spin("base", 1, 100);
   box.assign_3do("3do/box7.3do");
   init_timer();
   stereo.set_near_clip(0.15);
   sound.play("entering level2", 180, 128, 1000, 0);
   sound.play("engine", 180, 128, 1200, 0);
   enemyracer->shot.model3d.set_color("base", 17);
   enemyracer->shot.model3d.hide_grid("base");
   playerracer->shot.model3d.hide_grid("base");
   gr.assign_texture("head", 0, "circuit.bmp",  0, 0, 0, 1, 1, 1, 1, 0);
//   gr.assign_texture("head", 0, "textu2.bmp",  0, 0, 0, 1, 1, 1, 1, 0);
//   gr.start_function(change_level);

   ///////////
   game_loop();
   ///////////
 //  delete (&sound);
	allegro_exit();
	return 0;
}
END_OF_MAIN();

