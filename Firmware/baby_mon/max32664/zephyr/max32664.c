

#define DT_DRV_COMPAT max_max32664

#define RESET_GPIO_NODE DT_NODELABEL(rstpin)
//#define RESETPIN	DT_GPIO_LABEL(RESET_GPIO_NODE, gpios)

#include <logging/log.h>

#include "max32664.h"

// Variables -----------
uint8_t _resetPin;
uint8_t _mfioPin;
uint8_t _address;
uint32_t _writeCoefArr[3];
uint8_t _userSelectedMode;
uint8_t _sampleRate = 100;

static uint16_t _i2c_addr;
static struct device *_i2c_device;

struct bioData _bioData;

uint8_t bpmArr[MAXFAST_ARRAY_SIZE];
uint8_t bpmArrTwo[MAXFAST_ARRAY_SIZE + MAXFAST_EXTENDED_DATA];
uint8_t senArr[MAX30101_LED_ARRAY];
uint8_t bpmSenArr[MAXFAST_ARRAY_SIZE + MAX30101_LED_ARRAY];
uint8_t bpmSenArrTwo[MAXFAST_ARRAY_SIZE + MAXFAST_EXTENDED_DATA + MAX30101_LED_ARRAY];


LOG_MODULE_REGISTER(MAX32664, CONFIG_SENSOR_LOG_LEVEL);


// This function sends information to the MAX32664 to specifically write values
// to the registers of downward sensors and so also requires a
// register address and register value as parameters. Again there is the write
// of the specific bytes followed by a read to confirm positive transmission.
uint8_t writeByte44(uint8_t _familyByte, uint8_t _indexByte, uint8_t _writeByte, uint8_t _writeVal)
{

  uint8_t tmpBuf[5];

  tmpBuf[0] = _familyByte;
  tmpBuf[1] = _indexByte;
  tmpBuf[2] = _writeByte;
  tmpBuf[3] = _writeVal >> 8;
  tmpBuf[4] = _writeVal;


  i2c_write(_i2c_device, tmpBuf, 5, _i2c_addr);

  k_msleep(CMD_DELAY);


  i2c_read(_i2c_device, tmpBuf, 1,  _i2c_addr);

  uint8_t statusByte = tmpBuf[0];

  return statusByte;

}

// This function uses the given family, index, and write byte to communicate
// with the MAX32664 which in turn communicates with downward sensors. There
// are two steps demonstrated in this function. First a write to the MCU
// indicating what you want to do, a delay, and then a read to confirm positive
// transmission.
uint8_t writeByte3(uint8_t _familyByte, uint8_t _indexByte, uint8_t _writeByte)
{

  uint8_t tmpBuf[3];

  tmpBuf[0] = _familyByte;
  tmpBuf[1] = _indexByte;
  tmpBuf[2] = _writeByte;

   i2c_write(_i2c_device, tmpBuf, 3, _i2c_addr);

  k_msleep(CMD_DELAY);


  i2c_read(_i2c_device, tmpBuf, 1,  _i2c_addr);

  uint8_t statusByte = tmpBuf[0];

  return statusByte;

}

// This function is the same as the function above and uses the given family,
// index, and write byte, but also takes a 16 bit integer as a paramter to communicate
// with the MAX32664 which in turn communicates with downward sensors. There
// are two steps demonstrated in this function. First a write to the MCU
// indicating what you want to do, a delay, and then a read to confirm positive
// transmission.
uint8_t writeByte4(uint8_t _familyByte, uint8_t _indexByte, uint8_t _writeByte, uint16_t _val)
{

  uint8_t tmpBuf[5];

  tmpBuf[0] = _familyByte;
  tmpBuf[1] = _indexByte;
  tmpBuf[2] = _writeByte;
  tmpBuf[3] = _val >> 8;
  tmpBuf[4] = _val;


   i2c_write(_i2c_device, tmpBuf, 5, _i2c_addr);

  k_msleep(CMD_DELAY);


  i2c_read(_i2c_device, tmpBuf, 1,  _i2c_addr);

  uint8_t statusByte = tmpBuf[0];

  return statusByte;
}

