/***************************************************************************

             WINKAM TM strictly confidential 13.09.2017

 ***************************************************************************/
#ifndef TESTER_FILTERS_H
#define TESTER_FILTERS_H

#include <vector>
#include "data/sample_structuries.h"

class QString;

void test_raw_signal(const std::vector<XYZ_Sample>& input_raw_data_vector
                     , std::vector<XYZ_Sample>* filtered_data_vector) noexcept;

void test_aplf(const std::vector<XYZ_Sample> &input_raw_data_vector
                     , std::vector<XYZ_Sample>* filtered_data_vector) noexcept;

#ifdef ALGO_APL_RJ_DEFINE

void test_winkam_rj_btw(const std::vector<XYZ_Sample> &input_raw_data_vector
                     , std::vector<XYZ_Sample>* filtered_data_vector, const QString &parameters) noexcept;

void test_winkam_butter_lpf(const std::vector<XYZ_Sample> &input_raw_data_vector
                     , std::vector<XYZ_Sample>* filtered_data_vector, const QString &parameters) noexcept;

void test_winkam_ddsr(const std::vector<XYZ_Sample> &input_raw_data_vector
                     , std::vector<XYZ_Sample>* filtered_data_vector, const QString &parameters) noexcept;

#endif

#endif // TESTER_FILTERS_H
