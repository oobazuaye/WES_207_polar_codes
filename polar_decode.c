/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include "polar_codes.h"

void decodePolarData(dataSet * rx_data, bitSet * decoded_bits)
{
    int U_pair_iter = 0;
    int U_iter_inc = 0;
    int num_iter = 0;

    double p0;
    double p1;
    double q0;
    double q1;
    double normalizer;

    decoded_bits->length = rx_data->length;

    for (int i = rx_data->length >> 1; i >= 1; i >>= 1)
    {
        U_pair_iter = 0;
        U_iter_inc = i << 1;
        for (int U_iter = 0; U_iter < rx_data->length; U_iter += U_iter_inc)
        {
            for (U_pair_iter = U_iter; U_pair_iter < U_iter + i; U_pair_iter++)
            {
                p0 = rx_data->probabilities[U_pair_iter].prob0;
                p1 = rx_data->probabilities[U_pair_iter].prob1;
                q0 = rx_data->probabilities[U_pair_iter+i].prob0;
                q1 = rx_data->probabilities[U_pair_iter+i].prob1;

                rx_data->probabilities[U_pair_iter].prob0 = (p0 * q0) + (p1 * q1);
                rx_data->probabilities[U_pair_iter].prob1 = (p0 * q1) + (p1 * q0);
#ifdef DEBUG
                printf("top bit (index %d): p(0) = %g, p(1) = %g\n",
                       U_pair_iter,
                       rx_data->probabilities[U_pair_iter].prob0,
                       rx_data->probabilities[U_pair_iter].prob1);
#endif
                if (rx_data->probabilities[U_pair_iter].prob0 > rx_data->probabilities[U_pair_iter].prob1)
                {
#ifdef DEBUG
                printf("selecting branch 0 for top bit, index %d\n", U_pair_iter);
#endif

#ifdef NORMALIZE
                    normalizer = rx_data->probabilities[U_pair_iter].prob0;
#else
                    normalizer = 1.0;
#endif
                    rx_data->probabilities[U_pair_iter+i].prob0 = (p0 * q0)/normalizer;
                    rx_data->probabilities[U_pair_iter+i].prob1 = (p1 * q1)/normalizer;
#ifdef DEBUG
                printf("bottom bit (index %d, unnormalized): p(0) = %g, p(1) = %g\n",
                       U_pair_iter+i,
                       (p0 * q0),
                       (p1 * q1));
#endif
                }
                else
                {
#ifdef DEBUG
                printf("selecting branch 1 for top bit, index %d\n", U_pair_iter);
#endif

#ifdef NORMALIZE
                    normalizer = rx_data->probabilities[U_pair_iter].prob1;
#else
                    normalizer = 1.0;
#endif
                    rx_data->probabilities[U_pair_iter+i].prob0 = (p1 * q0)/normalizer;
                    rx_data->probabilities[U_pair_iter+i].prob1 = (p0 * q1)/normalizer;
#ifdef DEBUG
                printf("bottom bit (index %d, unnormalized): p(0) = %g, p(1) = %g\n",
                       U_pair_iter+i,
                       (p1 * q0),
                       (p0 * q1));
#endif
                }
                rx_data->probabilities[U_pair_iter].prob0 /= normalizer;
                rx_data->probabilities[U_pair_iter].prob1 /= normalizer;
#ifdef DEBUG
                printf("bit %d, normalized: p(0) = %g, p(1) = %g\n"
                       "bit %d, normalized: p(0) = %g, p(1) = %g\n",
                       U_pair_iter,
                       rx_data->probabilities[U_pair_iter].prob0,
                       rx_data->probabilities[U_pair_iter].prob1,
                       U_pair_iter+i,
                       rx_data->probabilities[U_pair_iter+i].prob0,
                       rx_data->probabilities[U_pair_iter+i].prob1);
#endif
                num_iter++;
            }
        }
    }

#ifdef DEBUG
    printDataSet(rx_data);
#endif

    for (int i = 0; i < rx_data->length; i++)
    {
        decoded_bits->is_frozen[i] = rx_data->is_frozen[i];
        if (rx_data->is_frozen[i] == true)
        {
            decoded_bits->bits[i] = 0;
        }
        else if (rx_data->probabilities[i].prob0 > rx_data->probabilities[i].prob1)
        {
            decoded_bits->bits[i] = 0;
        }
        else
        {
            decoded_bits->bits[i] = 1;
        }
    }

}

