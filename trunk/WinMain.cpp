// Zero Tolerance Password Recovery!!
// Author: Organic
// To contact: kvzorganic@mail.ru
#include "types.h"
#include "ZeroPass.h"

#include <stdio.h>

#include <windows.h>
#include "resource.h"

#define DEFAULT_PASS  "PHqzntKqA"

HWND dlg;

static bool weaptype[] = { // 0: gun, 1: equipment
 0,
 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0 };

static char *LevelDesc(int num)
{
    if(num >= 64 || num < 0) return "No such level!";

    switch(num)
    {
        case  0: return "Docking Bay Level 1";
        case  1: return "Docking Bay Level 2";
        case  2: return "Bridge Level 1";
        case  3: return "Engineering Level 1";
        case  4: return "Engineering Level 2";
        case  5: return "Engineering Level 3";
        case  6: return "Engineering Level 4";
        case  7: return "Greenhouse Level 1";
        case  8: return "Greenhouse Level 2";
        case  9: return "Greenhouse Level 3";
        case 10: return "Bridge Level 2";
        case 11: return "Reactor Level 1";
        case 12: return "Reactor Level 2";
        case 13: return "*DEBUG* Labyrinth";
        case 14: return "*DEBUG* Dockbridge Room 2";
        case 15: return "*DEBUG* Jail";

        case 16: return "High Rise Roof Top";
        case 17: return "High Rise Floor 164";
        case 18: return "High Rise Floor 163";
        case 19: return "High Rise Floor 162";
        case 20: return "High Rise Floor 161";
        case 21: return "High Rise Floor 160";
        case 22: return "High Rise Floor 159";
        case 23: return "High Rise Floor 158";
        case 24: return "High Rise Floor 157";
        case 25: return "High Rise Floor 156";
        case 26: return "High Rise Floor 155";
        case 27: return "High Rise Floor 154";
        case 28: return "High Rise Floor 153";
        case 29: return "High Rise Floor 152";
        case 30: return "High Rise Floor 151";
        case 31: return "*DEBUG* Boxing!!!";

        case 32: return "Sub Basement Level 1";
        case 33: return "Sub Basement Level 2";
        case 34: return "Sub Basement Level 3";
        case 35: return "Sub Basement Level 4";
        case 36: return "Sub Basement Level 5";
        case 37: return "Sub Basement Level 6";
        case 38: return "Sub Basement Level 7";
        case 39: return "Sub Basement Level 8";
        case 40: return "Sub Basement Level 9";
        case 41: return "Sub Basement Level 10";
        case 42: return "Sub Basement Level 11";
        case 43: return "Sub Basement Level 12";

        default: return "*DEBUG* Empty place";
    }
}

static char *WeapDesc(int type)
{
    switch(type)
    {
        case NONE: return "---";
        case BIOSCAN: return "Bio-Scanner";
        case MINE: return "Mines";
        case PROOFVEST: return "Bullet Proofvest";
        case EXTINGUISH: return "Fire Extinguisher";
        case FIRESUIT: return "Fire Proofsuit";
        case FLASHLIGHT: return "Flashligth";
        case GRENADE: return "Hand Grenade";
        case PISTOL: return "Handgun";
        case NIGHTVIS: return "Nightvision";
        case LASERGUN: return "Laser Aimed Gun";
        case ROCKET: return "Rocket Launcher";
        case SHOTGUN: return "Shotgun";
        case FLAMER: return "Flame Thrower";
        case PULSER: return "Pulse Laser";

        default: return "No such weapon!";
    }
}

static void FillCombos(void)
{
    int id[5] = { IDC_COMBO_W0, IDC_COMBO_W1, IDC_COMBO_W2, IDC_COMBO_W3, IDC_COMBO_W4 };

    for(int i=0; i<5; i++)
    {
        SendDlgItemMessage(dlg, id[i], CB_RESETCONTENT, 0, 0);
        for(int w=0; w<=PULSER; w++)
        {
            SendDlgItemMessage(dlg, id[i], CB_INSERTSTRING, -1, (LPARAM)(LPSTR)WeapDesc(w));
        }
        SendDlgItemMessage(dlg, id[i], CB_SETCURSEL, 0, 0);
    }
}