// This function handles all read commands or stated another way, all information
// requests. It starts a request by writing the family byte an index byte, and
// then delays 60 microseconds, during which the MAX32664 retrieves the requested 
// information. An I-squared-C request is then issued, and the information is read.
uint8_t readByte2(uint8_t _familyByte, uint8_t _indexByte )
{
  uint8_t tmpBuf[2];
  tmpBuf[0] = _familyByte;
  tmpBuf[1] = _indexByte;
 
  uint8_t returnByte;
  uint8_t statusByte;


   i2c_write(_i2c_device, tmpBuf, 2, _i2c_addr);

   k_msleep(CMD_DELAY);

   i2c_read(_i2c_device, tmpBuf, 2,  _i2c_addr);
   

   statusByte = tmpBuf[0];
   returnByte = tmpBuf[1];

  if( statusByte )// SUCCESS (0x00) - how do I know its 
    return statusByte; // Return the error, see: READ_STATUS_BYTE_VALUE 


  return returnByte; // If good then return the actual byte. 

}


// This function is exactly as the one above except it accepts also receives a
// Write Byte as a parameter. It starts a request by writing the family byte, index byte, and
// write byte to the MAX32664 and then delays 60 microseconds, during which
// the MAX32664 retrieves the requested information. A I-squared-C request is
// then issued, and the information is read.
uint8_t  readByte3(uint8_t _familyByte, uint8_t _indexByte,\
                                           uint8_t _writeByte)
{

 uint8_t tmpBuf[3];
  tmpBuf[0] = _familyByte;
  tmpBuf[1] = _indexByte;
  tmpBuf[2] = _writeByte;

  uint8_t returnByte;
  uint8_t statusByte;


   i2c_write(_i2c_device, tmpBuf, 3, _i2c_addr);

   k_msleep(CMD_DELAY);

   i2c_read(_i2c_device, tmpBuf, 2,  _i2c_addr);
   

   statusByte = tmpBuf[0];
   returnByte = tmpBuf[1];

  if( statusByte )// SUCCESS (0x00) - how do I know its 
    return statusByte; // Return the error, see: READ_STATUS_BYTE_VALUE 


  return returnByte; // If good then return the actual byte. 
}

uint8_t readFillArray(uint8_t _familyByte, uint8_t _indexByte, uint8_t _numOfReads, uint8_t *array )
{

  uint8_t statusByte;

  uint8_t tmpBuf[2];
  tmpBuf[0] = _familyByte;
  tmpBuf[1] = _indexByte;

  i2c_write(_i2c_device, tmpBuf, 2, _i2c_addr);
  
  k_msleep(CMD_DELAY);

  i2c_read(_i2c_device, array, _numOfReads + 1,  _i2c_addr);

  statusByte = array[0];

  return statusByte;
}

// This function uses the given family, index, and write byte to enable
// the given sensor.
uint8_t enableWrite(uint8_t _familyByte, uint8_t _indexByte, uint8_t _enableByte)
{

 uint8_t tmpBuf[3];
  tmpBuf[0] = _familyByte;
  tmpBuf[1] = _indexByte;
  tmpBuf[2] = _enableByte;

  i2c_write(_i2c_device, tmpBuf, 3, _i2c_addr);

   k_msleep(ENABLE_CMD_DELAY);

  i2c_read(_i2c_device, tmpBuf, 1,  _i2c_addr);
   
  uint8_t statusByte = tmpBuf[0];

  return statusByte;
}



// Family Byte: READ_ALGORITHM_CONFIG (0x51), Index Byte:
// READ_AGC_NUM_SAMPLES (0x00), Write Byte: READ_AGC_NUM_SAMPLES_ID (0x03)
// This function changes the number of samples that are averaged.
// It takes a paramater of zero to 255.
uint8_t readAlgoSamples() {

  uint8_t samples = readByte3(READ_ALGORITHM_CONFIG, READ_AGC_NUM_SAMPLES, READ_AGC_NUM_SAMPLES_ID );
  return samples;

}




// Family Byte: ENABLE_ALGORITHM (0x52), Index Byte:
// ENABLE_WHRM_ALGO (0x02)
// This function enables (one) or disables (zero) the wrist heart rate monitor
// algorithm.
uint8_t maximFastAlgoControl(uint8_t mode) {

  if( mode == 0 || mode == 1 || mode == 2) {}
  else
    return INCORR_PARAM;

  uint8_t statusByte = enableWrite(ENABLE_ALGORITHM, ENABLE_WHRM_ALGO, mode);
  if (statusByte != SUCCESS)
    return statusByte;
  else
    return SUCCESS;

}

