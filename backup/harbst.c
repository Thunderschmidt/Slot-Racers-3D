//HARBST
// Hardware Accelerated Red/Blue STereoscopy
//

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <allegro.h>
#include <alleggl.h>
#include <GL/glu.h>
#include "harbst.h"
#include "input3DO.h"
#include "Trans_3D.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////        TIMER         //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

volatile int chrono = 0;
float        fcount, fmult, fps_rate = 60.0;
int          frame_count = 0,
             frame_count_time = 0,
             frame_nr=0;


void the_timer(void)
{
	chrono++;
} END_OF_FUNCTION(the_timer);

void proceed_timer()
{
    frame_count++;
    frame_nr++;
    fmult=(chrono-fcount)/1000;
    fcount=chrono;
    if (frame_count >= 15)
	{
		fps_rate = frame_count * 1000.0 / (chrono - frame_count_time);
		frame_count -= 15;
		frame_count_time = chrono;
	}
}

float get_fps(){return(fps_rate);}
float get_fmult(){return(fmult);}
int get_chrono(){return(chrono);}

void init_timer()
{
   install_int(the_timer, 1);
	LOCK_FUNCTION(the_timer);
	LOCK_VARIABLE(chrono);
   fcount=chrono;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////        MATH         ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////

MATR_STACK_CLASS matr_stack;  //zur Simulation von OpenGL Matrixoperationen
GLfloat glm[16];

VECTOR vect(float x, float y, float z)
{
   VECTOR ret;
   ret.x=x;
   ret.y=y;
   ret.z=z;
   return(ret);
}

MATRIX_f matr(float x, float y, float z, float xrot, float yrot, float zrot)
{
   MATRIX_f m;
   get_transformation_matrix_f(&m, 1.0, xrot, yrot, zrot, x, y, z);
   return(m);
}

void glMultMatrix_allegro(MATRIX_f *m) //multipliziere Allegro-Matrix mit OpenGL-Modelview-Matrix
{
   allegro_gl_MATRIX_f_to_GLfloat(m, glm);
   glMultMatrixf(glm);
}

void glMultMatrix_allegro_inv(MATRIX_f *m)//multipliziere Allegro-Matrix mit OpenGL-Modelview-Matrix invers
{
   glm[0]=m->v[0][0];
   glm[1]=m->v[1][0];
   glm[2]=m->v[2][0];

   glm[4]=m->v[0][1];
   glm[5]=m->v[1][1];
   glm[6]=m->v[2][1];
    
   glm[8]=m->v[0][2];
   glm[9]=m->v[1][2];
   glm[10]=m->v[2][2];
    
   glm[12]=0;
   glm[13]=0;
   glm[14]=0;
   glMultMatrixf(glm);
   glTranslatef(-m->t[0],-m->t[1],-m->t[2]);
}

VECTOR trans_matrix_to_vector(MATRIX_f *m)
{
   VECTOR pos;
   pos.x=m->t[0];
   pos.y=m->t[1];
   pos.z=m->t[2];
   return(pos);
}

void translate_matrix_v(MATRIX_f *m, VECTOR *pos)
{
   m->t[0]=pos->x;
   m->t[1]=pos->y;
   m->t[2]=pos->z;
}

bool accelerate(float soll, float *ist, float a)
{
	a*=fmult;
	if (*ist<soll )
	{
		*ist+=a;
		if (*ist > soll)
        {
           *ist=soll;
           return(TRUE);
        }
	}
	if (*ist > soll)
	{
		*ist-=a;
		if (*ist < soll)
        {
           *ist=soll;
           return(TRUE);
        }
	}
    if (*ist==soll) return(TRUE);
    return(FALSE);
}

void MATR_STACK_CLASS::mul_alg(MATRIX_f *m) //Klasse f�r simulierten OpenGL-Stack. Wird f�r Unterobjekte ben�tigt
{
   matrix_mul_f(&actual, m, &actual);
}

void MATR_STACK_CLASS::translate(VECTOR *v)
{
   actual.t[0]+=v->x*actual.v[0][0];
   actual.t[1]+=v->x*actual.v[0][1];
   actual.t[2]+=v->x*actual.v[0][2];
     
   actual.t[0]+=v->y*actual.v[1][0];
   actual.t[1]+=v->y*actual.v[1][1];
   actual.t[2]+=v->y*actual.v[1][2];

   actual.t[0]+=v->z*actual.v[2][0];
   actual.t[1]+=v->z*actual.v[2][1];
   actual.t[2]+=v->z*actual.v[2][2];
}

void MATR_STACK_CLASS::mul(MATRIX_f *m)
{
   translate((VECTOR*)&m->t[0]);
   MATRIX_f m2=*m;
   m2.t[0]=0;m2.t[1]=0;m2.t[2]=0;
   matrix_mul_f(&m2, &actual, &actual);
}

void MATR_STACK_CLASS::push()
{
   stack[total]= new MATRIX_f;
   *stack[total]=actual;
   total++;
}

void MATR_STACK_CLASS::pop()
{
   if(total>0)
   {
      total--;
      actual=*stack[total];
      delete stack[total];
   }
}


void MATR_STACK_CLASS::loadIdentity()
{
   int i;
   for(i=0; i<total; i++)delete stack[i];
   total=0;
   actual=identity_matrix_f;
}

MATRIX_f MATR_STACK_CLASS::get()
{
   return(actual);
}

MATR_STACK_CLASS::MATR_STACK_CLASS()
{
   total=0;
}

void invert_matrix(MATRIX_f *in, MATRIX_f *out)
{
	out->v[0][0]=in->v[0][0]; out->v[0][1]=in->v[1][0]; out->v[0][2]=in->v[2][0];
	out->v[1][0]=in->v[0][1]; out->v[1][1]=in->v[1][1]; out->v[1][2]=in->v[2][1];
	out->v[2][0]=in->v[0][2]; out->v[2][1]=in->v[1][2]; out->v[2][2]=in->v[2][2];
	out->t[0]=-in->t[0];out->t[1]=-in->t[1];out->t[2]=-in->t[2];
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////       STEREO       ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/* Hier wird alles Stereoskopische erledigt.
 * - Es werden Berechnungen ausgef�hrt
 * - Es werden die 3D-Farben initiiert.
 */

const GLfloat light_ambient_l[] = {0.93, 1.0, 1.0, 1.0};
const GLfloat light_diffuse_l[] = {0.1 , 0.0, 0.0, 1.0};
const GLfloat light_ambient_r[] = {1.0 , 1.0, 0.93, 1.0};
const GLfloat light_diffuse_r[] = {0.0 , 0.0, 0.1, 1.0};
bool act_eye;

GLubyte col256[2][256][3];
GLubyte col16[2][24][3] =
{
{{ 0, 40, 40},
{ 11, 40, 40},
{ 21, 40, 40},
{ 32, 40, 40},
{ 43, 40, 39},
{ 53, 40, 39},
{ 64, 39, 39},
{ 75, 38, 39},
{ 85, 37, 38},
{ 96, 36, 37},
{107, 32, 34},
{117, 28, 34},
{128, 24, 33},
{139, 17, 29},
{149, 15, 29},
{160,  5, 20},
{  0,  0,  0},
{  0,  0,  0},
{  0,  0,  0},
{  0,  0,  0},
{  0,  0,  0},
{  0,  0,  0},
{  0,  0,  0},
{  0,  0,  0}},

{{ 0,  0,  0},
{  0,  0, 16},
{  0,  0, 32},
{  0,  0, 48},
{  0,  0, 64},
{  0,  0, 80},
{  0,  0, 96},
{  0,  0,112},
{  0,  0,128},
{  0,  0,144},
{  0,  0,160},
{  0,  0,176},
{  0,  0,192},
{  0,  0,208},
{  0,  0,224},
{  0,  0,240},
{  0,  0,  0},
{  0,  0,  0},
{  0,  0,  0},
{  0,  0,  0},
{  0,  0,  0},
{  0,  0,  0},
{  0,  0,  0},
{  0,  0,  0}}
};

void init_color_table(char file[])
{
   int i;
   int buf[MAX_POINTS];
   ColorConversion colortable;
   colortable.LoadConf(file);//"color_tables/editor.conf"
   for(i=0; i<255; i++)
   {
      //links
      col256[0][i][0]=colortable.GetValue(SIDE_LEFT, COLOR_RED, i);
      col256[0][i][1]=colortable.GetValue(SIDE_LEFT, COLOR_GREEN, i);
      col256[0][i][2]=colortable.GetValue(SIDE_LEFT, COLOR_BLUE, i);
      //rechts
      col256[1][i][0]=colortable.GetValue(SIDE_RIGHT, COLOR_RED, i);
      col256[1][i][1]=colortable.GetValue(SIDE_RIGHT, COLOR_GREEN, i);
      col256[1][i][2]=colortable.GetValue(SIDE_RIGHT, COLOR_BLUE, i);
   }
   for(i=0; i<16; i++)
   {
      //links
      col16[0][i][0]=colortable.GetValue(SIDE_LEFT, COLOR_RED, i*17);
      col16[0][i][1]=colortable.GetValue(SIDE_LEFT, COLOR_GREEN, i*17);
      col16[0][i][2]=colortable.GetValue(SIDE_LEFT, COLOR_BLUE, i*17);
      //rechts
      col16[1][i][0]=colortable.GetValue(SIDE_RIGHT, COLOR_RED, i*17);
      col16[1][i][1]=colortable.GetValue(SIDE_RIGHT, COLOR_GREEN, i*17);
      col16[1][i][2]=colortable.GetValue(SIDE_RIGHT, COLOR_BLUE, i*17);
   }
   GLubyte r,g,b;
   colortable.GetBackgnd(&r, &g, &b);
   float r2=r/255.0, g2=r/255.0, b2=r/255.0;
   glClearColor(r2, g2, b2, 1.0);
}

void update_color_fx()
{
   int a;
   a=(chrono)%16;
   col16[0][16][0]=col16[0][a][0];
   col16[0][16][1]=col16[0][a][1];
   col16[0][16][2]=col16[0][a][2];

   col16[1][16][0]=col16[1][a][0];
   col16[1][16][1]=col16[1][a][1];
   col16[1][16][2]=col16[1][a][2];
////////////////////////////////////
   a=((frame_nr/2)%2)*9+6;
   col16[0][17][0]=col16[0][a][0];
   col16[0][17][1]=col16[0][a][1];
   col16[0][17][2]=col16[0][a][2];

   col16[1][17][0]=col16[1][a][0];
   col16[1][17][1]=col16[1][a][1];
   col16[1][17][2]=col16[1][a][2];
////////////////////////////////////
   a=(chrono/70)%8;
   col16[0][18][0]=col16[0][8+a][0];
   col16[0][18][1]=col16[0][8+a][1];
   col16[0][18][2]=col16[0][8+a][2];

   col16[1][18][0]=col16[1][4+a/2][0];
   col16[1][18][1]=col16[1][4+a/2][1];
   col16[1][18][2]=col16[1][4+a/2][2];
}

void color16(char lumi)
{
   glColor3ubv(col16[act_eye][lumi]);
}

void color16(char lumi, unsigned char alpha)
{
   glColor4ub(col16[act_eye][lumi][0], col16[act_eye][lumi][1], col16[act_eye][lumi][2], alpha);
}

void color256(unsigned char lumi)
{
   glColor3ubv(col256[act_eye][lumi]);
}

void color256(unsigned char lumi, unsigned char alpha)
{
   glColor4ub(col256[act_eye][lumi][0], col256[act_eye][lumi][1], col256[act_eye][lumi][2], alpha);
}

bool get_eye() {return (act_eye);}   //inline machen!

void STEREO_CLASS::set_eye(bool eye)
{
   float width=half_monitor_width*clipscale;
   float height=half_monitor_height*clipscale;
   act_eye=eye;
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if(!act_eye)
   {
	   glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); //zeichne alle Farben
      glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient_l);
      glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse_l);
      glFrustum((-half_monitor_width+half_eye_seperation)*clipscale*scale,
                 (half_monitor_width+half_eye_seperation)*clipscale*scale,
                (-half_monitor_height)*clipscale*scale,
                 (half_monitor_height)*clipscale*scale,
                  near_clip*scale,
                  far_clip*scale);
   }
   else
   {
      glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE); //zeichne nur blau!
      glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient_r);
      glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse_r);
      glFrustum((-half_monitor_width-half_eye_seperation)*clipscale*scale,
                ( half_monitor_width-half_eye_seperation)*clipscale*scale,
                (-half_monitor_height)*clipscale*scale,
                ( half_monitor_height)*clipscale*scale,
                  near_clip*scale,
                  far_clip*scale);
    }
  	glMatrixMode(GL_MODELVIEW);
  	glLoadIdentity();
}

