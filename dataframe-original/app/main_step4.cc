#include <vector>
#include <unordered_set>
#include <chrono>
#include "internal.h"
#include "rvector.h"


template <typename K, typename T, typename F>
void get_data_by_sel (const char *name, F &sel_func, 
                      std::vector<T> &newvec) {
    auto &indices_ = get_index();

    const std::vector<K>    &vec = get_column<K>(name);
    const size_type         idx_s = indices_.size();
    const size_type         col_s = vec.size();
    std::vector<size_type>  col_indices;

    // TODO: measure col_indices size
    // make sure this do not trigger realloc
    // and consider remotelize
    col_indices.reserve(idx_s);
    for (size_type i = 0; i < col_s; ++i)
        if (sel_func (indices_[i], vec[i])) {
            col_indices.push_back(i);
        }
    // DataFrame       df;
    // IndexVecType    new_index;

    // new_index.reserve(col_indices.size());
    // for (const auto citer: col_indices)
    //     new_index.push_back(indices_[citer]);
    // df.load_index(std::move(new_index));

    // for (auto col_citer : column_tb_)  {
    //     sel_load_functor_<size_type, Ts ...>    functor (
    //         col_citer.first.c_str(),
    //         col_indices,
    //         idx_s,
    //         df);

    //     data_[col_citer.second].change(functor);
    // }

    std::vector<size_t> new_index;
    new_index.reserve(col_indices.size());
    for (const auto citer : col_indices)
        new_index.push_back(indices_[citer]);

    std::vector<SimpleTime> new_pickup_datetime;
    std::vector<SimpleTime> &pdt = get_column<SimpleTime>("tpep_pickup_datetime");
    sel_copy(new_pickup_datetime, pdt, col_indices, idx_s);

    std::vector<SimpleTime> new_dropoff_datetime;
    std::vector<SimpleTime> &ddt = get_column<SimpleTime>("tpep_dropoff_datetime");
    sel_copy(new_dropoff_datetime, ddt, col_indices, idx_s);

    std::vector<int> new_psg_count;
    std::vector<int> &psg = get_column<int>("passenger_count");
    sel_copy(new_psg_count, psg, col_indices, idx_s);

    std::vector<double> new_trip_dist;
    std::vector<double> &trip_dist = get_column<double>("trip_distance");
    sel_copy(new_trip_dist, trip_dist, col_indices, idx_s);

    std::vector<double> new_plon;
    std::vector<double> &plon = get_column<double>("pickup_longitude");
    sel_copy(new_plon, plon, col_indices, idx_s);

    std::vector<double> new_plat;
    std::vector<double> &plat = get_column<double>("pickup_latitude");
    sel_copy(new_plat, plat, col_indices, idx_s);

    std::vector<int> new_rate;
    std::vector<int> &rate = get_column<int>("RatecodeID");
    sel_copy(new_rate, rate, col_indices, idx_s);

    std::vector<char> new_flag;
    std::vector<char> &flag = get_column<char>("store_and_fwd_flag");
    sel_copy(new_flag, flag, col_indices, idx_s);

    std::vector<double> new_dlon;
    std::vector<double> &dlon = get_column<double>("dropoff_longitude");
    sel_copy(new_dlon, dlon, col_indices, idx_s);

    std::vector<double> new_dlat;
    std::vector<double> &dlat = get_column<double>("dropoff_latitude");
    sel_copy(new_dlat, dlat, col_indices, idx_s);

    std::vector<int> new_ptype;
    std::vector<int> &payment = get_column<int>("payment_type");
    sel_copy(new_ptype, payment, col_indices, idx_s);

    std::vector<double> new_famount;
    std::vector<double> &fare = get_column<double>("fare_amount");
    sel_copy(new_famount, fare, col_indices, idx_s);

    std::vector<double> new_extra;
    std::vector<double> &extra = get_column<double>("extra");
    sel_copy(new_extra, extra, col_indices, idx_s); 

    std::vector<double> new_mta;
    std::vector<double> &mta = get_column<double>("mta_tax");
    sel_copy(new_mta, mta, col_indices, idx_s);

    std::vector<double> new_tip;
    std::vector<double> &tip = get_column<double>("tip_amount");
    sel_copy(new_tip, tip, col_indices, idx_s);

    std::vector<double> new_tolls;
    std::vector<double> &tolls = get_column<double>("tolls_amount");
    sel_copy(new_tolls, tolls, col_indices, idx_s);

    std::vector<double> new_impv;
    std::vector<double> &impv = get_column<double>("improvement_surcharge");
    sel_copy(new_impv, impv, col_indices, idx_s);

    std::vector<double> new_total;
    std::vector<double> &total = get_column<double>("total_amount");
    sel_copy(new_total, total, col_indices, idx_s);

    // Target column
    // std::vector<int> new_vendor_id;
    std::vector<int> &vids = get_column<int>("VendorID");
    sel_copy(newvec, vids, col_indices, idx_s);
    return;
}

void calculate_distribution_store_and_fwd_flag()
{
    printf("calculate_distribution_store_and_fwd_flag()\n");

    auto sel_N_saff_functor = [&](const uint64_t&, const char& saff) -> bool {
        return saff == 'N';
    };

    std::vector<int> sel_whatever;
    get_data_by_sel<char>("store_and_fwd_flag", sel_N_saff_functor, sel_whatever);

    printf("%f\n", static_cast<double>(sel_whatever.size()) / get_index().size());

    auto sel_Y_saff_functor = [&](const uint64_t&, const char& saff) -> bool {
        return saff == 'Y';
    };

    std::vector<int> sel_vendor_ids;
    get_data_by_sel<char>("store_and_fwd_flag", sel_Y_saff_functor, sel_vendor_ids);

    std::vector<int> unique_vendor_id_vec;
    size_t N = sel_vendor_ids.size();
    for (size_t i = 0; i < N; i++)  {
        int e = sel_vendor_ids[i];
        if (step4_firstTime(e))
            unique_vendor_id_vec.push_back(e);
    }
    printf("{");
    for (auto& vector_id : unique_vendor_id_vec) {
        printf("%d, ", vector_id);
    }
    printf("}\n\n");
}

int main()
{
    std::chrono::time_point<std::chrono::steady_clock> times[10];
    void * df  = load_data();
    times[0] = std::chrono::steady_clock::now();
    calculate_distribution_store_and_fwd_flag();
    times[1] = std::chrono::steady_clock::now();
    printf("Step 4: %ld us\n", 
        std::chrono::duration_cast<std::chrono::microseconds>(times[1] - times[0])
        .count());
}