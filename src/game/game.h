#ifndef GAME_H
#define GAME_H

// all the externs come from game.syms extracted like this:
// mips64r5900el-ps2-elf-nm -f posix SLUS_202.24 | grep -iE " [tdb] " | awk '{print $1 " = 0x" $3 ";"}' > game.syms
// im unsure if other versions have full debug symbols, but this is for SLUS 2.0 specifically




///////// standard headers /////////


extern int vsprintf(unsigned char *str, char *fmt, unsigned long *ap);
extern int printf(char *fmt, ...);
extern int sprintf (char *__restrict __s, const char *__restrict __format, ...);
extern float sinf(float x);
extern float cosf(float x);
extern int strcmp(char *__s1, char *__s2);
extern int strncmp(char *__s1, char *__s2, unsigned int __n);
extern void srand(unsigned int seed);
extern int rand(void);


/*	Flush the EE instruction cache

	Use this after writing to executable memory
*/
extern void FlushCache(int);


///////// pmw2 headers /////////

typedef unsigned int BOOL;
typedef unsigned char byte;

#define true (BOOL)1;
#define false (BOOL)0;

typedef struct ACTIVESOUNDS
{
	/// @brief this is 1 if the sound is active, zero if it is inactive
	int status;
	unsigned int handle;
	int pitch;
	int paused_pitch;
	int pending;
	char *name;
	int loaded;
	int zone;
	/// @brief if status is 0 this is not reset normally
	int voll;
	/// @brief if status is 0 this is not reset normally
	int volr;
} active_sounds_t;

#define ACTIVE_SOUNDS_SIZE 48
extern active_sounds_t active_sounds[ACTIVE_SOUNDS_SIZE];

extern void soundUpdate(void);

/// @brief initializes the global ctors in the base game via __do_global_ctors, we inject our hooks after this
extern void __main(void);

/*	Writes formatted string to the screen at the specified coords
	
	This may not work depending on where you are calling it from

	X/Y ranges from 0 to 1
*/
extern void font_printfXY(float x, float y, char *fmt, ...);

/*	Resets the global state that controls how text is shown on screen to defualts */
extern void FontDefaults(void);

extern void SetFontAlpha(float alpha);
extern void SetFontColor(byte r, byte g, byte b);
extern void SetFontGradientTopColor(int r, int g, int b, int a);
extern void SetFontGradientBottomColor(int r, int g, int b, int a);
extern void SetFontScale(float scale);
extern void SetFontTranslucencyMode(unsigned int mode);
extern void SetFontWiggle(BOOL wiggle);
extern void SetFontSmallCaps(BOOL caps);

typedef enum font_alignment {
    FONT_ALIGN_LEFT=0,
    FONT_ALIGN_CENTER=1,
    FONT_ALIGN_RIGHT=2,
    FONT_ALIGN_CENTERCENTER=3
} font_alignment;

extern void SetFontAlignment(font_alignment fontAl);
extern void SetFontWierdZoom(float wiggle);
extern void SetFontGradient(BOOL b);
extern void SetFontShadow(BOOL b);
extern void SetWiggleWaveColor(char r, char g, char b, char a);

/*	Plays the specified FMV by name */
extern void vmain(char *name);

extern void Level_Update(void);
extern BOOL renderWorldObjects(void);

extern BOOL GiveTimeToPADforCALIBRATION(void);

extern void Game_Init(void);
extern void Game_DoShell(void);

typedef unsigned int tOFLG;

typedef struct _fvec {
    float x;
    float y;
    float z;
    float w;
} FVEC;

/* the message id passed to an object */
typedef enum messageType {
	/// @brief fired when object is first created, use this to initialize variables
    msg_init=0,
	/// @brief fired every frame, this should be used to render
    msg_render=1,
    msg_processFrameVisible=2,
	/// @brief only fired every frame when unpaused, it isn't fired when ghosts killed either
	msg_processFrame=3,
	/// @brief the object is being deleted, this is fired before it is fully removed
    msg_delete=4,
    msg_pacmaninrange=5,
    msg_collide=6,
    msg_setType=7,
    msg_startPath=8,
    msg_joinPath=9,
    msg_setScriptState=10,
    msg_setModel=11,
    msg_pacGrossCollide=12,
    msg_pacGetCollideRtn=13,
    msg_getScriptData=14,
    msg_bossDefeated=15,
    msg_buttBounce=16,
    msg_defeated=17,
    msg_finishedMaze=18,
    msg_finishedMini=19,
    msg_plrIn=20,
    msg_plrOn=21,
    msg_contact=22,
    msg_endContact=23,
    msg_restart=24,
    msg_plrRev=25,
    msg_pacSphereCollideRtn=26,
    msg_setPath=27,
    msg_reset=28,
    msg_setTime=29,
    msg_appear=30,
    msg_hide=31,
    msg_getCinemaCameraFocus=32,
    msg_getCinemaCameraRange=33,
    msg_tongue=34,
    msg_open=35,
    msg_close=36,
    msg_applause=37,
    msg_musicBoxStartPlaying=38,
    msg_activate=39,
    msg_deactivate=40,
    msg_stopBlue=41,
    msg_finished=42,
    msg_platformAdjustDone=43,
    msg_suspended=44,
    msg_unsuspended=45,
    msg_assignPrize=46,
    msg_musicFadeDone=47,
    msg_renderVU1=48,
    msg_setDelay=49,
    msg_setSpeed=50,
    msg_setReflectionMap=51,
    msg_tongueHit=52,
    msg_renderVU1xParent=53,
    msg_activateFromInventory=54,
    msg_getModel=55,
    msg_inventoryRenderSelected=56,
    msg_canActivateFromInventory=57,
    msg_hitByPooka=58,
    msg_resetPooka=59,
    msg_removePooka=60,
    msg_punch=61,
    msg_punchNear=62,
    msg_platformPreMoved=63,
    msg_buttBounceSomewhere=64,
    msg_BreakBridge=65,
    msg_getMouthPosOffset=66,
    msg_startIdleAnim=67,
    msg_startWalkAnim=68,
    msg_startRunAnim=69,
    msg_startTalkAnim=70,
    msg_startListenAnim=71,
    msg_startSpecialAnim=72,
    msg_specialAnimFinished=73,
    msg_cinemaEnded=74,
    msg_setAnimRate=75,
    msg_mazeGhostNewMode=76,
    msg_powerPelletEaten=77,
    msg_powerPelletExpired=78,
    msg_dotEaten=79,
    msg_pacDamaged=80,
    msg_pacKilled=81,
    msg_creatureEaten=82,
    msg_takeQuestItem=83,
    msg_useQuestItem=84,
    msg_displayItem=85,
    msg_getMoveToFunction=86,
    msg_moveStarted=87,
    msg_moveFinished=88,
    msg_makeSmoothie=89,
    msg_initmaze=90,
    msg_fruitEaten=91,
    msg_animDone=92,
    msg_animStop=93,
    msg_animContinue=94,
    msg_set=95,
    msg_mazeCleared=96,
    msg_steelPunch=97,
    msg_steelButtBounce=98,
    msg_steelSmashed=99,
    msg_powerPunch=100,
    msg_powerButtBounce=101,
    msg_powerSmashed=102,
    msg_smashed=103,
    msg_bumped=104,
    msg_gamePaused=105,
    msg_gameUnPaused=106,
    msg_pacHangingToYouRtn=107,
    msg_newObjCreated=108,
    msg_oldObjDeleted=109,
    msg_updateMyHeader=110,
    msg_nuke=111,
    msg_ready=112,
    msg_pollElectricShock=113,
    msg_electricShock=114,
    msg_kickNear=115,
    msg_sectionSuspended=116,
    msg_sectionUnsuspended=117,
    msg_setSpin=118,
    msg_setTranslucenyMode=119,
    msg_pacWL=120,
    msg_pacLW=121,
    msg_revving=122,
    msg_materialInfo=123,
    msg_pacGrossStandPos=124,
    msg_platformMotion=125,
    msg_platformRotation=126,
    msg_pacmanLeftBehind=127,
    msg_seaBossLevelMsg=128,
    msg_cinemaActivated=129,
    msg_shrink=130,
    msg_buttBounceNear=131,
    msg_cameraGrossCollide=132,
    msg_celebrate=133,
    msg_timeTrialOn=134,
    msg_pacManShrinkFromSmoothie=135,
    msg_pacManGrowFromSmoothie=136,
    msg_skeleton_died=137,
    msg_getEndOfLevel=138,
    msg_pacTeleported=139,
    msg_arcadeStart=140,
    msg_arcadeEnd=141,
    msg_renderGC=142,
    msg_renderGC2=143
} messageType;