static void SetDlgControls(char *password)
{
    char buf[256];
    PassDecode pd;

    if(CheckPassword(&pd, password) == false)
    {
        SetDlgItemText(dlg, IDC_EDIT_PASS, "INCORRECT");
        ShowWindow(dlg, SW_NORMAL);
        UpdateWindow(dlg);
        Sleep(400);
        SetDlgItemText(dlg, IDC_EDIT_PASS, password);
        return;
    }

    SetDlgItemText(dlg, IDC_EDIT_PASS, password);

    sprintf(buf, "%i", pd.Level);
    SetDlgItemText(dlg, IDC_EDIT_L, buf);
    SetDlgItemText(dlg, IDC_STATIC_LEVELDESC, LevelDesc(pd.Level));

    sprintf(buf, "%i", pd.Energy);
    SetDlgItemText(dlg, IDC_EDIT_E, buf);

    CheckDlgButton(dlg, IDC_CHECK_ISHII, pd.Ishii ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(dlg, IDC_CHECK_HAILE, pd.Haile ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(dlg, IDC_CHECK_WOLF, pd.Wolf ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(dlg, IDC_CHECK_RAMOS, pd.Ramos ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(dlg, IDC_CHECK_GJOERUP, pd.Gjoerup ? BST_CHECKED : BST_UNCHECKED);

    if(weaptype[pd.Weapon[0]]) sprintf(buf, "%i%%", pd.Ammo[0] * 10);
    else sprintf(buf, "%i", pd.Ammo[0]);
    SetDlgItemText(dlg, IDC_EDIT_AMMO0, buf);
    if(weaptype[pd.Weapon[1]]) sprintf(buf, "%i%%", pd.Ammo[1] * 10);
    else sprintf(buf, "%i", pd.Ammo[1]);
    SetDlgItemText(dlg, IDC_EDIT_AMMO1, buf);
    if(weaptype[pd.Weapon[2]]) sprintf(buf, "%i%%", pd.Ammo[2] * 10);
    else sprintf(buf, "%i", pd.Ammo[2]);
    SetDlgItemText(dlg, IDC_EDIT_AMMO2, buf);
    if(weaptype[pd.Weapon[3]]) sprintf(buf, "%i%%", pd.Ammo[3] * 10);
    else sprintf(buf, "%i", pd.Ammo[3]);
    SetDlgItemText(dlg, IDC_EDIT_AMMO3, buf);
    if(weaptype[pd.Weapon[4]]) sprintf(buf, "%i%%", pd.Ammo[4] * 10);
    else sprintf(buf, "%i", pd.Ammo[4]);
    SetDlgItemText(dlg, IDC_EDIT_AMMO4, buf);

    SendDlgItemMessage(dlg, IDC_COMBO_W0, CB_SETCURSEL, pd.Weapon[0], 0);
    SendDlgItemMessage(dlg, IDC_COMBO_W1, CB_SETCURSEL, pd.Weapon[1], 0);
    SendDlgItemMessage(dlg, IDC_COMBO_W2, CB_SETCURSEL, pd.Weapon[2], 0);
    SendDlgItemMessage(dlg, IDC_COMBO_W3, CB_SETCURSEL, pd.Weapon[3], 0);
    SendDlgItemMessage(dlg, IDC_COMBO_W4, CB_SETCURSEL, pd.Weapon[4], 0);
}

static void MakePassword(char password[10])
{
    char buf[256];
    PassDecode pd;

    GetDlgItemText(dlg, IDC_EDIT_L, buf, 3);
    pd.Level = atoi(buf);
    GetDlgItemText(dlg, IDC_EDIT_E, buf, 3);
    pd.Energy = atoi(buf);

    pd.Ishii = IsDlgButtonChecked(dlg, IDC_CHECK_ISHII) == BST_CHECKED ? 1 : 0;
    pd.Haile = IsDlgButtonChecked(dlg, IDC_CHECK_HAILE) == BST_CHECKED ? 1 : 0;
    pd.Wolf = IsDlgButtonChecked(dlg, IDC_CHECK_WOLF) == BST_CHECKED ? 1 : 0;
    pd.Ramos = IsDlgButtonChecked(dlg, IDC_CHECK_RAMOS) == BST_CHECKED ? 1 : 0;
    pd.Gjoerup = IsDlgButtonChecked(dlg, IDC_CHECK_GJOERUP) == BST_CHECKED ? 1 : 0;

    pd.Weapon[0] = SendDlgItemMessage(dlg, IDC_COMBO_W0, CB_GETCURSEL, 0, 0);
    pd.Weapon[1] = SendDlgItemMessage(dlg, IDC_COMBO_W1, CB_GETCURSEL, 0, 0);
    pd.Weapon[2] = SendDlgItemMessage(dlg, IDC_COMBO_W2, CB_GETCURSEL, 0, 0);
    pd.Weapon[3] = SendDlgItemMessage(dlg, IDC_COMBO_W3, CB_GETCURSEL, 0, 0);
    pd.Weapon[4] = SendDlgItemMessage(dlg, IDC_COMBO_W4, CB_GETCURSEL, 0, 0);

    // Check for the same entries.
    for(int i=0; i<4; i++)
    {
        for(int j=i+1; j<5; j++)
        {
            if(pd.Weapon[i] == pd.Weapon[j] && pd.Weapon[i])
            {
                pd.Weapon[j] = NONE;
                pd.Ammo[j] = 0;
            }
        }
    }

    GetDlgItemText(dlg, IDC_EDIT_AMMO0, buf, 5);
    if(weaptype[pd.Weapon[0]]) pd.Ammo[0] = atoi(buf) / 10;
    else pd.Ammo[0] = atoi(buf);
    GetDlgItemText(dlg, IDC_EDIT_AMMO1, buf, 5);
    if(weaptype[pd.Weapon[1]]) pd.Ammo[1] = atoi(buf) / 10;
    else pd.Ammo[1] = atoi(buf);
    GetDlgItemText(dlg, IDC_EDIT_AMMO2, buf, 5);
    if(weaptype[pd.Weapon[2]]) pd.Ammo[2] = atoi(buf) / 10;
    else pd.Ammo[2] = atoi(buf);
    GetDlgItemText(dlg, IDC_EDIT_AMMO3, buf, 5);
    if(weaptype[pd.Weapon[3]]) pd.Ammo[3] = atoi(buf) / 10;
    else pd.Ammo[3] = atoi(buf);
    GetDlgItemText(dlg, IDC_EDIT_AMMO4, buf, 5);
    if(weaptype[pd.Weapon[4]]) pd.Ammo[4] = atoi(buf) / 10;
    else pd.Ammo[4] = atoi(buf);

    MakePassword(&pd, password);
}

static BOOL CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM  wParam, LPARAM lParam)
{
    char password[10];

    switch(uMsg)
    {
        case WM_INITDIALOG:
        {
            dlg = hwndDlg;
            ShowWindow(dlg, SW_NORMAL);
            SendMessage(dlg, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON)));
            FillCombos();
            SetDlgControls(DEFAULT_PASS);
            return TRUE;
        }

        case WM_CLOSE:
        {
            DestroyWindow(dlg);
            dlg = NULL;
            break;
        }
        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case IDC_BUTTON_MAKE:
                    MakePassword(password);
                    SetDlgControls(password);
                    return TRUE;
                case IDC_BUTTON_CHECK:
                    GetDlgItemText(dlg, IDC_EDIT_PASS, password, 10);
                    SetDlgControls(password);
                    return TRUE;
            }
        }
    }

    return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{

    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG), NULL, DialogProc);


    return 0;
}
