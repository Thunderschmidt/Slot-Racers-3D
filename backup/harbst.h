#ifndef HARBST
#define HARBST


#define MAXMODELS 32
#define MAXACTIONS 256
#define MAXSTEREOBITMAPS 8
#define MAXSTACKMATRICES 64



////////////////////////////////////////////////////////////////////////////////
////////////////////////////        TIMER         //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void proceed_timer();
float get_fps();
float get_fmult();
int get_chrono();
void init_timer();

////////////////////////////////////////////////////////////////////////////////
////////////////////////////        MATH         ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef struct VECTOR
{
    float x, y, z;
} VECTOR;

class MATR_STACK_CLASS  //Zur Simulation von OpenGL-Matrix-Stackberechnungen
{
   private:
   int total;
   MATRIX_f actual;
   MATRIX_f *stack[MAXSTACKMATRICES];
   public:
   void mul(MATRIX_f *m);
   void mul_alg(MATRIX_f *m);
   void push();
   void pop();
   void loadIdentity();
   MATRIX_f get();
   void translate(VECTOR *v);
   MATR_STACK_CLASS();
};

VECTOR trans_matrix_to_vector(MATRIX_f *m);
bool accelerate(float soll, float *ist, float a);
void translate_matrix_v(MATRIX_f *m, VECTOR *pos);
void glMultMatrix_allegro(MATRIX_f *m);
MATRIX_f matr(float x, float y, float z, float xrot, float yrot, float zrot);
VECTOR vect(float x, float y, float z);
void invert_matrix(MATRIX_f *in, MATRIX_f *out);
void glMultMatrix_allegro_inv(MATRIX_f *m);
////////////////////////////////////////////////////////////////////////////////
////////////////////////////       MODEL3D       ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////


#define   NOW       1933
#define   DEFAULT   1945

typedef enum AXIS
{
   X_AXIS = 0,
   Y_AXIS = 1,
   Z_AXIS = 2
};

typedef struct O3D  //Unterobjekt, Teil der Model-Datenstruktur
{
	char name[32];   //Name (aus 3do-File)
	int vert_anz;  //wieviel Koordinatenpunkte?
	int poly_anz;  //wieviel Flächen?
	int line_anz;  //wieviel Linien (f. Outlines)
	int sibl_o;    //Schwesterobjekt
	int child_o;   //Tochterobjekt
	VECTOR pos;      //Offset
	VECTOR *n;       //Zeiger auf Normalvektoren
	VECTOR *vert;    //Zeiger auf Koordinatenpunkte
	char *ind_anz;   //Zeiger auf Datenfeld mit der Anzahl der Indizes zum Inhalt
	char *color;     //Zeiger auf Farbinformationen
	short *line;     //Zeiger auf Linien
	short *i;        //Zeiger auf Indizes
	char *c;         //
   bool single_colored; //ist das Objekt einfarbig?
} O3D;

typedef struct MODEL
{
   int obj_anz;               //Anzahl der Objekte (O3D)
   O3D  *o3d;                  //Zeiger auf Unterobjekte
   float size;                 //
   float radius;               //Größter Abstand vom Mittelpunkt
   VECTOR min;                 //der kleinste x, y und z-Wert des Modells
   VECTOR max;                 //der größte x, y und z-Wert des Modells
   VECTOR *n;                  //Normalvektoren des Kollisionsmodells
	const char *name;           //filename des 3dos
} MODEL;

typedef struct STEREO_BITMAP
{
   const char *name;
   GLuint glbmp[2];
} STEREO_BITMAP;

typedef struct TEXTURE
{
   int orientation;
   bool active;
   STEREO_BITMAP *stereo_bitmap;
   float texcoord[4][2];
} TEXTURE;

typedef struct OBJECT
{
   MATRIX_f m;                   //Trans. und Rot. des Objektes
   bool moved;                   //ist Objekt nicht auf Standardposition?
   bool visible;                 //ist Objekt sichtbar?
   float winkel[3];              //Drehung des Objektes in Winkelangaben
   bool grid_is_visible;         //Outlines sichtbar?
   bool fill_is_visible;         //Flächen sichtbar?
   int color;                    //Farbe des Objektes (überschreibt Standard)
   bool single_colored;          //Objekt einfarbig?
   unsigned char alpha;                   //Objekt (halb)durchsichtig?
   bool is_textured;             //werden Texturen verwendet
   TEXTURE *texture;             //Array mit Texturendaten
   char line_color;
} OBJECT;

void init_actions();
void proceed_actions();
int get_a_action_anz();
int get_i_action_anz();
    
void set_line_color(unsigned char color);
MODEL* load_3do(const char name[]);