// Family Byte: ENABLE_SENSOR (0x44), Index Byte: ENABLE_MAX30101 (0x03), Write
// Byte: senSwitch  (parameter - 0x00 or 0x01).
// This function enables the MAX30101.
uint8_t max30101Control(uint8_t senSwitch) {

  if(senSwitch == 0 || senSwitch == 1)
    { }
  else
    return INCORR_PARAM;

  // Check that communication was successful, not that the sensor is enabled.
  uint8_t statusByte = enableWrite(ENABLE_SENSOR, ENABLE_MAX30101, senSwitch);
  if( statusByte != SUCCESS )
    return statusByte;
  else
    return SUCCESS;

}


// Family Byte: ENABLE_ALGORITHM (0x52), Index Byte:
// ENABLE_AGC_ALGO (0x00)
// This function enables (one) or disables (zero) the automatic gain control algorithm.
uint8_t agcAlgoControl(uint8_t enable) {

  if( enable == 0 || enable == 1) {}
  else
    return INCORR_PARAM;

  uint8_t statusByte = enableWrite(ENABLE_ALGORITHM, ENABLE_AGC_ALGO, enable);
  if (statusByte != SUCCESS)
    return statusByte;
  else
    return SUCCESS;

}

// Family Byte: OUTPUT_MODE(0x10), Index Byte: WRITE_SET_THRESHOLD (0x01), Write byte: intThres
// (parameter - value betwen 0 and 0xFF).
// This function changes the threshold for the FIFO interrupt bit/pin. The
// interrupt pin is the MFIO pin which is set to INPUT after IC initialization
// (begin).
uint8_t setFifoThreshold(uint8_t intThresh) {

  // Checks that there was succesful communcation, not that the threshold was
  // set correctly.
  uint8_t statusByte = writeByte3(OUTPUT_MODE, WRITE_SET_THRESHOLD, intThresh);
  if( statusByte != SUCCESS)
    return statusByte;
  else
    return SUCCESS;

}


// Family Byte: OUTPUT_MODE (0x10), Index Byte: SET_FORMAT (0x00),
// Write Byte : outputType (Parameter values in OUTPUT_MODE_WRITE_BYTE)
uint8_t setOutputMode(uint8_t outputType) {

  if ( outputType > SENSOR_ALGO_COUNTER ) // Bytes between 0x00 and 0x07
    return INCORR_PARAM;

  // Check that communication was successful, not that the IC is outputting
  // correct format.
  uint8_t statusByte = writeByte3(OUTPUT_MODE, SET_FORMAT, outputType);
  if( statusByte != SUCCESS)
    return statusByte;
  else
    return SUCCESS;

}


// This function sets very basic settings to get sensor and biometric data.
// The biometric data includes data about heartrate, the confidence
// level, SpO2 levels, and whether the sensor has detected a finger or not.
uint8_t configBpm(uint8_t mode){

  uint8_t statusChauf = 0;
  if (mode == MODE_ONE || mode == MODE_TWO){}
  else return INCORR_PARAM;

  statusChauf = setOutputMode(ALGO_DATA); // Just the data
  if( statusChauf != SUCCESS )
    return statusChauf;

  statusChauf = setFifoThreshold(0x01); // One sample before interrupt is fired.
  if( statusChauf != SUCCESS )
    return statusChauf;

  statusChauf = agcAlgoControl(ENABLE); // One sample before interrupt is fired.
  if( statusChauf != SUCCESS )
    return statusChauf;

  LOG_INF("config Max30101");

  statusChauf = max30101Control(ENABLE);
  
  LOG_INF("statusChauf=0x%02x", statusChauf);
  if( statusChauf != SUCCESS )
    return statusChauf;
  

  statusChauf = maximFastAlgoControl(mode);
  if( statusChauf != SUCCESS )
    return statusChauf;

  _userSelectedMode = mode;
  _sampleRate = readAlgoSamples();

  k_msleep(1000);
  return SUCCESS;

}


