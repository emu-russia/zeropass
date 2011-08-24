#include "types.h"
#include "ZeroPass.h"

static char mess[] = "xB?DEjGH)JKLMNOPRSTUVQWXYZabefcdhigFk!mnopqrstuvwyAz27453968C/-*";

static int reorder[] = {    // Bit-reorder table.
 46, 40, 22, 5, 18, 14, 47, 38, 19, 29, 6, 15, 48, 41, 39, 30, 7, 16, 49,
 42, 37, 23, 8, 17, 34, 20, 24, 31, 9, 0, 50, 43, 25, 32, 10, 1, 51, 35,
 26, 33, 11, 2, 52, 44, 21, 27, 12, 3, 53, 45, 36, 28, 13, 4 };

static bool weaptype[] = { // 0: gun, 1: equipment
 0,
 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0 };

#define BOUND(x, min, max)  \
    if(x < min) x = min;    \
    if(x > max) x = max;    \

static void incorrect(char p[10])
{
    p[0] = 'I'; p[1] = 'N'; p[2] = 'C';
    p[3] = 'O'; p[4] = 'R'; p[5] = 'R';
    p[6] = 'E'; p[7] = 'C'; p[8] = 'T';
    p[9] = '\0';
}

static void bubbleSort(int numbers[], int numbers_ex[], int array_size)
{
    int i, j, temp;

    for(i = (array_size-1); i>=0; i--)
    {
        for(j=1; j<=i; j++)
        {
            if(numbers[j-1] > numbers[j])
            {
                temp = numbers[j-1];
                numbers[j-1] = numbers[j];
                numbers[j] = temp;

                temp = numbers_ex[j-1];
                numbers_ex[j-1] = numbers_ex[j];
                numbers_ex[j] = temp;
            }
        }
    }
}

static void stripEmptySlots(PassDecode *pd)
{
    int cnt = 0;

    // Cntlzw.
    for(int i=0; i<5; i++)
    {
        if(pd->Weapon[i] == NONE) cnt++;
    }

    switch(cnt)
    {
        case 1:
            pd->Weapon[0] = pd->Weapon[1]; pd->Ammo[0] = pd->Ammo[1];
            pd->Weapon[1] = pd->Weapon[2]; pd->Ammo[1] = pd->Ammo[2];
            pd->Weapon[2] = pd->Weapon[3]; pd->Ammo[2] = pd->Ammo[3];
            pd->Weapon[3] = pd->Weapon[4]; pd->Ammo[3] = pd->Ammo[4];
            pd->Weapon[4] = NONE; pd->Ammo[4] = NONE;
            break;
        case 2:
            pd->Weapon[0] = pd->Weapon[2]; pd->Ammo[0] = pd->Ammo[2];
            pd->Weapon[1] = pd->Weapon[3]; pd->Ammo[1] = pd->Ammo[3];
            pd->Weapon[2] = pd->Weapon[4]; pd->Ammo[2] = pd->Ammo[4];
            pd->Weapon[3] = NONE; pd->Ammo[3] = NONE;
            pd->Weapon[4] = NONE; pd->Ammo[4] = NONE;
            break;
        case 3:
            pd->Weapon[0] = pd->Weapon[3]; pd->Ammo[0] = pd->Ammo[3];
            pd->Weapon[1] = pd->Weapon[4]; pd->Ammo[1] = pd->Ammo[4];
            pd->Weapon[2] = NONE; pd->Ammo[2] = NONE;
            pd->Weapon[3] = NONE; pd->Ammo[3] = NONE;
            pd->Weapon[4] = NONE; pd->Ammo[4] = NONE;
            break;
        case 4:
            pd->Weapon[0] = pd->Weapon[4]; pd->Ammo[0] = pd->Ammo[4];
            pd->Weapon[1] = NONE; pd->Ammo[1] = NONE;
            pd->Weapon[2] = NONE; pd->Ammo[2] = NONE;
            pd->Weapon[3] = NONE; pd->Ammo[3] = NONE;
            pd->Weapon[4] = NONE; pd->Ammo[4] = NONE;
            break;
    }
}

