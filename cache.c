#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double get_elapsed(clock_t start, clock_t end) {
    return (double)(end - start) / (double)(CLOCKS_PER_SEC);
}

int main() {
    const int N_SAMPLES = 5;
    const int MIN_SIZE = 1024;
    const int MAX_SIZE = 32 * 1024 * 1024;
    const int MAX_STRIDE = 1024;

    for (int size = MIN_SIZE; size <= MAX_SIZE; size *= 2) {
        for (int stride = 1; stride <= MAX_STRIDE; stride *= 2) {
            double elapsed = 0;

            /* Repeat the measurement N_SAMPLES times and take the average */
            for (int _ = 0; _ < N_SAMPLES; _++) {
                assert(size % stride == 0);
                assert(MAX_SIZE % (size / stride) == 0);

                /*
                Measure time taken for to perform `MAX_SIZE` read/writes on an
                array with length `size` using a stride of `stride`.
                */
                int* arr = malloc(size * sizeof(int));
                clock_t start = clock();
                for (int iters = 0; iters < MAX_SIZE; iters += size / stride)
                    for (int i = 0; i < size; i += stride)
                        arr[i] += 1;
                double elapsed_with_mem_access = get_elapsed(start, clock());
                free(arr);

                /*
                Measure time taken for to perform the same loop as above but
                instead of reading, incrementing, and writing elements of an
                array, we read/inc/write to a variable (that should be stored
                in a register for the duration of the loop).
                */
                int register dummy = 0;
                start = clock();
                for (int iters = 0; iters < MAX_SIZE; iters += size / stride)
                    for (int i = 0; i < size; i += stride)
                        dummy += 1;
                double elapsed_with_reg_access = get_elapsed(start, clock());

                /*
                Since the only difference between the first loop and the second
                loop is the first requires array accesses, the difference in
                their elapsed time should measures the overhead of performing
                memory reads/writes.
                */
                elapsed += elapsed_with_mem_access - elapsed_with_reg_access;
            }
            /*
            Normalize the total time to by the number of N_samples we repeated and
            by the number of total read/writes we performed and convert to ns.
            */
            double ns_per_access = elapsed * 1.0e9 / (double)N_SAMPLES / (double)(MAX_SIZE);

            /*
            Write to stdout in csv format as:

            <size in bytes>,<stride in bytes>,<avg nanoseconds per access>
            */
            printf("%lu,%lu,%f\n", size * sizeof(int), stride * sizeof(int), ns_per_access);
        }
    }
}