// Family Byte: HUB_STATUS (0x00), Index Byte: 0x00, No Write Byte.
// The following function checks the status of the FIFO.
uint8_t readSensorHubStatus(){

  uint8_t status = readByte2(0x00, 0x00); // Just family and index byte.
  return status; // Will return 0x00

}

// Family Byte: READ_DATA_OUTPUT (0x12), Index Byte: NUM_SAMPLES (0x00), Write
// Byte: NONE
// This function returns the number of samples available in the FIFO.
uint8_t numSamplesOutFifo() {

  uint8_t sampAvail = readByte2(READ_DATA_OUTPUT, NUM_SAMPLES);
  return sampAvail;

}

// This function takes the 8 bytes from the FIFO buffer related to the wrist
// heart rate algortihm: heart rate (uint16_t), confidence (uint8_t) , SpO2 (uint16_t),
// and the finger detected status (uint8_t). Note that the the algorithm is stated as
// "wrist" though the sensor only works with the finger. The data is loaded
// into the whrmFifo and returned.
struct bioData readBpm(){

  struct bioData libBpm;
  uint8_t statusChauf; // The status chauffeur captures return values.

  statusChauf = readSensorHubStatus();

 // LOG_INF("Sensor Status=0x%02x", statusChauf);

  if (statusChauf == 1){ // Communication Error
    libBpm.heartRate = 0;
    libBpm.confidence = 0;
    libBpm.oxygen = 0;
    LOG_INF("communication error");
    return libBpm;
  }

  numSamplesOutFifo();

  if (_userSelectedMode == MODE_ONE) {

    readFillArray(READ_DATA_OUTPUT, READ_DATA, MAXFAST_ARRAY_SIZE, bpmArr);

    // Heart Rate formatting
    libBpm.heartRate = (uint16_t)bpmArr[1] << 8;
    libBpm.heartRate |= (bpmArr[2]);
    libBpm.heartRate /= 10;

    // Confidence formatting
    libBpm.confidence = bpmArr[3];

    //Blood oxygen level formatting
    libBpm.oxygen = (uint16_t)bpmArr[4] << 8;
    libBpm.oxygen |= bpmArr[5];
    libBpm.oxygen /= 10;

    //"Machine State" - has a finger been detected?
    libBpm.status = bpmArr[6];

    return libBpm;
  }
  else if (_userSelectedMode == MODE_TWO) {
    readFillArray(READ_DATA_OUTPUT, READ_DATA,\
        MAXFAST_ARRAY_SIZE + MAXFAST_EXTENDED_DATA, bpmArrTwo);

    // Heart Rate formatting
    libBpm.heartRate = (uint16_t)bpmArrTwo[1] << 8;
    libBpm.heartRate |= (bpmArrTwo[2]);
    libBpm.heartRate /= 10;

    // Confidence formatting
    libBpm.confidence = bpmArrTwo[3];

    //Blood oxygen level formatting
    libBpm.oxygen = (uint16_t)bpmArrTwo[4] << 8;
    libBpm.oxygen |= bpmArrTwo[5];
    libBpm.oxygen /= 10.0;

    //"Machine State" - has a finger been detected?
    libBpm.status = bpmArrTwo[6];

    //Sp02 r Value formatting
    uint16_t tempVal = (uint16_t)bpmArrTwo[7] << 8;
    tempVal |= bpmArrTwo[8];
    libBpm.rValue = tempVal;
    libBpm.rValue /= 10.0;

    //Extended Machine State formatting
    libBpm.extStatus = bpmArrTwo[9];

    // There are two additional bytes of data that were requested but that
    // have not been implemented in firmware 10.1 so will not be saved to
    // user's data.
    return libBpm;
  }
  else {
    libBpm.heartRate  = 0;
    libBpm.confidence = 0;
    libBpm.oxygen     = 0;
    return libBpm;
  }

}

static int max32664_sample_fetch(const struct device *dev,
				 enum sensor_channel chan)
{
  _bioData = readBpm();

  // LOG_INF("bpm=%d",  _bioData.heartRate);
  // LOG_INF("confidence=%d",  _bioData.confidence);
  // LOG_INF("oxygen=%d",  _bioData.oxygen);
  // LOG_INF("status=%d",  _bioData.status);

