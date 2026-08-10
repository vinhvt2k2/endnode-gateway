#ifndef LINEAR_REGRESSION_MODEL_H
#define LINEAR_REGRESSION_MODEL_H

#define NUM_FEATURES 3

// Feature order used during training:
// [0] aqi_prev_1
// [1] aqi_prev_2
// [2] aqi_prev_3

const float coefficients[NUM_FEATURES] = {1.170060f, -0.232662f, -0.031725f};

const float intercept = 8.782137f;

float predict_aqi(const float features[NUM_FEATURES]) {
    float result = intercept;
    for (int i = 0; i < NUM_FEATURES; i++) {
        result += coefficients[i] * features[i];
    }
    return result;
}

#endif // LINEAR_REGRESSION_MODEL_H
