#ifndef NOISE_H
#define NOISE_H
#include <math.h>

#define  NOISE_A1  (-3.969683028665376e+01)
#define  NOISE_A2   2.209460984245205e+02
#define  NOISE_A3  (-2.759285104469687e+02)
#define  NOISE_A4   1.383577518672690e+02
#define  NOISE_A5  (-3.066479806614716e+01)
#define  NOISE_A6   2.506628277459239e+00

#define  NOISE_B1  (-5.447609879822406e+01)
#define  NOISE_B2   1.615858368580409e+02
#define  NOISE_B3  (-1.556989798598866e+02)
#define  NOISE_B4   6.680131188771972e+01
#define  NOISE_B5  (-1.328068155288572e+01)

#define  NOISE_C1  (-7.784894002430293e-03)
#define  NOISE_C2  (-3.223964580411365e-01)
#define  NOISE_C3  (-2.400758277161838e+00)
#define  NOISE_C4  (-2.549732539343734e+00)
#define  NOISE_C5   4.374664141464968e+00
#define  NOISE_C6   2.938163982698783e+00

#define  NOISE_D1   7.784695709041462e-03
#define  NOISE_D2   3.224671290700398e-01
#define  NOISE_D3   2.445134137142996e+00
#define  NOISE_D4   3.754408661907416e+00

#define NOISE_P_LOW   0.02425
/* NOISE_P_high = 1 - p_low*/
#define NOISE_P_HIGH  0.97575

    template <typename T>
T normsinv(T p)
{
    T x;
    T q, r, u, e;
    if ((0 < p )  && (p < NOISE_P_LOW)){
        q = sqrt(-2*log(p));
        x = (((((NOISE_C1*q+NOISE_C2)*q+NOISE_C3)*q+NOISE_C4)*q+NOISE_C5)*q+NOISE_C6) / ((((NOISE_D1*q+NOISE_D2)*q+NOISE_D3)*q+NOISE_D4)*q+1);
    }
    else{
        if ((NOISE_P_LOW <= p) && (p <= NOISE_P_HIGH)){
            q = p - 0.5;
            r = q*q;
            x = (((((NOISE_A1*r+NOISE_A2)*r+NOISE_A3)*r+NOISE_A4)*r+NOISE_A5)*r+NOISE_A6)*q /(((((NOISE_B1*r+NOISE_B2)*r+NOISE_B3)*r+NOISE_B4)*r+NOISE_B5)*r+1);
        }
        else{
            if ((NOISE_P_HIGH < p)&&(p < 1)){
                q = sqrt(-2*log(1-p));
                x = -(((((NOISE_C1*q+NOISE_C2)*q+NOISE_C3)*q+NOISE_C4)*q+NOISE_C5)*q+NOISE_C6) / ((((NOISE_D1*q+NOISE_D2)*q+NOISE_D3)*q+NOISE_D4)*q+1);
            }
        }
    }

    /* If you are compiling this under UNIX OR LINUX, you may uncomment this block for better accuracy.
       if(( 0 < p)&&(p < 1)){
       e = 0.5 * erfc(-x/sqrt(2)) - p;
       u = e * sqrt(2*M_NOISE_PI) * exp(x*x/2);
       x = x - u/(1 + x*u/2);
       }
       */

    return x;
}

#endif
