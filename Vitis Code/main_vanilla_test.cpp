/*****************************************************************//**
 * @file main_vanilla_test.cpp
 *
 * @brief Basic test of 4 basic i/o cores
 *
 * @author p chu
 * @version v1.0: initial release
 *********************************************************************/

// #define _DEBUG
#include "chu_init.h"
#include "gpio_cores.h"
#include "sseg_core.h"
#include "i2c_core.h"

void hr_sensor_output(I2cCore *hr_sensor_p, SsegCore *sseg_p, PwmCore *pwm_p) {
	const uint8_t DEV_ADDR = 0x55;
	uint8_t rbytes[19];
	uint8_t wbytes[4];
	float heartRate;

	pwm_p->set_freq(50);

	wbytes[0] = 0x12;
	wbytes[1] = 0x1;
	hr_sensor_p->write_transaction(DEV_ADDR, wbytes, 2, 0);
	hr_sensor_p->read_transaction(DEV_ADDR, rbytes, 19, 0);

	heartRate = (float)((rbytes[13] << 8) + rbytes[14]);

	int digit1 = (int)heartRate/1000;
	int digit2 = (int)heartRate/100;
	int digit3 = ((int)heartRate % 100) / 10;
	int digit4 = (int)heartRate % 10;

	if(heartRate > 100.0 && heartRate < 3000.0 && rbytes[13] != 0 && heartRate != 1536.0){
		uart.disp(heartRate);
		uart.disp("\n\r");
		if (heartRate >= 1000.0) {
			sseg_p->write_1ptn(sseg_p->h2s(digit1), 3);
			sseg_p->write_1ptn(sseg_p->h2s(digit2), 2);
			sseg_p->write_1ptn(sseg_p->h2s(digit3), 1);
			sseg_p->write_1ptn(sseg_p->h2s(digit4), 0);
			sseg_p->set_dp(0x02);
		} else {
			sseg_p->write_1ptn(0xFF, 3);
			sseg_p->write_1ptn(sseg_p->h2s(digit2), 2);
			sseg_p->write_1ptn(sseg_p->h2s(digit3), 1);
			sseg_p->write_1ptn(sseg_p->h2s(digit4), 0);
			sseg_p->set_dp(0x02);
		}

		if(heartRate >= 600.0 && heartRate <= 1000.0) {
			pwm_p->set_duty(0.0, 2);
			pwm_p->set_duty(0.0, 5);
			pwm_p->set_duty(0.25, 1);
			pwm_p->set_duty(0.25, 4);
		} else {
			pwm_p->set_duty(0.0, 1);
			pwm_p->set_duty(0.0, 4);
			pwm_p->set_duty(0.5, 2);
			pwm_p->set_duty(0.5, 5);
		}

	} else {
		uart.disp("No Object Detected");
		uart.disp("\n\r");
	    sseg_p->write_1ptn(0xC8, 3);
		sseg_p->write_1ptn(0xC0, 2);
		sseg_p->write_1ptn(0xC8, 1);
		sseg_p->write_1ptn(0x86, 0);
		sseg_p->set_dp(0x00);

		pwm_p->set_duty(0.0, 2);
		pwm_p->set_duty(0.0, 5);
		pwm_p->set_duty(0.0, 1);
		pwm_p->set_duty(0.0, 4);
	}

}

