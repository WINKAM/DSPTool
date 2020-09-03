/***************************************************************************

             WINKAM TM strictly confidential 13.09.2017

 ***************************************************************************/
#include "tester_filters.h"
#include <QString>
#include <QDebug>
#include <unordered_map>
#include <iostream>

#include "low_pass_filter.h"
#include "butterworth_lpf.h"

#ifdef ALGO_APL_RJ_DEFINE

#include "wrap_ddsr.h"
#include "wrap_ddsr_3d.h"
#include "random_jump_filter.h"
#include "ddsr_filter.h"

#endif

void test_raw_signal(const std::vector<XYZ_Sample>& input_raw_data_vector
                     , std::vector<XYZ_Sample>* filtered_data_vector) noexcept
{
    for (const XYZ_Sample& s : input_raw_data_vector)
    {
        filtered_data_vector->emplace_back(s);
    }

    // we sort log data by timestamps
    std::sort(filtered_data_vector->begin(), filtered_data_vector->end()
              , [](const XYZ_Sample & a, const XYZ_Sample & b) -> bool
    {
        return a.m_time < b.m_time;
    });
}

void test_aplf(const std::vector<XYZ_Sample> &input_raw_data_vector
               , std::vector<XYZ_Sample>* filtered_data_vector) noexcept
{
    for (const XYZ_Sample& s : input_raw_data_vector)
    {
        filtered_data_vector->emplace_back(s);
    }
}

#ifdef ALGO_APL_RJ_DEFINE

