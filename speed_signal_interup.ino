volatile int pwm_value_speed = 0;
volatile int prev_time_speed = 0;

volatile int prev_time_stearing = 0;
volatile int pwm_value_stearing = 0;

volatile bool toggle_action = false;
volatile int lastRecordedVals[5] = {0, 0, 0, 0, 0};

volatile int i = -1;

const int MOTOR_ACTIVE_RANGE = 230 - 195;

void setup() {
  Serial.begin(115200);  
  attachInterrupt(0, risingSpeed, RISING);
  attachInterrupt(1, risingStearing, RISING);
}

void loop() {
  int val = scaleToAnalogSig(mean());

  int motor_pwm_left = val;
  int motor_pwm_right = val;

  float normalized_stearing = normalize_pwm(pwm_value_stearing);
  Serial.println(normalized_stearing);
  
  if(normalized_stearing < 0.45) { // stearing left
    motor_pwm_right = val;
    motor_pwm_left = val - (MOTOR_ACTIVE_RANGE * (1 - normalized_stearing) * 5);
  } else if (normalized_stearing > 0.55) { // stearing right
    //motor_pwm_left = val - (MOTOR_ACTIVE_RANGE * (1-normalized_stearing) * (1-normalized_stearing));
    //motor_pwm_right = val;  
  }
  
  analogWrite(11, motor_pwm_left); // 11 is left
  analogWrite(10, motor_pwm_right); // 10 is right
}

float scaleToAnalogSig(float val) {
  return ((230.0-195.0)*((val - 1500.0) / (2000.0 - 1500.0))) + 195;
}

float mean() {
  int sum = 0;
  for (byte i = 0; i < 5; i = i + 1) {
    sum += lastRecordedVals[i];
  }

  return sum / 5;
}

float normalize_pwm(int pwm) {
  const float MIN_PWM = 985.0;
  const float MAX_PWM = 2015.0;

  return (pwm - MIN_PWM) / (MAX_PWM - MIN_PWM);
}


void risingSpeed() {
  attachInterrupt(0, fallingSpeed, FALLING);
  prev_time_speed = micros();
}

void fallingSpeed() {
  attachInterrupt(0, risingSpeed, RISING);
  i++;
  pwm_value_speed = micros() - prev_time_speed;
  if (pwm_value_speed > 1600) {
    lastRecordedVals[i] = pwm_value_speed;
  } else {
    lastRecordedVals[i] = 0;
  }

  if (i == 4) {
    i = -1;
  }
}


void risingStearing() {
  attachInterrupt(1, fallingStearing, FALLING);
  prev_time_stearing = micros(); 
}

void fallingStearing() {
 attachInterrupt(1, risingStearing, RISING);
 pwm_value_stearing = micros() - prev_time_stearing;
}