void initialize_hr_sensor(I2cCore *hr_sensor_p) {
   const uint8_t DEV_ADDR = 0x55;
   uint8_t wbytes[4], rbytes[19];

   hr_sensor_p->set_freq(100000);

   // Read MAX32664 mode
   wbytes[0] = 0x2;
   wbytes[1] = 0x0;
   hr_sensor_p->write_transaction(DEV_ADDR, wbytes, 2, 0);
   hr_sensor_p->read_transaction(DEV_ADDR, rbytes, 19, 0);

   // Read Sensor Hub Version
   wbytes[0] = 0xFF;
   wbytes[1] = 0x03;
   hr_sensor_p->write_transaction(DEV_ADDR, wbytes, 2, 0);
   hr_sensor_p->read_transaction(DEV_ADDR, rbytes, 19, 0);

   // Get register attributes
   wbytes[0] = 0x42;
   wbytes[1] = 0x3;
   hr_sensor_p->write_transaction(DEV_ADDR, wbytes, 2, 0);
   hr_sensor_p->read_transaction(DEV_ADDR, rbytes, 19, 0);

   // Read all registers
   wbytes[0] = 0x43;
   wbytes[1] = 0x3;
   hr_sensor_p->write_transaction(DEV_ADDR, wbytes, 2, 0);
   hr_sensor_p->read_transaction(DEV_ADDR, rbytes, 19, 0);

   // Read register 7
   wbytes[0] = 0x41;
   wbytes[1] = 0x3;
   wbytes[1] = 0x7;
   hr_sensor_p->write_transaction(DEV_ADDR, wbytes, 2, 0);
   hr_sensor_p->read_transaction(DEV_ADDR, rbytes, 19, 0);

   // Set output mode of MAX32664.
   wbytes[0] = 0x10;
   wbytes[1] = 0x0;
   wbytes[2] = 0x3;
   hr_sensor_p->write_transaction(DEV_ADDR, wbytes, 3, 0);
   hr_sensor_p->read_transaction(DEV_ADDR, rbytes, 19, 0);

   // Set FIFO threshold
   wbytes[0] = 0x10;
   wbytes[1] = 0x1;
   wbytes[2] = 0xF;
   hr_sensor_p->write_transaction(DEV_ADDR, wbytes, 3, 0);
   hr_sensor_p->read_transaction(DEV_ADDR, rbytes, 19, 0);

   // Enable HR sensor
   wbytes[0] = 0x44;
   wbytes[1] = 0x3;
   wbytes[2] = 0x1;
   hr_sensor_p->write_transaction(DEV_ADDR, wbytes, 3, 0);
   hr_sensor_p->read_transaction(DEV_ADDR, rbytes, 19, 0);

   // Enable algorithm
   wbytes[0] = 0x52;
   wbytes[1] = 0x2;
   wbytes[2] = 0x1;
   hr_sensor_p->write_transaction(DEV_ADDR, wbytes, 3, 0);
   hr_sensor_p->read_transaction(DEV_ADDR, rbytes, 19, 0);

   // Read Sensor Hub Status
   wbytes[0] = 0x0;
   wbytes[1] = 0x0;
   hr_sensor_p->write_transaction(DEV_ADDR, wbytes, 2, 0);
   hr_sensor_p->read_transaction(DEV_ADDR, rbytes, 19, 0);

   // Get num samples
   wbytes[0] = 0x12;
   wbytes[1] = 0x0;
   hr_sensor_p->write_transaction(DEV_ADDR, wbytes, 2, 0);
   hr_sensor_p->read_transaction(DEV_ADDR, rbytes, 19, 0);


   // Read Data Stored in FIFO
   wbytes[0] = 0x12;
   wbytes[1] = 0x1;
   hr_sensor_p->write_transaction(DEV_ADDR, wbytes, 2, 0);
   hr_sensor_p->read_transaction(DEV_ADDR, rbytes, 19, 0);

}

void config_app_mode(GpoCore *user_p) {
	// reset = pin 1 on JA
	// MFIO = pin2 on JA
	user_p->write(0x0);
	user_p->write(0x2);
	sleep_ms(10);
	user_p->write(0x3);
	sleep_ms(50);
}

void test_gpio(GpoCore *user_p) {
	user_p->write(0x1);
}


GpoCore user(get_slot_addr(BRIDGE_BASE, S4_USER));
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
PwmCore pwm(get_slot_addr(BRIDGE_BASE, S6_PWM));
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));
I2cCore hr_sensor(get_slot_addr(BRIDGE_BASE, S10_I2C));


int main() {
	config_app_mode(&user);

	sleep_ms(1000);
	GpiCore mfio(get_slot_addr(BRIDGE_BASE, S3_SW));

	initialize_hr_sensor(&hr_sensor);

   while (1) {
	  hr_sensor_output(&hr_sensor, &sseg, &pwm);
   } //while
} //main