struct sOBJHEAD;

typedef int (*OBJ_HNDL)(struct sOBJHEAD *hd, messageType message, void *data);

typedef struct sOBJHEAD {
	struct sOBJHEAD *next;
    struct sOBJHEAD *prev;
    struct sOBJHEAD *nextDeleted;
    tOFLG mFlags;
    unsigned int nType: 24;
    unsigned int nSubType: 8;
    OBJ_HNDL Handler;
    char *zsName;
    byte field8_0x1c;
    byte field9_0x1d;
    byte field10_0x1e;
    byte field11_0x1f;
    FVEC pos;
    FVEC rot;
    FVEC scale;
    short unsigned int actorScriptUniqueID;
    short unsigned int section;
    byte field17_0x54;
    byte field18_0x55;
    byte field19_0x56;
    byte field20_0x57;
    byte field21_0x58;
    byte field22_0x59;
    byte field23_0x5a;
    byte field24_0x5b;
    byte field25_0x5c;
    byte field26_0x5d;
    byte field27_0x5e;
    byte field28_0x5f;
} OBJHEAD;

extern void obj_Setup(OBJ_HNDL Handler, int nSize, int nType, int nSubType, char *zsName, int fLast);

extern void LoadDomedSky(void);
extern void CreatePacInventory(void);

extern void DrawPadDisconnectedString(void);

/// @brief more like struct PMO am i right?
struct PMI {
    unsigned char magic[4];
    unsigned int header;
    float version;
    short unsigned int width;
    short unsigned int height;
    unsigned char depth;
    unsigned char trans;
    unsigned char tw;
    unsigned char th;
    short unsigned int clutDepth;
    short unsigned int clutLen;
    short unsigned int rowLen;
    unsigned char flags;
    unsigned char pmode;
    short unsigned int texBP;
    short unsigned int clutBP;
};

typedef struct _nxf_material {
    struct PMI *texpmi;
    struct PMI *refpmi;
    char *texname;
    char *refmap;
    unsigned char refR;
    unsigned char refG;
    unsigned char refB;
    unsigned char refA;
    unsigned int flags;
    unsigned int alphaMode;
    unsigned int envMapAlphaMode;
    unsigned int pad1;
    unsigned int pad2;
    struct _nxf_material *next_material;
} NXF_MATERIAL;

typedef struct _nxf_vert3 {
    float x;
    float y;
    float z;
} NXF_VERT3;

typedef struct _nxf_rgba {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} NXF_RGBA;

typedef struct _nxf_uv {
    float u;
    float v;
} NXF_UV;

typedef struct _nxf_arrays {
    float minX;
    float minY;
    float minZ;
    unsigned int numUVs;
    float maxX;
    float maxY;
    float maxZ;
    unsigned int numNormals;
    float cx;
    float cy;
    float cz;
    float radius;
    unsigned int numVerts;
    unsigned int numCols;
    unsigned int maxVerts;
    unsigned int maxNormals;
    unsigned int maxCols;
    unsigned int maxUVs;
    NXF_VERT3 *verts;
    void *normals;
    NXF_RGBA *colors;
    NXF_UV *uvs;
    unsigned int flags;
    unsigned int pad1;
    unsigned int pad2;
} NXF_ARRAYS;

typedef struct _nxf_facelist {
    short unsigned int flags;
    unsigned char type;
    unsigned char attribs;
    unsigned int pad;
    NXF_MATERIAL *material;
    unsigned int num_faces;
    void *faces;
    struct _nxf_facelist *next_facelist;
    unsigned int display_list;
    unsigned int display_list_size;
} NXF_FACELIST;

typedef struct _nxf_matrix_pallet {
    unsigned int num_mats;
    short unsigned int *mat_idx;
} NXF_MATRIX_PALETTE;

