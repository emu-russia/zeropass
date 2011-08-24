
// Weapon type.
#define NONE        0
#define BIOSCAN     1
#define MINE        2
#define PROOFVEST   3
#define EXTINGUISH  4
#define FIRESUIT    5
#define FLASHLIGHT  6
#define GRENADE     7
#define PISTOL      8
#define NIGHTVIS    9
#define LASERGUN    10
#define ROCKET      11
#define SHOTGUN     12
#define FLAMER      13
#define PULSER      14

typedef struct PassDecode
{
    int     Level;
    int     Energy;

    int     Weapon[5];
    int     Ammo[5];

    bool    Ishii;
    bool    Haile;
    bool    Wolf;
    bool    Ramos;
    bool    Gjoerup;

} PassDecode;

void    MakePassword(PassDecode *pd, char password[10]);
bool    CheckPassword(PassDecode *pd, char password[10]);