void decodePolarDataStep(dataSet * rx_data, decodeIterator loop_position, bitSet * decoded_bits)
{
    double p0;
    double p1;
    double q0;
    double q1;
    double normalizer;

    int i = loop_position.branch_num;
    int U_pair_iter = loop_position.upper_bit;
#ifdef DEBUG
                printf("XORing index %d into index %d\n", U_pair_iter + i, U_pair_iter);
#endif
    p0 = rx_data->probabilities[U_pair_iter].prob0;
    p1 = rx_data->probabilities[U_pair_iter].prob1;
    q0 = rx_data->probabilities[U_pair_iter+i].prob0;
    q1 = rx_data->probabilities[U_pair_iter+i].prob1;

    rx_data->probabilities[U_pair_iter].prob0 = (p0 * q0) + (p1 * q1);
    rx_data->probabilities[U_pair_iter].prob1 = (p0 * q1) + (p1 * q0);

    if (rx_data->probabilities[U_pair_iter].prob0 > rx_data->probabilities[U_pair_iter].prob1)
    {
        normalizer = rx_data->probabilities[U_pair_iter].prob0;
        rx_data->probabilities[U_pair_iter+i].prob0 = (p0 * q0)/normalizer;
        rx_data->probabilities[U_pair_iter+i].prob1 = (p1 * q1)/normalizer;
    }
    else
    {
        normalizer = rx_data->probabilities[U_pair_iter].prob1;
        rx_data->probabilities[U_pair_iter+i].prob0 = (p1 * q0)/normalizer;
        rx_data->probabilities[U_pair_iter+i].prob1 = (p0 * q1)/normalizer;
    }
    rx_data->probabilities[U_pair_iter].prob0 /= normalizer;
    rx_data->probabilities[U_pair_iter].prob1 /= normalizer;
}

int findListMaxLikelihoodPath(int bit_idx, decodeList * decode_list)
{
    int max_idx = 0;
    double max_prob_val = 0.0;

    for (int i = 0; i < decode_list->list_size; i++)
    {
        if (decode_list->path_active[bit_idx][i] == true)
        {
            if (probPairMaxValue(decode_list->path_list[bit_idx][i].probabilities) > max_prob_val)
            {
                max_idx = i;
                max_prob_val = probPairMaxValue(decode_list->path_list[bit_idx][i].probabilities);
            }
        }
    }
    return max_idx;
}

int findListMinLikelihoodPath(int bit_idx, decodeList * decode_list)
{
    int min_idx = 0;
    double min_prob_val = 200.0;

    for (int i = 0; i < decode_list->list_size; i++)
    {
        if (decode_list->path_active[bit_idx][i] == true)
        {
            if (probPairMinValue(decode_list->path_list[bit_idx][i].probabilities) < min_prob_val)
            {
                min_idx = i;
                min_prob_val = probPairMinValue(decode_list->path_list[bit_idx][i].probabilities);
            }
        }
    }
    return min_idx;
}

void removePath(int bit_idx, int path_idx, decodeList * decode_list)
{
    if (decode_list->path_active[bit_idx][path_idx] == true)
    {
        decode_list->path_active[bit_idx][path_idx] = false;
        decode_list->paths_in_use[bit_idx]--;
    }
}