typedef struct _nxf_facelist_set {
    unsigned int flags;
    unsigned int pad;
    unsigned int num_lists;
    NXF_FACELIST *first_facelist;
    NXF_MATRIX_PALETTE *mat_palette;
    struct _nxf_facelist_set *next_facelist_set;
} NXF_FACELIST_SET;

typedef struct _nxf_obj_geom {
    char ID[4];
    unsigned int endian;
    float version;
    unsigned int flags;
    unsigned int alphaMode;
    unsigned int envMapAlphaMode;
    short unsigned int num_strings;
    short unsigned int pad;
    char **strings;
    NXF_MATERIAL *first_material;
    NXF_ARRAYS *arrays;
    NXF_FACELIST_SET *first_facelist_set;
    unsigned int display_list;
    unsigned int display_list_size;
    struct _nxf_obj_geom *expanded;
    unsigned int pad1;
    unsigned int pad2;
    unsigned int pad3;
} NXF_OBJ_GEOM;

typedef struct _sprite_geom {
    char name[32];
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
    unsigned int flags;
    struct PMI *image;
    unsigned int pad;
    unsigned int offset;
} SPRITE_GEOM;


union _obj_geom__geom {
    struct VU0_OBJ_GEOM *vu0_geom;
    struct VU1_OBJ_GEOM *vu1_geom;
    NXF_OBJ_GEOM *nxf_geom;
    SPRITE_GEOM *sprite_geom;
    void *tribuf_geom;
    unsigned int offset;
    void *data;
};

typedef enum OBJ_GEOM_TYPE {
    OBJ_GEOM_TYPE_NULL=0,
    OBJ_GEOM_TYPE_VU0=1,
    OBJ_GEOM_TYPE_VU1=2,
    OBJ_GEOM_TYPE_SPRITE=3,
    OBJ_GEOM_TYPE_NXF=4,
    OBJ_GEOM_TYPE_TRIBUF=5
} OBJ_GEOM_TYPE;

typedef struct _obj_geom {
    unsigned int flags;
    enum OBJ_GEOM_TYPE type;
    union _obj_geom__geom geom;
    unsigned int pad[4];
} OBJ_GEOM;

typedef enum GEOM_FILE_FORMAT {
    formatUnknown=0,
    formatIMF=1,
    formatHMF=2,
    formatHXF=3,
    formatHXF2=4,
    formatVU1=6,
    formatVU1paged=7,
    formatIXF=8,
    formatNXF=9
} GEOM_FILE_FORMAT;

typedef struct _obj_shape {
    unsigned int num_geoms;
    OBJ_GEOM *geoms;
    unsigned int pad1;
    unsigned int pad2;
} OBJ_SHAPE;

typedef struct _bbox_tpl {
    FVEC scale;
    FVEC rot;
    FVEC trans;
    float minX;
    float maxX;
    float minY;
    float maxY;
    float minZ;
    float maxZ;
    char name[32];
    unsigned char pad[8];
} BBOX_TPL;

typedef struct _bsphere_tpl {
    FVEC scale;
    FVEC trans;
    float boundingSphereRadius;
    char name[32];
    unsigned char pad[12];
} BSPHERE_TPL;

typedef struct sCOLL_POLY {
    FVEC normal;
    FVEC v0;
    FVEC v1;
    FVEC v2;
} COLL_POLY;

typedef struct _vert_mat_weights {
    short unsigned int num_weights;
    short int node_idx[5];
    float weight[5];
} VERT_MAT_WEIGHTS;

typedef float FMATRIX[4][4];

typedef struct _shadow_line {
    short unsigned int aLines[2];
    short unsigned int aNormals[2];
} SHADOW_LINE;

typedef struct _shadow_tpl {
    unsigned int numVerts;
    unsigned int numNorms;
    unsigned int numLines;
    FVEC *verts;
    FVEC *norms;
    SHADOW_LINE *lines;
} SHADOW_TPL;


typedef struct _node_tpl {
    char geom_name[32];
    FVEC rot;
    FVEC trans;
    FVEC scale;
    FVEC baseRot;
    FVEC baseTrans;
    FVEC baseScale;
    unsigned int flags;
    short int initialVisibility;
    short unsigned int num_boxes;
    short unsigned int num_bspheres;
    short unsigned int num_bpolys;
    BBOX_TPL *bbox_tpls;
    BSPHERE_TPL *bsphere_tpls;
    COLL_POLY *bpolys;
    unsigned int num_shapes;
    OBJ_SHAPE *shapes;
    unsigned int num_mats;
    VERT_MAT_WEIGHTS *vert_mats;
    unsigned int num_vert_mat_idx;
    short int *vert_mat_idx;
    float lastTransSet;
    unsigned int hash_name;
    FMATRIX *transMats;
    void *model_data;
    FMATRIX basePoseMat;
    FMATRIX invBasePoseMat;
    FMATRIX unscaledBasePoseMat;
    FMATRIX invUnscaledBasePoseMat;
    unsigned int num_shadows;
    SHADOW_TPL *shadow_tpls;
    float collisionRange;
    unsigned int pad[5];
} NODE_TPL;

typedef struct _model_data {
    char ID[4];
    enum GEOM_FILE_FORMAT format;
    float version;
    unsigned int platform;
    unsigned int fixedup;
    char name[32];
    short unsigned int type;
    short unsigned int num_nodes;
    short int *node_rel_list;
    unsigned int num_geoms;
    OBJ_GEOM *geoms;
    unsigned int num_shapes;
    OBJ_SHAPE *shapes;
    NODE_TPL *node_tpls;
    unsigned int flags;
    unsigned int pad1;
    unsigned int pad2;
    float gbl_bsphere_radius;
    FVEC gbl_bsphere_offset;
    FVEC gbl_bbox_min;
    FVEC gbl_bbox_max;
    unsigned int num_bbox_tpl;
    BBOX_TPL *bbox_tpls;
    unsigned int num_bsphere_tpl;
    BSPHERE_TPL *bsphere_tpls;
    struct VU0_OBJ_GEOM *_vu0_geoms;
    unsigned int num_shadow_tpl;
    SHADOW_TPL *shadow_tpls;
    unsigned int pad[4];
    short unsigned int pad_short;
    short unsigned int num_working_mats;
} MODEL_DATA;

