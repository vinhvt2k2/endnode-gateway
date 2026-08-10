#ifndef KALMAN_FILTER_H
#define KALMAN_FILTER_H

#include <math.h>

class KalmanFilter {
  private:
    float _err_measure;
    float _err_estimate;
    float _q;
    float _current_estimate;
    float _last_estimate;
    float _kalman_gain;

  public:
    KalmanFilter(float mea_e, float est_e, float q) {
      _err_measure = mea_e;
      _err_estimate = est_e;
      _q = q;
      _current_estimate = 0;
      _last_estimate = 0;
    }

    float update(float mea) {
      _kalman_gain = _err_estimate / (_err_estimate + _err_measure);
      _current_estimate = _last_estimate + _kalman_gain * (mea - _last_estimate);
      _err_estimate = (1.0 - _kalman_gain) * _err_estimate + fabs(_last_estimate - _current_estimate) * _q;
      _last_estimate = _current_estimate;
      return _current_estimate;
    }

    void setInitial(float initial) {
      _last_estimate = initial;
    }
};

#endif