// function for adding a dataSet struct to a decodeList
void addToDecodeList(dataSet * rx_data, int bit_idx, int path_idx, decodeList * decode_list)
{
    int num_paths = decode_list->paths_in_use[bit_idx];
    int available_path_idx = 0;
    //int worst_path_idx = 0;
    if (num_paths < decode_list->list_size)
    {
        // find index of first open path
        for (int i = 0; i < decode_list->list_size; i++)
        {
            if (decode_list->path_active[bit_idx][i] == false)
            {
                available_path_idx = i;
                break;
            }
        }
        copyDataSet(&(decode_list->path_list[bit_idx][available_path_idx]), rx_data);
        decode_list->path_active[bit_idx][available_path_idx] = true;
        decode_list->paths_in_use[bit_idx]++;
    }
    else
    {
        // if there aren't any open paths, just overwrite the path given
        // in the arguments (should be the parent path)
        removePath(bit_idx, path_idx, decode_list);
        copyDataSet(&(decode_list->path_list[bit_idx][path_idx]), rx_data);
        decode_list->path_active[bit_idx][path_idx] = true;
        decode_list->paths_in_use[bit_idx]++;
    }
}

// add a dataSet to the decoding list
// bit_idx is which XOR bit ("a") i'm in
// bit_pair_idx is which paired bit ("b") i'm going to split paths of
void splitPathAndAddToDecodeList(int path_idx, int bit_idx, int bit_pair_idx, probSet * bit_pair_probs, decodeList * decode_list)
{
    double p0;
    double p1;
    double q0;
    double q1;
    double a0;
    double a1;
    double b_prob0_a0;
    double b_prob1_a0;
    double b_prob0_a1;
    double b_prob1_a1;
    dataSet * bit_pair_path;
    double normalizer;
    bool a0_greaterthan_a1 = false;
    static bool path_active_copy[MAX_NUM_PATHS];
    static dataSet new_path;

    dataSet * rx_data = &(decode_list->path_list[bit_idx][path_idx]);
    new_path.length = rx_data->length;

    // get probabilities from top bit
    p0 = rx_data->probabilities[bit_idx].prob0;
    p1 = rx_data->probabilities[bit_idx].prob1;
#ifdef DEBUG
    printf("TOP BIT (bit_idx %d, path_idx %d):\n", bit_idx, path_idx);
    printDataSet(rx_data);
    printf("\n");
#endif
    // we expect every bit should have at least 1 path. we copy the active paths
    // so that we only iterate over the ones that are active when we entered the function
    copyPathActiveArray(path_active_copy,
                        decode_list->path_active[bit_pair_idx],
                        decode_list->list_size);
    for (int curr_path = 0; curr_path < bit_pair_probs->length; curr_path++)
    {

#ifdef DEBUG
            printf("index %d (branch width %d), path %d!\n",
                   bit_idx,
                   abs(bit_idx - bit_pair_idx),
                   curr_path);
#endif
            bit_pair_path = &(decode_list->path_list[bit_pair_idx][curr_path]);
#ifdef DEBUG
            printf("BOTTOM BIT (bit_idx %d, path_idx %d):\n", bit_pair_idx, curr_path);
            //printDataSet(bit_pair_path);
#endif
            q0 = bit_pair_probs->probabilities[curr_path].prob0;
            q1 = bit_pair_probs->probabilities[curr_path].prob1;
#ifdef DEBUG
            printf("\tp0 = %g, p1 = %g, q0 = %g, q1 = %g\n",
                   p0,
                   p1,
                   q0,
                   q1);
            printf("\n");
#endif
            a0 = (p0 * q0) + (p1 * q1);
            a1 = (p0 * q1) + (p1 * q0);

            if (a0 > a1)
            {
                a0_greaterthan_a1 = true;
        #ifdef NORMALIZE
                normalizer = a0;
        #else
                normalizer = 1.0;
        #endif
            }
            else
            {
                a0_greaterthan_a1 = false;
        #ifdef NORMALIZE
                normalizer = a1;
        #else
                normalizer = 1.0;
        #endif
            }

            // normalize probabilities for a, the top bit
            rx_data->probabilities[bit_idx].prob0 = a0/normalizer;
            rx_data->probabilities[bit_idx].prob1 = a1/normalizer;
            //bit_pair_path->probabilities[bit_idx].prob0 = a0/normalizer;
            //bit_pair_path->probabilities[bit_idx].prob1 = a1/normalizer;


            // copy the newly calculated values into the path array for the top bit
            if (decode_list->paths_in_use[bit_idx] < decode_list->list_size)
            {
                addToDecodeList(rx_data, bit_idx, path_idx, decode_list);
                removePath(bit_idx, path_idx, decode_list);
            }
            else
            {
                addToDecodeList(rx_data, bit_idx, path_idx, decode_list);
            }


            // calculate and normalize probabilities for all possible branches of b;
            b_prob0_a0 = (p0 * q0)/normalizer;
            b_prob1_a0 = (p1 * q1)/normalizer;
            b_prob0_a1 = (p1 * q0)/normalizer;
            b_prob1_a1 = (p0 * q1)/normalizer;
#ifdef DEBUG
            printf("\tbit_idx %d, path %d: a0 = %g, a1 = %g\n",
                   bit_idx,
                   path_idx,
                   a0,
                   a1);
            printf("\tbit_pair_idx %d, path %d: b_prob0_a0 = %g, b_prob1_a0 = %g\n",
                   bit_pair_idx,
                   curr_path,
                   b_prob0_a0,
                   b_prob1_a0);
            printf("\tbit_pair_idx %d, path %d: b_prob0_a1 = %g, b_prob1_a1 = %g\n",
                   bit_pair_idx,
                   curr_path,
                   b_prob0_a1,
                   b_prob1_a1);
#endif
            if (decode_list->paths_in_use[bit_pair_idx] == decode_list->list_size)
            {
                if (a0_greaterthan_a1)
                {
                    // split off the path for when the top bit goes with 0
                    new_path.probabilities[bit_pair_idx].prob0 = b_prob0_a0;
                    new_path.probabilities[bit_pair_idx].prob1 = b_prob1_a0;
                    addToDecodeList(&new_path, bit_pair_idx, curr_path, decode_list);
                    /*
                    bit_pair_path->probabilities[bit_pair_idx].prob0 = b_prob0_a0;
                    bit_pair_path->probabilities[bit_pair_idx].prob1 = b_prob1_a0;
                    */
                }
                else
                {
                    // split off the path for when the top bit goes with 1
                    new_path.probabilities[bit_pair_idx].prob0 = b_prob0_a1;
                    new_path.probabilities[bit_pair_idx].prob1 = b_prob1_a1;
                    addToDecodeList(bit_pair_path, bit_pair_idx, curr_path, decode_list);
                    /*
                    bit_pair_path->probabilities[bit_pair_idx].prob0 = b_prob0_a1;
                    bit_pair_path->probabilities[bit_pair_idx].prob1 = b_prob1_a1;
                    */
                }
                //addToDecodeList(bit_pair_path, bit_pair_idx, curr_path, decode_list);
                addToDecodeList(&new_path, bit_pair_idx, curr_path, decode_list);
            }
            else
            {
                // split off the path for when the top bit goes with 0
                new_path.probabilities[bit_pair_idx].prob0 = b_prob0_a0;
                new_path.probabilities[bit_pair_idx].prob1 = b_prob1_a0;
                addToDecodeList(&new_path, bit_pair_idx, curr_path, decode_list);
                /*
                bit_pair_path->probabilities[bit_pair_idx].prob0 = b_prob0_a0;
                bit_pair_path->probabilities[bit_pair_idx].prob1 = b_prob1_a0;

                addToDecodeList(bit_pair_path, bit_pair_idx, curr_path, decode_list);
                */

                // split off the path for when the top bit goes with 1
                new_path.probabilities[bit_pair_idx].prob0 = b_prob0_a0;
                new_path.probabilities[bit_pair_idx].prob1 = b_prob1_a0;
                addToDecodeList(&new_path, bit_pair_idx, curr_path, decode_list);
                /*
                bit_pair_path->probabilities[bit_pair_idx].prob0 = b_prob0_a1;
                bit_pair_path->probabilities[bit_pair_idx].prob1 = b_prob1_a1;
                */
                /*
                if (decode_list->paths_in_use[bit_pair_idx] < decode_list->list_size)
                {
                    addToDecodeList(bit_pair_path, bit_pair_idx, curr_path, decode_list);
                    removePath(bit_pair_idx, curr_path, decode_list);
                }
                else
                {
                    addToDecodeList(bit_pair_path, bit_pair_idx, curr_path, decode_list);
                }
                */
            }
    }
}