void test_winkam_rj_btw(const std::vector<XYZ_Sample> &input_raw_data_vector
                        , std::vector<XYZ_Sample>* filtered_data_vector, const QString &parameters) noexcept
{
    WKTR::Position_Sample rj_sample_in, rj_sample_out;
    XYZ_Sample res_s;

    std::unordered_map<uint64_t, WKRJ::Random_Jump_Filter*> rj_filters_map;
    std::unordered_map<uint64_t, WKTR::Butterworth_LPF*> x_lp_filters_map;
    std::unordered_map<uint64_t, WKTR::Butterworth_LPF*> y_lp_filters_map;
    std::unordered_map<uint64_t, WKTR::Butterworth_LPF*> z_lp_filters_map;

    //auto coeffs_B_2 = {3.7264144987, -5.2160481952,  3.2500182574, -0.7604898175};
    //auto filter_gain_2 = 1.520093384e+05; //2.

    //    auto coeffs_B_2_5 = { 3.6580603024, -5.0314335334, 3.0832283018, -0.7101038983};
    //    double filter_gain_2_5 = 6.430156186e+04; //2.5

    //    auto coeffs_B_2_3 = { 3.6853989451, -5.1047419027, 3.1490126437, -0.7298502470};
    //    double filter_gain_2_3 =  8.861270442e+04; //2.3


    //auto coeffs_B = {3.5897338871, -4.8512758825, 2.9240526562, -0.6630104844};
    //double filter_gain =  3.201129162e+04; //3

        auto coeffs_B = { 3.3168079106 , -4.1742455501, 2.3574027806, -0.5033753607 };
        double filter_gain =  4.691779923e+03; //5

    //    auto coeffs_B = { 3.4531851376  , -4.5041390916,  2.6273036182, -0.5778338981 };
    //    double filter_gain =  1.077997190e+04; //4

    //    auto coeffs_B_2_2 = { 3.6990698273,  -5.1416644591, 3.1823680365 , -0.7399255301};
    //    double filter_gain_2_2 =  1.051764170e+05; //2.2

    //auto coeffs_B_1_9 = {3.7400882011, -5.2535101224,  3.2843183397, -0.7709827093};
    //double filter_gain_1_9 =  1.854194132e+05; //1.9

    //auto coeffs_B_1_8 = { 3.7537627567, -5.2911525842, 3.3189386048 , -0.7816187403};
    //double filter_gain_1_8 =  2.286922409e+05; //1.8

    auto coeffs_A = {1.0, 4.0, 6.0, 4.0, 1.0};

       // auto coeffs_B_1_7 = { 3.7674381215,  -5.3289759499, 3.3538817279 , -0.7923999281};
       // double filter_gain_1_7 =  2.855683536e+05; //1.7


    for (const XYZ_Sample& s : input_raw_data_vector)
    {
        if (rj_filters_map.find(s.m_object_id) == rj_filters_map.end()) // no filter found
        {
            auto rj_filter_ptr = new WKRJ::Random_Jump_Filter;
            rj_filter_ptr->set_enabled_butterworth_filter(true);
            rj_filters_map.insert(std::make_pair(s.m_object_id, rj_filter_ptr));

            auto x_lp_filter_ptr = new WKTR::Butterworth_LPF;
            x_lp_filter_ptr->set_coefficients(coeffs_A, coeffs_B, filter_gain);
            x_lp_filter_ptr->set_initial_value(s.m_x);
            x_lp_filters_map.insert(std::make_pair(s.m_object_id, x_lp_filter_ptr));

            auto y_lp_filter_ptr = new WKTR::Butterworth_LPF;
            y_lp_filter_ptr->set_coefficients(coeffs_A, coeffs_B, filter_gain);
            y_lp_filter_ptr->set_initial_value(s.m_y);
            y_lp_filters_map.insert(std::make_pair(s.m_object_id, y_lp_filter_ptr));

            auto z_lp_filter_ptr = new WKTR::Butterworth_LPF;
            z_lp_filter_ptr->set_coefficients(coeffs_A, coeffs_B, filter_gain);
            z_lp_filter_ptr->set_initial_value(s.m_z);
            z_lp_filters_map.insert(std::make_pair(s.m_object_id, z_lp_filter_ptr));
        }

        // INPUT

        rj_sample_in.m_time = s.m_time;
        rj_sample_in.m_x = s.m_x;
        rj_sample_in.m_y = s.m_y;
        rj_sample_in.m_z = s.m_z;

        //auto x_vr_filter = x_vr_filters_map.find(s.m_object_id);
        //auto y_vr_filter = y_vr_filters_map.find(s.m_object_id);
        //auto z_vr_filter = z_vr_filters_map.find(s.m_object_id);

        //double sx, sy, sz;
        //sx = x_vr_filter->second->filter(rj_sample_in.m_x, rj_sample_in.m_time);
        //sy = y_vr_filter->second->filter(rj_sample_in.m_y, rj_sample_in.m_time);
        //sz = z_vr_filter->second->filter(rj_sample_in.m_z, rj_sample_in.m_time);

        //rj_sample_in.m_x = sx;
        //rj_sample_in.m_y = sy;
        //rj_sample_in.m_z = sz;

        // RJ_3D
        auto rj_filter = rj_filters_map.find(s.m_object_id);
        rj_sample_out = rj_filter->second->process_position_sample(rj_sample_in, true);

        /*rj_sample_out = rj_sample_in;

        // LPF BTR
        auto x_lp_btrw_filter = x_lp_filters_map.find(s.m_object_id);
        auto y_lp_btrw_filter = y_lp_filters_map.find(s.m_object_id);
        auto z_lp_btrw_filter = z_lp_filters_map.find(s.m_object_id);

        rj_sample_out.m_x = x_lp_btrw_filter->second->filter(rj_sample_out.m_x);
        rj_sample_out.m_y = y_lp_btrw_filter->second->filter(rj_sample_out.m_y);
        rj_sample_out.m_z =  z_lp_btrw_filter->second->filter(rj_sample_out.m_z);
        */


        // OUT
        res_s.m_object_id = s.m_object_id;
        res_s.m_time = rj_sample_out.m_time;
        res_s.m_x = rj_sample_out.m_x;
        res_s.m_y = rj_sample_out.m_y;
        res_s.m_z = rj_sample_out.m_z;

        filtered_data_vector->emplace_back(res_s);
    }

    for (auto& rj_filter : rj_filters_map)
    {
        delete rj_filter.second;
    }
    for (auto& f : x_lp_filters_map)
    {
        delete f.second;
    }
    for (auto& f : y_lp_filters_map)
    {
        delete f.second;
    }
    for (auto& f : z_lp_filters_map)
    {
        delete f.second;
    }
}