typedef struct _key_frame_quat {
    float vx;
    float vy;
    float vz;
    float vw;
    float time;
    unsigned int pad[3];
} KEY_FRAME_QUAT;

typedef struct _key_frame {
    float vx;
    float vy;
    float vz;
    float time;
} KEY_FRAME;

typedef struct _anim_node_tpl {
    char name[32];
    short unsigned int num_s_kframes;
    short unsigned int num_r_kframes;
    short unsigned int num_t_kframes;
    short unsigned int num_sh_kframes;
    short unsigned int num_nv_kframes;
    short unsigned int num_bv_kframes;
    short unsigned int num_ev_kframes;
    short unsigned int num_qr_kframes;
    short unsigned int index_in_array;
    short unsigned int pad1;
    KEY_FRAME *first_s_kframe;
    KEY_FRAME *first_r_kframe;
    KEY_FRAME *first_t_kframe;
    KEY_FRAME *first_sh_kframe;
    KEY_FRAME *first_nv_kframe;
    KEY_FRAME *first_bv_kframe;
    KEY_FRAME *first_ev_kframe;
    KEY_FRAME_QUAT *first_qr_kframe;
    unsigned int hash_name;
    unsigned int last_node_index;
    unsigned int flags;
} ANIM_NODE_TPL;

/// @brief Anim Template
typedef struct _anim_tpl {
    char ID[4];
    float version;
    unsigned int platform;
    unsigned int fixedup;
    char name[32];
    float total_anim_time;
    unsigned int num_anim_node_tpls;
    unsigned int numEvents;
    unsigned int eventTable;
    unsigned int animEventsTable;
    unsigned int defaultEventCallback;
    unsigned int flags;
    unsigned int pad[2];
    MODEL_DATA *last_model_data;
    char *nextChunk;
    ANIM_NODE_TPL **anim_node_tpls;
} ANIM_TPL;

typedef struct _anim {
    ANIM_TPL *tpl;
    BOOL reverse;
    unsigned int control_flags;
    float time;
    float rate;
    float weight;
    float weight_start;
    float weight_end;
    float anim_start_time;
    float ramp_start_time;
    float ramp_end_time;
    float last_anim_time;
    float last_update_time;
    float last_update_game_time;
    float normalized_weight;
    unsigned int flags;
    struct _anim *next_free;
} ANIM;

typedef struct _node_envelope_data {
    union FMAT *localWorldVertMats;
    union FMAT *unscaledLocalWorldVertMats;
    union FMAT *preCalc_lw_invBasePose;
    union FMAT *preCalc_unscaled_lw_invBasePose;
} NODE_ENVELOPE_DATA;

typedef float FVECTOR[4];
typedef FVEC QUAT;

typedef enum VISIBILITY {
    visibility_off=-1,
    invisible=0,
    visible=1
} VISIBILITY;

typedef struct _transform {
    FVEC rot;
    FVEC trans;
    FVEC scale;
    QUAT quat_rot;
    short unsigned int use_quat_rot;
    short unsigned int flags;
    enum VISIBILITY branchInvisibility;
    enum VISIBILITY nodeInvisibility;
    float render_shape;
} TRANSFORM;

typedef struct _anim_node {
    ANIM_NODE_TPL *tpl;
    short unsigned int prev_s_kframe_num;
    short unsigned int prev_r_kframe_num;
    short unsigned int prev_t_kframe_num;
    short unsigned int prev_sh_kframe_num;
    short unsigned int prev_nv_kframe_num;
    short unsigned int prev_bv_kframe_num;
    short unsigned int prev_qr_kframe_num;
    byte field8_0x12;
    byte field9_0x13;
    KEY_FRAME *prev_ev_kframe;
    unsigned int flags;
    struct _anim_node *next_free;
    struct _anim_node *next;
} ANIM_NODE;

typedef struct _model_bbox_info {
    BBOX_TPL *bbox_tpl;
    byte field1_0x4;
    byte field2_0x5;
    byte field3_0x6;
    byte field4_0x7;
    byte field5_0x8;
    byte field6_0x9;
    byte field7_0xa;
    byte field8_0xb;
    byte field9_0xc;
    byte field10_0xd;
    byte field11_0xe;
    byte field12_0xf;
    FMATRIX lw_mat;
    unsigned int flags;
    struct _model_bbox_info *pnext;
    byte field16_0x58;
    byte field17_0x59;
    byte field18_0x5a;
    byte field19_0x5b;
    byte field20_0x5c;
    byte field21_0x5d;
    byte field22_0x5e;
    byte field23_0x5f;
} MODEL_BBOX;

typedef struct _model_bsphere_info {
    BSPHERE_TPL *bsphere_tpl;
    byte field1_0x4;
    byte field2_0x5;
    byte field3_0x6;
    byte field4_0x7;
    byte field5_0x8;
    byte field6_0x9;
    byte field7_0xa;
    byte field8_0xb;
    byte field9_0xc;
    byte field10_0xd;
    byte field11_0xe;
    byte field12_0xf;
    FMATRIX lw_mat;
    unsigned int flags;
    struct _model_bsphere_info *pnext;
    byte field16_0x58;
    byte field17_0x59;
    byte field18_0x5a;
    byte field19_0x5b;
    byte field20_0x5c;
    byte field21_0x5d;
    byte field22_0x5e;
    byte field23_0x5f;
} MODEL_BSPHERE;