void STEREO_CLASS::position_camera(MATRIX_f *camera_matrix)
{
   if(!act_eye) glTranslatef( half_eye_seperation*scale, 0, 0);
   else         glTranslatef(-half_eye_seperation*scale, 0, 0);
   glMultMatrix_allegro_inv(camera_matrix);
   glPushMatrix();
}

STEREO_CLASS::STEREO_CLASS()
{
   act_eye=LEFT_EYE;
   monitor_height=0.24;
   half_monitor_height=0.12;
   monitor_width=0.32;
   half_monitor_width=0.16;
   eye_seperation=0.068;
   half_eye_seperation=0.034;
   distance_to_monitor=0.55;
   near_clip=distance_to_monitor;
   far_clip=3000.0;
   clipscale=1.0;
   scale=1.0;
}

void STEREO_CLASS::init_lighting()
{
   GLfloat lmodel_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
   GLfloat light_position[] = {1, 1, 1, 0};
   GLfloat light_specular[] = {0, 0, 0, 0};
   GLfloat light_ambient[] = {0.9, 1, 1, 1};
   GLfloat light_diffuse[] = {0.1, 0, 0, 1};
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
   glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
   glEnable(GL_COLOR_MATERIAL);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
}

void convert_to_stereo_bitmap(BITMAP *bmp, bool eye)
{
   int y, x;
   unsigned char lum_val;
   BITMAP *luminance = create_bitmap(bmp->w, bmp->h);
   set_luminance_blender(255, 255, 255, 255);
   draw_lit_sprite(luminance, bmp, 0, 0, 255);
   bmp_select(luminance);
   for (y=0; y<(luminance->h); y++)
   {
	   for (x=0; x<luminance->w; x++)
      {
         bmp_select(luminance);
         lum_val=getb32(_getpixel32(luminance, x, y))-1;
         _putpixel32(bmp,  x, y, makecol24(col256[eye][lum_val][0], col256[eye][lum_val][1], col256[eye][lum_val][2]));
      }
   }
 	destroy_bitmap(luminance);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////       MODEL3D       ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// 9 3 9 23
typedef enum ACTIONTYPE
{
   ACT_TURN=0,  //0
   ACT_MOVE,
   ACT_TURN_NOW,
   ACT_MOVE_NOW,
   ACT_SPIN,
   ACT_STOP_SPIN,
   ACT_WAITFOR,
   ACT_WAIT,
   ACT_ACTIVE,
   ACT_INACTIVE,
   ACT_FINISHED,
   ACT_KILL_IT, //10
   ACT_HIDE,
   ACT_SHOW,
   ACT_ALL,
   ACT_FUNCTION0,
   ACT_FUNCTION1,
   ACT_FUNCTION2,
   ACT_HIDE_GRID,
   ACT_SHOW_GRID,
   ACT_HIDE_FILL,
   ACT_SHOW_FILL,//20
   ACT_BLINK,
   ACT_STOP_BLINK,
   ACT_SET_COLOR,
   ACT_RESET
};

typedef struct ACTION
{
   int index;         //index
   int action_group;
   bool active;
   ACTIONTYPE type;    //ACT_TURN, ACT_MOVE, ACT_WAIT, ACT_WAITFORACT_TURN, ACT_WAITFORACT_MOVE
   MODEL3D_CLASS *model3d;          //welches model3d?
   int o;             //welches Objejkt?
   char axis;           //x- y- z-Achse?
   int t;               //f�r wait-Actions
   float amount;        //Winkel oder Translation
   float speed;         //Geschwindigkeit der Bewegung
   ACTION *link;        //n�chste Aktion (bei Wait und Waitfor)
   ACTION *waitforaction; //f�r Aktionstyp ACT_WAITFOR: warte auf Beendigung dieser Aktion in dieser Verkn�pfung
   ACTION *whowaitsforme; //gibt es eine Funktion, die auf mich wartet? Wenn ja, welche?
   ACTION_FUNCTION0 action_function0; //Funktionspointer
   ACTION_FUNCTION1 action_function1; //Funktionspointer
   ACTION_FUNCTION2 action_function2; //Funktionspointer
   void *pointer, *pointer2;
} ACTION;

typedef struct BLINKDATA
{
   int time_on, time_off;
   unsigned char col16_on, col16_off;
   bool is_on;
   bool active; //schon geblinkt?
}BLINKDATA;

BLINKDATA *blinking;

ACTION act_none;
ACTION *ACT_NONE=&act_none;
ACTION a_actions[MAXACTIONS]; //alle aktiven Actions
ACTION *a_action[MAXACTIONS]; //alle inaktiven Actions
ACTION i_actions[MAXACTIONS];
ACTION *i_action[MAXACTIONS];
volatile int a_action_anz;
volatile int i_action_anz;
MODEL *models[MAXMODELS];      //Zeiger auf alle geladenen Models
int models_anz=0;
int stereo_bitmap_anz=0;
STEREO_BITMAP *stereo_bitmaps[MAXSTEREOBITMAPS];


MODEL* load_3do(const char name[])
{
   int i;
   for (i=0; i<models_anz; i++)
   {
       if(!strcmp(name, models[i]->name)) return (models[i]);
   }
   models[models_anz]= new MODEL;
   lese_3do(models[models_anz], name);
   models_anz++;
   return(models[models_anz-1]);
}

void unload_3do(const char name[])
{
}

int MODEL3D_CLASS::prepare_object_matrices()
{
   int o;
   this->object=new OBJECT[this->model->obj_anz];
   for(o=0;o<this->model->obj_anz;o++)
   {
      this->object[o].m=identity_matrix_f;
      this->object[o].moved=FALSE;
      this->object[o].visible=TRUE;
      this->object[o].grid_is_visible=TRUE;
      this->object[o].fill_is_visible=TRUE;
      this->object[o].winkel[0]=0;
      this->object[o].winkel[1]=0;
      this->object[o].winkel[2]=0;
      this->object[o].single_colored=FALSE;
      this->object[o].alpha=255;
      this->object[o].is_textured=FALSE;
   }
}

void MODEL3D_CLASS::assign_3do(const char name[])  //weist einer MODEL3D_CLASS ein 3do-Modell zu
{
   this->model=load_3do(name);
   this->prepare_object_matrices();   
}

MODEL3D_CLASS::~MODEL3D_CLASS()
{
   free(this->object);
   this->action_group=0;
}

bool MODEL3D_CLASS::get_object_matrix_recursive(int o, int o_soll)
{
   matr_stack.push();
   matr_stack.translate(&this->model->o3d[o].pos);
   if(this->object[o].moved)matr_stack.mul(&this->object[o].m);
   if(o==o_soll) return(TRUE);
   else
   {
      if(this->model->o3d[o].child_o)
      {
         if(get_object_matrix_recursive(this->model->o3d[o].child_o, o_soll)) return(TRUE);
      }
      matr_stack.pop();
      if(this->model->o3d[o].sibl_o)
      {
         if(get_object_matrix_recursive(this->model->o3d[o].sibl_o, o_soll)) return(TRUE);
      }
   }
   return(FALSE);
}

MATRIX_f MODEL3D_CLASS::get_object_matrix_world(int o, MATRIX_f *m)
{
   matr_stack.loadIdentity();
   matr_stack.mul_alg(m);
   this->get_object_matrix_recursive(0, o);
   return(matr_stack.get());
}

MATRIX_f MODEL3D_CLASS::get_object_matrix(int o)
{
   matr_stack.loadIdentity();
   this->get_object_matrix_recursive(0, o);
   return(matr_stack.get());
}

int MODEL3D_CLASS::get_object_by_name(const char name[])
{
   char o;
   for(o=0;o<this->model->obj_anz;o++) if(!strcmp(name, this->model->o3d[o].name)) return(o);
   return(0);
}

STEREO_BITMAP *get_stereo_bitmap(const char name[]) //konvertiert normale Bitmaps zu Stereo-Texturen
{
   int i, y, x;
   unsigned char lum_val;
   for(i=0;i<stereo_bitmap_anz; i++)if(!strcmp(name, stereo_bitmaps[i]->name))return (stereo_bitmaps[i]);
   stereo_bitmaps[stereo_bitmap_anz]=new STEREO_BITMAP;
   stereo_bitmaps[stereo_bitmap_anz]->name=name;
	BITMAP *bmp = load_bmp (name, NULL);
	if (!bmp)
   {
		allegro_message ("Error loading texture bitmap!");
		exit (1);
	}
	BITMAP *luminance = create_bitmap(bmp->w, bmp->h);
	BITMAP *left  = create_bitmap(bmp->w, bmp->h);
	BITMAP *right = create_bitmap(bmp->w, bmp->h);
 	set_luminance_blender(255, 255, 255, 255);
   draw_lit_sprite(luminance, bmp, 0, 0, 255);
   for (y=0; y<(luminance->h); y++)
   {
	   for (x=0; x<luminance->w; x++)
      {
         lum_val=getb32(_getpixel32(luminance, x, y))-1;
         _putpixel32(left,  x, y,  makecol24(col256[0][lum_val][0], col256[0][lum_val][1], col256[0][lum_val][2]));
         _putpixel32(right,  x, y, makecol24(col256[1][lum_val][0], col256[1][lum_val][1], col256[1][lum_val][2]));
      }
   }
   allegro_gl_begin();
	glEnable (GL_TEXTURE_2D);
	glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
   stereo_bitmaps[stereo_bitmap_anz]->glbmp[0]=allegro_gl_make_texture(left);
   stereo_bitmaps[stereo_bitmap_anz]->glbmp[1]=allegro_gl_make_texture(right);
	destroy_bitmap(bmp);
	destroy_bitmap(luminance);
	destroy_bitmap(left);
	destroy_bitmap(right);
	stereo_bitmap_anz++;
	return(stereo_bitmaps[stereo_bitmap_anz-1]);
}

bool MODEL3D_CLASS::assign_texture(const char name[], int face, const char texture_name[], float ax, float ay, float bx, float by, float cx, float cy, float dx, float dy)
{
   int i;
   int o=get_object_by_name(name);
   int face_anz=this->model->o3d[o].poly_anz;
   if(!this->object[o].is_textured)this->object[o].texture=new TEXTURE[face_anz];
   for(i=0;i<face_anz;i++)this->object[o].texture[i].active=FALSE;
   this->object[o].texture[face].active=TRUE;
   this->object[o].texture[face].texcoord[0][0]=ax;
   this->object[o].texture[face].texcoord[0][1]=ay;
   this->object[o].texture[face].texcoord[1][0]=bx;
   this->object[o].texture[face].texcoord[1][1]=by;
   this->object[o].texture[face].texcoord[2][0]=cx;
   this->object[o].texture[face].texcoord[2][1]=cy;
   this->object[o].texture[face].texcoord[3][0]=dx;
   this->object[o].texture[face].texcoord[3][1]=dy;
   this->object[o].texture[face].stereo_bitmap=get_stereo_bitmap(texture_name);
   this->object[o].is_textured=TRUE;
}

void init_actions()
{
   int i;
   for(i=0;i<MAXACTIONS;i++)
   {
      a_actions[i].index=i;
      i_actions[i].index=i;
      a_actions[i].type=ACT_INACTIVE;
      i_actions[i].type=ACT_INACTIVE;
      a_actions[i].active=TRUE;
      i_actions[i].active=FALSE;
      a_actions[i].link=ACT_NONE;
      i_actions[i].link=ACT_NONE;
      a_actions[i].waitforaction=ACT_NONE;
      i_actions[i].waitforaction=ACT_NONE;
      a_actions[i].whowaitsforme=ACT_NONE;
      i_actions[i].whowaitsforme=ACT_NONE;
      a_action[i]=&a_actions[i];
      i_action[i]=&i_actions[i];
    }
}

int get_a_action_anz()
{
   return (a_action_anz);
}

int get_i_action_anz()
{
   return (i_action_anz);
}

void kill_a_action(int i)
{
   a_action[i]->type=ACT_INACTIVE;
   a_action[i]->link=ACT_NONE;
   a_action[i]->waitforaction=ACT_NONE;
   a_action[i]->whowaitsforme=ACT_NONE;

   ACTION *temp=a_action[i];
   a_action[i]= a_action[a_action_anz-1];
   a_action[a_action_anz-1]=temp;
   a_action[a_action_anz-1]->index=a_action_anz-1;
   a_action[i]->index=i;
   a_action_anz--;
}

void kill_i_action(int i)
{
   i_action[i]->type=ACT_INACTIVE;
   i_action[i]->link=ACT_NONE;
   i_action[i]->waitforaction=ACT_NONE;
   i_action[i]->whowaitsforme=ACT_NONE;

   ACTION *temp=i_action[i];
   i_action[i]=i_action[i_action_anz-1];
   i_action[i_action_anz-1]=temp;
   i_action[i_action_anz-1]->index=i_action_anz-1;
   i_action[i]->index=i;
   i_action_anz--;
}

void MODEL3D_CLASS::clear_a_action(int o, char axis, signed char type)
{
   int i;
   for(i=0;i<a_action_anz;i++)
   if((a_action[i]->model3d==this) &&
      (a_action[i]->o==o) &&
      (a_action[i]->axis==axis) &&
      (a_action[i]->type==type) &&
      (a_action[i]->action_group==this->action_group))//????!!!!
   kill_a_action(i);
}

void stop_a_action(ACTION *act) //l�scht alle aktiven actions, die so sind wie act
{
   if(act->active)
   {
   a_action_anz--;
   act->model3d->clear_a_action(act->o, act->axis, act->type);
   a_action_anz++;
   }
}

ACTION* find_action(MODEL3D_CLASS *model3d, int o, char axis, signed char type)
{
   int i;
   ACTION *ret, *link;
   for(i=0; i<i_action_anz; i++)
   {
      if ((i_action[i]->model3d==model3d) && (i_action[i]->o==o) && (i_action[i]->axis==axis) && (i_action[i]->type==type) && (i_action[i]->action_group==model3d->get_action_group()))
      {
         ret=i_action[i];
         link=ret->link;
         while(link!=ACT_NONE)
         {
            if ((link->model3d==model3d) && (link->o==o) && (link->axis==axis)&& (link->type==type) && (link->action_group==model3d->get_action_group())) ret=link;
            link=link->link;
         }
         return(ret);
      }
   }
   for(i=0; i<a_action_anz; i++)
   if ((a_action[i]->model3d==model3d) && (a_action[i]->o==o) && (a_action[i]->axis==axis)&& (a_action[i]->type==type) && (a_action[i]->action_group==model3d->get_action_group()))
   return(a_action[i]);
   return(ACT_NONE);
}

ACTION* get_action(MODEL3D_CLASS *model3d)//Funktion findet die letzte Action einer Verkn�pfungsreihe
{
   int i, j;
   ACTION *link;
   for(i=0; i<a_action_anz; i++)
   if(((a_action[i]->type == ACT_WAIT) || (a_action[i]->type == ACT_WAITFOR)) && (a_action[i]->model3d == model3d)&&(a_action[i]->action_group==model3d->get_action_group()))  //es gibt immer nur h�chstens eine AKTIVE WAIT(_FOR)-Action. Ist eine aktiv?
   {
      link=a_action[i];
	   while(link->link != ACT_NONE) link=link->link;//sto�e zur letzten Funktion der Verkn�pfunsreihe vor
	   link->link=i_action[i_action_anz];//verkn�pfe die neue Aktion
      i_action_anz++;
//      if(a_action_anz>MAXACTIONS)allegro_message("harbst: too much ACTIONS. Increase MAXACTIONS.");
      i_action[i_action_anz-1]->action_group=model3d->get_action_group();
      return(i_action[i_action_anz-1]);             //gib Pointer auf die neue Aktion zur�ck
   }
   a_action_anz++;
//   if(a_action_anz>MAXACTIONS)allegro_message("harbst: too much ACTIONS. Increase MAXACTIONS.");
   a_action[a_action_anz-1]->action_group=model3d->get_action_group();
   return(a_action[a_action_anz-1]);
}

void activate_action(ACTION *act)//Diese Funktion aktiviert inaktive Actions (rekursiv, alles zwischen Wait-Befehlen)
{
   int i;
   ACTION link;
   act->model3d->clear_a_action(act->o, act->axis, act->type); //k�nnte probleme machen!!
   *a_action[a_action_anz]=*act;
   if(act->whowaitsforme!=ACT_NONE)act->whowaitsforme->waitforaction=a_action[a_action_anz];
   a_action_anz++;
   if(act->link != ACT_NONE)
   if((act->type != ACT_WAIT) && (act->type != ACT_WAITFOR))
   activate_action(act->link);
   kill_i_action(act->index);
}

void MODEL3D_CLASS::clear_action_group(int a_g)
{
   int i;
   for(i=0;i<a_action_anz;i++)
   if((a_action[i]->model3d==this) && (a_action[i]->action_group==a_g))kill_a_action(i);
   for(i=0;i<i_action_anz;i++)
   if((i_action[i]->model3d==this) && (i_action[i]->action_group==a_g))kill_i_action(i);
}

void MODEL3D_CLASS::clear_all_actions()
{
   int i;
   for(i=0;i<a_action_anz;i++)if(a_action[i]->model3d==this)kill_a_action(i);
   for(i=0;i<i_action_anz;i++)if(i_action[i]->model3d==this)kill_i_action(i);
}

void MODEL3D_CLASS::reset_object_matrix(int o)
{
   this->object[o].m=identity_matrix_f;
   this->object[o].winkel[0]=0;
   this->object[o].winkel[1]=0;
   this->object[o].winkel[2]=0;
   this->object[o].moved=FALSE;
}

void MODEL3D_CLASS::kill_all_actions() //l�scht alle actions eines MODEL3D-Objektes
{
   int i;
   for(i=0;i<a_action_anz;i++)
   if(a_action[i]->model3d==this)
   {
      kill_a_action(i);
      i--;
   }
   for(i=0;i<i_action_anz;i++)
   if(i_action[i]->model3d==this)
   {
      kill_i_action(i);
      i--;
   }
}

//////////////////////////   ACTIONS  //////////////////////////////////////////

void MODEL3D_CLASS::blink(const char name[], int time_on, int time_off, unsigned char col16_on, unsigned char col16_off)
{
   ACTION *act=get_action(this);
   act->model3d=this;
   act->o=this->get_object_by_name(name);
   act->type=ACT_BLINK;
   BLINKDATA *blinkdata=new BLINKDATA;
   blinkdata->col16_on=col16_on;
   blinkdata->col16_off=col16_off;
   blinkdata->time_on=time_on;
   blinkdata->time_off=time_off;
   blinkdata->is_on=TRUE;
   blinkdata->is_on=FALSE;
   blinkdata->active=FALSE;
   act->pointer=blinkdata;
   act->t=chrono;
   act->axis=0;
}

bool MODEL3D_CLASS::stop_blink(const char name[])
{
   int o=this->get_object_by_name(name);
   ACTION *act, *link=find_action(this, o, 0, ACT_BLINK);
   if(link==ACT_NONE) return(FALSE);
   act=get_action(this);
   act->o=o;
   link->whowaitsforme=act;
   act->model3d=this;
   act->type=ACT_STOP_BLINK;
   act->waitforaction=link;
   return(TRUE);
}

void MODEL3D_CLASS::hide_fill(const char name[])
{
   ACTION *act=get_action(this);
   act->model3d=this;
   act->o=this->get_object_by_name(name);
   act->type=ACT_HIDE_FILL;
}

void MODEL3D_CLASS::show_fill(const char name[])
{
   ACTION *act=get_action(this);
   act->model3d=this;
   act->o=this->get_object_by_name(name);
   act->type=ACT_SHOW_FILL;
}

void MODEL3D_CLASS::hide_grid(const char name[])
{
   ACTION *act=get_action(this);
   act->model3d=this;
   act->o=this->get_object_by_name(name);
   act->type=ACT_HIDE_GRID;
}

void MODEL3D_CLASS::hide_grid(int o)
{
   ACTION *act=get_action(this);
   act->model3d=this;
   act->o=o;
   act->type=ACT_HIDE_GRID;
}

void MODEL3D_CLASS::show_grid(const char name[])
{
   ACTION *act=get_action(this);
   act->model3d=this;
   act->o=this->get_object_by_name(name);
   act->type=ACT_SHOW_GRID;
}

void MODEL3D_CLASS::hide(int o)
{
   ACTION *act=get_action(this);
   act->model3d=this;
   act->o=o;
   act->type=ACT_HIDE;
}

void MODEL3D_CLASS::hide(const char name[])
{
   int o=this->get_object_by_name(name);
   this->hide(o);
}

void MODEL3D_CLASS::show(const char name[])
{
   ACTION *act=get_action(this);
   act->model3d=this;
   act->o=this->get_object_by_name(name);
   act->type=ACT_SHOW;
}

void MODEL3D_CLASS::set_color(const char name[], int col)
{
   ACTION *act=get_action(this);
   act->model3d=this;
   act->o=this->get_object_by_name(name);
   act->type=ACT_SET_COLOR;
   act->t=col;
}

void MODEL3D_CLASS::start_function(ACTION_FUNCTION0 action_function)
{
   ACTION *act=get_action(this);
   act->model3d=this;
   act->type=ACT_FUNCTION0;
   act->action_function0=action_function;
}

void MODEL3D_CLASS::start_function(ACTION_FUNCTION1 action_function, void *pointer)
{
   ACTION *act=get_action(this);
   act->model3d=this;
   act->type=ACT_FUNCTION1;
   act->pointer=pointer;
   act->action_function1=action_function;
}

void MODEL3D_CLASS::start_function(ACTION_FUNCTION2 action_function, void *pointer, void *pointer2)
{
   ACTION *act=get_action(this);
   act->model3d=this;
   act->type=ACT_FUNCTION2;
   act->action_function2=action_function;
   act->pointer=pointer;
   act->pointer2=pointer2;
}

void MODEL3D_CLASS::move(const char name[], int axis, float amount, float speed)
{
   ACTION *act=get_action(this);
   act->model3d=this;
   act->o=this->get_object_by_name(name);
   act->axis=axis;
   act->amount=amount;
   act->speed=speed;
   if(speed==NOW)act->type=ACT_MOVE_NOW;
   else act->type=ACT_MOVE;
   stop_a_action(act);
}

void MODEL3D_CLASS::turn(const char name[], int axis, float amount, float speed)
{
   ACTION *act=get_action(this);
   act->model3d=this;
   act->o=this->get_object_by_name(name);
   act->axis=axis;
   act->amount=amount;
   act->speed=speed;
   if(speed==NOW)act->type=ACT_TURN_NOW;
   else act->type=ACT_TURN;
   act->type=ACT_TURN;
   stop_a_action(act);
}

void MODEL3D_CLASS::spin(const char name[], char axis, float speed)
{
   ACTION *act=get_action(this);
   act->model3d=this;
   act->o=this->get_object_by_name(name);
   act->axis=axis;
   act->speed=speed;
   act->type=ACT_SPIN;
   stop_a_action(act);
}

bool MODEL3D_CLASS::stop_spin(const char name[])
{
   int o=this->get_object_by_name(name);
   ACTION *act, *link=find_action(this, o, 0, ACT_SPIN);
   if(link==ACT_NONE) return(FALSE);
   act=get_action(this);
   act->o=o;
   link->whowaitsforme=act;
   act->model3d=this;
   act->type=ACT_STOP_SPIN;
   act->waitforaction=link;
   return(TRUE);
}

void MODEL3D_CLASS::reset(const char name[])
{
   ACTION *act=get_action(this);
   act->model3d=this;
   act->o=this->get_object_by_name(name);
   act->type=ACT_RESET;
}

void MODEL3D_CLASS::wait(int t)
{
   ACTION *act=get_action(this);
   act->model3d=this;
   act->t=t;
   act->type=ACT_WAIT;
}

bool MODEL3D_CLASS::wait_for_move(const char name[], char axis)
{
   ACTION *act, *link=find_action(this, this->get_object_by_name(name), axis, ACT_MOVE);
   if(link==ACT_NONE) return(FALSE);
   act=get_action(this);
   link->whowaitsforme=act;
   act->model3d=this;
   act->type=ACT_WAITFOR;
   act->waitforaction=link;
   return(TRUE);
}

bool MODEL3D_CLASS::wait_for_turn(const char name[], char axis)
{
   ACTION *act,
   *link=find_action(this, this->get_object_by_name(name), axis, ACT_TURN);
   if(link==ACT_NONE) return(FALSE);
   act=get_action(this);
   link->whowaitsforme=act;
   act->model3d=this;
   act->type=ACT_WAITFOR;
   act->waitforaction=link;
   return(TRUE);
}

void proceed_actions()
{
   int i, k;
   MATRIX_f m;
   bool ready;
   VECTOR pos;
   for(i=0;i<a_action_anz; i++)
   {
//      printf("%d:  %d\n", i, a_action[i]->type);
      switch(a_action[i]->type)
      {
         case ACT_FINISHED:
              kill_a_action(i);
              i--;
         break;
         case ACT_MOVE:
              a_action[i]->model3d->object[a_action[i]->o].moved=TRUE;
              if(accelerate(a_action[i]->amount, &a_action[i]->model3d->object[a_action[i]->o].m.t[a_action[i]->axis], a_action[i]->speed))
              a_action[i]->type=ACT_FINISHED;
         break;
         case ACT_TURN:
              a_action[i]->model3d->object[a_action[i]->o].moved=TRUE;
              ready=accelerate(a_action[i]->amount, &a_action[i]->model3d->object[a_action[i]->o].winkel[a_action[i]->axis], a_action[i]->speed);
              pos=trans_matrix_to_vector(&a_action[i]->model3d->object[a_action[i]->o].m);
              get_rotation_matrix_f(&a_action[i]->model3d->object[a_action[i]->o].m, a_action[i]->model3d->object[a_action[i]->o].winkel[0], a_action[i]->model3d->object[a_action[i]->o].winkel[1], a_action[i]->model3d->object[a_action[i]->o].winkel[2]);
              translate_matrix_v(&a_action[i]->model3d->object[a_action[i]->o].m, &pos);
              if(ready)a_action[i]->type=ACT_FINISHED;
         break;
         case ACT_MOVE_NOW:
              a_action[i]->model3d->object[a_action[i]->o].moved=TRUE;
              a_action[i]->model3d->object[a_action[i]->o].m.t[a_action[i]->axis]=a_action[i]->amount;
              a_action[i]->type=ACT_FINISHED;
         break;
         case ACT_TURN_NOW:
              a_action[i]->model3d->object[a_action[i]->o].moved=TRUE;
              a_action[i]->model3d->object[a_action[i]->o].winkel[a_action[i]->axis]=a_action[i]->amount;
              pos=trans_matrix_to_vector(&a_action[i]->model3d->object[a_action[i]->o].m);
              get_rotation_matrix_f(&a_action[i]->model3d->object[a_action[i]->o].m, a_action[i]->model3d->object[a_action[i]->o].winkel[0], a_action[i]->model3d->object[a_action[i]->o].winkel[1], a_action[i]->model3d->object[a_action[i]->o].winkel[2]);
              translate_matrix_v(&a_action[i]->model3d->object[a_action[i]->o].m, &pos);
              a_action[i]->type=ACT_FINISHED;
         break;
         case ACT_SPIN:
              a_action[i]->model3d->object[a_action[i]->o].moved=TRUE;
              a_action[i]->model3d->object[a_action[i]->o].winkel[a_action[i]->axis]+=a_action[i]->speed*fmult;
              pos=trans_matrix_to_vector(&a_action[i]->model3d->object[a_action[i]->o].m);
              get_rotation_matrix_f(&a_action[i]->model3d->object[a_action[i]->o].m, a_action[i]->model3d->object[a_action[i]->o].winkel[0], a_action[i]->model3d->object[a_action[i]->o].winkel[1], a_action[i]->model3d->object[a_action[i]->o].winkel[2]);
              translate_matrix_v(&a_action[i]->model3d->object[a_action[i]->o].m, &pos);
         break;
         case ACT_STOP_SPIN:
              a_action[i]->type = ACT_FINISHED;
              a_action[i]->waitforaction->type = ACT_FINISHED;
         break;
         case ACT_WAIT:
              a_action[i]->t-=fmult*1000;
              if(a_action[i]->t<=0)
              {
                 a_action[i]->type=ACT_FINISHED;
                 if(a_action[i]->link != ACT_NONE) activate_action(a_action[i]->link);
              }
         break;
         case ACT_WAITFOR:
              if (a_action[i]->waitforaction->type == ACT_FINISHED)
              {
                 a_action[i]->type=ACT_FINISHED;
                 if(a_action[i]->link != ACT_NONE) activate_action(a_action[i]->link);
              }
         break;
         case ACT_HIDE:
              a_action[i]->model3d->object[a_action[i]->o].visible=FALSE;
              a_action[i]->type=ACT_FINISHED;
         break;
         case ACT_SHOW:
              a_action[i]->model3d->object[a_action[i]->o].visible=TRUE;
              a_action[i]->type=ACT_FINISHED;
         break;
         case ACT_HIDE_GRID:
              a_action[i]->model3d->object[a_action[i]->o].grid_is_visible=FALSE;
              a_action[i]->type=ACT_FINISHED;
         break;
         case ACT_SHOW_GRID:
              a_action[i]->model3d->object[a_action[i]->o].grid_is_visible=TRUE;
              a_action[i]->type=ACT_FINISHED;
         break;
         case ACT_HIDE_FILL:
              a_action[i]->model3d->object[a_action[i]->o].fill_is_visible=FALSE;
              a_action[i]->type=ACT_FINISHED;
         break;
         case ACT_SHOW_FILL:
              a_action[i]->model3d->object[a_action[i]->o].fill_is_visible=TRUE;
              a_action[i]->type=ACT_FINISHED;
         break;
         case ACT_BLINK:
              blinking=(BLINKDATA*)a_action[i]->pointer;
              a_action[i]->model3d->object[a_action[i]->o].single_colored=TRUE;
              if(!blinking->active)
              {
                 a_action[i]->t=chrono;
                 blinking->active=TRUE;
                 blinking->is_on=TRUE;
              }
              else
              switch (blinking->is_on)
              {
                 case 1:
                 if((chrono-a_action[i]->t)>blinking->time_on)
                 {
                    blinking->is_on=0;
                    a_action[i]->t=chrono;// + (chrono-(a_action[i]->t+blinking->time_on));
                 }
                 break;
                 case 0:
                 if((chrono-a_action[i]->t)>blinking->time_off)
                 {
                    blinking->is_on=1;
                    a_action[i]->t=chrono;// + (chrono-(a_action[i]->t+blinking->time_off));
                 }
                 break;
              }
              switch (blinking->is_on)
              {
                 case 1:
                 a_action[i]->model3d->object[a_action[i]->o].color=blinking->col16_on;
                 break;
                 case 0:
                 a_action[i]->model3d->object[a_action[i]->o].color=blinking->col16_off;
                 break;
              }
                 if(a_action[i]->model3d->object[a_action[i]->o].color==DEFAULT)a_action[i]->model3d->object[a_action[i]->o].single_colored=0;
         break;
         case ACT_STOP_BLINK:
              a_action[i]->type = ACT_FINISHED;
              a_action[i]->waitforaction->type = ACT_FINISHED;
              blinking=(BLINKDATA*)a_action[i]->waitforaction->pointer;
              delete blinking;
              a_action[i]->model3d->object[a_action[i]->o].single_colored=0;
         break;
         case ACT_RESET:
               a_action[i]->model3d->reset_object_matrix(a_action[i]->o);
               a_action[i]->type=ACT_FINISHED;
         break;
         case ACT_FUNCTION0:
               a_action[i]->action_function0();
               a_action[i]->type=ACT_FINISHED;
         break;
         case ACT_FUNCTION1:
               a_action[i]->action_function1(a_action[i]->pointer);
               a_action[i]->type=ACT_FINISHED;
         break;
         case ACT_FUNCTION2:
               a_action[i]->action_function2(a_action[i]->pointer, a_action[i]->pointer2);
               a_action[i]->type=ACT_FINISHED;
         break;
              case ACT_SET_COLOR:
              if(a_action[i]->t==DEFAULT)a_action[i]->model3d->object[a_action[i]->o].single_colored=0;
              else
              {
                 a_action[i]->model3d->object[a_action[i]->o].single_colored=1;
                 a_action[i]->model3d->object[a_action[i]->o].color=a_action[i]->t;
              }
              a_action[i]->type = ACT_FINISHED;
         break;
         case ACT_INACTIVE:
              printf(" o nein!!!\n");
         break;

      }
   }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////       DRAWING       ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////

unsigned char line_color=15;
void set_line_color(unsigned char color)
{
   line_color=color;
}

void MODEL3D_CLASS::draw_object_color(int o)
{
   int p, j=0;
   glVertexPointer(3, GL_FLOAT, 0, this->model->o3d[o].vert);
  	for (p = 0; p < this->model->o3d[o].poly_anz; p++)
	{
	   glNormal3fv(&this->model->o3d[o].n[p].x);
	   glDrawElements(GL_POLYGON, this->model->o3d[o].ind_anz[p], GL_UNSIGNED_SHORT, &this->model->o3d[o].i[j]);
	   j+=this->model->o3d[o].ind_anz[p];
	}
}

void MODEL3D_CLASS::draw_object_alpha(int o, unsigned char alpha)
{
   int p, j=0;
   color16(15, alpha);
   glVertexPointer(3, GL_FLOAT, 0, this->model->o3d[o].vert);
  	for (p = 0; p < this->model->o3d[o].poly_anz; p++)
	{ 
	   glNormal3fv(&this->model->o3d[o].n[p].x);
	   glDrawElements(GL_POLYGON, this->model->o3d[o].ind_anz[p], GL_UNSIGNED_SHORT, &this->model->o3d[o].i[j]);
	   j+=this->model->o3d[o].ind_anz[p];
	} 
}

void MODEL3D_CLASS::draw_object(int o)
{
   int  p, j=0;
   glVertexPointer(3, GL_FLOAT, 0, this->model->o3d[o].vert);
 	for (p = 0; p < this->model->o3d[o].poly_anz; p++)
	{
	   glNormal3fv(&this->model->o3d[o].n[p].x);
	   color16(this->model->o3d[o].color[p]);
	   glDrawElements(GL_POLYGON, this->model->o3d[o].ind_anz[p], GL_UNSIGNED_SHORT, &this->model->o3d[o].i[j]);
	   j+=this->model->o3d[o].ind_anz[p];
	}
}

void MODEL3D_CLASS::draw_object_textures(int o)
{
   int  p, j=0;
   glVertexPointer(3, GL_FLOAT, 0, this->model->o3d[o].vert);
	for (p = 0; p < this->model->o3d[o].poly_anz; p++)
   {
      if(this->object[o].texture[p].active)
      {
         glColor4f(1, 1, 1, 1);
         glEnable (GL_TEXTURE_2D);
	      glTexEnvf(GL_TEXTURE_ENV, GL_BLEND, GL_DECAL);
	      glBindTexture(GL_TEXTURE_2D, this->object[o].texture[p].stereo_bitmap->glbmp[act_eye]);
         glBegin (GL_QUADS);
		   glTexCoord2f(this->object[o].texture[p].texcoord[0][0], this->object[o].texture[p].texcoord[0][1]); glVertex3d(this->model->o3d[o].vert[this->model->o3d[o].i[j]].x, this->model->o3d[o].vert[this->model->o3d[o].i[j]].y, this->model->o3d[o].vert[this->model->o3d[o].i[j]].z);
   	   glTexCoord2f(this->object[o].texture[p].texcoord[1][0], this->object[o].texture[p].texcoord[1][1]); glVertex3d(this->model->o3d[o].vert[this->model->o3d[o].i[j+1]].x, this->model->o3d[o].vert[this->model->o3d[o].i[j+1]].y, this->model->o3d[o].vert[this->model->o3d[o].i[j+1]].z);
         glTexCoord2f(this->object[o].texture[p].texcoord[2][0], this->object[o].texture[p].texcoord[2][1]); glVertex3d(this->model->o3d[o].vert[this->model->o3d[o].i[j+2]].x, this->model->o3d[o].vert[this->model->o3d[o].i[j+2]].y, this->model->o3d[o].vert[this->model->o3d[o].i[j+2]].z);
         glTexCoord2f(this->object[o].texture[p].texcoord[3][0], this->object[o].texture[p].texcoord[3][1]); glVertex3d(this->model->o3d[o].vert[this->model->o3d[o].i[j+3]].x, this->model->o3d[o].vert[this->model->o3d[o].i[j+3]].y, this->model->o3d[o].vert[this->model->o3d[o].i[j+3]].z);
         glEnd();
      	glDisable (GL_TEXTURE_2D);
      }
      else
      {
	      glNormal3fv(&this->model->o3d[o].n[p].x);
	      color16(this->model->o3d[o].color[p]);
	      glDrawElements(GL_POLYGON, this->model->o3d[o].ind_anz[p], GL_UNSIGNED_SHORT, &this->model->o3d[o].i[j]);
      }
      j+=this->model->o3d[o].ind_anz[p];
   }
}

void MODEL3D_CLASS::draw_object_lines(int o)
{
   glVertexPointer(3, GL_FLOAT, 0, this->model->o3d[o].vert);
   glDrawElements(GL_LINES, this->model->o3d[o].line_anz*2, GL_UNSIGNED_SHORT, this->model->o3d[o].line);
}

void MODEL3D_CLASS::draw_objects_lines(int o)
{
   glPushMatrix();
   glTranslatef(this->model->o3d[o].pos.x, this->model->o3d[o].pos.y, this->model->o3d[o].pos.z);
   if(this->object[o].moved)glMultMatrix_allegro(&this->object[o].m);
   if(this->object[o].visible && this->object[o].grid_is_visible)this->draw_object_lines(o);
   if(this->model->o3d[o].child_o)
   {
      this->draw_objects_lines(this->model->o3d[o].child_o);
   }
   glPopMatrix();
   if(this->model->o3d[o].sibl_o)
   {
      this->draw_objects_lines(this->model->o3d[o].sibl_o);
   }
}

void MODEL3D_CLASS::draw_objects_alpha(int o, unsigned char alpha)
{
   glPushMatrix();
   glTranslatef(this->model->o3d[o].pos.x, this->model->o3d[o].pos.y, this->model->o3d[o].pos.z);
   if(this->object[o].moved)glMultMatrix_allegro(&this->object[o].m);
   if(this->object[o].visible && this->object[o].grid_is_visible)this->draw_object_alpha(o, alpha);
   if(this->model->o3d[o].child_o)
   {
      this->draw_objects_alpha(this->model->o3d[o].child_o, alpha);
   }
   glPopMatrix();
   if(this->model->o3d[o].sibl_o)
   {
      this->draw_objects_alpha(this->model->o3d[o].sibl_o, alpha);
   }
}

void MODEL3D_CLASS::draw_objects_color(int o)
{
   glPushMatrix();
   glTranslatef(this->model->o3d[o].pos.x, this->model->o3d[o].pos.y, this->model->o3d[o].pos.z);
   if(this->object[o].moved)glMultMatrix_allegro(&this->object[o].m);
   if(this->object[o].visible && this->object[o].fill_is_visible)
   {
      if(this->object[o].single_colored)
      {
         color16(this->object[o].color);
         this->draw_object_color(o);
      }
      else this->draw_object_color(o);
   }
   if(this->object[o].visible && this->object[o].grid_is_visible)
   {
      glDisable(GL_POLYGON_OFFSET_FILL);
      color16(line_color);
      this->draw_object_lines(o);
      glEnable(GL_POLYGON_OFFSET_FILL);
   }


   if(this->model->o3d[o].child_o)
   {
      this->draw_objects_color(this->model->o3d[o].child_o);
   }
   glPopMatrix();
   if(this->model->o3d[o].sibl_o)
   {
      this->draw_objects_color(this->model->o3d[o].sibl_o);
   }
}


void MODEL3D_CLASS::draw_objects(int o)
{
   glPushMatrix();
   glTranslatef(this->model->o3d[o].pos.x, this->model->o3d[o].pos.y, this->model->o3d[o].pos.z);
   if(this->object[o].moved)glMultMatrix_allegro(&this->object[o].m);
   if(this->object[o].visible && this->object[o].fill_is_visible)
   {
      if(this->object[o].single_colored)
      {
         color16(this->object[o].color);
         this->draw_object_color(o);
      }
      else if(this->object[o].is_textured) this->draw_object_textures(o);
      else this->draw_object(o);
      }
   if(this->object[o].visible && this->object[o].grid_is_visible)
   {
      glDisable(GL_POLYGON_OFFSET_FILL);
      color16(15);
      this->draw_object_lines(o);
      glEnable(GL_POLYGON_OFFSET_FILL);
   }
   if(this->model->o3d[o].child_o)
   {
      this->draw_objects(this->model->o3d[o].child_o);
   }
   glPopMatrix();
   if(this->model->o3d[o].sibl_o)
   {
      this->draw_objects(this->model->o3d[o].sibl_o);
   }
}

void MODEL3D_CLASS::draw()
{
   this->draw_objects(0);
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////     FONT-DRAWING     //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

FONT3D_CLASS::FONT3D_CLASS(const char filename[])
{
   this->model3d.assign_3do(filename);
}


void FONT3D_CLASS::draw_char(unsigned char a)
{
   if((a >= 33) && (a <=96)) a-= 33;
   else if((a >= 97) && (a <=122)) a-= 65;
   else if((a >= 123) && (a <=128)) a-= 63;
   else if (a=='�') a=71;
   else if (a=='�' ||a=='�') a=72;
   else if (a=='�' ||a=='�') a=73;
   else if (a=='�' ||a=='�') a=74;
   else if (a==223) a=73;
   glRotatef(180.0, 0.0, 1.0, 0.0);
   this->model3d.draw_object_color(a);
   glRotatef(-180.0, 0.0, 1.0, 0.0);
}

void FONT3D_CLASS::draw(char string[])
{
     glFrontFace(GL_CCW);
    int i=0, column=0;
    while(string[i]!='\0')
    {
       if(string[i]=='\n')
       {
          glTranslatef(-column*4,-6, 0);
          column=0;
       }
       else if(string[i]==' ')
       {
          glTranslatef(4,0,0);
          column++;
       }
       else
       {
          this->draw_char(string[i]);
          glTranslatef(4,0,0);
          column++;
       }
       i++;
    }
    glFrontFace(GL_CW);
}