void test_winkam_butter_lpf(const std::vector<XYZ_Sample>& input_raw_data_vector
                            , std::vector<XYZ_Sample>* filtered_data_vector
                            , const QString& parameters) noexcept
{
    bool if_rj_used = false;
    int filter_order;
    int cutoff_freq;
    double filter_gain = 0.0;
    std::vector<double> coeffs_A;
    std::vector<double> coeffs_B;

    if (parameters.split(" ").length() >= 2)
    {
        filter_order = parameters.split(" ").at(0).toInt();
        cutoff_freq = parameters.split(" ").at(1).toInt();
        if (parameters.contains("use_rj_filter"))
        {
            if_rj_used = true;
        }
        qDebug() << "Butter order" << filter_order << "cutoff" << cutoff_freq << "rj_filter" << if_rj_used;
    }
    else
    {
        qDebug() << "error parameters";
        return;
    }

    switch (cutoff_freq)
    {
    case 5:
        switch (filter_order)
        {
        case 2:
            coeffs_A = {1.0, 2.0, 1.0};
            coeffs_B = {1.6329931619, -0.6905989232};
            filter_gain = 69.43749902;
            break;
        case 4:
            coeffs_A = {1.0, 4.0, 6.0, 4.0, 1.0};
            coeffs_B = {3.3168079106, -4.1742455501, 2.3574027806, -0.5033753607};
            filter_gain = 4691.779923;
        default:
            break;
        }
        break;

    case 10:
        switch (filter_order)
        {
        case 2:
            coeffs_A = {1.0, 2.0, 1.0};
            coeffs_B = {1.2796324250, -0.4775922501};
            filter_gain = 20.20612010;
            break;
        case 4:
            coeffs_A = {1.0, 4.0, 6.0, 4.0, 1.0};
            coeffs_B = {2.6386277439, -2.7693097862, 1.3392807613, -0.2498216698};
            filter_gain = 388.1333017;
        default:
            break;
        }
        break;

    case 15:
        switch (filter_order)
        {
        case 2:
            coeffs_A = {1.0, 2.0, 1.0};
            coeffs_B = {0.9428090416, -0.3333333333};
            filter_gain = 10.24264069;
            break;
        case 4:
            coeffs_A = {1.0, 4.0, 6.0, 4.0, 1.0};
            coeffs_B = {1.9684277869, -1.7358607092, 0.7244708295, -0.1203895999};
            filter_gain = 97.94817390;
        default:
            break;
        }
        break;

    case 20:
        switch (filter_order)
        {
        case 2:
            coeffs_A = {1.0, 2.0, 1.0};
            coeffs_B = {0.6202041029, -0.2404082058};
            filter_gain = 6.449489743;
            break;
        case 4:
            coeffs_A = {1.0, 4.0, 6.0, 4.0, 1.0};
            coeffs_B = {1.3066051440, -1.0304538354, 0.3623690447, -0.0557639007};
            filter_gain = 38.34690819;
        default:
            break;
        }
        break;

    default:
        return;
    }

    WKTR::Position_Sample rj_sample_in, rj_sample_out;
    WKRJ::Random_Jump_Filter* rj_filter_ptr;
    WKTR::Butterworth_LPF* x_lp_btrw_filter_ptr;
    WKTR::Butterworth_LPF* y_lp_btrw_filter_ptr;
    WKTR::Butterworth_LPF* z_lp_btrw_filter_ptr;
    XYZ_Sample res_s;

    std::unordered_map<uint64_t, WKRJ::Random_Jump_Filter*> rj_filters_map;
    std::unordered_map<uint64_t, WKTR::Butterworth_LPF*> x_lp_btrw_filters_map;
    std::unordered_map<uint64_t, WKTR::Butterworth_LPF*> y_lp_btrw_filters_map;
    std::unordered_map<uint64_t, WKTR::Butterworth_LPF*> z_lp_btrw_filters_map;

    for (const XYZ_Sample& s : input_raw_data_vector)
    {
        if (rj_filters_map.find(s.m_object_id) == rj_filters_map.end()) // no filter found
        {
            rj_filter_ptr = new WKRJ::Random_Jump_Filter;
            rj_filters_map.insert(std::make_pair(s.m_object_id, rj_filter_ptr));

            x_lp_btrw_filter_ptr = new WKTR::Butterworth_LPF;
            x_lp_btrw_filter_ptr->set_coefficients(coeffs_A, coeffs_B, filter_gain);
            x_lp_btrw_filters_map.insert(std::make_pair(s.m_object_id, x_lp_btrw_filter_ptr));

            y_lp_btrw_filter_ptr = new WKTR::Butterworth_LPF;
            y_lp_btrw_filter_ptr->set_coefficients(coeffs_A, coeffs_B, filter_gain);
            y_lp_btrw_filters_map.insert(std::make_pair(s.m_object_id, y_lp_btrw_filter_ptr));

            z_lp_btrw_filter_ptr = new WKTR::Butterworth_LPF;
            z_lp_btrw_filter_ptr->set_coefficients(coeffs_A, coeffs_B, filter_gain);
            z_lp_btrw_filters_map.insert(std::make_pair(s.m_object_id, z_lp_btrw_filter_ptr));
        }

        rj_sample_in.m_time = s.m_time;
        rj_sample_in.m_x = s.m_x;
        rj_sample_in.m_y = s.m_y;
        rj_sample_in.m_z = s.m_z;
        auto rj_filter = rj_filters_map.find(s.m_object_id);
        rj_sample_out = rj_filter->second->process_position_sample(rj_sample_in, true);

        auto x_lp_btrw_filter = x_lp_btrw_filters_map.find(s.m_object_id);
        auto y_lp_btrw_filter = y_lp_btrw_filters_map.find(s.m_object_id);
        auto z_lp_btrw_filter = z_lp_btrw_filters_map.find(s.m_object_id);

        // if we don't use rj_filter we use just raw data
        if (!if_rj_used)
        {
            rj_sample_out = rj_sample_in;
        }

        rj_sample_out.m_x = x_lp_btrw_filter->second->filter(rj_sample_out.m_x);
        rj_sample_out.m_y = y_lp_btrw_filter->second->filter(rj_sample_out.m_y);
        rj_sample_out.m_z = z_lp_btrw_filter->second->filter(rj_sample_out.m_z);

        res_s.m_object_id = s.m_object_id;
        res_s.m_time = rj_sample_out.m_time;
        res_s.m_x = rj_sample_out.m_x;
        res_s.m_y = rj_sample_out.m_y;
        res_s.m_z = rj_sample_out.m_z;

        filtered_data_vector->emplace_back(res_s);
    }

    for (auto& rj_filter : rj_filters_map)
    {
        delete rj_filter.second;
    }
    for (auto& f : x_lp_btrw_filters_map)
    {
        delete f.second;
    }
    for (auto& f : y_lp_btrw_filters_map)
    {
        delete f.second;
    }
    for (auto& f : z_lp_btrw_filters_map)
    {
        delete f.second;
    }
}

