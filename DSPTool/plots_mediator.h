/***************************************************************************

             WINKAM TM strictly confidential 29.04.2020

 ***************************************************************************/
#ifndef PLOTS_MEDIATOR_H
#define PLOTS_MEDIATOR_H

#include <QObject>
#include <QString>
#include <vector>

class Base_Plot_Window;

class Plots_Mediator : public QObject
{
    Q_OBJECT

public:
    static Plots_Mediator& get_instance()
    {
        static Plots_Mediator instance;
        return instance;
    }

void add_ptr_plot(Base_Plot_Window* ptr_plot) noexcept;
void clear_ptr_plots() noexcept;

public slots:
    void change_label_subtype_combobox_slot(const QString& subtype);

protected:
    void change_label_subtype_combobox() const noexcept;

private:
    explicit Plots_Mediator(QObject* parent = nullptr);
    ~Plots_Mediator()= default;
    Plots_Mediator(const Plots_Mediator&)= delete;
    Plots_Mediator& operator=(const Plots_Mediator&)= delete;

    std::vector<Base_Plot_Window*> m_ptr_plot_windows;
    QString m_label_subtype_combobox_value;

};

#endif // PLOTS_MEDIATOR_H
