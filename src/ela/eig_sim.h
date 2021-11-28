#ifndef eig_sim_h
#define eig_sim_h

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

void eig_sym(float* A, uint16_t row, float d[]);

#ifdef __cplusplus
}
#endif

#endif /* eig_sim_h */