typedef struct _node {
    unsigned int VIFcmd0[4];
    FMATRIX lw_mat;
    FMATRIX ll_mat;
    FMATRIX lw_unscaled_mat;
    unsigned int VIFcmd1[4];
    char *name;
    NODE_ENVELOPE_DATA *node_envelope_data;
    void *model;
    short unsigned int flags;
    short unsigned int anim_flags;
    FVECTOR rot;
    FVECTOR trans;
    FVECTOR scale;
    QUAT quat_rot;
    short unsigned int use_quat_rot;
    short unsigned int transform_flags;
    enum VISIBILITY branchInvisibility;
    enum VISIBILITY nodeInvisibility;
    float render_shape;
    TRANSFORM *anim_xform;
    byte field20_0x144;
    byte field21_0x145;
    byte field22_0x146;
    byte field23_0x147;
    byte field24_0x148;
    byte field25_0x149;
    byte field26_0x14a;
    byte field27_0x14b;
    byte field28_0x14c;
    byte field29_0x14d;
    byte field30_0x14e;
    byte field31_0x14f;
    FVEC global_scale;
    OBJ_SHAPE *shape;
    NODE_TPL *node_tpl;
    ANIM_NODE *anim_node[2][2];
    struct _node *next;
    struct _node *child;
    struct _node *parent;
    MODEL_BBOX *bboxes;
    MODEL_BSPHERE *bspheres;
    struct _node *next_bpoly_node;
    unsigned char total_filters;
    unsigned char pad;
    short unsigned int pad2;
    byte field45_0x194;
    byte field46_0x195;
    byte field47_0x196;
    byte field48_0x197;
    byte field49_0x198;
    byte field50_0x199;
    byte field51_0x19a;
    byte field52_0x19b;
    byte field53_0x19c;
    byte field54_0x19d;
    byte field55_0x19e;
    byte field56_0x19f;
} NODE;

typedef struct _model {
    FMATRIX light_dir_mat;
    FMATRIX world_mat;
    FMATRIX unscaled_world_mat;
    NODE *root_node;
    MODEL_DATA *model_data;
    unsigned int flags;
    float gbl_bsphere_radius;
    FVEC gbl_bsphere_offset;
    FVEC gbl_bbox_min;
    FVEC gbl_bbox_max;
    NODE **node_list;
    unsigned int num_nodes;
    unsigned int num_alloc_list;
    float max_draw_dist;
    float cam_dist;
    float last_clip;
    unsigned int num_bbox;
    MODEL_BBOX *bboxes;
    unsigned int num_bsphere;
    MODEL_BSPHERE *bspheres;
    NODE *first_bpoly_node;
    union FMAT *envelope_mats;
    char *aux_data;
    unsigned char rr;
    unsigned char rg;
    unsigned char rb;
    unsigned char ra;
    byte field27_0x138;
    byte field28_0x139;
    byte field29_0x13a;
    byte field30_0x13b;
    byte field31_0x13c;
    byte field32_0x13d;
    byte field33_0x13e;
    byte field34_0x13f;
} MODEL;

typedef struct _anim_ctrl {
    ANIM *anim[2];
    unsigned int model_anim_idx;
    unsigned int next_anim_slot;
    NODE *start_anim_node;
    NODE *end_child_anim_node;
    NODE *end_anim_node;
    MODEL *model;
    char *name;
    float optim_dist_close;
    float optim_dist_far;
    float optim_time_gap;
    float last_update_master_time;
    unsigned int num_running;
    unsigned int flags;
    struct _anim_ctrl *next_free;
    FVEC root_pos;
    FVEC root_rot;
} ANIM_CTRL;

typedef struct sMOBILEPHYSICS {
    FVEC force;
    FVEC localinstantforce;
    FVEC instantforce;
    FVEC speed;
    FVEC environmentforce;
    FVEC framespeed;
    float mass;
    float groundresistance;
    float airresistance;
    float vertairresistance;
    float drive;
    float sidewayresistance;
    float vertgroundresistance;
    float waterresistancerate;
    float framespeedScalar;
    float speedScalar;
    float RealSpeed;
    byte field17_0x8c;
    byte field18_0x8d;
    byte field19_0x8e;
    byte field20_0x8f;
} MOBILEPHYSICS;

typedef enum MAT_TYPE {
    matUnknown=-1,
    matRock=0,
    matMetal=1,
    matLava=2,
    matSand=3,
    matMud=4,
    matSnow=5,
    matLeaves=6,
    matWood=7,
    matIce=8,
    matRubber=9,
    matNet=10,
    matWater=11,
    MAX_MATERIALS=12
} MAT_TYPE;

typedef enum ATT_TYPE {
    attUnknown=-1,
    attMaterial=0,
    attSticky=1,
    attSlippery=2,
    attRough=3,
    attSink=4,
    attShimmy=5,
    attThorny=6,
    attMuddy=7,
    attNoSteps=8,
    attQuick=9,
    attCoral=10,
    attMetalic=11,
    effBoost=12,
    effDeadly=13,
    effRolling=14,
    effNoHang=15,
    effWeb=16,
    effCurrent=17,
    effFreeze=18,
    effNoDamage=19,
    attLava=20,
    attIceberg=21,
    MAX_MATERIAL_ATTRIBUTES=22
} ATT_TYPE;

struct DefPacCollPartInfo {
    void *Foot;
    void *Head;
    void *Cube;
};

struct Dma2d_Reg_Value_t {
    long unsigned int nValue;
    long unsigned int nReg;
};

struct Dma2dInit_s__Gif {
    struct Dma2d_Reg_Value_t BITBLTBUF;
};

struct Dma2dTrans_s__Gif {
    struct Dma2d_Reg_Value_t TRXPOS;
    struct Dma2d_Reg_Value_t TRXREG;
    struct Dma2d_Reg_Value_t TRXDIR;
};

typedef struct Dma2dTrans_s {
    long unsigned int Dma[2];
    long unsigned int GifTag;
    long unsigned int GifRegs;
    struct Dma2dTrans_s__Gif Gif;
    long unsigned int ImgGifTag;
    long unsigned int ImgGifRegs;
    long unsigned int Ref[2];
    long unsigned int Ret[2];
} Dma2d_Trans_t;

typedef struct _texpage_entry {
    Dma2d_Trans_t Dma2d_Trans;
    char name[32];
    short unsigned int width;
    short unsigned int height;
    short unsigned int xPos;
    short unsigned int yPos;
    struct _texpage_entry *shared_entry;
    struct PMI *image;
    short unsigned int used;
    short unsigned int flags;
    unsigned int nByteSize;
    unsigned int flags3;
    byte field12_0xbc;
    byte field13_0xbd;
    byte field14_0xbe;
    byte field15_0xbf;
} TEXPAGE_ENTRY;


typedef struct Dma2dInit_s {
    long unsigned int GifTag;
    long unsigned int GifRegs;
    struct Dma2dInit_s__Gif Gif;
} Dma2d_Init_t;

