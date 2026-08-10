#include "SensorManager.h"
#include "Globals.h"
#include "Utils.h"
#include <ArduinoJson.h>
#include <math.h>
#include <linear_regression_model.h>

float getCO_mg_m3(float filtered_adc) {
  float RvRo = filtered_adc * (VOLTAGE_REF / ADC_RESOLUTION);
  float ppm = 3.027 * exp(1.0698 * RvRo);
  return ppm * (28.01 / 24.45);
}

float getUVIndex(float filtered_adc) {
  float voltage = filtered_adc * (VOLTAGE_REF / ADC_RESOLUTION);
  if (voltage < 0.1) return 0.0;
  return voltage / 0.1;
}

void getFilteredDHT22(float &tempOut, float &humOut) {
  float sumTemp = 0.0;
  float sumHum = 0.0;
  int validSamples = 0;

  for (int i = 0; i < 3; i++) {
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (!isnan(t) && !isnan(h)) {
      sumTemp += t;
      sumHum += h;
      validSamples++;
    }

    delay(1000);
  }

  if (validSamples > 0) {
    tempOut = sumTemp / validSamples;
    humOut = sumHum / validSamples;
  } else {
    tempOut = NAN;
    humOut = NAN;
  }
}

void updatePmHistory(float pm25, float pm10) {
  for (int i = 11; i > 0; i--) {
    array_pm25[i] = array_pm25[i - 1];
    array_pm10[i] = array_pm10[i - 1];
  }

  array_pm25[0] = pm25;
  array_pm10[0] = pm10;
}

void updateAqiHistory(int currentAqi) {
  aqi_history_features[2] = aqi_history_features[1];
  aqi_history_features[1] = aqi_history_features[0];
  aqi_history_features[0] = (float)currentAqi;
}

bool validateSensorData(const SensorData &data) {
  if (!inRangeFloat(data.temperature, TEMP_MIN, TEMP_MAX)) {
    Serial.println("[KHONG_HOP_LE] Nhiet do = " + String(data.temperature));
    return false;
  }

  if (!inRangeFloat(data.humidity, HUM_MIN, HUM_MAX)) {
    Serial.println("[KHONG_HOP_LE] Do am = " + String(data.humidity));
    return false;
  }

  if (!inRangeFloat(data.co, CO_MIN, CO_MAX)) {
    Serial.println("[KHONG_HOP_LE] Khi CO = " + String(data.co));
    return false;
  }

  if (!inRangeFloat(data.pm25, PM25_MIN, PM25_MAX)) {
    Serial.println("[KHONG_HOP_LE] PM2.5 = " + String(data.pm25));
    return false;
  }

  if (!inRangeFloat(data.pm10, PM10_MIN, PM10_MAX)) {
    Serial.println("[KHONG_HOP_LE] PM10 = " + String(data.pm10));
    return false;
  }

  if (!inRangeFloat(data.uvIndex, UV_MIN, UV_MAX)) {
    Serial.println("[KHONG_HOP_LE] UV = " + String(data.uvIndex));
    return false;
  }

  if (!inRangeInt(data.aqIndex, AQI_MIN, AQI_MAX)) {
    Serial.println("[KHONG_HOP_LE] AQI hien tai = " + String(data.aqIndex));
    return false;
  }

  if (!inRangeInt(data.preAQI, AQI_MIN, AQI_MAX)) {
    Serial.println("[KHONG_HOP_LE] AQI du doan = " + String(data.preAQI));
    return false;
  }

  return true;
}

bool readSensorData(SensorData &data) {
  float temp = NAN;
  float hum = NAN;
  getFilteredDHT22(temp, hum);

  float pm25 = NAN;
  float pm10 = NAN;
  PmResult pm = sds.readPm();
  if (pm.isOk()) {
    pm25 = pm.pm25;
    pm10 = pm.pm10;
  } else {
    Serial.println("[CANH_BAO] Doc SDS011 that bai");
  }

  float co_mg_m3 = getCO_mg_m3(filterMQ7.update(analogRead(MQ7_PIN)));
  float uv_index = getUVIndex(filterUV.update(analogRead(UV_PIN)));

  if (!isValidFloat(pm25) || !isValidFloat(pm10)) {
    data.pm25 = NAN;
    data.pm10 = NAN;
  } else {
    updatePmHistory(pm25, pm10);
    data.pm25 = round(pm25 * 10) / 10.0;
    data.pm10 = round(pm10 * 10) / 10.0;
  }

  data.temperature = round(temp * 10) / 10.0;
  data.humidity = round(hum * 10) / 10.0;
  data.co = round(co_mg_m3 * 100) / 100.0;
  data.uvIndex = round(uv_index * 100) / 100.0;

  if (isValidFloat(data.pm25) && isValidFloat(data.pm10) && isValidFloat(data.co)) {
    data.aqIndex = (int)round(aqiCalculator.getTotalAQI(array_pm25, array_pm10, data.co));
  } else {
    data.aqIndex = -1;
  }

  if (inRangeInt(data.aqIndex, AQI_MIN, AQI_MAX)) {
    updateAqiHistory(data.aqIndex);
    float predictedAqi = predict_aqi(aqi_history_features);
    if (!isValidFloat(predictedAqi) || predictedAqi < 0) predictedAqi = 0;
    if (predictedAqi > AQI_MAX) predictedAqi = AQI_MAX;

    data.preAQI = (int)round(predictedAqi);
    lastAQI = data.aqIndex;
    lastPreAQI = data.preAQI;
  } else {
    data.preAQI = -1;
  }

  return validateSensorData(data);
}

bool readValidSensorData(SensorData &data) {
  for (int attempt = 1; attempt <= SENSOR_READ_RETRY; attempt++) {
    Serial.println("\n[CAM_BIEN] Lan doc " + String(attempt) + "/" + String(SENSOR_READ_RETRY));

    if (readSensorData(data)) {
      Serial.println("[OK] Du lieu cam bien hop le");
      printSensorData(data);
      return true;
    }

    Serial.println("[CANH_BAO] Du lieu cam bien khong hop le, doc lai...");
    delay(SENSOR_RETRY_DELAY_MS);
  }

  Serial.println("[LOI] Du lieu cam bien van khong hop le sau so lan doc toi da");
  return false;
}

String buildValuePayload(const SensorData &data) {
  StaticJsonDocument<320> doc;
  doc["type"] = "VALUE";
  doc["TEMP"] = data.temperature;
  doc["HUM"] = data.humidity;
  doc["CO"] = data.co;
  doc["PM25"] = data.pm25;
  doc["PM10"] = data.pm10;
  doc["UV"] = data.uvIndex;
  doc["AQIndex"] = (int)data.aqIndex;
  doc["PreAQI"] = (int)data.preAQI;

  String output;
  serializeJson(doc, output);
  return output;
}
