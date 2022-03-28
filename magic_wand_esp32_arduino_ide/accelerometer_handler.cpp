/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "accelerometer_handler.h"
#include <Adafruit_MPU6050.h>
Adafruit_MPU6050 mpu;
int begin_index = 0;

float save_data[600] = {0.0};
bool pending_initial_data = true;
long last_sample_millis = 0;

TfLiteStatus SetupAccelerometer(tflite::ErrorReporter* error_reporter) {
  mpu.begin();
 // error_reporter->Report("failed");

  return kTfLiteOk;
}

static bool UpdateData() {
  bool new_data = false;
  if ((millis() - last_sample_millis) < 40) {
    return false;
  }
  last_sample_millis = millis();

  
  sensors_event_t acc, gyr, temp;
  mpu.getEvent(&acc, &gyr, &temp);

  save_data[begin_index++] = 1000 * acc.acceleration.z;
  save_data[begin_index++] = 1000 * acc.acceleration.x;
  save_data[begin_index++] = 1000 * acc.acceleration.y;
//  Serial.begin(115200);
//  Serial.print("z:");
  //Serial.println(1000 * acc.acceleration.z);
  delay(1000);
  if (begin_index >= 600) {
    begin_index = 0;
  }
  new_data = true;

  return new_data;
}

bool ReadAccelerometer(tflite::ErrorReporter* error_reporter, float* input,
                       int length, bool reset_buffer) {
  if (reset_buffer) {
    memset(save_data, 0, 600 * sizeof(float));
    begin_index = 0;
    pending_initial_data = true;
  }
//  Serial.begin(115200);
//  Serial.println(UpdateData());
//  Serial.println(begin_index);
//  Serial.println(pending_initial_data); 
  
  if (!UpdateData()) {
     Serial.begin(9600);
  Serial.print("error");
    return false;
  }

//  if (pending_initial_data && begin_index >= 200) {
//    Serial.begin(115200);
//    Serial.println("checkpoint3");
//    pending_initial_data = false;
//  }

//  if (pending_initial_data) {
//    Serial.begin(115200);
//    Serial.println("checkpoint4");
//    return false;
//  }

  for (int i = 0; i < length; ++i) {
    int ring_array_index = begin_index + i - length;
    if (ring_array_index < 0) {
      ring_array_index += 600;
    }
    input[i] = save_data[ring_array_index];
  }
  return true;
}