void MakePassword(PassDecode *pd, char password[10])
{
    int i;
    u8 buf1[16], buf2[16];

    incorrect(password);

    // Bound values.
    BOUND(pd->Level, 0, 43);
    BOUND(pd->Energy, 0, 99);

    for(i=0; i<5; i++)
    {
        BOUND(pd->Weapon[i], NONE, PULSER);
        if(weaptype[pd->Weapon[i]]) { BOUND(pd->Ammo[i], 0, 9); }
        else { BOUND(pd->Ammo[i], 0, 99); }
    }

    /*
     * Step 1: ENCODE.
    */
    buf1[0] = buf1[1] = buf1[2] = buf1[3] = buf1[4] = buf1[5] = buf1[6] = 0;

    // Commandos.
    if(pd->Ishii) buf1[0] |= 1;
    if(pd->Haile) buf1[0] |= 2;
    if(pd->Wolf) buf1[0] |= 4;
    if(pd->Ramos) buf1[0] |= 8;
    if(pd->Gjoerup) buf1[0] |= 0x10;

    bubbleSort(pd->Weapon, pd->Ammo, 5);
    stripEmptySlots(pd);

    // Weapons/equipment.
    int mask = 0;
    for(i=0; i<5; i++)
    {
        if(pd->Weapon[i] == NONE) pd->Ammo[i] = 0;
        else mask |= 1 << (pd->Weapon[i] - 1);
    }
    buf1[0] |= (mask & 7) << 5;
    buf1[1] = (mask >> 3) & 0xFF;
    buf1[2] = (mask >> 11) & 7;

    // Ammo.
    int Ammo[5] = {0, 0, 0, 0, 0};
    for(i=0; i<5; i++)
    {
        if(weaptype[pd->Weapon[i]]) Ammo[i] = ((pd->Ammo[i] << 4) / 10) / 2;
        else Ammo[i] = ((pd->Ammo[i] << 4) / 99) / 2;
    }
    buf1[2] |= Ammo[0] << 3;
    buf1[2] |= (Ammo[1] & 3) << 6;
    buf1[3]  = Ammo[1] >> 2;
    buf1[3] |= Ammo[2] << 1;
    buf1[3] |= Ammo[3] << 4;
    buf1[3] |= (Ammo[4] & 1) << 7;
    buf1[4]  = Ammo[4] >> 1;

    // Energy
    int Energy0 = ((pd->Energy << 7) / 100) / 2;
    buf1[4] |= Energy0 << 2;

    // Level
    buf1[5] = (pd->Level - 1) & 0x3F;

    /*
     * Step 2: CHECKSUM.
    */
    int sum = ( buf1[0] + buf1[1] + buf1[2] + buf1[3] + buf1[4] + buf1[5] ) & 0xFF;
    buf1[5] |= (sum & 3) << 6;
    buf1[6] = sum >> 2;

    /*
     * Step 3: REORDER.
    */
    buf2[0] = buf2[1] = buf2[2] = buf2[3] = buf2[4] = buf2[5] = buf2[6] = 0;
    for(i=0; i<54; i++)
    {
        u8 b = buf1[i/8];
        int set = b & (1 << (i & 7));
        if(set) buf2[reorder[i]/8] |= 1 << (reorder[i] & 7);
    }

    /*
     * Step 4: XOR.
    */
    buf2[0] ^= 0x69;
    buf2[1] ^= 0xAD;
    buf2[2] ^= 0x28;
    buf2[3] ^= 0x6B;
    buf2[4] ^= 0x95;
    buf2[5] ^= 0xD9;
    buf2[6] ^= 0x8B;

    /*
     * Step 5: UNPACK.
    */
    buf1[0] = buf2[0] & 0x3F;
    buf1[1] = ((buf2[1] & 0xF) << 2) | (buf2[0] >> 6);
    buf1[2] = ((buf2[2] & 3) << 4) | (buf2[1] >> 4);
    buf1[3] = buf2[2] >> 2;
    buf1[4] = buf2[3] & 0x3F;
    buf1[5] = ((buf2[4] & 0xF) << 2) | (buf2[3] >> 6);
    buf1[6] = ((buf2[5] & 3) << 4) | (buf2[4] >> 4);
    buf1[7] = buf2[5] >> 2;
    buf1[8] = buf2[6] & 0x3F;

    /*
     * Step 6: XLAT.
    */
    for(i=0; i<9; i++)
    {
        for(int alph=0; alph<64; alph++)
        {
            if(alph == buf1[i])
            {
                password[i] = mess[alph];
                break;
            }
        }
    }
}

// --------------------------- Decoder.

