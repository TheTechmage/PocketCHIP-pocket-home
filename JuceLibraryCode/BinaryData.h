/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#ifndef BINARYDATA_H_30357753_INCLUDED
#define BINARYDATA_H_30357753_INCLUDED

namespace BinaryData
{
    extern const char*   appsIcon_png;
    const int            appsIcon_pngSize = 945;

    extern const char*   backIcon_png;
    const int            backIcon_pngSize = 417;

    extern const char*   bluetoothIcon_png;
    const int            bluetoothIcon_pngSize = 938;

    extern const char*   brightnessIconHi_png;
    const int            brightnessIconHi_pngSize = 1130;

    extern const char*   brightnessIconLo_png;
    const int            brightnessIconLo_pngSize = 870;

    extern const char*   check_png;
    const int            check_pngSize = 836;

    extern const char*   gamesIcon_png;
    const int            gamesIcon_pngSize = 1630;

    extern const char*   lock_png;
    const int            lock_pngSize = 810;

    extern const char*   seguibl_ttf;
    const int            seguibl_ttfSize = 323400;

    extern const char*   settingsIcon_png;
    const int            settingsIcon_pngSize = 18555;

    extern const char*   volumeIconHi_png;
    const int            volumeIconHi_pngSize = 1034;

    extern const char*   volumeIconLo_png;
    const int            volumeIconLo_pngSize = 330;

    extern const char*   wifiIcon_png;
    const int            wifiIcon_pngSize = 1293;

    extern const char*   wifiStrength0_png;
    const int            wifiStrength0_pngSize = 1644;

    extern const char*   wifiStrength1_png;
    const int            wifiStrength1_pngSize = 1726;

    extern const char*   wifiStrength2_png;
    const int            wifiStrength2_pngSize = 1743;

    extern const char*   wifiStrength3_png;
    const int            wifiStrength3_pngSize = 1472;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Number of elements in the namedResourceList array.
    const int namedResourceListSize = 17;

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes) throw();
}

#endif