	return 0;
}

static int max32664_channel_get(const struct device *dev,
				enum sensor_channel chan,
				struct sensor_value *val)
{
  switch (chan) {
	case SENSOR_CHAN_HEARTRATE:
		val->val1 = _bioData.heartRate;
		val->val2 = 0;
		break;
	case SENSOR_CHAN_SPO2:
		val->val1 = _bioData.oxygen;
		val->val2 = 0;
		break;
	case SENSOR_CHAN_CONFIDENCE:
		val->val1 = _bioData.confidence;
		val->val2 = 0;
		break;
	case SENSOR_CHAN_HR_STATUS:
		val->val1 = _bioData.status;
		val->val2 = 0;
		break;
	default:
		LOG_ERR("Unsupported sensor channel");
		return -ENOTSUP;
	}

	return 0;
}

static const struct sensor_driver_api max32664_driver_api = {
	.sample_fetch = max32664_sample_fetch,
	.channel_get = max32664_channel_get,
};



static int max32664_init(const struct device *dev)
{

	const struct max32664_config *config = dev->config;
	struct max32664_data *data = dev->data;

  const struct device *rst_gpio, *mfio_gpio;

  rst_gpio = device_get_binding(DT_INST_GPIO_LABEL(0, rst_gpios));
  mfio_gpio = device_get_binding(DT_INST_GPIO_LABEL(0, mfio_gpios));

  if (rst_gpio == NULL || mfio_gpio == NULL)
  {
    	LOG_ERR("Could not get pointer to %s device.",
      DT_INST_GPIO_LABEL(0, rst_gpios));
      return -EINVAL;
  }

  

  gpio_pin_configure(rst_gpio, DT_INST_GPIO_PIN(0, rst_gpios), GPIO_OUTPUT | DT_INST_GPIO_FLAGS(0, rst_gpios));
  gpio_pin_configure(mfio_gpio, DT_INST_GPIO_PIN(0, mfio_gpios),  GPIO_OUTPUT);

  gpio_pin_set(mfio_gpio, DT_INST_GPIO_PIN(0, mfio_gpios), true);
  gpio_pin_set(rst_gpio, DT_INST_GPIO_PIN(0, rst_gpios), false);
  k_msleep(10);
  gpio_pin_set(rst_gpio, DT_INST_GPIO_PIN(0, rst_gpios), true);
  k_msleep(1000);
  gpio_pin_configure(mfio_gpio, DT_INST_GPIO_PIN(0, mfio_gpios),  DT_INST_GPIO_FLAGS(0, mfio_gpios));

  // const struct device *gpio;

  // gpio = device_get_binding(RESETPIN);



	/* Get the I2C device */
	data->i2c = device_get_binding(config->i2c_label);
	if (!data->i2c) {
		LOG_ERR("Could not find I2C device");
		return -EINVAL;
	}

	_i2c_device = data->i2c;
	_i2c_addr = config->i2c_addr;


  uint8_t responseByte = readByte2(READ_DEVICE_MODE, 0x00); // 0x00 only possible Index Byte.

  if (responseByte == 0 )
  {
    LOG_INF("Sensor OK!");
  }
  else 
  {
    LOG_INF("Sensor error=%d", responseByte);
  }

  int error = configBpm(MODE_ONE); // Configuring just the BPM settings. 
  if(error == 0){ // Zero errors!
    LOG_INF("Sensor configured.");
  }
  else {
     LOG_INF("Error configuring sensor.%d", error);
  }

  uint8_t returnByte = readByte3(IDENTITY, READ_MCU_TYPE, NO_WRITE);

  LOG_INF("mcu_info=0x%02x",returnByte);

  k_msleep(4000);

	return 0;
}


static struct max32664_config max32664_config = {
	.i2c_label = DT_INST_BUS_LABEL(0),
	.i2c_addr = DT_INST_REG_ADDR(0)
};

static struct max32664_data max32664_data;

DEVICE_DT_INST_DEFINE(0, max32664_init, device_pm_control_nop,
		    &max32664_data, &max32664_config,
		    APPLICATION, CONFIG_SENSOR_INIT_PRIORITY,
		    &max32664_driver_api);
