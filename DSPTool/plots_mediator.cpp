/***************************************************************************

             WINKAM TM strictly confidential 29.04.2020

 ***************************************************************************/
#include "plots_mediator.h"
#include "plot/base_plot_window.h"

Plots_Mediator::Plots_Mediator(QObject *parent) : QObject(parent)
{

}

void Plots_Mediator::add_ptr_plot(Base_Plot_Window* ptr_plot) noexcept
{
    m_ptr_plot_windows.emplace_back(ptr_plot);
}

void Plots_Mediator::clear_ptr_plots() noexcept
{
    m_ptr_plot_windows.clear();
}

void Plots_Mediator::change_label_subtype_combobox_slot(const QString& subtype)
{
    if (QString::compare(subtype, m_label_subtype_combobox_value) == 0)
    {
        return;
    }

    m_label_subtype_combobox_value = subtype;
    change_label_subtype_combobox();
}

void Plots_Mediator::change_label_subtype_combobox() const noexcept
{
    for (Base_Plot_Window* ptr_plot : m_ptr_plot_windows)
    {
        ptr_plot->change_label_subtype_combobox_current_text(m_label_subtype_combobox_value);
    }

}

