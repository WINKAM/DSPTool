/***************************************************************************

             WINKAM TM strictly confidential 06.04.2020

 ***************************************************************************/
#ifndef PALETTE_MANAGER_H
#define PALETTE_MANAGER_H


class Palette_Manager
{
public:
    static void set_dark_palette() noexcept;
    static void set_light_palette() noexcept;

    // 0 - light, 1 - dark
    static void set_palette_by_id(int id) noexcept;
    static int get_palette_id() noexcept;

private:
    Palette_Manager();
    static int s_palette_id;
};

#endif // PALETTE_MANAGER_H
