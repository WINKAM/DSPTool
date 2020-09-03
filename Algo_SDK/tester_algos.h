/***************************************************************************

             WINKAM TM strictly confidential 11.10.2017

 ***************************************************************************/
#ifndef TESTER_ALGOS_H
#define TESTER_ALGOS_H

#include <vector>
#include "data/sample_structuries.h"
#include "data/label/log_labels.h"
#include "data/data_manager.h"

class QString;

void write_stretch_compressed(const std::vector<XYZ_Sample> &input_raw_data_vector, const QString &parameters) noexcept;

#endif // TESTER_ALGOS_H
