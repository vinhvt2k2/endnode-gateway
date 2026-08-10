#ifndef VN_AQI_H
#define VN_AQI_H
#include <Arduino.h>
#include <math.h>

class VN_AQI {
  public:
    int calcAQI(float C, float *BP, int *I, int size) {
        for (int i = 0; i < size - 1; i++) {
            if (C >= BP[i] && C <= BP[i+1]) {
                return round(((float)(I[i+1] - I[i]) / (BP[i+1] - BP[i])) * (C - BP[i]) + I[i]);
            }
        }
        return 500; 
    }

    float calcNowcast(float c[12]) {
        int valid_early_count = 0;
        for (int i = 0; i < 3; i++) {
            if (c[i] >= 0.0) valid_early_count++;
        }
        if (valid_early_count < 2) return -1.0; 

        float c_min = 999999.0;
        float c_max = -1.0;

        for (int i = 0; i < 12; i++) {
            if (c[i] >= 0.0) { 
                if (c[i] < c_min) c_min = c[i];
                if (c[i] > c_max) c_max = c[i];
            }
        }

        float w_star = 0.0;
        if (c_max > 0.0) w_star = c_min / c_max;
        float w = (w_star <= 0.5) ? 0.5 : w_star;

        float numerator = 0.0;
        float denominator = 0.0;

        for (int i = 0; i < 12; i++) {
            if (c[i] >= 0.0) { 
                float weight = pow(w, i); 
                numerator += weight * c[i];
                denominator += weight;
            }
        }

        if (denominator == 0.0) return -1.0;
        return numerator / denominator;
    }

    int getTotalAQI(float pm25_12h[12], float pm10_12h[12], float co_1h_max) {
        float nowcast_pm25 = calcNowcast(pm25_12h);
        float nowcast_pm10 = calcNowcast(pm10_12h);
        float co_ug = co_1h_max * 1000.0; // Đổi mg/m3 sang µg/m3

        float bp_pm25[] = {0, 25, 50, 80, 150, 250, 350, 500};
        float bp_pm10[] = {0, 50, 150, 250, 350, 420, 500, 600};
        float bp_co[] = {0, 10000, 30000, 45000, 60000, 90000, 120000, 150000};
        int i_val[] = {0, 50, 100, 150, 200, 300, 400, 500};

        int aqi_pm25 = -1, aqi_pm10 = -1, aqi_co = -1;

        if (nowcast_pm25 >= 0) aqi_pm25 = calcAQI(nowcast_pm25, bp_pm25, i_val, 8);
        if (nowcast_pm10 >= 0) aqi_pm10 = calcAQI(nowcast_pm10, bp_pm10, i_val, 8);
        if (co_ug >= 0) aqi_co = calcAQI(co_ug, bp_co, i_val, 8);

        int max_aqi = aqi_pm25;
        if (aqi_pm10 > max_aqi) max_aqi = aqi_pm10;
        if (aqi_co > max_aqi) max_aqi = aqi_co;

        return max_aqi;
    }
};
#endif