typedef void (*ACTION_FUNCTION0)();
typedef void (*ACTION_FUNCTION1)(void*);
typedef void (*ACTION_FUNCTION2)(void*, void*); //Funktionspointer

class MODEL3D_CLASS
{
   private:
   bool get_object_matrix_recursive(int o, int o_soll);
   MODEL *model;
   int action_group;

   public:
   OBJECT *object;               //Zeiger auf Baumstruktur mit den Eigenschaften der Unterobjekte zum Inhalt
   void kill_all_actions();
   int prepare_object_matrices();
   int get_object_by_name(const char name[]);
   MATRIX_f get_object_matrix_world(int o, MATRIX_f *m);
   MATRIX_f get_object_matrix(int o);
   void assign_3do(const char name[]);
   void clear_a_action(int o, char axis, signed char type);
   void clear_action_group(int a_g);
   void clear_all_actions();
   void reset_object_matrix(int o);
   bool assign_texture(const char name[], int face, const char texture_name[], float ax, float ay, float bx, float by, float cx, float cy, float dx, float dy);

   void set_action_group(int i){action_group=i;}
   int  get_action_group(){return(action_group);}

   void turn(const char name[], int axis, float amount, float speed);
   void move(const char name[], int axis, float amount, float speed);
   void spin(const char name[], char axis, float speed);
   bool stop_spin(const char name[]);
   void hide(int o);
   void hide(const char name[]);
   void show(const char name[]);
   void hide_grid(const char name[]);
   void hide_grid(int o);
   void hide_fill(const char name[]);
   void show_fill(const char name[]);
   void show_grid(const char name[]);
   void blink(const char name[], int time_on, int time_off, unsigned char col16_on, unsigned char col16_off);
   bool stop_blink(const char name[]);
   void set_color(const char name[], int col);
   void start_function(ACTION_FUNCTION0 action_function);
   void start_function(ACTION_FUNCTION1 action_function, void *pointer);
   void start_function(ACTION_FUNCTION2 action_function, void *pointer1, void *pointer2);
   void reset(const char name[]);
   bool wait_for_move(const char name[], char axis);
   bool wait_for_turn(const char name[], char axis);
   void wait(int t);

   void draw_object(int o);
   void draw_object_alpha(int o, unsigned char alpha);
   void draw_object_lines(int o);
   void draw_object_color(int o);
   void draw_objects(int o);
   void draw_objects_lines(int o);
   void draw_objects_color(int o);
   void draw_objects_alpha(int o, unsigned char alpha);
   void draw_object_textures(int o);
   void draw();
   ~MODEL3D_CLASS();
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////       STEREO       ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define LEFT_EYE 0
#define RIGHT_EYE 1


class STEREO_CLASS
{
   private:
   float distance_to_monitor;
   float half_eye_seperation;
   float monitor_width;
   float half_monitor_width;
   float monitor_height;
   float half_monitor_height;
   float far_clip;
   float clipscale;
   float near_clip;
   float eye_seperation;
   float scale;

   public:
   STEREO_CLASS();
   void init_lighting();
   void set_eye(bool eye);
   void position_camera(MATRIX_f *camera_matrix);

   void  set_eye_seperation(float value){eye_seperation=value; half_eye_seperation=value/2;}
   float get_eye_seperation(){return(eye_seperation);}
   void  set_monitor_height(float value){monitor_height=value;half_monitor_height=value/2;}
   float get_monitor_height(){return(monitor_height);}
   void  set_distance_to_monitor(float value){distance_to_monitor=value;}
   float get_distance_to_monitor(){return(distance_to_monitor);}
   void  set_far_clip(float value){far_clip=value;}
   float get_far_clip(){return(far_clip);}
   void  set_near_clip(float distance_to_face){if(distance_to_face > 0){near_clip=distance_to_face;clipscale=near_clip/distance_to_monitor;}}
   void  set_near_clip_to_monitor(){near_clip=distance_to_monitor;clipscale=1.0;}
   void  set_scale(float value){scale=value;}
   float get_scale(){return(scale);}
};

void init_color_table(char file[]);
void color16(char lumi);
void color16(char lumi, unsigned char alpha);
void color256(unsigned char lumi);
void color256(unsigned char lumi, unsigned char alpha);
inline bool get_eye();
void update_color_fx();
void convert_to_stereo_bitmap(BITMAP *bmp, bool eye);


////////////////////////////////////////////////////////////////////////////////
////////////////////////////     FONT-DRAWING     //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class FONT3D_CLASS
{
   private:
   void draw_char(unsigned char a);
   public:
   MODEL3D_CLASS model3d;
   void draw(char string[]);
   FONT3D_CLASS(const char filename[]);
};

#endif