typedef struct _texpage {
    Dma2d_Init_t Dma2dInit;
    short unsigned int squareWidth;
    short unsigned int squareHeight;
    unsigned char pad[92];
    char name[32];
    short unsigned int used;
    short unsigned int width;
    short unsigned int height;
    short unsigned int numEntries;
    short unsigned int maxEntries;
    short unsigned int dependancies;
    short unsigned int imagesLoaded;
    short unsigned int VRAMuploaded;
    struct _texpage *shared_page;
    unsigned int flags;
    void *firstObj;
    unsigned char pad2[63];
    unsigned char pmode;
    TEXPAGE_ENTRY *entries;
} TEXPAGE;

typedef struct _vu1_obj {
    unsigned int VIFcmd0[4];
    FMATRIX rottrans;
    FMATRIX light;
    FMATRIX effect;
    unsigned int VIFcmd1[4];
    float px;
    float py;
    float pz;
    unsigned int pad0;
    float rx;
    float ry;
    float rz;
    unsigned int pad1;
    float sx;
    float sy;
    float sz;
    unsigned int pad2;
    unsigned short *data;
    unsigned int numQW;
    unsigned int numFaces;
    short unsigned int flags;
    short unsigned int used;
    byte code;
    unsigned int unused;
    OBJ_GEOM obj_geom;
    unsigned char *name;
    TEXPAGE *shared_page;
    struct TEXPAGE_USAGE *page_usage;
    struct _vu1_obj *next_page_obj;
    void *propPtr;
    unsigned int propCnt;
    float rad;
    float screenZ;
    unsigned char wasDrawn;
    byte field34_0x165;
    byte field35_0x166;
    byte field36_0x167;
    unsigned int vis_vol_bits;
    unsigned char dmaPad[11];
    byte field39_0x177;
    byte field40_0x178;
    byte field41_0x179;
    byte field42_0x17a;
    byte field43_0x17b;
    byte field44_0x17c;
    byte field45_0x17d;
    byte field46_0x17e;
    byte field47_0x17f;
} VU1_OBJ;

typedef struct sMazePath {
    FVEC Pos;
    FVEC Up;
    struct sMazePath *NB[4];
} MazePath;

typedef struct AreaBox_Struct {
    FMATRIX localWorldMat;
    FMATRIX invLocalWorldMat;
    FVEC center;
    FVEC min;
    FVEC max;
    float radius;
    byte field6_0xb4;
    byte field7_0xb5;
    byte field8_0xb6;
    byte field9_0xb7;
    byte field10_0xb8;
    byte field11_0xb9;
    byte field12_0xba;
    byte field13_0xbb;
    byte field14_0xbc;
    byte field15_0xbd;
    byte field16_0xbe;
    byte field17_0xbf;
} AreaBox;

typedef struct sMazeOBJ {
    OBJHEAD hd;
    AreaBox ABox;
    int nX;
    int nZ;
    int numMaze;
    MazePath *pMaze;
    float globaltime;
    BOOL CountEnable;
    int AttackMode;
    int numDot;
    int restDot;
    float counter;
    int mode;
    MazePath *FruitPoint;
    OBJHEAD *FruitObj;
    float FruitTime;
    int IdxDotToFruit;
    int defaultPacDir;
} MazeOBJ;

typedef enum DefWarpMode {
    _WARP_IN=0,
    _WARP_OUT=1
} DefWarpMode;

struct DefForMaze {
    BOOL OnMaze;
    int GoingDir;
    MazeOBJ *maze;
    MazePath *GoingElement;
    MazePath *CurrentElement;
    byte field5_0x14;
    byte field6_0x15;
    byte field7_0x16;
    byte field8_0x17;
    byte field9_0x18;
    byte field10_0x19;
    byte field11_0x1a;
    byte field12_0x1b;
    byte field13_0x1c;
    byte field14_0x1d;
    byte field15_0x1e;
    byte field16_0x1f;
    FVEC TargetPoint;
    MazePath *NestElement;
    MazePath *StartElement;
    byte field20_0x38;
    byte field21_0x39;
    byte field22_0x3a;
    byte field23_0x3b;
    byte field24_0x3c;
    byte field25_0x3d;
    byte field26_0x3e;
    byte field27_0x3f;
    FVEC TerritoryPoint;
    MazePath *FirstElement;
    OBJHEAD *Clyde;
    int mySecLevel;
    float counter;
    float StartWaitTime;
    float WarpTime;
    enum DefWarpMode WarpMode;
    MazePath *WarpOutElement;
};

typedef enum EnumStand {
    _NORMAL=0,
    _TOE=1,
    _HEEL=2,
    _RIGHT=3,
    _LEFT=4
} EnumStand;

struct OCEANLINER {
    struct DefNurbsCurve *path;
    byte field1_0x4;
    byte field2_0x5;
    byte field3_0x6;
    byte field4_0x7;
    byte field5_0x8;
    byte field6_0x9;
    byte field7_0xa;
    byte field8_0xb;
    byte field9_0xc;
    byte field10_0xd;
    byte field11_0xe;
    byte field12_0xf;
    FVEC dir;
    float time;
    float OffsetX;
    float OffsetY;
    float SpeedX;
    float SpeedY;
    byte field19_0x34;
    byte field20_0x35;
    byte field21_0x36;
    byte field22_0x37;
    byte field23_0x38;
    byte field24_0x39;
    byte field25_0x3a;
    byte field26_0x3b;
    byte field27_0x3c;
    byte field28_0x3d;
    byte field29_0x3e;
    byte field30_0x3f;
    FVEC currentCenter;
    float speedRate;
    byte field33_0x54;
    byte field34_0x55;
    byte field35_0x56;
    byte field36_0x57;
    byte field37_0x58;
    byte field38_0x59;
    byte field39_0x5a;
    byte field40_0x5b;
    byte field41_0x5c;
    byte field42_0x5d;
    byte field43_0x5e;
    byte field44_0x5f;
    FVEC LWaxis;
    float LWrad;
    int point;
    float alpha;
    struct DefOceanLiner *obj;
    float TrembleTime;
    float trembledX;
    float trembleX;
    float trembledY;
    float trembleY;
    byte field55_0x94;
    byte field56_0x95;
    byte field57_0x96;
    byte field58_0x97;
    byte field59_0x98;
    byte field60_0x99;
    byte field61_0x9a;
    byte field62_0x9b;
    byte field63_0x9c;
    byte field64_0x9d;
    byte field65_0x9e;
    byte field66_0x9f;
};