bool CheckPassword(PassDecode *pd, char password[10])
{
    int i;
    u8 buf1[16], buf2[16], *ptr;

    /*
     * Step 1: XLAT.
    */
    for(i=0, ptr=buf1; i<9; i++)
    {
        for(int alph=0; alph<64; alph++)
        {
            if(mess[alph] == password[i])
            {
                *ptr++ = alph;
                break;
            }
        }
        if(alph == 64) return false;    // Wrong letter encountered in password string
    }

    /*
     * Step 2: PACK.
    */
    buf2[0]  = buf1[0];
    buf2[0] |= buf1[1] << 6;
    buf2[1]  = buf1[1] >> 2;
    buf2[1] |= buf1[2] << 4;
    buf2[2]  = buf1[2] >> 4;
    buf2[2] |= buf1[3] << 2;
    buf2[3]  = buf1[4];
    buf2[3] |= buf1[5] << 6;
    buf2[4]  = buf1[5] >> 2;
    buf2[4] |= buf1[6] << 4;
    buf2[5]  = buf1[6] >> 4;
    buf2[5] |= buf1[7] << 2;
    buf2[6]  = buf1[8];

    /*
     * Step 3: XOR.
    */
    buf2[0] ^= 0x69;
    buf2[1] ^= 0xAD;
    buf2[2] ^= 0x28;
    buf2[3] ^= 0x6B;
    buf2[4] ^= 0x95;
    buf2[5] ^= 0xD9;
    buf2[6] ^= 0x8B;

    /*
     * Step 4: REORDER.
    */
    buf1[0] = buf1[1] = buf1[2] = buf1[3] = buf1[4] = buf1[5] = buf1[6] = 0;
    for(i=0; i<54; i++)
    {
        u8 b = buf2[reorder[i]/8];
        int set = b & (1 << (reorder[i] & 7));
        if(set) buf1[i/8] |= 1 << (i & 7);
    }

    /*
     * Step 5: CHECKSUM.
    */
    int sum = ( buf1[0] + buf1[1] + buf1[2] + buf1[3] + buf1[4] + (buf1[5] & 0x3F) ) & 0xFF;
    if(( (buf1[5] >> 6) | ((buf1[6] & 0x3F) << 2) ) != sum) return false;

    /*
     * Step 6: DECODE.
    */

    // Commandos
    pd->Ishii = (buf1[0] & 1) ? true : false;
    pd->Haile = (buf1[0] & 2) ? true : false;
    pd->Wolf = (buf1[0] & 4) ? true : false;
    pd->Ramos = (buf1[0] & 8) ? true : false;
    pd->Gjoerup = (buf1[0] & 0x10) ? true : false;

    // Weapons/equipment.
    int mask = ((buf1[2] & 7) << 11) | (buf1[1] << 3) | (buf1[0] >> 5), weap;
    for(weap=1, i=0; weap<=14; weap++)
    {
        if(mask & 1) pd->Weapon[i++] = weap;
        mask >>= 1;
        if(i > 5 && mask) return false; 
    }
    while(i<5) pd->Weapon[i++] = NONE;

    // Ammo
    int Ammo[5];
    Ammo[0] = (buf1[2] >> 3) & 7;
    Ammo[1] = ((buf1[3] & 1) << 2) | (buf1[2] >> 6);
    Ammo[2] = (buf1[3] >> 1) & 7;
    Ammo[3] = (buf1[3] >> 4) & 7;
    Ammo[4] = ((buf1[4] & 3) << 1) | (buf1[3] >> 7);

    for(i=0; i<5; i++)
    {
        if(pd->Weapon[i])
        {
            if(weaptype[pd->Weapon[i]])
            {
                pd->Ammo[i] = ((Ammo[i] * 2 + 1) * 10) >> 4;
                if(pd->Ammo[i] == 0) pd->Ammo[i] = 10;
            }
            else pd->Ammo[i] = ((Ammo[i] * 2 + 1) * 99) >> 4;
        }
        else pd->Ammo[i] = 0;
    }

    // Energy
    int Energy0 = buf1[4] >> 2;
    pd->Energy = ((Energy0 * 2 + 1) * 100) >> 7;

    // Level
    int Level = ((buf1[5] & 0x3F) + 1) % 64;
    if(Level == 13 || Level == 14 || Level == 15 || Level == 31 || Level > 43) return false;
    pd->Level = Level;

    return true;
}
