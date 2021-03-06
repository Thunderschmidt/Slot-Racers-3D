#ifndef SND
#define SND
void init_sound();
void proceed_music();
void terminate_sound();



class SOUND_CLASS
{
    private:
    SAMPLE *sample[256];
    const char *name[256];
    int index[256];
    int total;
    MATRIX_f *camera_m;
    VECTOR *camera_pos;
    const char *dir;
    float maxdist, maxdist2;
//    int maxsounds;
    public:
    void adjust(int sound_nr, unsigned char volume, unsigned char pan, int speed, bool loop);
    void adjust(int i, VECTOR *pos, int volume);
    void adjust(const char filename[],  VECTOR *pos, int volume);

    void play(int i, VECTOR *pos);
    void play(const char filename[], VECTOR *pos);
    void play(const char filename[], unsigned char volume, unsigned char pan, int speed, bool loop);
    void play(const char filename[], float pos[3]);
    int load(const char filename[]);
    SOUND_CLASS(const char filedir[], MATRIX_f *m, VECTOR *pos, float max_distance);
    ~SOUND_CLASS();
    void stop(const char filename[]);
};

#endif