struct DefAnimLinkedSound {
    int Anim;
    int Sound;
};

/// @brief this may not be the correct enum for PACRTN handles? needs further invesitgation
typedef enum PacmanStateMessage {
    pacNormal=0,
    pacInitAct=1,
    pacDoneAct=2,
    pacHit=3,
    pacLanded=4,
    pacUnderGround=5,
    pacOnSteep=6,
    pacDraw=7,
    pacAtePowerPellet=8,
    pacAteDot=9,
    pacKnocked=10,
    pacDamaged=11,
    pacKilled=12,
    pacAteCreature=13,
    pacOceanLinerEnd=14,
    pacStuck=15,
    pacHitChin=16,
    pacTouchSphere=17,
    pacSmashedHard=18,
    pacEndOfLevel=19
} PacmanStateMessage;

/// @brief Pacman State Handle
typedef int (*PACRTN)(OBJHEAD *obj, PacmanStateMessage msg);

typedef struct PAC_OBJ {
    OBJHEAD Head;
    ANIM_CTRL *anim_ctrl;
    MODEL *model;
    byte field3_0x68;
    byte field4_0x69;
    byte field5_0x6a;
    byte field6_0x6b;
    byte field7_0x6c;
    byte field8_0x6d;
    byte field9_0x6e;
    byte field10_0x6f;
    MOBILEPHYSICS motion;
    unsigned int PMCurrentAnim;
    PACRTN PMAction;
    byte field17_0x10b;
    byte field18_0x10c;
    byte field19_0x10d;
    byte field20_0x10e;
    byte field21_0x10f;
    FVEC ActualRot;
    FVEC TargetRot;
    FVEC VisualRot;
    enum ATT_TYPE surfaceatt;
    enum ATT_TYPE newsurfaceatt;
    enum ATT_TYPE surfaceeff;
    float waterlevel;
    float oldwaterlevel;
    float VU1waterlevel;
    int PMActionNo;
    float temporalmovement;
    int OnGround;
    byte field34_0x164;
    byte field35_0x165;
    byte field36_0x166;
    byte field37_0x167;
    byte field38_0x168;
    byte field39_0x169;
    byte field40_0x16a;
    byte field41_0x16b;
    byte field42_0x16c;
    byte field43_0x16d;
    byte field44_0x16e;
    byte field45_0x16f;
    FVEC GroundNormal;
    FVEC SphereCollNormal;
    enum MAT_TYPE surfacemat;
    float ground;
    float HeightAboveGround;
    byte field51_0x19c;
    byte field52_0x19d;
    byte field53_0x19e;
    byte field54_0x19f;
    struct DefForMaze Maze;
    OBJHEAD *Platform;
    OBJHEAD *HangingObj;
    BOOL RevPlatform;
    int JumpPress;
    float JumpPressTime;
    float JumpForce;
    float RevTime;
    float RevPress;
    int MultiButtBouncing;
    int ButtBouncePending;
    BOOL Sneak;
    BOOL CanSneak;
    int SwimmingState;
    byte field69_0x244;
    byte field70_0x245;
    byte field71_0x246;
    byte field72_0x247;
    byte field73_0x248;
    byte field74_0x249;
    byte field75_0x24a;
    byte field76_0x24b;
    byte field77_0x24c;
    byte field78_0x24d;
    byte field79_0x24e;
    byte field80_0x24f;
    FVEC HangingDir;
    FVEC HangingPos;
    COLL_POLY *leftPoly;
    COLL_POLY *hangingPoly;
    BOOL canButtBounce;
    BOOL canDoubleJump;
    BOOL canHighAirPunch;
    float MaxAltitude;
    BOOL RecordAltitude;
    OBJHEAD *revPlatform;
    float LastHangTime;
    BOOL PunchingHand;
    float whenLeftWater;
    BOOL amDying;
    float sunk;
    BOOL swimming;
    enum EnumStand standingmode;
    int IdleMode;
	/// @brief idle animation counter, once it reaches 0 an idle animation will play
    int IdleFrame;
    int IdleReplay;
    BOOL OnOceanLiner;
    byte field102_0x2bc;
    byte field103_0x2bd;
    byte field104_0x2be;
    byte field105_0x2bf;
    struct OCEANLINER OceanLiner;
    int realAttackMsg;
    float DeathTimer;
    BOOL NeedMoreSpeed;
    float boostRate;
    FVEC PlatformLocalPos;
    BOOL StickyPlatform;
    struct DefPacCollPartInfo CollPart;
    BOOL IgnoreCollision;
    BOOL JumpGravity;
    OBJHEAD *DotChainObj;
    byte field117_0x39c;
    byte field118_0x39d;
    byte field119_0x39e;
    byte field120_0x39f;
    FVEC LastLeanedPolyNormal;
    MODEL *pacReflection;
    NODE **reflectNodes;
    byte field124_0x3b8;
    byte field125_0x3b9;
    byte field126_0x3ba;
    byte field127_0x3bb;
    byte field128_0x3bc;
    byte field129_0x3bd;
    byte field130_0x3be;
    byte field131_0x3bf;
    FVEC HeadCenterPos;
    COLL_POLY *waterSurfacePoly;
    enum ATT_TYPE watersurfaceeff;
    byte field135_0x3d8;
    byte field136_0x3d9;
    byte field137_0x3da;
    byte field138_0x3db;
    byte field139_0x3dc;
    byte field140_0x3dd;
    byte field141_0x3de;
    byte field142_0x3df;
    FVEC WaterNormal;
    FVEC LastWaterNormal;
    VU1_OBJ *iceCube;
    float followSplineTime;
    struct SCENE_NURBSCURVE *followSpline;
    BOOL onFrostUp;
    float frostrate;
    BOOL BDoingEffect;
    int DeathType;
    float lowestYpos;
    float SquashedTime;
	/// @brief this appears to be fully unused, this screws up your controls and while it tries to put stars above your head it doesn't load it if set manually, so it just uses the rendered output itself
    int controls_whacked;
    struct DefAnimLinkedSound Event;
    float possiblyTrappedTime;
    BOOL DotChainEffect;
    byte field158_0x438;
    byte field159_0x439;
    byte field160_0x43a;
    byte field161_0x43b;
    byte field162_0x43c;
    byte field163_0x43d;
    byte field164_0x43e;
    byte field165_0x43f;
} PAC_OBJ;

