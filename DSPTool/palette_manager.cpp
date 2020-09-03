/***************************************************************************

             WINKAM TM strictly confidential 06.04.2020

 ***************************************************************************/
#include "palette_manager.h"

#include <QPalette>
#include <QApplication>
#include <QStyle>

int Palette_Manager::s_palette_id = 0;

Palette_Manager::Palette_Manager()
{
}

void Palette_Manager::set_dark_palette() noexcept
{
    s_palette_id = 1;

    QPalette darkPalette;
    // https://github.com/pawelsalawa/sqlitestudio
    darkPalette.setColor(QPalette::Window, QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25,25,25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    darkPalette.setColor(QPalette::ToolTipText, Qt::black);

    qApp->setPalette(darkPalette);
}

void Palette_Manager::set_light_palette() noexcept
{
    s_palette_id = 0;

    qApp->setPalette(QApplication::style()->standardPalette());
}

void Palette_Manager::set_palette_by_id(int id) noexcept
{
    switch (id)
    {
    case 0:
        set_light_palette();
        break;

    case 1:
        set_dark_palette();
        break;

    default:
        break;
    }
}

int Palette_Manager::get_palette_id() noexcept
{
    return s_palette_id;
}