void decodeListPolarData(dataSet * rx_data, bitSet * decoded_bits, int list_size)
{
    int U_pair_iter = 0;
    int U_iter_inc = 0;

    int best_path_idx = 0;
    static bool path_active_copy[MAX_NUM_PATHS];
    static probSet bit_pair_probs;

    decodeList decode_list;
    initDecodeList(rx_data, list_size, &decode_list);

    //dataSet * current_path;

    decoded_bits->length = rx_data->length;

    for (int i = rx_data->length >> 1; i >= 1; i >>= 1)
    {
        U_pair_iter = 0;
        U_iter_inc = i << 1;
#ifdef DEBUG
        printf("i = %d\n", i);
#endif
        for (int U_iter = 0; U_iter < rx_data->length; U_iter += U_iter_inc)
        {
#ifdef DEBUG
            printf("\tU_iter = %d\n", U_iter);
#endif
            for (U_pair_iter = U_iter; U_pair_iter < U_iter + i; U_pair_iter++)
            {
#ifdef DEBUG
                printf("\t\tU_pair_iter = %d\n", U_pair_iter);
                printf("XORing index %d into index %d\n", U_pair_iter + i, U_pair_iter);
#endif

                if (decode_list.paths_in_use[U_pair_iter] == 0)
                {
#ifdef DEBUG
                    printf("SHOUD NEVER HAPPEN index %d (branch width %d) has no paths in use!\n", U_pair_iter, i);
#endif
                    splitPathAndAddToDecodeList(0, U_pair_iter, U_pair_iter+i, &bit_pair_probs, &decode_list);
                }
                else
                {
                    copyPathActiveArray(path_active_copy,
                                        decode_list.path_active[U_pair_iter],
                                        decode_list.list_size);
                    dumpPathsToProbSetArrayAndClear(&decode_list, U_pair_iter+i, &bit_pair_probs);
                    for (int path_idx = 0; path_idx < decode_list.list_size; path_idx++)
                    {
                        // explore the path if it's active
                        if (path_active_copy[path_idx] == true)
                        {
#ifdef DEBUG
                            printf("index %d (branch width %d) has path %d active!\n", U_pair_iter, i, path_idx);
#endif
                            //current_path = &(decode_list.path_list[U_pair_iter][path_idx]);
                            splitPathAndAddToDecodeList(path_idx, U_pair_iter, U_pair_iter+i, &bit_pair_probs, &decode_list);
                            //removePath(U_pair_iter, path_idx, &decode_list);
                        }
                    }
                }
            }
        }
#ifdef DEBUG
        printf("\n");
#endif
    }

    for (int i = 0; i < rx_data->length; i++)
    {
        decoded_bits->is_frozen[i] = rx_data->is_frozen[i];
        if (rx_data->is_frozen[i] == true)
        {
            decoded_bits->bits[i] = 0;
        }
        else
        {
            best_path_idx = findListMaxLikelihoodPath(i, &decode_list);
            rx_data->probabilities[i].prob0 = decode_list.path_list[i][best_path_idx].probabilities[i].prob0;
            rx_data->probabilities[i].prob1 = decode_list.path_list[i][best_path_idx].probabilities[i].prob1;
            if (decode_list.path_list[i][best_path_idx].probabilities[i].prob0 >
                decode_list.path_list[i][best_path_idx].probabilities[i].prob1)
            {
                decoded_bits->bits[i] = 0;
            }
            else
            {
                decoded_bits->bits[i] = 1;
            }
        }
    }
#ifdef DEBUG
    printDataSet(rx_data);
#endif
}