extern PAC_OBJ* pacManObject;

extern BOOL drawPacManShadow;

// all pacman states

extern int PMNormal(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMRun(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMJump(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMJumpDown(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMRollDownHill(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMSwim(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMButtBounce(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMRevUp(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMStandingPunch(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMDolphin(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMRunPunch(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMJustLeftWater(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMRevRolling(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMHang(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMSlideDown(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMHang2Climb(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMClimb2ClimbUp(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMClimbUp(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMFallBack(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMDoNothing(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMDummyState(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMAIState(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMJumpForce(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMBurnInHell(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMDie(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMRunRadiallyAt(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMRunAt(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMOnSwingPlat(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMSucked(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMHurled(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMJumpPunch(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMHighAirPunch(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMKnocked(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMRecoverButtBounce(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMFrozenBlock(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMFrozenBlockMelt(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMFrozenBlockSleeping(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMRunOnMaze(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMOnPooCannon(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMOnBDoing(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMPunchSwitch(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMKnockedDown(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMChainFly(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMRevSkid(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMRevHelivate(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMButtBounce2Stand(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMJab(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMFall2Land(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMParalyzed(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMThrown(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMHangJump(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMSlipOnIce(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMFall2Hang(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMSwimRevUp(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMTorpedo(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMSquashed(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMSkating(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMDeadTorpedo(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMOceanLiner(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMElectrified(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMStunned(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMBurnOuch(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMFallDown(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMWipeOut(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMOceanLinerTorpedo(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMOceanLinerGlider(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMRollerSkating(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMHangJumpDown(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMOnSteep(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMRollSkid(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMOnFrostUp(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMTeleportIn(OBJHEAD *obj, PacmanStateMessage msg);
extern int PMTeleportOut(OBJHEAD *obj, PacmanStateMessage msg);

/// @brief resets to 0 at the start of each level and only counts up when unpaused
extern float gameTime;
/// @brief same as gameTime but it tracks the value from last frame
extern float oldGameTime;

typedef enum DefPadState {
    PAD_ACTIVE=0,
    PAD_INACTIVE=1
} DefPadState;

typedef struct PAD_INFO {
    int fd;
    short int status;
    short int type;
    short int change_flag;
    unsigned char mode;
    unsigned char oldid;
    union {
		short unsigned int data;
		struct {
			unsigned short l2       : 1; // (1 << 0)  | 0x0001
			unsigned short r2       : 1; // (1 << 1)  | 0x0002
			unsigned short l1       : 1; // (1 << 2)  | 0x0004
			unsigned short r1       : 1; // (1 << 3)  | 0x0008
			
			unsigned short triangle : 1; // (1 << 4)  | 0x0010
			unsigned short circle   : 1; // (1 << 5)  | 0x0020
			unsigned short cross    : 1; // (1 << 6)  | 0x0040
			unsigned short square   : 1; // (1 << 7)  | 0x0080
			
			unsigned short select   : 1; // (1 << 8)  | 0x0100
			unsigned short l3       : 1; // (1 << 9)  | 0x0200
			unsigned short r3       : 1; // (1 << 10) | 0x0400
			unsigned short start    : 1; // (1 << 11) | 0x0800
			
			unsigned short up       : 1; // (1 << 12) | 0x1000
			unsigned short right    : 1; // (1 << 13) | 0x2000
			unsigned short down     : 1; // (1 << 14) | 0x4000
			unsigned short left     : 1; // (1 << 15) | 0x8000
		} btn;
	};
    union {
		short unsigned int old_data;
		struct {
			unsigned short l2       : 1; // (1 << 0)  | 0x0001
			unsigned short r2       : 1; // (1 << 1)  | 0x0002
			unsigned short l1       : 1; // (1 << 2)  | 0x0004
			unsigned short r1       : 1; // (1 << 3)  | 0x0008
			
			unsigned short triangle : 1; // (1 << 4)  | 0x0010
			unsigned short circle   : 1; // (1 << 5)  | 0x0020
			unsigned short cross    : 1; // (1 << 6)  | 0x0040
			unsigned short square   : 1; // (1 << 7)  | 0x0080
			
			unsigned short select   : 1; // (1 << 8)  | 0x0100
			unsigned short l3       : 1; // (1 << 9)  | 0x0200
			unsigned short r3       : 1; // (1 << 10) | 0x0400
			unsigned short start    : 1; // (1 << 11) | 0x0800
			
			unsigned short up       : 1; // (1 << 12) | 0x1000
			unsigned short right    : 1; // (1 << 13) | 0x2000
			unsigned short down     : 1; // (1 << 14) | 0x4000
			unsigned short left     : 1; // (1 << 15) | 0x8000
		} old_btn;
	};
    short unsigned int trigger;
    short unsigned int release;
    unsigned char analog[4];
    short unsigned int arrow;
    short unsigned int old_arrow;
    short unsigned int arrowT;
    short unsigned int arrowR;
    float arrowRepeatX;
    float arrowRepeatZ;
    float arrowRepeatResetX;
    float arrowRepeatResetZ;
    unsigned char motor0_power;
    byte field20_0x31;
    byte field21_0x32;
    byte field22_0x33;
    int motor0_time;
    unsigned char motor1_power;
    byte field25_0x39;
    byte field26_0x3a;
    byte field27_0x3b;
    int motor1_time;
    int reset_counter;
    enum DefPadState state;
} pad_info_t;

extern pad_info_t pads[2];

extern int soundFindSymbol(char *symbol);
extern int playSound(int soundNo, int tone);
extern int soundPlay(int soundNum);
extern BOOL widescreenAspect;

/// @brief called before playing FMVs
extern void QueueHandler_Disable(void);

/// @brief if set to 1 the game is paused and the world objects will not send msg_processFrame messages
extern BOOL paused;

/// @brief returns 1 (true) if the screen fader is currently active 
extern BOOL ScreenFaderActive(void);

/// @brief returns 1 (true) if the screen fader is currently fading, ScreenFaderActive can be more reliable though
extern BOOL ScreenFaderFading(void);

#endif // GAME_H