void test_winkam_ddsr(const std::vector<XYZ_Sample>& input_raw_data_vector
                      , std::vector<XYZ_Sample>* filtered_data_vector
                      , const QString& parameters) noexcept
{
    double lpf_basic_coef;
    double lpf_additional_coef;
    int stair_srart_threshold;
    int stair_length;

    if (parameters.split(" ").length() == 4)
    {
        lpf_basic_coef          = parameters.split(" ").at(0).toDouble();
        lpf_additional_coef     = parameters.split(" ").at(1).toDouble();
        stair_srart_threshold   = parameters.split(" ").at(2).toInt();
        stair_length            = parameters.split(" ").at(3).toInt();
        qDebug() << "basic coef:" << lpf_basic_coef
                 << "additional coef:" << lpf_additional_coef
                 << "start threshold:" << stair_srart_threshold
                 << "stair length:" << stair_length;
        std::cout << "basic coef: " << lpf_basic_coef
                  << ", additional coef: " << lpf_additional_coef
                  << ", start threshold: " << stair_srart_threshold
                  << ", stair length: " << stair_length << std::endl;
    }
    else
    {
        qDebug() << "error parameters";
        return;
    }

    WKTR::Position_Sample ddsr_sample_in, ddsr_sample_out;

    WKRJ::DDSR_Filter* x_ddsr_filter_ptr;
    WKRJ::DDSR_Filter* y_ddsr_filter_ptr;
    WKRJ::DDSR_Filter* z_ddsr_filter_ptr;
    XYZ_Sample res_s;

    std::unordered_map<uint64_t, WKRJ::DDSR_Filter*> x_ddsr_filters_map;
    std::unordered_map<uint64_t, WKRJ::DDSR_Filter*> y_ddsr_filters_map;
    std::unordered_map<uint64_t, WKRJ::DDSR_Filter*> z_ddsr_filters_map;

    for (const XYZ_Sample& s : input_raw_data_vector)
    {
        if (x_ddsr_filters_map.find(s.m_object_id) == x_ddsr_filters_map.end()) // no filter found
        {
            x_ddsr_filter_ptr = new WKRJ::DDSR_Filter(/*lpf_basic_coef, lpf_additional_coef, stair_srart_threshold, stair_length*/);
            x_ddsr_filters_map.insert({s.m_object_id, x_ddsr_filter_ptr});

            y_ddsr_filter_ptr = new WKRJ::DDSR_Filter(/*lpf_basic_coef, lpf_additional_coef, stair_srart_threshold, stair_length*/);
            y_ddsr_filters_map.insert({s.m_object_id, y_ddsr_filter_ptr});

            z_ddsr_filter_ptr = new WKRJ::DDSR_Filter(/*lpf_basic_coef, lpf_additional_coef, stair_srart_threshold, stair_length*/);
            z_ddsr_filters_map.insert({s.m_object_id, z_ddsr_filter_ptr});
        }

        ddsr_sample_in.m_time = s.m_time;
        ddsr_sample_in.m_x = s.m_x;
        ddsr_sample_in.m_y = s.m_y;
        ddsr_sample_in.m_z = s.m_z;

        auto ddsr_filter_z = z_ddsr_filters_map.find(s.m_object_id);
        ddsr_sample_out.m_z = ddsr_filter_z->second->process_point(ddsr_sample_in.m_z, false);

        auto ddsr_filter_x = x_ddsr_filters_map.find(s.m_object_id);
        ddsr_sample_out.m_x = ddsr_filter_x->second->process_point(ddsr_sample_in.m_x, ddsr_filter_z->second->get_stair_filter_start_flag());

        auto ddsr_filter_y = y_ddsr_filters_map.find(s.m_object_id);
        ddsr_sample_out.m_y = ddsr_filter_y->second->process_point(ddsr_sample_in.m_y, ddsr_filter_z->second->get_stair_filter_start_flag());

        res_s.m_object_id = s.m_object_id;
        res_s.m_time = s.m_time;
        res_s.m_x = ddsr_sample_out.m_x;
        res_s.m_y = ddsr_sample_out.m_y;
        res_s.m_z = ddsr_sample_out.m_z;

        filtered_data_vector->emplace_back(res_s);
    }

    for (auto& f : x_ddsr_filters_map)
    {
        delete f.second;
    }
    for (auto& f : y_ddsr_filters_map)
    {
        delete f.second;
    }
    for (auto& f : z_ddsr_filters_map)
    {
        delete f.second;
    }
}

#endif
