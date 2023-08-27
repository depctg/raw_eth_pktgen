#ifndef _INTERNAL_H__
#define _INTERNAL_H__

#include <vector>
#include "simple_time.hpp"

typedef size_t size_type;

template<typename T>
std::vector<T>& get_column(const char * name);
template<typename T>
void load_column(const char *name, std::vector<T> &&vec);


void * load_data();
extern template std::vector<char>& get_column<char>(const char *name);
extern template std::vector<short>& get_column<short>(const char *);
extern template std::vector<int>& get_column<int>(const char *name);
extern template std::vector<uint64_t>& get_column<uint64_t>(const char *name);
extern template std::vector<double>& get_column<double>(const char *name);
extern template std::vector<SimpleTime>& get_column<SimpleTime>(const char *);

extern std::vector<size_t>& get_index();

extern template void
load_column<uint64_t>(const char *name, std::vector<uint64_t> &&vec);
extern template void
load_column<double>(const char *name, std::vector<double> &&vec);

template<typename T>
static void inline sel_copy (std::vector<T> &dst, std::vector<T> &src,
                             std::vector<size_t> &sel_indices, size_t indices_size) {
	const size_t vec_size = src.size();
    dst.reserve(std::min(sel_indices.size(), vec_size));

  	for (const size_t citer : sel_indices)  {
		const size_t index =
			citer >= 0 ? citer : indices_size + citer;

		if (index < vec_size)
			dst.push_back(src[index]);
		else
			break;
	}
}


extern bool step1_firstTime(int i);

extern void step21_passage_count(int pid);
extern void step21_count_result();

extern void step22_passage_count(int pid);
extern void step22_count_result();

extern bool step4_firstTime(int i);

extern void load_trip_timestamp();

extern void step7_do_process(const char* key_col_name);
#endif
